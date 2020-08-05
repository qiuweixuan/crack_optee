#include<iostream>
using namespace std;
#include "generate_key.h"
#include "tee_key.h"
#include "crack_fs.h"

int main(){
	// 1.构建SSK
	crack::tee_key::tee_fs_ssk tee_fs_ssk;
    crack::generate_key::tee_fs_init_key_manager(tee_fs_ssk);

	// 2.构建索引文件的TSK
	 crack::crack_fs::crack_dirfdb("/",tee_fs_ssk);

    return 0;
}