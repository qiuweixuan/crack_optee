#include "generate_key.h"
#include <memory.h>
#include <cstdio>

void crack::generate_key::tee_otp_get_hw_unique_key(struct crack::tee_key::tee_hw_unique_key *hwkey) {
    memset(&hwkey->data[0], 0, sizeof(hwkey->data));
}


void crack::generate_key::tee_fs_init_key_manager() {
    struct crack::tee_key::tee_hw_unique_key huk;
	// uint8_t chip_id[crack::tee_key::TEE_FS_KM_CHIP_ID_LENGTH];
	
	// uint8_t message[sizeof(chip_id) + sizeof(string_for_ssk_gen)];

	/* Secure Storage Key Generation:
	 *
	 *     SSK = HMAC(HUK, message)
	 *     message := concatenate(chip_id, static string)
	 * */
	crack::generate_key::tee_otp_get_hw_unique_key(&huk);

    printf("HUK: ");
	for(size_t i = 0; i < sizeof(huk.data); i++)
	{
		printf(" %0x ", huk.data[i]);
	}
    printf("\n");
    return;
}