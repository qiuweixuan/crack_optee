#ifndef TEE_FS_HTREE_H
#define TEE_FS_HTREE_H

#include <stdint.h>
#include <memory>

namespace crack
{
    namespace tee_fs_htree
    {
        /* 常量设置 */
        const uint32_t TEE_FS_HTREE_IV_SIZE = 16;
        const uint32_t TEE_FS_HTREE_FEK_SIZE = 16;
        const uint32_t TEE_FS_HTREE_TAG_SIZE = 16;
        const uint32_t TEE_FS_HTREE_HASH_SIZE = 32;
        const uint32_t BLOCK_SIZE = 4096;
        const uint32_t TEE_OBJECT_ID_MAX_LEN = 64;

        /*
        * This struct is not interpreted by the hash tree, it's up to the user of
        * the interface to update etc if needed.
        */
        struct tee_fs_htree_meta
        {
            uint64_t length;
        };

        /* Internal struct needed by struct tee_fs_htree_image */
        struct tee_fs_htree_imeta
        {
            struct tee_fs_htree_meta meta;
            uint32_t max_node_id;
        };

        /* Internal struct provided to let the rpc callbacks know the size if needed */
        struct tee_fs_htree_image
        {
            uint8_t iv[TEE_FS_HTREE_IV_SIZE];
            uint8_t tag[TEE_FS_HTREE_TAG_SIZE];
            uint8_t enc_fek[TEE_FS_HTREE_FEK_SIZE];
            uint8_t imeta[sizeof(struct tee_fs_htree_imeta)];
            uint32_t counter;
        };

        /**
         * enum tee_fs_htree_type - type of hash tree element
         * @TEE_FS_HTREE_TYPE_HEAD: indicates a struct tee_fs_htree_image
         * @TEE_FS_HTREE_TYPE_NODE: indicates a struct tee_fs_htree_node_image
         * @TEE_FS_HTREE_TYPE_BLOCK: indicates a data block
         */
        enum class tee_fs_htree_type
        {
            TEE_FS_HTREE_TYPE_HEAD,
            TEE_FS_HTREE_TYPE_NODE,
            TEE_FS_HTREE_TYPE_BLOCK,
        };

        /* Internal struct provided to let the rpc callbacks know the size if needed */
        struct tee_fs_htree_node_image
        {
            /* Note that calc_node_hash() depends on hash first in struct */
            uint8_t hash[TEE_FS_HTREE_HASH_SIZE];
            uint8_t iv[TEE_FS_HTREE_IV_SIZE];
            uint8_t tag[TEE_FS_HTREE_TAG_SIZE];
            uint16_t flags;
        };

        typedef struct
        {
            uint32_t timeLow;
            uint16_t timeMid;
            uint16_t timeHiAndVersion;
            uint8_t clockSeqAndNode[8];
        } TEE_UUID;

        //索引数据块为 dirfile_entry
        struct dirfile_entry
        {
            TEE_UUID uuid;
            uint8_t oid[TEE_OBJECT_ID_MAX_LEN];
            uint32_t oidlen;
            //hash树ID数组，索引ID
            uint8_t hash[TEE_FS_HTREE_HASH_SIZE];
            uint32_t file_number;
        };

        //存储数据块为 tee_fs_data_block
        struct tee_fs_data_block
        {
            uint8_t block[BLOCK_SIZE];
        };


       

        /* 智能指针设置 */
        using TEE_FS_HTREE_IMAGE_PTR = std::unique_ptr<tee_fs_htree_image>;
        using TEE_FS_HTREE_NODE_IMAGE_PTR = std::unique_ptr<tee_fs_htree_node_image>;
        using TEE_FS_HTREE_IMETA_PTR = std::unique_ptr<tee_fs_htree_imeta>;
        using TEE_FS_DATA_BLOCK_PTR = std::unique_ptr<tee_fs_data_block>;
        using DIRFILE_ENTRY_PTR = std::unique_ptr<dirfile_entry>;


    } // namespace tee_fs_htree
} // namespace crack

#endif /* TEE_FS_HTREE_H */
