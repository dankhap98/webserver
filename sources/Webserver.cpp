#include "../includes/Webserver.hpp"
#include <vector>


std::string		readHtml(const std::string& path)
{
    std::ofstream		file;
    std::stringstream	buffer;

    file.open(path.c_str(), std::ifstream::in);
    if (file.is_open() == false)
        return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");

    buffer << file.rdbuf();
    file.close();
    return (buffer.str());
}

Webserver::Webserver()
{
    int rc;
    int on = 1;

   listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
   check_errors(listen_sd, "socket() failed");

   rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
                   (char *)&on, sizeof(on));
   check_errors(rc, "setsockopt() failed", 1);

   rc = ioctl(listen_sd, FIONBIO, (char *)&on);
   check_errors(rc, "ioctl() failed", 1);

   memset(&(addr), 0, sizeof(addr));
   addr.sin6_family      = AF_INET6;
   memcpy(&(addr.sin6_addr), &in6addr_any, sizeof(in6addr_any));
   addr.sin6_port        = htons(SERVER_PORT);
   rc = bind(listen_sd,
             (struct sockaddr *)&(addr), sizeof(addr));
   check_errors(rc, "bind() failed", 1);

   rc = listen(listen_sd, 32);
   check_errors(rc, "listen() failed", 1);

   FD_ZERO(&(master_set));
   max_sd = listen_sd;
   FD_SET(listen_sd, &master_set);

   timeout.tv_sec  = 14;
   timeout.tv_usec = 0;

   std::cout << "Server created\n";
}

Webserver::Webserver(ConfigServer& conf_s)
{
    int on = 1;
    this->cs = new ConfigServer(conf_s);
    int port= cs->getPort();

    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    check_errors(listen_sd, "socket() failed");

    if((setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
        check_errors(-1, "setsockopt() failed", 1);

    if((ioctl(listen_sd, FIONBIO, (char *)&on)) < 0)
        check_errors(-1, "ioctl() failed", 1);

    memset((char *)&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    //_addr.sin_addr.s_addr = cs->getIpAddressInt();
    _addr.sin_addr.s_addr = htonl(cs->getIpAddressInt());
    //_addr.sin_addr.s_addr = INADDR_ANY;
    //_addr.sin_addr.s_addr = htonl(cs->getConfig()[0].props["server_name"].c_str());
    _addr.sin_port = htons(port);
    if (bind(listen_sd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
    {
        std::cerr << "Could not bind port " << port << "." << std::endl;
        exit(-1);
    }
    else if (listen(listen_sd, 1000) == -1)
    {
        std::cerr << "Could not listen." << std::endl;
        exit(-1);
    }
    else {
        FD_ZERO(&(master_set));
        max_sd = listen_sd;
        FD_SET(listen_sd, &master_set);
        std::cout << "Sever created!";
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
    }
}

Webserver::~Webserver() 
{
    for (int i=0; i <= max_sd; ++i)
    {
      if (FD_ISSET(i, &(master_set)))
         close(i);
    }
}

void  Webserver::check_errors(int flag, std::string msg, int cls)
{
   if (flag < 0)
   {
      perror(msg.c_str());
      if (cls)
         close(listen_sd);
      exit(-1);
   }
}

void    Webserver::start()
{
    int end_server = FALSE;
    int close_conn;
    std::string	dot[3] = {".  ", ".. ", "..."};
    int			n = 0;

    while(1) {
        int rc = 0;
        while (rc == 0) {
            memcpy(&(working_set), &(master_set), sizeof(master_set));
            FD_ZERO(&(write_set));
            std::cout << "\rWaiting on select()" << dot[n++] << std::flush;
            if (n == 3)
                n = 0;
            rc = select(max_sd + 1, &(working_set), NULL, NULL, &(timeout));

            check_descriptors(rc, end_server, close_conn);

        }

        if (rc < 0) {
            perror("  select() failed");
            break;
        }
    }
}

void    Webserver::check_descriptors(int desc_ready,
    int& end_server, int& close_conn)
{

    for (int i=0; i <= max_sd  &&  desc_ready > 0; ++i)
      {
         if (FD_ISSET(i, &(working_set)))
         {
            desc_ready -= 1;

            if (i == listen_sd)
            {
               std::cout << "  Listening socket is readable\n";
               accept_connections(end_server);
            }
            else
            {
               std::cout << "  Descriptor " << i << " is readable\n";
               close_conn = FALSE;
               
               receive_data(i, close_conn);

               if (close_conn)
               {
                  close(i);
                  FD_CLR(i, &(master_set));
                  if (i == max_sd)
                  {
                     while (FD_ISSET(max_sd, &(master_set)) == FALSE)
                        max_sd -= 1;
                  }
               }
            } /* End of existing connection is readable */
         } /* End of if (FD_ISSET(i, &working_set)) */
      } /* End of loop through selectable descriptors */
}

void    Webserver::accept_connections(int& end_server)
{
    int new_sd = 0;
    while (new_sd != -1)
    {
        new_sd = accept(listen_sd, NULL, NULL);
        if (new_sd < 0)
        {
            if (errno != EWOULDBLOCK)
            {
                perror("  accept() failed");
                end_server = TRUE;
            }
            break;
        }
        std::cout << "  New incoming connection - " << new_sd << "\n";
        FD_SET(new_sd, &(master_set));
        if (new_sd > max_sd)
            max_sd = new_sd;
    }
}

void    Webserver::receive_data(int i, int& close_conn)
{
    Request request;
    std::vector<char> buffer(5000);
    int rc;

	rc = recv(i, buffer.data(), buffer.size(), 0);

    if (rc > 0)
    {
        std::cout << "  recv() succes\n";
        request.parseRequest(buffer.data());
        Response response(*cs, request);
        rc = send(i, response.GetResponseMsg().c_str(), response.GetResponseMsg().size(), 0);
        if (rc < 0)
        {
            perror("  send() failed");
            close_conn = TRUE;
        }
    }
    if (rc == 0)
    {
        std::cout << "  Connection closed\n";
        close_conn = TRUE;
    }
    if (rc < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            perror("  recv() failed");
            close_conn = TRUE;
        }
    }
}

int		Webserver::send_all(int socket, const void *buffer, size_t length, int flags)
{
	size_t ret, bytes = 0;
	while (bytes < length) {
		std::cout<<"SEND\n";
		if (ret <= 0)
			return -1;
		ret = send(socket, (char *)buffer+bytes, length-bytes, flags);
		//check for errors
		bytes+=ret;
	}
	return (0);
}
//	ssize_t n;
//	const char *p = (char *)buffer;
//	while (length > 0)
//	{
//		n = send(socket, p, length, flags);
//		if (n <= 0)
//			return -1;
//		p += n;
//		length -= n;
//	}
//	return 0;
//}
