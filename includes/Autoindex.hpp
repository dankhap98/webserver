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
#include <algorithm>
#include <vector>

std::string autoindex(std::string url, int root);
bool find_str_in_vector(const std::vector<std::string>& source, std::string key);

#endif //WEBSERV_AUTOINDEX_HPP
