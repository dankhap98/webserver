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

   timeout.tv_sec  = 2;
   timeout.tv_usec = 50000;

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

//    if((ioctl(listen_sd, FIONBIO, (char *)&on)) < 0)
if (fcntl(listen_sd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error using fcntl" << std::endl;
	}
//        check_errors(-1, "ioctl() failed", 1);

    memset((char *)&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(cs->getIpAddressInt());
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
        timeout.tv_sec = 1000000;
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
    //std::vector<int> _sockets;
    //std::vector<int> _ready;

    while(1) {
        int rc = 0;
        while (rc == 0) {
            memcpy(&(working_set), &(master_set), sizeof(master_set));
            FD_ZERO(&(write_set));
			//for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)
			//	FD_SET(*it, &write_set);
            std::cout << "\rWaiting on select()" << dot[n++] << std::flush;
            if (n == 3)
                n = 0;
            rc = select(max_sd + 1, &(working_set), NULL, NULL, &(timeout));

            //check_descriptors(rc, end_server, close_conn);

        }

        if (rc > 0)
            check_descriptors(rc, end_server, close_conn);

        if (rc < 0) {
            perror("  select() failed");
            break;
        }
    }
}

void    Webserver::start2()
{
    //int end_server = FALSE;
    int close_conn;
    std::string	dot[3] = {".  ", ".. ", "..."};
    int			n = 0;
    //std::vector<int> _sockets;
    std::vector<int> _ready;

    while(1) {
        int rc = 0;
        while (rc == 0) {
            memcpy(&(working_set), &(master_set), sizeof(master_set));
            FD_ZERO(&(write_set));
			for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)
				FD_SET(*it, &write_set);
            //std::cout << "before select " << _ready.size() << _sockets.size() << "\n";
            std::cout << "\rWaiting on select()" << dot[n++] << std::flush;
            if (n == 3)
                n = 0;
            rc = select(max_sd + 1, &(working_set), &(write_set), NULL, &(timeout));

            //check_descriptors(rc, end_server, close_conn);

        }

        if (rc > 0)
            check_descriptors2(rc, close_conn, _ready);
            //check_descriptors2(rc, close_conn, _ready, _sockets);

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
            } 
         } 
      }
}

void    Webserver::check_descriptors2(int desc_ready,
   int& close_conn, std::vector<int>& _ready)//, std::vector<int>& _sockets)
{
    int ret = desc_ready;
    for (std::vector<int>::iterator it = _ready.begin() ; ret && it != _ready.end() ; it++)
	{
		if (FD_ISSET(*it, &write_set))
		{
			int	ret = send_data(*it, close_conn);//_sockets[*it]->send(*it);

			if (ret == 0)
				_ready.erase(it);
			else if (ret == -1 || close_conn)
			{
                close(*it);
				FD_CLR(*it, &master_set);
				FD_CLR(*it, &working_set);
					//_sockets.erase(*it);
				_ready.erase(it);
                requests.erase(*it);
                //_sockets.erase(it);
			}
            ret = 0;
			break;
		}
	}

    /*for (std::vector<int>::iterator it = _sockets.begin() ; ret && it != _sockets.end() ; it++)
	{
		if (FD_ISSET(*it, &write_set))
		{
			std::cout << "  Descriptor " << *it << " is readable\n";
            close_conn = FALSE;
               
            ret = receive_data2(*it, close_conn, _ready);
            if (ret == 0)
            {
                close_conn = TRUE;
                break;
            }
            if (ret == -1)
            {
                FD_CLR(*it, &master_set);
				FD_CLR(*it, &working_set);
                _sockets.erase(it);
                it = _sockets.begin();*/
                /*close(i);
                FD_CLR(i, &(master_set));
                if (i == max_sd)
                {
                    while (FD_ISSET(max_sd, &(master_set)) == FALSE)
                        max_sd -= 1;
                }*/
            /*}
            ret = 0;
			break;
		}
	}

    for (int i=0; i <= max_sd  &&  ret; ++i)
    {
        if (FD_ISSET(i, &(working_set)))
        {
            desc_ready -= 1;

            if (i == listen_sd)
            {
               std::cout << "  Listening socket is readable\n";
               accept_connections2(_sockets);
            }

        }
    }*/
    
    for (int i=0; i <= max_sd  &&  ret; ++i)
    {
        if (FD_ISSET(i, &(working_set)))
        {
            desc_ready -= 1;

            if (i == listen_sd)
            {
               std::cout << "  Listening socket is readable\n";
               accept_connections2();
            }
            else
            {
               std::cout << "  Descriptor " << i << " is readable\n";
               close_conn = FALSE;
               
               ret = receive_data2(i, close_conn, _ready);
               if (ret == 0)
               {
                    close_conn = TRUE;
                    break;
               }
            }
            if (ret == -1)
            {
                FD_CLR(i, &master_set);
				FD_CLR(i, &working_set);
                /*close(i);
                FD_CLR(i, &(master_set));
                if (i == max_sd)
                {
                    while (FD_ISSET(max_sd, &(master_set)) == FALSE)
                        max_sd -= 1;
                }*/
            }
            ret = 0;
			break;
        } 
    }

    /*for (int i=0; i <= max_sd  &&  ret; ++i)//desc_ready > 0; ++i)
    {
         if (FD_ISSET(i, &(working_set)))
         {
            //desc_ready -= 1;

            if (i != listen_sd)
            {
               std::cout << "  Descriptor " << i << " is readable\n";
               close_conn = FALSE;
               
               ret = receive_data2(i, close_conn, _ready);

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
            }
        }
    }
    for (int i=0; i <= max_sd  &&  desc_ready > 0; ++i)
    {
         if (FD_ISSET(i, &(working_set)))
         {
            desc_ready -= 1;

            if (i == listen_sd)
            {
               std::cout << "  Listening socket is readable\n";
               accept_connections2();
            }
        }
    }*/
    
      /*for (std::vector<int>::iterator it = _ready.begin() ; ret && it != _ready.end() ; it++)
		{
			if (FD_ISSET(*it, &writing_set))
			{
				long	ret = _sockets[*it]->send(*it);

				if (ret == 0)
					_ready.erase(it);
				else if (ret == -1)
				{
					FD_CLR(*it, &_fd_set);
					FD_CLR(*it, &reading_set);
					_sockets.erase(*it);
					_ready.erase(it);
				}

				ret = 0;
				break;
			}
		}

			//if (ret)
			//	std::cout << "\rReceived a connection !   " << std::flush;

		for (std::vector<int>::iterator it = _sockets.begin() ; ret && it != _sockets.end() ; it++)
		{
			//long	socket = it->first;
            int socket = *it;

			if (FD_ISSET(socket, &reading_set))
			{
				long	ret = it->second->recv(socket);

				if (ret == 0)
				{
					it->second->process(socket, _config);
					_ready.push_back(socket);
				}
				else if (ret == -1)
				{
					FD_CLR(socket, &_fd_set);
					FD_CLR(socket, &reading_set);
					_sockets.erase(socket);
					it = _sockets.begin();
				}
				ret = 0;
				break;
			}
		}*/

        //if (FD_ISSET(fd, &reading_set))
		//{
			//long	socket = it->second.accept();
            //this->accept_connections();

			/*if (socket != -1)
			{
				FD_SET(socket, &_fd_set);
				_sockets.insert(std::make_pair(socket, &(it->second)));
				if (socket > _max_fd)
					_max_fd = socket;
			}
			ret = 0;
			break;*/
		//}

		/*for (std::map<long, Server>::iterator it = _servers.begin() ; ret && it != _servers.end() ; it++)
		{
			long	fd = it->first;

			if (FD_ISSET(fd, &reading_set))
			{
				long	socket = it->second.accept();

				if (socket != -1)
				{
					FD_SET(socket, &_fd_set);
					_sockets.insert(std::make_pair(socket, &(it->second)));
					if (socket > _max_fd)
						_max_fd = socket;
				}
				ret = 0;
				break;
			}
		}*/
}

void    Webserver::accept_connections2()//std::vector<int>& _sockets)
{
    int new_sd = 0;
    while (new_sd != -1)
    {
        new_sd = accept(listen_sd, NULL, NULL);
        if (new_sd < 0)
        {
            /*if (errno != EWOULDBLOCK)
            {
                perror("  accept() failed");
                end_server = TRUE;
            }
            break;*/
            break;
            //return ;
        }
        std::cout << "  New incoming connection - " << new_sd << "\n";
        FD_SET(new_sd, &(master_set));
        if (new_sd > max_sd)
            max_sd = new_sd;
        //_sockets.push_back(new_sd);
    }
}

void    Webserver::accept_connections(int& end_server)
{
    (void)end_server;
    int new_sd = 0;
    while (new_sd != -1)
    {
        new_sd = accept(listen_sd, NULL, NULL);
        if (new_sd < 0)
        {
            /*if (errno != EWOULDBLOCK)
            {
                perror("  accept() failed");
                end_server = TRUE;
            }
            break;*/
            break;
            //return ;
        }
        std::cout << "  New incoming connection - " << new_sd << "\n";
        FD_SET(new_sd, &(master_set));
        if (new_sd > max_sd)
            max_sd = new_sd;
    }
}

int   Webserver::receive_data2(int i, int& close_conn, std::vector<int>& _ready)
{
    Request request;
    std::vector<char> buffer(5000);
    int rc;

    if (requests.find(i) == requests.end())
        requests.insert(std::make_pair(i, request));
	rc = recv(i, buffer.data(), buffer.size(), 0);
    //std::cout << "rc: " << rc << "\n";
    //std::cout << "bdata: " << buffer.data() << "\n";
    if (rc > 0)
    {
        requests[i].parseRequest(buffer.data());
        requests[i].show();
        _ready.push_back(i);
        close_conn = TRUE;
    }

    /*else if (rc < 0)
    {
        if (errno != EWOULDBLOCK)
        {
            perror("  recv() failed");
            close_conn = TRUE;
        }
        close_conn = TRUE;
    }*/
    return rc;
}

int    Webserver::send_data(int i, int& close_conn)
{
    int rc = 0;
    Response response(*cs, requests[i]);
    rc = send(i, response.GetResponseMsg().c_str(), response.GetResponseMsg().size(), 0);
    if (rc < 0)
    {
        perror("  send() failed");
        close_conn = TRUE;
    }
    requests.erase(i);
    return rc;
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
    else if (rc == 0)
    {
        std::cout << "  Connection closed"<<i <<"\n";
        close_conn = TRUE;
    }
    else if (rc < 0)
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
