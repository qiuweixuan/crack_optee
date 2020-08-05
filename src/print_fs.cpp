#include "print_fs.h"
#include <cstdio>


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