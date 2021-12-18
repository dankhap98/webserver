//
// Created by Stoops Ysilla on 12/6/21.
//

#ifndef SERVER_HPP
#define SERVER_HPP

//#include <cstdio>
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unistd.h> // For read
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#define SERVER_PORT  8080

#define TRUE             1
#define FALSE            0

int    file_exist(std::string path);

#endif //WEBSRV_SERVER_HPP
