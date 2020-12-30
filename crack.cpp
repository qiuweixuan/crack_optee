#include<iostream>
#include<memory>
#include<cstdio>

#include "generate_key.h"
#include "tee_key.h"
#include "crack_fs.h"
#include "print_fs.h"
#include "read_fs.h"


int main(int argc,char** argv){

	if(argc != 2){
		printf("[usage]:\n");
		printf("%s storage_path\n",argv[0]);
		return -1;
	}
	// 1.构建SSK
	auto tee_fs_ssk = crack::generate_key::tee_fs_init_key_manager();

	// 2.构建索引文件的TSK
	std::string storage_path(argv[1]);

	// 3.破解索引文件
	crack::crack_fs::crack_dirfdb(storage_path, *tee_fs_ssk);

    return 0;
}