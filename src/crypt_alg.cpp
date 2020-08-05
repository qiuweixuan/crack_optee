#include "crypt_alg.h"
#include <openssl/hmac.h>
#include <openssl/aes.h>
#include <string>
#include <memory.h>

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


/**
 *@brief hmac_sha256加密算法实现
 *@param hash 存储32字节的hmac_sha256哈希值
 *@param key 密钥
 *@param key_len 密钥长度
 *@param data 数据
 *@param data_len 数据长度
 */
void crack::crypt_alg::hmac_sha256(uint8_t *hash, uint32_t hash_len, const uint8_t *key, const uint32_t key_len,const  uint8_t *data, const uint32_t data_len)
{
	
#if (OPENSSL_VERSION_NUMBER >= 0x10100001L)     
	HMAC_CTX *ctx;

	ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key, key_len, EVP_sha256(), NULL);
	HMAC_Update(ctx, data, data_len);
	HMAC_Final(ctx, hash, &hash_len);
	HMAC_CTX_free(ctx);
#else
	HMAC_CTX ctx;

	HMAC_CTX_init(&ctx);
  HMAC_Init_ex(&ctx, key, key_len, EVP_sha256(), NULL);
	HMAC_Update(&ctx, data, data_len);
	HMAC_Final(&ctx, hash, &hash_len);
  HMAC_CTX_cleanup(&ctx);
#endif
    
}


/**
 *@brief AES算法的ECB模式解密算法实现
 *@param in 要解密的数据
 *@param out 解密后的数据
 *@param userKey 用户密钥
 *@param length 要解密的数据的字节数
 */
int aes_ecb_decrypt(const unsigned char *in, unsigned char *out, const unsigned char *userKey, const int length, const int keySize)
{
	if (!in || !userKey || !out)
		return -1;
	
	AES_KEY akey;
	//! 1. 设定AES加密用的key及密钥长度
	memset(&akey, 0, sizeof(AES_KEY));
	
	/* if (AES_set_encrypt_key(userKey, 128, &akey) < 0)
	{
		printf("ecb: AES_set_encrypt_key failed!\n");
		return -1;
	} */
	memset(&akey, 0, sizeof(AES_KEY));
	if(keySize == 128) 
	{
		if(AES_set_decrypt_key(userKey, 128, &akey) < 0) 
		{
			printf("cbc: AES_set_encrypt_key failed!\n");
			return -1;
		}
	}
	else if (keySize == 256) 
	{
		if(AES_set_decrypt_key(userKey, 256, &akey) < 0) 
		{
			printf("cbc: AES_set_encrypt_key failed!\n");
			return -1;
		}
	}
	else
	{
		printf("keySize = %d is error!\n", keySize);
		return -1;
	}
	
	int len = 0;
	//! 2. 一次加密一个AES块
	while(len < length)
	{
		AES_ecb_encrypt(in + len, out + len, &akey, AES_DECRYPT);
		len += 16;
	}
	return 0;
}