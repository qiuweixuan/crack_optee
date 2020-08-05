#ifndef CRACK_FS_H
#define CRACK_FS_H

#include "tee_key.h"
#include <string>

namespace crack
{
    namespace crack_fs
    {
        void crack_dirfdb(const std::string& storage_dir, crack::tee_key::tee_fs_ssk& tee_fs_ssk);
    }
} // namespace crack

#endif /* CRACK_FS_H */
