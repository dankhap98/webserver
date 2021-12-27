//
// Created by Nigellus Cammy on 12/20/21.
//

#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP

#include <dirent.h>
#include <iostream>
#include <string>
#include <ctime>
#include <sys/stat.h>

std::string autoindex(std::string url, int root);

#endif //WEBSERV_AUTOINDEX_HPP
