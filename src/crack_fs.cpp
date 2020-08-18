#include "crack_fs.h"
#include "generate_key.h"
#include "print_fs.h"
#include "read_fs.h"
#include "crypt_alg.h"

#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <iostream>

/**
 * @brief 恢复索引文件内容
 * @param  storage_dir 存储加密文件的目录
 * @param tee_fs_ssk 安全存储密钥
 */
void crack::crack_fs::crack_dirfdb(std::string& storage_dir, crack::tee_key::tee_fs_ssk& tee_fs_ssk) {
    // 初始化 UUID
    std::string dirfdb_uuid(1,'\0');
    // 获取索引文件路径
    std::string index_file_name("dirf.db");
	std::string file_path = crack::read_fs::path_join(storage_dir,index_file_name);

    // 初始化TSK
    crack::tee_key::tee_fs_tsk tee_fs_tsk;
    crack::generate_key::tee_fs_init_tsk(tee_fs_tsk,tee_fs_ssk,dirfdb_uuid);
    crack::print_fs::print_tsk(tee_fs_tsk);

    // 打开文件
    int fd = open(file_path.c_str(),O_RDONLY);
    if(fd < 0){
        printf("open file: %s error!\n",file_path.c_str());
        return ; 
    }

    // 获取索引文件头部
    int vers;
    auto htree_image_ptr = crack::read_fs::get_dirfdb_htree_image(fd,vers);
    // 错误处理
    if(!htree_image_ptr){
        printf("get_dirfdb_htree_image error!");
        // 关闭文件
        close(fd);
        return;
    }
    // 读取根节点
    uint32_t idx = 0;
    auto htree_node_image_root_ptr  =  crack::read_fs::read_htree_node_image(fd, idx , vers);

    // 初始化FEK
    crack::tee_key::tee_fs_fek tee_fs_fek;
    crack::print_fs::print_array_hex("ENC FEK: ",htree_image_ptr->enc_fek,sizeof(htree_image_ptr->enc_fek));
    crack::generate_key::tee_fs_init_fek(tee_fs_fek, tee_fs_tsk, *htree_image_ptr);
    crack::print_fs::print_fek(tee_fs_fek);

    // 解密元数据
    crack::print_fs::print_array_hex("ENC imeta: ",htree_image_ptr->imeta,sizeof(htree_image_ptr->imeta));
    auto imeta_ptr = crack::crack_fs::decrypt_imeta(tee_fs_fek,*htree_image_ptr,*htree_node_image_root_ptr);
    

    crack::print_fs::print_imeta(*imeta_ptr);
    // crack::print_fs::print_array_hex("imeta: ",(uint8_t*)imeta_ptr.get(),sizeof(crack::tee_fs_htree::tee_fs_htree_imeta));

    // 计算 node_image | dirfile_entry 个数
    uint32_t node_image_cnt =  imeta_ptr->meta.length / 4096;
    if(imeta_ptr->meta.length % 4096){
        node_image_cnt++;
    }
    std::cout<<"node_image_cnt : "<<  node_image_cnt << std::endl;
    uint32_t dirfile_entry_cnt =  crack::read_fs::get_dirfile_entry_cnt(*imeta_ptr);
    std::cout<<"dirfile_entry_cnt : "<< dirfile_entry_cnt << std::endl;



    // 存储node_image_ptr
    std::vector<crack::tee_fs_htree::TEE_FS_HTREE_NODE_IMAGE_PTR> node_image_ptr_vec;
    node_image_ptr_vec.emplace_back(std::move(htree_node_image_root_ptr));

    // 读取一系列node_image
    crack::read_fs::get_node_images(fd,node_image_ptr_vec,node_image_cnt);

    // 关闭文件
    close(fd);
}






static crack::tee_fs_htree::TEE_FS_HTREE_IMETA_PTR  crack::crack_fs::decrypt_imeta(crack::tee_key::tee_fs_fek& fek, crack::tee_fs_htree::tee_fs_htree_image& image,crack::tee_fs_htree::tee_fs_htree_node_image& node_image){
   
   
    // 密文
    uint8_t* gcm_ct = image.imeta;
    uint32_t gcm_ct_len = sizeof(image.imeta);

    // 密钥
    uint8_t* gcm_key = fek.key;
    uint32_t gcm_key_len = sizeof(fek.key) * 8;

    // 初试话向量
    uint8_t* gcm_iv = image.iv;
    uint32_t gcm_iv_len = sizeof(image.iv);

    // 认证数据
    std::string aad((char*)node_image.hash,sizeof(node_image.hash));
    aad.append(std::string((char*)&image.counter,sizeof(image.counter)));
    aad.append(std::string((char*)image.enc_fek,sizeof(image.enc_fek)));
    aad.append(std::string((char*)image.iv,sizeof(image.iv)));

    uint8_t* gcm_aad = (uint8_t*)&aad[0];
    uint32_t gcm_aad_len = aad.size();

    // 认证标签
    uint8_t* gcm_tag = image.tag;
    uint32_t gcm_tag_len = sizeof(image.tag);


    // 解密元数据
    crack::tee_fs_htree::TEE_FS_HTREE_IMETA_PTR  imeta_ptr = std::make_unique<crack::tee_fs_htree::tee_fs_htree_imeta>();
    uint8_t* gcm_pt = (uint8_t* )imeta_ptr.get();
    uint32_t gcm_pt_len = sizeof(crack::tee_fs_htree::tee_fs_htree_imeta);

    // 进行解密操作
    crack::crypt_alg::aes_gcm_decrypt(gcm_pt,gcm_pt_len,
						 gcm_ct,gcm_ct_len,
						 gcm_key,gcm_key_len,
						 gcm_iv, gcm_iv_len,
						 gcm_aad, gcm_aad_len,
						gcm_tag, gcm_tag_len);

    return std::move(imeta_ptr);
}