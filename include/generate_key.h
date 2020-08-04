#ifndef GENERATE_KEY_H
#define GENERATE_KEY_H

#include "tee_key.h"
#include <cstdio>

namespace crack
{
    namespace generate_key
    {
        // 获取HUK
        void tee_otp_get_hw_unique_key(struct crack::tee_key::tee_hw_unique_key *hwkey);

        // 获取芯片ID
        void tee_otp_get_die_id(uint8_t *buffer, size_t len);

        // 初始化key_manager -> 获取ssk
        void tee_fs_init_key_manager();

        



    } // namespace generate_key
} // namespace crack

#endif /* GENERATE_KEY_H */
