#ifndef TEE_FS_HTREE_H
#define TEE_FS_HTREE_H

#include<stdint.h>

namespace crack
{
    namespace tee_fs_htree
    {

        const uint32_t TEE_FS_HTREE_IV_SIZE	 = 	16;
        const uint32_t TEE_FS_HTREE_FEK_SIZE =  16;
        const uint32_t TEE_FS_HTREE_TAG_SIZE = 	16;

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

    } // namespace tee_fs_htree
} // namespace crack

#endif /* TEE_FS_HTREE_H */