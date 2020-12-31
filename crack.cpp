#include<iostream>
#include<memory>
#include<cstdio>

#include "generate_key.h"
#include "tee_key.h"
#include "crack_fs.h"
#include "print_fs.h"
#include "read_fs.h"


int main(int argc,char** argv){

	if(argc != 3){
		printf("[usage]:\n");
		printf("%s storage_path recover_save_path\n",argv[0]);
		return -1;
	}
	// 1.构建SSK
	auto tee_fs_ssk = crack::generate_key::tee_fs_init_key_manager();

	// 2. 获取破解路径目录,以及恢复后保存路径
	std::string storage_path(argv[1]);
	std::string recover_save_path(argv[2]);

	// 3.破解索引文件
	uint32_t dirfile_entry_cnt = crack::crack_fs::crack_dirfdb(storage_path, recover_save_path, *tee_fs_ssk);

	// 4.破解所有的数据文件
	crack::crack_fs::crack_all_datafiles(storage_path, recover_save_path, *tee_fs_ssk,dirfile_entry_cnt);

    return 0;
}