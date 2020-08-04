#ifndef CRYPT_ALG_H
#define CRYPT_ALG_H

#include <string>

namespace crack
{
    namespace crypt_alg
    {
        std::string hmac_sha256(const std::string &key, const std::string &data);
    }

} // namespace crack

#endif /* CRYPT_ALG_H */
