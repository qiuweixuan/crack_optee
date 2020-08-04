#include "crypt_alg.h"
#include <openssl/hmac.h>
#include <string>

/**
 *@brief hmac_sha256哈希算法实现
 *@param key 密钥
 *@param data 数据
 *@return hash 存储32字节的hmac_sha256哈希值
 */
std::string crack::crypt_alg::hmac_sha256(const std::string &key, const std::string &data)
{
	uint32_t hash_value_len = 0;
	uint32_t key_len = key.size();
	uint32_t data_len = data.size();

	// 设置
	const uint32_t HMAC_SHA256_HASH_LEN = 32;
	std::string hash(HMAC_SHA256_HASH_LEN, '\0');

#if (OPENSSL_VERSION_NUMBER >= 0x10100001L)
	HMAC_CTX *ctx;

	ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key.c_str(), key_len, EVP_sha256(), NULL);
	HMAC_Update(ctx, (unsigned char *)data.c_str(), data_len);
	HMAC_Final(ctx, (unsigned char *)hash.c_str(), &hash_value_len);
	HMAC_CTX_free(ctx);
#else
	HMAC_CTX ctx;

	HMAC_CTX_init(&ctx);
	HMAC_Init_ex(&ctx, key.c_str(), key_len, EVP_sha256(), NULL);
	HMAC_Update(&ctx, (unsigned char *)data.c_str(), data_len);
	HMAC_Final(&ctx, (unsigned char *)hash.c_str(), &hash_value_len);
	HMAC_CTX_cleanup(&ctx);
#endif
	return hash;
}
