#ifndef CRACK_FS_H
#define CRACK_FS_H

#include "tee_key.h"
#include "read_fs.h"
#include <string>

namespace crack
{
    namespace crack_fs
    {
        void crack_dirfdb(std::string& storage_dir, crack::tee_key::tee_fs_ssk& tee_fs_ssk);

        static int get_idx_from_counter(uint32_t counter0, uint32_t counter1);

        static crack::read_fs::TEE_FS_HTREE_IMAGE_PTR get_dirfdb_htree_image(int fd,int& vers);
    }
} // namespace crack

#endif /* CRACK_FS_H */
