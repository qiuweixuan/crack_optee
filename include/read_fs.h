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
        

        using TEE_FS_HTREE_IMAGE_PTR = std::unique_ptr<crack::tee_fs_htree::tee_fs_htree_image>;
        using TEE_FS_HTREE_NODE_IMAGE_PTR = std::unique_ptr<crack::tee_fs_htree::tee_fs_htree_node_image>;
        using TEE_FS_HTREE_IMAGE_PTR =  std::unique_ptr<crack::tee_fs_htree::tee_fs_htree_image>;

        std::string path_join(std::string& dir_path,std::string file_name);
        int get_offs_size(crack::tee_fs_htree::tee_fs_htree_type type, uint32_t idx,
                           uint8_t vers, uint32_t &offs, uint32_t &size);

        TEE_FS_HTREE_IMAGE_PTR read_htree_image(int fd, uint8_t vers);

    }
} // namespace crack

#endif /* READ_FS_H */
