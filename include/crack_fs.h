#ifndef CRACK_FS_H
#define CRACK_FS_H

#include "tee_key.h"
#include <string>

namespace crack
{
    namespace crack_fs
    {
        void crack_dirfdb(const char* storage_dir, crack::tee_key::tee_fs_ssk& tee_fs_ssk);

        static int get_idx_from_counter(uint32_t counter0, uint32_t counter1);
    
    }
} // namespace crack

#endif /* CRACK_FS_H */
