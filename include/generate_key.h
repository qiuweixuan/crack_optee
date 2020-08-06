#ifndef GENERATE_KEY_H
#define GENERATE_KEY_H

#include "tee_key.h"
#include "tee_fs_htree.h"
#include <cstdio>
#include <string>
#include <memory>

namespace crack
{
    namespace generate_key
    {

        using TEE_FS_TSK_PTR_TYPE = std::unique_ptr<crack::tee_key::tee_fs_ssk>;
        // 获取HUK
        void tee_otp_get_hw_unique_key(struct crack::tee_key::tee_hw_unique_key *hwkey);

        // 获取芯片ID
        void tee_otp_get_die_id(uint8_t *buffer, size_t len);

        // 初始化key_manager -> 获取SSK
        void  tee_fs_init_key_manager(crack::tee_key::tee_fs_ssk& tee_fs_ssk);

        TEE_FS_TSK_PTR_TYPE  tee_fs_init_key_manager();

        // 获取TSK
        void tee_fs_init_tsk(crack::tee_key::tee_fs_tsk& tee_fs_tsk, crack::tee_key::tee_fs_ssk& tee_fs_ssk, std::string& uuid);

         // 获取FEK
        void tee_fs_init_fek(crack::tee_key::tee_fs_fek& tee_fs_fek, crack::tee_key::tee_fs_tsk& tee_fs_tsk, crack::tee_fs_htree::tee_fs_htree_image& image);

    } // namespace generate_key
} // namespace crack

#endif /* GENERATE_KEY_H */
