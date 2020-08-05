#include "crack_fs.h"
#include "generate_key.h"
#include "print_fs.h"
#include <string>

/**
 * @brief 恢复索引文件内容
 * @param  storage_dir 存储加密文件的目录
 * @param tee_fs_ssk 安全存储密钥
 */
void crack::crack_fs::crack_dirfdb(char* storage_dir, crack::tee_key::tee_fs_ssk& tee_fs_ssk) {
    // 初始化 UUID
    std::string dirfdb_uuid(1,'\0');
    // 初始化TSK
    crack::tee_key::tee_fs_tsk tee_fs_tsk;
    crack::generate_key::tee_fs_init_tsk(tee_fs_tsk,tee_fs_ssk,dirfdb_uuid);
    // 打印相关信息
    crack::print_fs::print_tsk(tee_fs_tsk);

}
