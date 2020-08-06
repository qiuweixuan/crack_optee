#ifndef CRYPT_ALG_H
#define CRYPT_ALG_H

#include <string>

namespace crack
{
    namespace crypt_alg
    {
        std::string hmac_sha256(const std::string &key, const std::string &data);

        void hmac_sha256(uint8_t *hash, uint32_t hash_len, const uint8_t *key, const uint32_t key_len, const uint8_t *data, const uint32_t data_len);

        int aes_ecb_decrypt(uint8_t *out, const uint8_t *userKey, const uint32_t keySize, const uint8_t *in,  const uint32_t length);
    } // namespace crypt_alg

} // namespace crack

#endif /* CRYPT_ALG_H */
