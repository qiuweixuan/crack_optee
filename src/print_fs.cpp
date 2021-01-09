#include "print_fs.h"
#include "tee_fs_htree.h"

#include <cstdio>
#include <iostream>
#include <cstring>

void crack::print_fs::print_huk(const crack::tee_key::tee_hw_unique_key& huk) {
    printf("HUK: ");
    for (size_t i = 0; i < sizeof(huk.data); i++)
    {
        printf(" %0x ", huk.data[i]);
    }
    printf("\n");
}

void crack::print_fs::print_string_for_ssk_gen(uint8_t* message,uint32_t len){
    printf("string_for_ssk_gen: ");
    for (size_t i = 0; i < len; i++)
    {
        printf("%c", message[i]);
    }
    printf("\n");
}

void crack::print_fs::print_ssk(const crack::tee_key::tee_fs_ssk& tee_fs_ssk) {
    printf("SSK: ");
    for (size_t i = 0; i < sizeof(tee_fs_ssk.key); i++)
    {
        printf(" %0x ", tee_fs_ssk.key[i]);
    }
    printf("\n");
}

void crack::print_fs::print_tsk(const crack::tee_key::tee_fs_tsk& tee_fs_tsk) {
    printf("TSK: ");
    for (size_t i = 0; i < sizeof(tee_fs_tsk.key); i++)
    {
        printf(" %0x ", tee_fs_tsk.key[i]);
    }
    printf("\n");
}

void crack::print_fs::print_fek(const crack::tee_key::tee_fs_fek& tee_fs_fek) {
    printf("FEK: ");
    for (size_t i = 0; i < sizeof(tee_fs_fek.key); i++)
    {
        printf(" %0x ", tee_fs_fek.key[i]);
    }
    printf("\n");
}

void crack::print_fs::print_imeta(const crack::tee_fs_htree::tee_fs_htree_imeta & imeta) {
    std::cout<< "length: " << imeta.meta.length << std::endl;
    std::cout<< "max_node_id: " << imeta.max_node_id << std::endl;
}

void crack::print_fs::print_array_hex(std::string pre_str,uint8_t* array, uint32_t len) {
    printf("%s",pre_str.c_str());
    for (size_t i = 0; i < len; i++)
    {
        printf("%02x", array[i]);
    }
    printf("\n");
}

void crack::print_fs::print_dirfile_entry(const crack::tee_fs_htree::dirfile_entry & entry){
    printf("dirfile_entry: \n");
    printf("UUID: %08x-%04x-%04x-",entry.uuid.timeLow,entry.uuid.timeMid,entry.uuid.timeHiAndVersion);
    crack::print_fs::print_array_hex("",(uint8_t*)entry.uuid.clockSeqAndNode,sizeof(entry.uuid.clockSeqAndNode));
    printf("oid:");
    for (uint32_t i = 0; i < entry.oidlen; i++)
    {
        printf("%c", entry.oid[i]);
    }
    printf("\n");
    crack::print_fs::print_array_hex("hash:",(uint8_t*)entry.hash,sizeof(entry.hash));
    printf("file_number: %x",entry.file_number);
}


void tee_uuid_to_octets(char *d, const crack::tee_fs_htree::TEE_UUID& s)
{
	d[0] = s.timeLow >> 24;
	d[1] = s.timeLow >> 16;
	d[2] = s.timeLow >> 8;
	d[3] = s.timeLow;
	d[4] = s.timeMid >> 8;
	d[5] = s.timeMid;
	d[6] = s.timeHiAndVersion >> 8;
	d[7] = s.timeHiAndVersion;
	memcpy(d + 8, s.clockSeqAndNode, sizeof(s.clockSeqAndNode));
}


std::string crack::print_fs::tee_uuid_to_octet_string(const crack::tee_fs_htree::TEE_UUID& uuid)
{
    char hexChars[] = "0123456789abcdef";
	char data[16];
    tee_uuid_to_octets(data,uuid);
    std::string result(32,'\0');
    for (int i = 0; i < 16; ++i)
	{
		unsigned char a = (data[i] >> 4) & 0x0f;
		unsigned char b = data[i] & 0x0f;
		result[2 * i] = hexChars[a];
		result[2 * i + 1] = hexChars[b];
	}
    return result;
}