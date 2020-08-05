#include "read_fs.h"
#include <climits>
#include <cstdlib>
#include <iostream>

std::string crack::read_fs::path_join(std::string &dir_path, std::string file_name)
{   
    char buf[PATH_MAX + 1];
    char *res = realpath(dir_path.c_str(), buf);
    std::string abs_dir_path(res);
    std::string abs_file_path = abs_dir_path.append("/").append(file_name);

    return abs_file_path;
}
