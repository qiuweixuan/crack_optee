#include "crypt_alg.h"
#include "print_fs.h"

#include <openssl/hmac.h>
#include <openssl/aes.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

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
 *@param keySize 密钥长度，决定ECB模式
 *@param length 要解密的数据的字节数
 */
int crack::crypt_alg::aes_ecb_decrypt(uint8_t *out, const uint8_t *userKey, const uint32_t keySize, const uint8_t *in,  const uint32_t length)
{
	if (!in || !userKey || !out)
		return -1;
	
	AES_KEY akey;
	//! 1. 设定AES加密用的key及密钥长度
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


/**
 *@brief AES算法的GCM模式解密算法实现
 *@param gcm_pt 解密后的数据
 *@param gcm_ct 要解密的数据
 *@param gcm_key 密钥
 *@param gcm_key_len 密钥长度，决定GCM模式
 *@param gcm_iv 初始化向量
 *@param gcm_aad 附加数据
 *@param gcm_tag 认证标签
 */
void crack::crypt_alg::aes_gcm_decrypt(uint8_t* gcm_pt,uint32_t gcm_pt_len,
						 uint8_t* gcm_ct,uint32_t gcm_ct_len,
						 uint8_t* gcm_key,uint32_t gcm_key_len,
						 uint8_t* gcm_iv, uint32_t gcm_iv_len,
						 uint8_t* gcm_aad, uint32_t gcm_aad_len,
						 uint8_t* gcm_tag, uint32_t gcm_tag_len)
{
        EVP_CIPHER_CTX *ctx;
        int outlen, tmplen, rv;
        unsigned char outbuf[gcm_ct_len];

        ctx = EVP_CIPHER_CTX_new();
        /* Select cipher */
		if(gcm_key_len == 128){
			EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL);
		}
		else if(gcm_key_len == 256){
			EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
		}
		else{
			printf("key size : %d error \n",gcm_key_len);
			return;
		}
		
        
        /* Set IV length, omit for 96 bits */
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, gcm_iv_len, NULL);
		printf("iv size : %d \n",gcm_iv_len);

        /* Specify key and IV */
        EVP_DecryptInit_ex(ctx, NULL, NULL, gcm_key, gcm_iv);

        /* Zero or more calls to specify any AAD */
        EVP_DecryptUpdate(ctx, NULL, &outlen, gcm_aad, gcm_aad_len);
        /* Decrypt plaintext */
        EVP_DecryptUpdate(ctx, outbuf, &outlen, gcm_ct, gcm_ct_len);
        
        

        /* Set expected tag value. Works in OpenSSL 1.0.1d and later */
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, gcm_tag_len, gcm_tag);
        /* Finalise: note get no output for GCM */
        rv = EVP_DecryptFinal_ex(ctx, outbuf, &outlen);

		/* copy data to pt */
		memcpy(gcm_pt,outbuf,gcm_pt_len);

        /* Print out return value. If this is not successful authentication
         * failed and plaintext is not trustworthy.
         */
        // printf("Tag Verify %s\n", rv > 0 ? "Successful!" : "Failed!");
        EVP_CIPHER_CTX_free(ctx);

 }
