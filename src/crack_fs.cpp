#include "crack_fs.h"
#include "generate_key.h"
#include "print_fs.h"
#include "read_fs.h"

#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

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
    // 打印相关信息
    crack::print_fs::print_tsk(tee_fs_tsk);

    // 打开文件
    int fd = open(file_path.c_str(),O_RDONLY);
    if(fd < 0){
        printf("open file: %s error!\n",file_path.c_str());
        return ; 
    }

    // 获取索引文件头部
    int vers;
    auto htree_image_ptr = crack::crack_fs::get_dirfdb_htree_image(fd,vers);
    // 错误处理
    if(!htree_image_ptr){
        printf("get_dirfdb_htree_image error!");
        // 关闭文件
        close(fd);
        return;
    }

    // 初始化TSK
    crack::tee_key::tee_fs_fek tee_fs_fek;
    crack::generate_key::tee_fs_init_fek(tee_fs_fek, tee_fs_tsk, *htree_image_ptr);
    // 打印相关信息
    crack::print_fs::print_fek(tee_fs_fek);


   
    


    // 关闭文件
    close(fd);
}


static int crack::crack_fs::get_idx_from_counter(uint32_t counter0, uint32_t counter1)
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

static crack::read_fs::TEE_FS_HTREE_IMAGE_PTR crack::crack_fs::get_dirfdb_htree_image(int fd,int& vers){
     // 读取第一个htree_image
    auto htree_image_ptr_0 = crack::read_fs::read_htree_image(fd,0);
    // 读取第二个htree_image
    auto htree_image_ptr_1 = crack::read_fs::read_htree_image(fd,1);

    // 判断是否有错误
    if(!htree_image_ptr_0 || !htree_image_ptr_1){
        return nullptr;
    }

    // 获取版本vers
    vers = crack::crack_fs::get_idx_from_counter(htree_image_ptr_0->counter,
    htree_image_ptr_1->counter);
    printf("vers: %d \n",vers);

    return vers == 0? std::move(htree_image_ptr_0) : std::move(htree_image_ptr_1);
}