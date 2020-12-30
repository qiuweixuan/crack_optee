#include "read_fs.h"
#include "tee_fs_htree.h"
#include "tee_key.h"
#include "crack_fs.h"

#include <climits>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>

using namespace crack::read_fs;
using namespace crack::tee_fs_htree;
using namespace crack::tee_key;

std::string crack::read_fs::path_join(std::string &dir_path, std::string file_name)
{
    char buf[PATH_MAX + 1];
    char *res = realpath(dir_path.c_str(), buf);
    std::string abs_dir_path(res);
    std::string abs_file_path = abs_dir_path.append("/").append(file_name);

    return abs_file_path;
}

int crack::read_fs::get_offs_size(tee_fs_htree_type type, uint32_t idx,
                                  uint8_t vers, uint32_t &offs, uint32_t &size)
{

    const uint32_t BLOCK_SIZE = crack::tee_fs_htree::BLOCK_SIZE;
    const uint32_t node_size = sizeof(tee_fs_htree_node_image);
    const uint32_t block_nodes = BLOCK_SIZE / (node_size * 2);

    uint32_t pbn;
    uint32_t bidx;

    assert(vers == 0 || vers == 1);

    /*
	 * File layout
	 * [demo with input:
	 * BLOCK_SIZE = 4096,
	 * node_size = 66,
	 * block_nodes = 4096/(66*2) = 31 ]
	 *
	 * phys block 0:
	 * tee_fs_htree_image vers 0 @ offs = 0
	 * tee_fs_htree_image vers 1 @ offs = sizeof(tee_fs_htree_image)
	 *
	 * phys block 1:
	 * tee_fs_htree_node_image 0  vers 0 @ offs = 0
	 * tee_fs_htree_node_image 0  vers 1 @ offs = node_size
	 * tee_fs_htree_node_image 1  vers 0 @ offs = node_size * 2
	 * tee_fs_htree_node_image 1  vers 1 @ offs = node_size * 3
	 * ...
	 * tee_fs_htree_node_image 30 vers 0 @ offs = node_size * 60
	 * tee_fs_htree_node_image 30 vers 1 @ offs = node_size * 61
	 *
	 * phys block 2:
	 * data block 0 vers 0
	 *
	 * phys block 3:
	 * data block 0 vers 1
	 *
	 * ...
	 * phys block 62:
	 * data block 30 vers 0
	 *
	 * phys block 63:
	 * data block 30 vers 1
	 *
	 * phys block 64:
	 * tee_fs_htree_node_image 31  vers 0 @ offs = 0
	 * tee_fs_htree_node_image 31  vers 1 @ offs = node_size
	 * tee_fs_htree_node_image 32  vers 0 @ offs = node_size * 2
	 * tee_fs_htree_node_image 32  vers 1 @ offs = node_size * 3
	 * ...
	 * tee_fs_htree_node_image 61 vers 0 @ offs = node_size * 60
	 * tee_fs_htree_node_image 61 vers 1 @ offs = node_size * 61
	 *
	 * phys block 65:
	 * data block 31 vers 0
	 *
	 * phys block 66:
	 * data block 31 vers 1
	 * ...
	 */

    switch (type)
    {
    case tee_fs_htree_type::TEE_FS_HTREE_TYPE_HEAD:
        offs = sizeof(tee_fs_htree_image) * vers;
        size = sizeof(tee_fs_htree_image);
        return 0;
    case tee_fs_htree_type::TEE_FS_HTREE_TYPE_NODE:
        pbn = 1 + ((idx / block_nodes) * block_nodes * 2);
        offs = pbn * BLOCK_SIZE +
               2 * node_size * (idx % block_nodes) +
               node_size * vers;
        size = node_size;
        return 0;
    case tee_fs_htree_type::TEE_FS_HTREE_TYPE_BLOCK:
        bidx = 2 * idx + vers;
        pbn = 2 + bidx + bidx / (block_nodes * 2 - 1);
        offs = pbn * BLOCK_SIZE;
        size = BLOCK_SIZE;
        return 0;
    default:
        return -1;
    }
}

TEE_FS_HTREE_IMAGE_PTR crack::read_fs::read_htree_image(int fd, uint8_t vers)
{

    uint32_t offs;
    uint32_t size;
    uint32_t idx = 0;

    int res = get_offs_size(tee_fs_htree_type::TEE_FS_HTREE_TYPE_HEAD, idx, vers, offs, size);
    if (res < 0)
    {
        printf("get idx:%d , vers:%d TEE_FS_HTREE_TYPE_HEAD offs_size error!\n", idx, vers);
        return nullptr;
    }
    else{
        printf("get idx:%d , vers:%d TEE_FS_HTREE_TYPE_HEAD , offs:%d, size: %d \n", idx, vers,offs,size);
    }
    // 创建对象
    TEE_FS_HTREE_IMAGE_PTR htree_image_ptr = std::make_unique<tee_fs_htree_image>();
    // 设置偏移量
    lseek(fd,offs,SEEK_SET);

    // 读取数据
    res = read(fd, htree_image_ptr.get(), size);


    if (res < 0)
    {
        printf("read idx:%d , vers:%d TEE_FS_HTREE_TYPE_HEAD error!\n", idx, vers);
        return nullptr;
    }

    return htree_image_ptr;
}
crack::tee_fs_htree::TEE_FS_HTREE_NODE_IMAGE_PTR crack::read_fs::read_htree_node_image(int fd, uint32_t idx, uint8_t vers)
{
    uint32_t offs;
    uint32_t size;

    int res = get_offs_size(tee_fs_htree_type::TEE_FS_HTREE_TYPE_NODE, idx, vers, offs, size);
    if (res < 0)
    {
        printf("get idx:%d , vers:%d TEE_FS_HTREE_TYPE_NODE offs_size error!\n", idx, vers);
        return nullptr;
    }
    // 创建对象
    TEE_FS_HTREE_NODE_IMAGE_PTR htree_node_image_ptr = std::make_unique<tee_fs_htree_node_image>();
    // 设置偏移量
    lseek(fd,offs,SEEK_SET);
    // 读取数据
    res = read(fd, htree_node_image_ptr.get(), size);
    if (res < 0)
    {
        printf("read idx:%d , vers:%d TEE_FS_HTREE_TYPE_NODE error!\n", idx, vers);
        return nullptr;
    }
    else{
         printf("read idx:%d , vers:%d TEE_FS_HTREE_TYPE_NODE , offs:%d, size: %d\n", idx, vers, offs ,size);
    }

    return htree_node_image_ptr;
}

crack::tee_fs_htree::TEE_FS_DATA_BLOCK_PTR crack::read_fs::read_data_block(int fd, uint32_t idx, uint8_t vers)
{
    uint32_t offs;
    uint32_t size;

    int res = get_offs_size(tee_fs_htree_type::TEE_FS_HTREE_TYPE_BLOCK, idx, vers, offs, size);
    if (res < 0)
    {
        printf("get idx:%d , vers:%d TEE_FS_HTREE_TYPE_BLOCK offs_size error!\n", idx, vers);
        return nullptr;
    }
    // 创建对象
    TEE_FS_DATA_BLOCK_PTR data_block_ptr = std::make_unique<tee_fs_data_block>();
    // 设置偏移量
    lseek(fd,offs,SEEK_SET);
    // 读取数据
    res = read(fd, data_block_ptr->block, size);
    if (res < 0)
    {
        printf("read idx:%d , vers:%d TEE_FS_HTREE_TYPE_BLOCK error!\n", idx, vers);
        return nullptr;
    }
    else{
         printf("read idx:%d , vers:%d TEE_FS_HTREE_TYPE_BLOCK , offs:%d, size: %d\n", idx, vers, offs ,size);
    }

    return data_block_ptr;
}



static int crack::read_fs::get_idx_from_counter(uint32_t counter0, uint32_t counter1)
{
	if (!(counter0 & 1)) {
		if (!(counter1 & 1))
			return 0;
		if (counter0 > counter1)
			return 0;
		else
			return 1;
	}

	if (counter1 & 1)
		return 1;
	else 
		return -1;
}


crack::tee_fs_htree::TEE_FS_HTREE_IMAGE_PTR crack::read_fs::get_dirfdb_htree_image(int fd,int& vers){
     // 读取第一个htree_image
    auto htree_image_ptr_0 = crack::read_fs::read_htree_image(fd,0);
    // 读取第二个htree_image
    auto htree_image_ptr_1 = crack::read_fs::read_htree_image(fd,1);

    // 判断是否有错误
    if(!htree_image_ptr_0 || !htree_image_ptr_1){
        return nullptr;
    }

    // 获取版本vers
    vers = crack::read_fs::get_idx_from_counter(htree_image_ptr_0->counter, htree_image_ptr_1->counter);

    printf("vers: %d \n",vers);

    return vers == 0? std::move(htree_image_ptr_0) : std::move(htree_image_ptr_1);
}

uint32_t crack::read_fs::get_dirfile_entry_cnt(crack::tee_fs_htree::tee_fs_htree_imeta& imeta){
    return imeta.meta.length / sizeof(crack::tee_fs_htree::dirfile_entry);
}

void crack::read_fs::get_node_images(int fd, std::vector<crack::tee_fs_htree::TEE_FS_HTREE_NODE_IMAGE_PTR>& node_image_ptr_vec ,uint32_t node_image_cnt){
    
    for (uint32_t node_id = 2; node_id <= node_image_cnt; node_id++)
    {
        uint32_t p = node_id / 2 - 1;
        auto&& p_node = node_image_ptr_vec[p];

        uint8_t committed_version = !!(p_node->flags &
				   (1 << (1 + (node_id & 1) )) );

        auto node_image_ptr = crack::read_fs::read_htree_node_image(fd,node_id - 1,committed_version);
        
        node_image_ptr_vec.emplace_back(std::move(node_image_ptr));
    }
}

void crack::read_fs::save_data_blocks(int fd, int recover_fd,tee_fs_fek &fek,tee_fs_htree_image& image,std::vector<TEE_FS_HTREE_NODE_IMAGE_PTR>& node_image_ptr_vec ){
    // 设置恢复文件偏移量
    // lseek(recover_fd,0,SEEK_SET);

    // 循环读取块
    for (uint32_t block_num = 0; block_num <  node_image_ptr_vec.size(); block_num++)
    {
        // 获取node_image
        auto&& node = node_image_ptr_vec[block_num];

        // 获取块版本
        uint8_t block_vers = !!(node->flags & (1 << 0));

        // 读取块
        auto data_block_ptr = crack::read_fs::read_data_block(fd,block_num,block_vers);

        // 解密数据块
        crack::crack_fs::decrypt_data_block(fek,image,*node,*data_block_ptr);

        // 写数据
        int res = write(recover_fd, data_block_ptr->block, sizeof(data_block_ptr->block));
        if (res < 0)
        {
            printf("write idx:%d , vers:%d TEE_FS_HTREE_TYPE_BLOCK error!\n", block_num, block_vers);
            return ;
        }

    }
}