#ifndef PRINT_FS_H
#define PRINT_FS_H

#include "tee_key.h"
#include "tee_fs_htree.h"
// #include <cstdio>

namespace crack
{
    namespace print_fs
    {
        void print_huk(const crack::tee_key::tee_hw_unique_key& huk);

        void print_string_for_ssk_gen(uint8_t* message,uint32_t len);

        void print_ssk(const crack::tee_key::tee_fs_ssk& tee_fs_ssk);
        
        void print_tsk(const crack::tee_key::tee_fs_tsk& tee_fs_tsk);

        void print_fek(const crack::tee_key::tee_fs_fek& tee_fs_fek);

        void print_imeta(const crack::tee_fs_htree::tee_fs_htree_imeta & imeta);

        void print_array_hex(std::string pre_str,uint8_t* array, uint32_t len);
    }
} // namespace crack

#endif /* PRINT_FS_H */
