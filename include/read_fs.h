#ifndef READ_FS_H
#define READ_FS_H
#include <string>
#include <stdint.h>
#include <memory>
#include <vector>
#include "tee_fs_htree.h"
#include "tee_key.h"


namespace crack
{
    namespace read_fs
    {
        using namespace crack::tee_key;
        using namespace tee_fs_htree;


        std::string path_join(std::string& dir_path,std::string file_name);

        crack::tee_fs_htree::TEE_FS_HTREE_IMAGE_PTR read_htree_image(int fd, uint8_t vers);

        crack::tee_fs_htree::TEE_FS_HTREE_NODE_IMAGE_PTR read_htree_node_image(int fd, uint32_t idx,uint8_t vers);

        crack::tee_fs_htree::TEE_FS_DATA_BLOCK_PTR read_data_block(int fd, uint32_t idx, uint8_t vers);

        crack::tee_fs_htree::TEE_FS_HTREE_IMAGE_PTR get_dirfdb_htree_image(int fd,int& vers);

        crack::tee_fs_htree::TEE_FS_HTREE_IMAGE_PTR get_datafile_htree_image(int fd,int& vers, crack::tee_fs_htree::dirfile_entry& entry);

        uint32_t get_dirfile_entry_cnt(crack::tee_fs_htree::tee_fs_htree_imeta& imeta);

        static int get_idx_from_counter(uint32_t counter0, uint32_t counter1);

        int get_offs_size(crack::tee_fs_htree::tee_fs_htree_type type, uint32_t idx,
                           uint8_t vers, uint32_t &offs, uint32_t &size);

        void get_node_images(int fd,std::vector<crack::tee_fs_htree::TEE_FS_HTREE_NODE_IMAGE_PTR>& node_image_ptr_vec,uint32_t node_image_cnt);

        void save_data_blocks(int fd, int recover_fd,tee_fs_fek &fek,tee_fs_htree_image& image,std::vector<TEE_FS_HTREE_NODE_IMAGE_PTR>& node_image_ptr_vec );
    
        void get_dirfile_entrys(int recover_fd, std::vector<crack::tee_fs_htree::DIRFILE_ENTRY_PTR>& dirfile_entry_ptr_vec ,uint32_t dirfile_entry_cnt);
    }
} // namespace crack

#endif /* READ_FS_H */
