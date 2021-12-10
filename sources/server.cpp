//
// Created by Stoops Ysilla on 12/10/21.
//

#include "../includes/server.hpp"

int file_exist(std::string path)
{
    struct stat s;
    if (stat(path.c_str(), &s) == 0 )
    {
        if (s.st_mode & S_IFDIR)
            return 2;
        else if (s.st_mode & S_IFREG)
            return 1;
        else
            return 0;
    }
    else
        return 0;
}
