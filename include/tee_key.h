#ifndef TEE_KEY_H
#define TEE_KEY_H

#include <stdint.h>
#include <vector>
namespace crack
{
    namespace tee_key
    {
        // tee HUK 长度
        const uint32_t HW_UNIQUE_KEY_LENGTH = 16;

        /**
         * @brief tee HUK 结构体
         * @property data HUK数据
         */
        struct tee_hw_unique_key
        {
            uint8_t data[HW_UNIQUE_KEY_LENGTH];
        };

        // tee SSK 长度
        const uint32_t TEE_FS_KM_SSK_SIZE = 32;

        /**
         * @brief tee SSK 结构体
         * @property key SSK数据
         */
        struct tee_fs_ssk
        {
            uint8_t key[TEE_FS_KM_SSK_SIZE];
        };

        // 芯片ID长度
        const uint32_t  TEE_FS_KM_CHIP_ID_LENGTH = 32;

    } // namespace tee_key
} // namespace crack

#endif /* TEE_KEY_H */
