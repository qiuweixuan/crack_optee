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

        static crack::tee_fs_htree::TEE_FS_HTREE_IMETA_PTR  decrypt_imeta(crack::tee_key::tee_fs_fek& fek, crack::tee_fs_htree::tee_fs_htree_image& image,crack::tee_fs_htree::tee_fs_htree_node_image& node_image);
    }
} // namespace crack

#endif /* CRACK_FS_H */
