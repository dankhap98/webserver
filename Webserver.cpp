#include "Webserver.hpp"
#include <vector>

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
    int rc;
    int end_server = FALSE;
    int close_conn;
    
    //do
   while (end_server == FALSE)
   {
      memcpy(&(this->working_set), &(this->master_set), sizeof(this->master_set));

      std::cout << "Waiting on select()...\n";
      rc = select(this->max_sd + 1, &(this->working_set), NULL, NULL, &(this->timeout));

      if (rc < 0)
      {
         perror("  select() failed");
         break;
      }

      if (rc == 0)
      {
         std::cout << "  select() timed out.  End program.\n";
         break;
      }

      this->check_descriptors(rc, end_server, close_conn);

   } //while (end_server == FALSE);
}

void    Webserver::check_descriptors(int desc_ready,
    int& end_server, int& close_conn)
{
    //std::cout << "max_sd: " << this->max_sd << " " << desc_ready << "\n";
    for (int i=0; i <= this->max_sd  &&  desc_ready > 0; ++i)
    {
        //std::cout << "i: " << i << " " << this->max_sd << "\n";
        if (FD_ISSET(i, &(this->working_set)))
        {
            desc_ready -= 1;
            //std::cout << "lid: " << this->listen_sd << " " << i << "\n";
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
               //std::cout << "close: " << close_conn << "\n";
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
            //std::cout << "after max_sd: " << this->max_sd << " " << i << "\n";
        } /* End of if (FD_ISSET(i, &working_set)) */
    } /* End of loop through selectable descriptors */
    //std::cout << "why?\n";
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
    //char   buffer[800];
    char    buffer[1];
    std::string str = "";
    std::vector<std::string> request;
    int rc, rr;

    //do
    while (TRUE)
    {
        //std::cout << "there\n";
        //rc = recv(i, buffer, sizeof(buffer), 0);
        while ((rr = recv(i, buffer, sizeof(buffer), 0)) > 0)
        {
            //std::cout << "rr: " << rr << "\n";
            if (buffer[0] == '\n')
            {
                std::cout << "str: " << str << "\n";
                request.push_back(str);
                str.clear();
            }
            else
                str += buffer[0];
        }
        if (!str.empty())
            request.push_back(str);
        //std::cout << "recv: " << rr << "\n";
        /*if (rr < 0)
        {
            //std::cout << "errno: " << errno << " " << EWOULDBLOCK << "\n";
            if (errno != EWOULDBLOCK)
            {
                perror("  recv() failed");
                close_conn = TRUE;
            }
            //close_conn = TRUE;
            break;
        }
        if (rr == 0)
        {
            std::cout << "  Connection closed\n";
            close_conn = TRUE;
            break;
        }*/
        len = rr;
        std::cout << "  " << len << " bytes received\n";
        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<!DOCTYPE html><html><head><title>Webserver page</title></head><body><h1>Webserver hello!</h1><br><img src='https://i.otzovik.com/objects/b/1380000/1371129.png'></body></html>";
      
        rc = send(i, response.c_str(), response.size(), 0);
        //std::cout << "response sent\n";
        //rc = send(i, buffer, len, 0);
        if (rc < 0)
        {
            perror("  send() failed");
            close_conn = TRUE;
            break;
        }

        if (rr < 0)
        {
            //std::cout << "errno: " << errno << " " << EWOULDBLOCK << "\n";
            if (errno != EWOULDBLOCK)
            {
                perror("  recv() failed");
                close_conn = TRUE;
            }
            //close_conn = TRUE;
            break;
        }
        if (rr == 0)
        {
            std::cout << "  Connection closed\n";
            close_conn = TRUE;
            break;
        }
    } //while (TRUE);
}