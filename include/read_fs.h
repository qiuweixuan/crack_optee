#ifndef READ_FS_H
#define READ_FS_H
#include <string>
#include <stdint.h>
#include <memory>
#include "tee_fs_htree.h"


namespace crack
{
    namespace read_fs
    {
        
        std::string path_join(std::string& dir_path,std::string file_name);

        int get_offs_size(crack::tee_fs_htree::tee_fs_htree_type type, uint32_t idx,
                           uint8_t vers, uint32_t &offs, uint32_t &size);
        crack::tee_fs_htree::TEE_FS_HTREE_IMAGE_PTR read_htree_image(int fd, uint8_t vers);

        crack::tee_fs_htree::TEE_FS_HTREE_NODE_IMAGE_PTR read_htree_node_image(int fd, uint32_t idx,uint8_t vers);

        static int get_idx_from_counter(uint32_t counter0, uint32_t counter1);

        crack::tee_fs_htree::TEE_FS_HTREE_IMAGE_PTR get_dirfdb_htree_image(int fd,int& vers);


    }
} // namespace crack

#endif /* READ_FS_H */
