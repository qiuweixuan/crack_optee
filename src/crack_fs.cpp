#include "crack_fs.h"
#include "generate_key.h"
#include "print_fs.h"
#include "read_fs.h"
#include "crypt_alg.h"
#include "tee_key.h"
#include "tee_fs_htree.h"

#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <iostream>

using namespace crack::tee_key;
using namespace crack::tee_fs_htree;

/**
 * @brief 恢复索引文件内容
 * @param  storage_dir 存储加密文件的目录
 * @param tee_fs_ssk 安全存储密钥
 * @return 返回dirfile_entry_cnt（索引列表个数）
 */
uint32_t crack::crack_fs::crack_dirfdb(std::string &storage_dir, std::string &recover_save_dir,crack::tee_key::tee_fs_ssk &tee_fs_ssk)
{
    
    // 获取索引文件路径
    std::string index_file_name("dirf.db");
    std::string file_path = crack::read_fs::path_join(storage_dir, index_file_name);

    // 打开索引文件
    int fd = open(file_path.c_str(), O_RDONLY);
    if (fd < 0)
    {
        printf("open file: %s error!\n", file_path.c_str());
        return 0;
    }

    // 设置恢复索引文件路径
    std::string recover_index_file_name("recover_dirf.db");
    std::string recover_file_path = crack::read_fs::path_join(recover_save_dir, recover_index_file_name);
    // 创建恢复索引文件
    int recover_fd = open(recover_file_path.c_str(), O_RDWR| O_CREAT | O_TRUNC );
    if (recover_fd < 0)
    {
        printf("create file: %s error!\n", recover_file_path.c_str());
        return 0;
    }

    // 初始化 UUID
    std::string dirfdb_uuid(1, '\0');

    // 初始化TSK
    crack::tee_key::tee_fs_tsk tee_fs_tsk;
    crack::generate_key::tee_fs_init_tsk(tee_fs_tsk, tee_fs_ssk, dirfdb_uuid);
    crack::print_fs::print_tsk(tee_fs_tsk);

    // 获取索引文件头部
    int vers;
    auto htree_image_ptr = crack::read_fs::get_dirfdb_htree_image(fd, vers);
    // 错误处理
    if (!htree_image_ptr)
    {
        printf("get_dirfdb_htree_image error!");
        // 关闭文件
        close(fd);
        return 0;
    }
    // 读取根节点
    uint32_t idx = 0;
    auto htree_node_image_root_ptr = crack::read_fs::read_htree_node_image(fd, idx, vers);

    // 初始化FEK
    crack::tee_key::tee_fs_fek tee_fs_fek;
    std::cout << "\nStart decrypt enc_fek:\n";
    crack::print_fs::print_array_hex("ENC FEK: ", htree_image_ptr->enc_fek, sizeof(htree_image_ptr->enc_fek));
    crack::generate_key::tee_fs_init_fek(tee_fs_fek, tee_fs_tsk, *htree_image_ptr);
    crack::print_fs::print_fek(tee_fs_fek);

    // 解密元数据
    crack::print_fs::print_array_hex("ENC imeta: ", htree_image_ptr->imeta, sizeof(htree_image_ptr->imeta));
    std::cout << "\nStart decrypt imeta\n";
    auto imeta_ptr = crack::crack_fs::decrypt_imeta(tee_fs_fek, *htree_image_ptr, *htree_node_image_root_ptr);
    crack::print_fs::print_imeta(*imeta_ptr);
    

    // 计算 node_image | dirfile_entry 个数
    uint32_t node_image_cnt = imeta_ptr->meta.length / 4096;
    if (imeta_ptr->meta.length % 4096)
    {
        node_image_cnt++;
    }
    std::cout << "node_image_cnt : " << node_image_cnt << std::endl;
    uint32_t dirfile_entry_cnt = crack::read_fs::get_dirfile_entry_cnt(*imeta_ptr);
    std::cout << "dirfile_entry_cnt : " << dirfile_entry_cnt << std::endl;

    // 存储node_image_ptr的数组
    std::vector<crack::tee_fs_htree::TEE_FS_HTREE_NODE_IMAGE_PTR> node_image_ptr_vec;
    node_image_ptr_vec.emplace_back(std::move(htree_node_image_root_ptr));

    // 读取一系列node_image（从第2个节点开始读）
    crack::read_fs::get_node_images(fd, node_image_ptr_vec, node_image_cnt);

    // 根据node_image读取data_block,并保存到recover_fd
    crack::read_fs::save_data_blocks(fd, recover_fd, tee_fs_fek, *htree_image_ptr, node_image_ptr_vec);

    // 从recover_fd中读取dirfile_entrys
    std::vector<crack::tee_fs_htree::DIRFILE_ENTRY_PTR> dirfile_entrys;
    crack::read_fs::get_dirfile_entrys(recover_fd, dirfile_entrys, dirfile_entry_cnt);
    for(auto&& elem : dirfile_entrys){
        crack::print_fs::print_dirfile_entry(*elem);
    }

    // 关闭文件
    close(recover_fd);
    close(fd);

    return dirfile_entry_cnt;
}


void crack::crack_fs::crack_all_datafiles(std::string& storage_dir, std::string &recover_save_dir,crack::tee_key::tee_fs_ssk& tee_fs_ssk,uint32_t dirfile_entry_cnt){
    // 设置恢复索引文件路径
    std::string recover_index_file_name("recover_dirf.db");
    std::string recover_file_path = crack::read_fs::path_join(recover_save_dir, recover_index_file_name);
    // 读取恢复索引文件
    int recover_fd = open(recover_file_path.c_str(), O_RDONLY );
    if (recover_fd < 0)
    {
        printf("read file: %s error!\n", recover_file_path.c_str());
        return;
    }

    // 从recover_fd中读取dirfile_entrys
    std::vector<crack::tee_fs_htree::DIRFILE_ENTRY_PTR> dirfile_entrys;
    crack::read_fs::get_dirfile_entrys(recover_fd, dirfile_entrys, dirfile_entry_cnt);
    for(auto&& elem : dirfile_entrys){
        crack::print_fs::print_dirfile_entry(*elem);
    }

    // 关闭文件
    close(recover_fd);

    // 遍历索引文件，进行全部索引文件破解
    for (auto&& entry : dirfile_entrys)
    {
        // 破解单个数据文件
         
        // 获取数据文件路径
        char fname[100];
        int flen = snprintf(fname, 100, "%x", entry->file_number );
        std::string index_file_name(fname,flen);
        std::string file_path = crack::read_fs::path_join(storage_dir, index_file_name);

        printf("\ncrack file: %s \n",file_path.c_str());

        // 打开数据文件
        int fd = open(file_path.c_str(), O_RDONLY);
        if (fd < 0)
        {
            printf("open file: %s error!\n", file_path.c_str());
            return ;
        }
        else {
            printf("open file: %s success!\n", file_path.c_str());
        }

        // 设恢复数据文件路径
        std::string save_file_name((const char*) entry->oid,entry->oidlen);
        std::string save_file_path = crack::read_fs::path_join(recover_save_dir, save_file_name);
        // 创建恢复数据文件
        int save_fd = open(save_file_path.c_str(), O_RDWR| O_CREAT | O_TRUNC );
        if (save_fd < 0)
        {
            printf("create file: %s error!\n", save_file_path.c_str());
            return ;
        }
        else {
            printf("create file: %s success!\n", save_file_path.c_str());
        }

        // 初始化 UUID
        std::string datafile_uuid((const char*)&entry->uuid, sizeof(entry->uuid));

        // 初始化TSK
        crack::tee_key::tee_fs_tsk tee_fs_tsk;
        crack::generate_key::tee_fs_init_tsk(tee_fs_tsk, tee_fs_ssk, datafile_uuid);
        crack::print_fs::print_tsk(tee_fs_tsk);


         // 获取数据文件头部
        int vers;
        auto htree_image_ptr = crack::read_fs::get_datafile_htree_image(fd, vers,*entry);
        printf("vers: %d \n",vers);

        // 读取根节点node_image
        uint32_t idx = 0;
        auto htree_node_image_root_ptr = crack::read_fs::read_htree_node_image(fd, idx, vers);
        
        // 初始化FEK
        crack::tee_key::tee_fs_fek tee_fs_fek;
        std::cout << "\nStart decrypt enc_fek:\n";
        crack::print_fs::print_array_hex("ENC FEK: ", htree_image_ptr->enc_fek, sizeof(htree_image_ptr->enc_fek));
        crack::generate_key::tee_fs_init_fek(tee_fs_fek, tee_fs_tsk, *htree_image_ptr);
        crack::print_fs::print_fek(tee_fs_fek);

        // 解密元数据
        crack::print_fs::print_array_hex("ENC imeta: ", htree_image_ptr->imeta, sizeof(htree_image_ptr->imeta));
        std::cout << "\nStart decrypt imeta\n";
        auto imeta_ptr = crack::crack_fs::decrypt_imeta(tee_fs_fek, *htree_image_ptr, *htree_node_image_root_ptr);
        crack::print_fs::print_imeta(*imeta_ptr);

        // 计算node_image_cnt | 输出保存的数据长度
        uint32_t node_image_cnt = imeta_ptr->meta.length / 4096;
        if (imeta_ptr->meta.length % 4096)
        {
            node_image_cnt++;
        }
        std::cout << "node_image_cnt : " << node_image_cnt << std::endl;
        std::cout << "data_length : " << imeta_ptr->meta.length << std::endl;

        // 存储node_image_ptr的数组
        std::vector<crack::tee_fs_htree::TEE_FS_HTREE_NODE_IMAGE_PTR> node_image_ptr_vec;
        node_image_ptr_vec.emplace_back(std::move(htree_node_image_root_ptr));

        // 读取一系列node_image（从第2个节点开始读）
        crack::read_fs::get_node_images(fd, node_image_ptr_vec, node_image_cnt);

        // 根据node_image读取data_block,并保存到save_fd
        crack::read_fs::save_data_blocks(fd, save_fd , tee_fs_fek, *htree_image_ptr, node_image_ptr_vec);


        // 关闭数据文件
        close(fd);
        // 关闭恢复数据文件s
        close(save_fd);
    }
}

crack::tee_fs_htree::TEE_FS_HTREE_IMETA_PTR crack::crack_fs::decrypt_imeta(crack::tee_key::tee_fs_fek &fek, crack::tee_fs_htree::tee_fs_htree_image &image, crack::tee_fs_htree::tee_fs_htree_node_image &node_image)
{

    // 密文
    uint8_t *gcm_ct = image.imeta;
    uint32_t gcm_ct_len = sizeof(image.imeta);

    // 密钥
    uint8_t *gcm_key = fek.key;
    uint32_t gcm_key_len = sizeof(fek.key) * 8;

    // IV向量
    uint8_t *gcm_iv = image.iv;
    uint32_t gcm_iv_len = sizeof(image.iv);
    crack::print_fs::print_array_hex("iv: ", gcm_iv, gcm_iv_len);
    std::cout << "iv len:" << gcm_iv_len << std::endl;

    // 认证数据
    std::string aad((char *)node_image.hash, crack::tee_fs_htree::TEE_FS_HTREE_FEK_SIZE);
    aad.append(std::string((char *)&image.counter, sizeof(image.counter)));
    aad.append(std::string((char *)image.enc_fek, sizeof(image.enc_fek)));
    aad.append(std::string((char *)image.iv, sizeof(image.iv)));

    uint8_t *gcm_aad = (uint8_t *)&aad[0];
    uint32_t gcm_aad_len = aad.size();
    crack::print_fs::print_array_hex("aad : ", gcm_aad, gcm_aad_len);

    // 认证标签
    uint8_t *gcm_tag = image.tag;
    uint32_t gcm_tag_len = sizeof(image.tag);
    crack::print_fs::print_array_hex("tag: ", gcm_tag, gcm_tag_len);

    // 解密元数据
    crack::tee_fs_htree::TEE_FS_HTREE_IMETA_PTR imeta_ptr = std::make_unique<crack::tee_fs_htree::tee_fs_htree_imeta>();
    uint8_t *gcm_pt = (uint8_t *)imeta_ptr.get();
    uint32_t gcm_pt_len = sizeof(crack::tee_fs_htree::tee_fs_htree_imeta);

    // 进行解密操作
    crack::crypt_alg::aes_gcm_decrypt(gcm_pt, gcm_pt_len,
                                      gcm_ct, gcm_ct_len,
                                      gcm_key, gcm_key_len,
                                      gcm_iv, gcm_iv_len,
                                      gcm_aad, gcm_aad_len,
                                      gcm_tag, gcm_tag_len);

    crack::print_fs::print_array_hex("plain: ", gcm_pt, gcm_pt_len);

    return std::move(imeta_ptr);
}

void crack::crack_fs::decrypt_data_block(tee_fs_fek &fek,tee_fs_htree_image &image, tee_fs_htree_node_image &node_image, tee_fs_data_block &data_block)
{

    // 密文
    uint8_t *gcm_ct = data_block.block;
    uint32_t gcm_ct_len = sizeof(data_block.block);

    // 密钥
    uint8_t *gcm_key = fek.key;
    uint32_t gcm_key_len = sizeof(fek.key) * 8;

    // IV向量
    uint8_t *gcm_iv = node_image.iv;
    uint32_t gcm_iv_len = sizeof(node_image.iv);
    crack::print_fs::print_array_hex("iv: ", gcm_iv, gcm_iv_len);
    std::cout << "iv len:" << gcm_iv_len << std::endl;

    // 认证数据
    std::string aad;
    aad.append(std::string((char *)image.enc_fek, sizeof(image.enc_fek)));
    aad.append(std::string((char *)node_image.iv, sizeof(node_image.iv)));

    uint8_t *gcm_aad = (uint8_t *)&aad[0];
    uint32_t gcm_aad_len = aad.size();
    crack::print_fs::print_array_hex("aad : ", gcm_aad, gcm_aad_len);

    // 认证标签
    uint8_t *gcm_tag = node_image.tag;
    uint32_t gcm_tag_len = sizeof(node_image.tag);
    crack::print_fs::print_array_hex("tag: ", gcm_tag, gcm_tag_len);

    // 解密块数据
    uint8_t *gcm_pt =  data_block.block;
    uint32_t gcm_pt_len = sizeof(data_block.block);
    


    // 进行解密操作
    crack::crypt_alg::aes_gcm_decrypt(gcm_pt, gcm_pt_len,
                                      gcm_ct, gcm_ct_len,
                                      gcm_key, gcm_key_len,
                                      gcm_iv, gcm_iv_len,
                                      gcm_aad, gcm_aad_len,
                                      gcm_tag, gcm_tag_len);

    crack::print_fs::print_array_hex("plain: ", gcm_pt, gcm_pt_len);

}