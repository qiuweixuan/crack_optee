#include<iostream>
using namespace std;
#include "generate_key.h"
#include "tee_key.h"

int main(){
	// 1.构建ssk
	crack::tee_key::tee_fs_ssk tee_fs_ssk;
    crack::generate_key::tee_fs_init_key_manager(tee_fs_ssk);
    return 0;
}