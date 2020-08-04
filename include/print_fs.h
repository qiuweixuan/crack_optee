#ifndef PRINT_FS_H
#define PRINT_FS_H

#include "tee_key.h"
// #include <cstdio>

namespace crack
{
    namespace print_fs
    {
        void print_huk(crack::tee_key::tee_hw_unique_key& huk);

        void print_string_for_ssk_gen(uint8_t* message,uint32_t len);

        void print_ssk(crack::tee_key::tee_fs_ssk& tee_fs_ssk);
        
    }
} // namespace crack

#endif /* PRINT_FS_H */
