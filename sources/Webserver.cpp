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

   timeout.tv_sec  = 0;
   timeout.tv_usec = 200;

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
    //if (fcntl(listen_sd, F_SETFL, O_NONBLOCK) == -1) {
	//	std::cerr << "Error using fcntl" << std::endl;
	//}

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

void    show_vectors(std::vector<int> _sockets, std::vector<int> _ready,
std::map<int, Request> requests)
{
    std::cerr << "\nbefore select " << _ready.size() << _sockets.size() << "\n";
    std::cerr << "\nREADY: \n";
    for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)
    {
        std::cerr << *it << "\n";
    }
    std::cerr << "SOCKETS: \n";
    for (std::vector<int>::iterator it2 = _sockets.begin() ; it2 != _sockets.end() ; it2++)
    {
        std::cerr << *it2 << "\n";
    }
    std::cerr << "END SHOW\n";

     std::cerr << "REQUEST: \n";
    for (std::map<int, Request>::iterator it3 = requests.begin() ; it3 != requests.end() ; it3++)
    {
        std::cerr << (*it3).first << "\n";
    }
    std::cerr << "END SHOW\n";
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
        //struct timeval	timeout;

        while (rc == 0) {
            //timeout.tv_sec  = 1;
			//timeout.tv_usec = 0;
            memcpy(&(working_set), &(master_set), sizeof(master_set));
            FD_ZERO(&(write_set));
			for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() ; it++)
				FD_SET(*it, &write_set);
            //std::cout << "before select " << _ready.size() << _sockets.size() << "\n";
            //show_vectors(_sockets,_ready, requests);
            std::cout << "\rWaiting on select()" << dot[n++] << std::flush;
            if (n == 3)
                n = 0;
            rc = select(max_sd + 1, &(working_set), &(write_set), NULL, &(timeout));
            //std::cerr << "select rc " << rc << "\n\n";
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
   int& close_conn, std::vector<int>& _ready)
{
    int ret = desc_ready;
    for (std::vector<int>::iterator it = _ready.begin() ; ret && it != _ready.end() ; it++)
	{
		if (FD_ISSET(*it, &write_set))
		{
			int	ret = send_data(*it, close_conn);

			if (ret == 0)
				_ready.erase(it);
			else if (ret == -1 || close_conn)
			{
                close(*it);
				FD_CLR(*it, &master_set);
				FD_CLR(*it, &working_set);
				_ready.erase(it);
                //requests.erase(*it);
			}
            ret = 0;
			break;
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
                if (i == max_sd)
                {
                    while (FD_ISSET(max_sd, &(master_set)) == FALSE)
                        max_sd -= 1;
                }
            }
            ret = 0;
			break;
        } 
    }
}

/*void    Webserver::check_descriptors2(int desc_ready,
   int& close_conn, std::vector<int>& _ready, std::vector<int>& _sockets)
{
    int ret = desc_ready;
    for (std::vector<int>::iterator it = _ready.begin() ; it != _ready.end() && ret; it++)
	{
		if (FD_ISSET(*it, &write_set))
		{
			int	ret = send_data(*it, close_conn);

            if (ret == 0)
				_ready.erase(it);
			else if (ret == -1 || close_conn)
			{
				FD_CLR(*it, &master_set);
				FD_CLR(*it, &working_set);
				_ready.erase(it);
                _sockets.erase(it);
			}
            ret = 0;
			break;
		}
	}*/
    /*for (std::vector<int>::iterator it = _sockets.begin() ; it != _sockets.end() && ret; it++)
	{
		if (FD_ISSET(*it, &working_set))
		{
			std::cout << "  Descriptor " << *it << " is readable\n";
            close_conn = FALSE;
               
            int ret = receive_data2(*it, close_conn, _ready);
            //if (ret == 0)
            //    _ready.push_back(*it);
            if (ret == -1)
            {
                FD_CLR(*it, &master_set);
				FD_CLR(*it, &working_set);
                _sockets.erase(it);
                it = _sockets.begin();
            }
            ret = 0;
			break;
		}
	}

    for (int i=0; i <= max_sd &&  desc_ready > 0; ++i)
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
    
    /*for (int i=0; i <= max_sd  &&  ret; ++i)
    {
        if (FD_ISSET(i, &(working_set)))
        {
            desc_ready -= 1;

            if (i == listen_sd)
            {
               std::cout << "  Listening socket is readable\n";
               accept_connections2(_sockets);
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
                close(i);
                FD_CLR(i, &(master_set));
                if (i == max_sd)
                {
                    while (FD_ISSET(max_sd, &(master_set)) == FALSE)
                        max_sd -= 1;
                }
            }
            ret = 0;
			break;
        } 
    }*/

//}

void    Webserver::accept_connections2()//std::vector<int>& _sockets)
{
    int new_sd = 0;
    //while (new_sd != -1)
    //{
        new_sd = accept(listen_sd, NULL, NULL);
        if (new_sd < 0)
        {
            /*if (errno != EWOULDBLOCK)
            {
                perror("  accept() failed");
                end_server = TRUE;
            }
            break;*/
            //break;
            return ;
        }
        else
        {
            fcntl(new_sd, F_SETFL, O_NONBLOCK);
            std::cout << "  New incoming connection - " << new_sd << "\n";
            FD_SET(new_sd, &(master_set));
            if (new_sd > max_sd)
                max_sd = new_sd;
            //_sockets.push_back(new_sd);
        }
   //}
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
    if (rc > 0)
    {
        requests[i].parseRequest(buffer.data());
        _ready.push_back(i);
        //requests[i].show();
        close_conn = TRUE;
    }

    if (rc <= 0)
    {
		close(i);
	    requests.erase(i);
        return (-1);
    }
    

    size_t	j = ((std::string)buffer.data()).find("\r\n\r\n");
	if (j != std::string::npos)
	{
		if (requests[i].getHeaders().count("Content-Length") == 0)
				return (0);

        size_t	len = std::atoi(requests[i].getHeader("Content-Length").c_str());
		if (requests[i].size() >= len + j + 4)
			return (0);
		else
			return (1);
	}

	return (1);
}

int    Webserver::send_data(int i, int& close_conn)
{
    //int rc = 0;
    //static std::map<int, int>  sent;
    //if (responses.find(i) == responses.end())
    //{
        Response response(*cs, requests[i]);
    //    responses.insert(std::make_pair(i, response.GetResponseMsg()));
    //    sent[i] = 0;
        requests.erase(i);
    //}
    //std::string	str = responses[i].substr(sent[i], responses[i].size() - sent[i]);
    (void)close_conn;
    //int rc = send(i, str.c_str(), str.size(), 0);
    //std::cout << "SEND STR: " << response.GetResponseMsg() << "\n";
    int rc = send(i, response.GetResponseMsg().c_str(), response.GetResponseMsg().size(), 0);
    if (rc < 0 || (requests[i].getHeaders().count("Connection")
        && requests[i].getHeader("Connection").find("keep-alive") == std::string::npos))
    {
        perror("  send() failed");
        
    //    sent[i] = 0;
    //    sent.erase(i);
		close(i);
	    //requests.erase(i);
     //   responses.erase(i);
    }
    /*else
    {
        sent[i] += rc;
        if (sent[i] >= (int)(responses[i].size()))
        {
            //responses.erase(i);
            sent[i] = 0;
            sent.erase(i);
            return (0);
        }
        else
            return (1);
    }*/
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
        if (rc < 0 || (request.getHeaders().count("Connection") && request.getHeader("Connection") != "keep-alive"))
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
