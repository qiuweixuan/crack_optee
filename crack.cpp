#include<iostream>
#include<memory>

#include "generate_key.h"
#include "tee_key.h"
#include "crack_fs.h"
#include "print_fs.h"


int main(){
	// 1.构建SSK
	auto tee_fs_ssk = crack::generate_key::tee_fs_init_key_manager();

	// 2.构建索引文件的TSK
	crack::crack_fs::crack_dirfdb("/", *tee_fs_ssk);

	

	

    return 0;
}