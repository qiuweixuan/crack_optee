#include "generate_key.h"
#include "print_fs.h"
#include "crypt_alg.h"

#include <memory.h>
#include <memory>
#include <cstdio>
// 静态变量
static uint8_t string_for_ssk_gen[] = "ONLY_FOR_tee_fs_ssk";
/**
 *@brief 获取硬件根密钥HUK常量
 *@param hwkey 需要初始化的硬件密钥
 */

void crack::generate_key::tee_otp_get_hw_unique_key(struct crack::tee_key::tee_hw_unique_key *hwkey)
{
	memset(&hwkey->data[0], 0, sizeof(hwkey->data));
}

/**
 *@brief 获取芯片ID常量
 *@param buffer 芯片ID缓冲区
 *@param len 缓冲区长度
 */
void crack::generate_key::tee_otp_get_die_id(uint8_t *buffer, std::size_t len)
{
	static const char pattern[4] = {'B', 'E', 'E', 'F'};
	size_t i;

	for (i = 0; i < len; i++)
	{
		buffer[i] = pattern[i % 4];
	}
}

/**
 *@brief 获取安全存储密钥SSK
 */

void crack::generate_key::tee_fs_init_key_manager(crack::tee_key::tee_fs_ssk& tee_fs_ssk)
{
	struct crack::tee_key::tee_hw_unique_key huk;
	uint8_t chip_id[crack::tee_key::TEE_FS_KM_CHIP_ID_LENGTH];
	uint8_t message[sizeof(chip_id) + sizeof(string_for_ssk_gen)];

	/* Secure Storage Key Generation:
	 *
	 *     SSK = HMAC(HUK, message)
	 *     message := concatenate(chip_id, static string)
	 * */
	// 获取huk
	crack::generate_key::tee_otp_get_hw_unique_key(&huk);
	// 打印huk信息
	crack::print_fs::print_huk(huk);

	// 获取芯片ID
	crack::generate_key::tee_otp_get_die_id(chip_id, sizeof(chip_id));

	// 字符串拼接
	memcpy(message, chip_id, sizeof(chip_id));
	memcpy(message + sizeof(chip_id), string_for_ssk_gen,
			sizeof(string_for_ssk_gen));
	// 输出拼接后的字符串
	crack::print_fs::print_string_for_ssk_gen(message,sizeof(message));


	// 使用hmac_sha256计算安全存储密钥SSK
	// crack::tee_key::tee_fs_ssk tee_fs_ssk;
	

	crack::crypt_alg::hmac_sha256(tee_fs_ssk.key, sizeof(tee_fs_ssk.key),
			huk.data, sizeof(huk.data),
			message, sizeof(message));
	// 打印SSK信息
	crack::print_fs::print_ssk(tee_fs_ssk);

	return ;
}

/**
 *@brief 获取安全存储密钥SSK
 */
crack::generate_key::TEE_FS_TSK_PTR_TYPE crack::generate_key::tee_fs_init_key_manager()
{

	auto tee_fs_ssk =  std::make_unique<crack::tee_key::tee_fs_ssk>();

	struct crack::tee_key::tee_hw_unique_key huk;
	uint8_t chip_id[crack::tee_key::TEE_FS_KM_CHIP_ID_LENGTH];
	uint8_t message[sizeof(chip_id) + sizeof(string_for_ssk_gen)];

	/* Secure Storage Key Generation:
	 *
	 *     SSK = HMAC(HUK, message)
	 *     message := concatenate(chip_id, static string)
	 * */
	// 获取huk
	crack::generate_key::tee_otp_get_hw_unique_key(&huk);
	// 打印huk信息
	crack::print_fs::print_huk(huk);

	// 获取芯片ID
	crack::generate_key::tee_otp_get_die_id(chip_id, sizeof(chip_id));

	// 字符串拼接
	memcpy(message, chip_id, sizeof(chip_id));
	memcpy(message + sizeof(chip_id), string_for_ssk_gen,
			sizeof(string_for_ssk_gen));
	// 输出拼接后的字符串
	crack::print_fs::print_string_for_ssk_gen(message,sizeof(message));


	// 使用hmac_sha256计算安全存储密钥SSK
	// crack::tee_key::tee_fs_ssk tee_fs_ssk;
	

	crack::crypt_alg::hmac_sha256(tee_fs_ssk->key, sizeof(tee_fs_ssk->key),
			huk.data, sizeof(huk.data),
			message, sizeof(message));
	// 打印SSK信息
	crack::print_fs::print_ssk(*tee_fs_ssk);

	return std::move(tee_fs_ssk);
}

/**
 *@brief 获取可信应用存储密钥TSK
 *@param tee_fs_tsk 可信应用相应存储文件的密钥TSK
 *@param tee_fs_ssk 整个安全存储文件的密钥SSK
 *@param uuid 可信应用UUID
 */

void crack::generate_key::tee_fs_init_tsk(crack::tee_key::tee_fs_tsk& tee_fs_tsk, crack::tee_key::tee_fs_ssk& tee_fs_ssk, std::string& uuid){
	crack::crypt_alg::hmac_sha256( tee_fs_tsk.key, sizeof(tee_fs_tsk.key),
			tee_fs_ssk.key, sizeof(tee_fs_ssk.key),
			(const  uint8_t *)uuid.c_str(), (const uint32_t)uuid.size());
 }

 void crack::generate_key::tee_fs_init_fek(crack::tee_key::tee_fs_fek& tee_fs_fek, crack::tee_key::tee_fs_tsk& tee_fs_tsk, crack::tee_fs_htree::tee_fs_htree_image& image){
	const uint32_t key_size = sizeof(tee_fs_tsk.key) * 8;
	crack::crypt_alg::aes_ecb_decrypt(tee_fs_fek.key,tee_fs_tsk.key, key_size, image.enc_fek, sizeof(image.enc_fek));
 }