#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <fstream>
#include <sstream>
#include <vector>
#include <unistd.h> // For read
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include "Request.hpp"
#include "Configuration.hpp"

#define SERVER_PORT  8080

#define TRUE             1
#define FALSE            0

class Webserver
{
    private:
        int     listen_sd;
        int     max_sd;
        struct sockaddr_in	_addr;
        struct sockaddr_in6 addr;
        struct timeval timeout;
        fd_set  write_set;
        fd_set  master_set;
        fd_set  working_set;

        Webserver(const Webserver& w);
        Webserver();
        Webserver & operator=(const Webserver& w);

        void    check_errors(int flag, std::string msg, int cls = 0);

        void    accept_connections(int& end_server);
        void    receive_data(int i, int& close_conn);
        void    check_descriptors(int desc_ready, int& end_server, int& close_conn);
        Request &parse_request(std::string request);

    public:
        Webserver(int port);
        ~Webserver();

        void    start();

};

#endif