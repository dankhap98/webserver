#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "server.hpp"

#include "Request.hpp"
#include "Response.hpp"
#include "ConfigServer.hpp"

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
        ConfigServer    *cs;
        //std::map<int, Request> requests;
        std::map<int, Request> requests;
        //std::map<int, std::string> requests;
        std::map<int, std::string> responses;
        //std::map<int, std::string> responses;

        Webserver(const Webserver& w);
        Webserver();
        Webserver & operator=(const Webserver& w);

        void    check_errors(int flag, std::string msg, int cls = 0);

		int		send_all(int socket, const void *buffer, size_t length, int flags);
        void    accept_connections(int& end_server);
        void    receive_data(int i, int& close_conn);
        void    check_descriptors(int desc_ready, int& end_server, int& close_conn);
        Request &parse_request(std::string request);

        void    accept_connections2();//std::vector<int>& _sockets);
        int    receive_data2(int i, int& close_conn, std::vector<int>& _ready);
        void    check_descriptors2(int desc_ready, int& close_conn, std::vector<int>& _ready);//, std::vector<int>& _sockets);
        int    send_data(int i, int& close_conn);

    public:
        Webserver(ConfigServer& cs);
        //Webserver(int port);
        ~Webserver();


        void    start();

        void    start2();
};

#endif
