#include "Webserver.hpp"
#include <vector>


std::string		readHtml(const std::string& path)
{
    std::ofstream		file;
    std::stringstream	buffer;

//    if (pathIsFile(path))
//    {
        file.open(path.c_str(), std::ifstream::in);
        if (file.is_open() == false)
            return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");

        buffer << file.rdbuf();
        file.close();
//        _type = "text/html";
        return (buffer.str());
//    }
//    else
//        return ("<!DOCTYPE html>\n<html><title>40404</title><body>There was an error finding your error page</body></html>\n");
}

Webserver::Webserver()
{
    int rc;
    int on = 1;

   this->listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
   this->check_errors(this->listen_sd, "socket() failed");

   rc = setsockopt(this->listen_sd, SOL_SOCKET,  SO_REUSEADDR,
                   (char *)&on, sizeof(on));
   this->check_errors(rc, "setsockopt() failed", 1);

   rc = ioctl(this->listen_sd, FIONBIO, (char *)&on);
   this->check_errors(rc, "ioctl() failed", 1);

   memset(&(this->addr), 0, sizeof(this->addr));
   this->addr.sin6_family      = AF_INET6;
   memcpy(&(this->addr.sin6_addr), &in6addr_any, sizeof(in6addr_any));
   this->addr.sin6_port        = htons(SERVER_PORT);
   rc = bind(this->listen_sd,
             (struct sockaddr *)&(this->addr), sizeof(this->addr));
   this->check_errors(rc, "bind() failed", 1);

   rc = listen(this->listen_sd, 32);
   this->check_errors(rc, "listen() failed", 1);

   FD_ZERO(&(this->master_set));
   this->max_sd = this->listen_sd;
   FD_SET(this->listen_sd, &master_set);

   this->timeout.tv_sec  = 1;
   this->timeout.tv_usec = 0;

   std::cout << "Server created\n";
}

Webserver::Webserver(int port)
{
    int rc;
    int on = 1;

   this->listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
   this->check_errors(this->listen_sd, "socket() failed");

   rc = setsockopt(this->listen_sd, SOL_SOCKET,  SO_REUSEADDR,
                   (char *)&on, sizeof(on));
   this->check_errors(rc, "setsockopt() failed", 1);

   rc = ioctl(this->listen_sd, FIONBIO, (char *)&on);
   this->check_errors(rc, "ioctl() failed", 1);

   memset(&(this->addr), 0, sizeof(this->addr));
   this->addr.sin6_family      = AF_INET6;
   memcpy(&(this->addr.sin6_addr), &in6addr_any, sizeof(in6addr_any));
   this->addr.sin6_port        = htons(port);
   rc = bind(this->listen_sd,
             (struct sockaddr *)&(this->addr), sizeof(this->addr));
   this->check_errors(rc, "bind() failed", 1);

   rc = listen(this->listen_sd, 32);
   this->check_errors(rc, "listen() failed", 1);

   FD_ZERO(&(this->master_set));
   this->max_sd = this->listen_sd;
   FD_SET(this->listen_sd, &master_set);

   this->timeout.tv_sec  = 3 * 60;
   this->timeout.tv_usec = 0;

   std::cout << "Server created\n";
}

Webserver::~Webserver() 
{
    for (int i=0; i <= this->max_sd; ++i)
    {
      if (FD_ISSET(i, &(this->master_set)))
         close(i);
    }
}

void  Webserver::check_errors(int flag, std::string msg, int cls)
{
   if (flag < 0)
   {
      perror(msg.c_str());
      if (cls)
         close(this->listen_sd);
      //if (ex)
      exit(-1);
   }
}

void    Webserver::start()
{
    //int rc = 0;
    int end_server = FALSE;
    int close_conn;
    std::string	dot[3] = {".  ", ".. ", "..."};
    int			n = 0;

    while(1) {
        int rc = 0;
        while (rc == 0) {
            memcpy(&(this->working_set), &(this->master_set), sizeof(this->master_set));
            FD_ZERO(&(this->write_set));
            std::cout << "\rWaiting on select()" << dot[n++] << std::flush;
            if (n == 3)
                n = 0;
            rc = select(this->max_sd + 1, &(this->working_set), NULL, NULL, &(this->timeout));

            this->check_descriptors(rc, end_server, close_conn);

        }

        if (rc < 0) {
            perror("  select() failed");
//            FD_CLR()
            break;
        }
    }
}

void    Webserver::check_descriptors(int desc_ready,
    int& end_server, int& close_conn)
{

    for (int i=0; i <= this->max_sd  &&  desc_ready > 0; ++i)
      {
         if (FD_ISSET(i, &(this->working_set)))
         {
            desc_ready -= 1;

            if (i == this->listen_sd)
            {
               std::cout << "  Listening socket is readable\n";
               this->accept_connections(end_server);
            }
            else
            {
               std::cout << "  Descriptor " << i << " is readable\n";
               close_conn = FALSE;
               
               this->receive_data(i, close_conn);

               if (close_conn)
               {
                  close(i);
                  FD_CLR(i, &(this->master_set));
                  if (i == this->max_sd)
                  {
                     while (FD_ISSET(this->max_sd, &(this->master_set)) == FALSE)
                        this->max_sd -= 1;
                  }
               }
            } /* End of existing connection is readable */
         } /* End of if (FD_ISSET(i, &working_set)) */
      } /* End of loop through selectable descriptors */
}

void    Webserver::accept_connections(int& end_server)
{
    int new_sd = 0;
    //do
    while (new_sd != -1)
    {
        new_sd = accept(this->listen_sd, NULL, NULL);
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
        FD_SET(new_sd, &(this->master_set));
        if (new_sd > this->max_sd)
            this->max_sd = new_sd;
    }
    //} while (new_sd != -1);
}

void    Webserver::receive_data(int i, int& close_conn)
{
    int len;
    std::vector<char> buffer(5000);
    int rc;

    //do
    while (TRUE)
    {
        rc = recv(i, buffer.data(), buffer.size(), 0);
            if (rc > 0)
            {
                std::cout << "  recv() succes\n";
                //std::cout << buffer.data() << std::endl;
                //len = rc;
                //std::cout << "  " << len << " bytes received\n";
                Request req = this->parse_request(buffer.data());
                req.show();
                std::string html_text = readHtml("test.html");
                std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length:  " + std::to_string(html_text.size()) + "\n\n" + html_text; //+ readHtml("test.html");
                std::cout << "response";
                rc = send(i, response.c_str(), response.size(), 0);
                if (rc < 0)
                {
                    perror("  send() failed");
                    close_conn = TRUE;
                    break;
                }
                break;
            }
            if (rc == 0)
            {
                std::cout << "  Connection closed\n";
                close_conn = TRUE;
                break;
            }
            if (rc < 0)
            {
                if (errno != EWOULDBLOCK)
                {
                    perror("  recv() failed");
                    close_conn = TRUE;
                }
                break;
            }
        break;
    } //while (TRUE);
}

Request &Webserver::parse_request(std::string request)
{
    Request *req = new Request();
    int start = 0;
    int pos = 0;
    int npos = 0;

    if ((*req).getMethod().size() == 0)
    {
        npos = request.find_first_of("\n", start);
        std::string method_row = request.substr(start, npos - start);
        pos = method_row.find_first_of(" \t", start);
        (*req).setMethod(method_row.substr(start, pos - start));
        start = pos + 1;
        pos = method_row.find_first_of(" \t", start);
        (*req).setUrl(method_row.substr(start, pos - start));
        start = pos + 1;
        pos = method_row.find_first_of(" \t", start);
        (*req).setStatus(method_row.substr(start, pos - start));
        start = npos + 1;
        pos = 0;
        npos = 0;
    }
    while (true)
    {
        npos = request.find_first_of("\n", start);
        std::string header_row = request.substr(start, npos - start);
        if (header_row.size() == 0 || header_row.size() == 1)
            break ;
        pos = header_row.find_first_of(":", 0);
        std::string key = header_row.substr(0, pos);
        std::string value = header_row.substr(pos + 2, header_row.size() - pos - 2);
        (*req).addHeader(key, value);
        start = npos + 1;
        pos = 0;
        if (npos == -1)
            break;
        npos = 0;
    }
    if ((*req).getMethod() == "POST" && npos > 0)
    {
        start = npos + 1;
        npos = request.find_first_of("\n", start);
        std::string params_row = request.substr(start, npos - start);
        start = 0;
        if (npos != -1 || params_row.size() > 0)
        {
            while (true)
            {
                npos = params_row.find_first_of("&", start);
                std::string param = params_row.substr(start, npos - start);
                pos = param.find_first_of("=", 0);
                (*req).addParam(param.substr(0, pos), param.substr(pos + 1, param.size() - pos - 1));
                start = npos + 1;
                pos = 0;
                if (npos == params_row.size() || npos == -1)
                    break;
                npos = 0;
            }
        }
    }
    else if ((*req).getMethod() == "GET" && npos > 0)
    {
        npos = (*req).getUrl().find_first_of("?", 0);
        if (npos != -1)
        {
            std::string params_row = (*req).getUrl().substr(npos + 1, (*req).getUrl().size() - npos - 1);
            start = 0;
            while (true)
            {
                npos = params_row.find_first_of("&", start);
                std::string param = params_row.substr(start, npos - start);
                pos = param.find_first_of("=", 0);
                (*req).addParam(param.substr(0, pos), param.substr(pos + 1, param.size() - pos - 1));
                start = npos + 1;
                pos = 0;
                if (npos == params_row.size() || npos == -1)
                    break;
                npos = 0;
            }
        }
    }
    if (npos != -1)
        (*req).setBody(request.substr(npos, request.size() - npos));
    
    return *req;
}

