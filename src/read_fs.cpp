#include "read_fs.h"
#include "tee_fs_htree.h"

#include <climits>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <unistd.h>


using namespace crack::read_fs;
using namespace crack::tee_fs_htree;

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
    if (res < 0 )
    {
        printf("get idx:%d , vers:%d TEE_FS_HTREE_TYPE_HEAD offs_size error!\n", idx, vers);
        return nullptr;
    }
    // 创建对象
    TEE_FS_HTREE_IMAGE_PTR htree_image_ptr = std::make_unique<tee_fs_htree_image>();
    // 设置偏移量
    lseek(fd,SEEK_CUR,offs);
    // 读取数据
    res = read(fd,htree_image_ptr.get(),size);
     if (res < 0 )
    {
        printf("read idx:%d , vers:%d TEE_FS_HTREE_TYPE_HEAD error!\n", idx, vers);
        return nullptr;
    }

    return htree_image_ptr;
}