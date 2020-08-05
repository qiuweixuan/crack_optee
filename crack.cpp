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
	crack::crack_fs::crack_dirfdb("/", *tee_fs_ssk);

	// 3.获取索引文件路径
	std::string storage_path(argv[1]);
	std::string index_file_name("dirf.db");
	auto file_path = crack::read_fs::path_join(storage_path,index_file_name);
	

	

	

    return 0;
}