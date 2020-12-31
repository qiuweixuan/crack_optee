#ifndef CRACK_FS_H
#define CRACK_FS_H

#include "tee_key.h"
#include "tee_fs_htree.h"
#include "read_fs.h"
#include <string>

namespace crack
{
    namespace crack_fs
    {
        using namespace crack::tee_key;
        using namespace crack::tee_fs_htree;
        
        uint32_t crack_dirfdb(std::string& storage_dir, std::string &recover_save_dir,crack::tee_key::tee_fs_ssk& tee_fs_ssk);

        void crack_all_datafiles(std::string& storage_dir, std::string &recover_save_dir,crack::tee_key::tee_fs_ssk& tee_fs_ssk,uint32_t dirfile_entry_cnt);

        crack::tee_fs_htree::TEE_FS_HTREE_IMETA_PTR  decrypt_imeta(crack::tee_key::tee_fs_fek& fek, crack::tee_fs_htree::tee_fs_htree_image& image,crack::tee_fs_htree::tee_fs_htree_node_image& node_image);
   
        void decrypt_data_block(tee_fs_fek &fek,tee_fs_htree_image& image,tee_fs_htree_node_image &node_image, tee_fs_data_block &data_block);
   
    }  
} // namespace crack

#endif /* CRACK_FS_H */
