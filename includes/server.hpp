//
// Created by Stoops Ysilla on 12/6/21.
//

#ifndef WEBSRV_SERVER_HPP
#define WEBSRV_SERVER_HPP

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
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT  8080

#define TRUE             1
#define FALSE            0

#endif //WEBSRV_SERVER_HPP
