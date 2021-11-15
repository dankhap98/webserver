#include "Webserver.hpp"

Webserver::Webserver()
{
    int rc;
    int on = 1;
    /*************************************************************/
   /* Create an AF_INET6 stream socket to receive incoming      */
   /* connections on                                            */
   /*************************************************************/
   this->listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
   this->check_errors(this->listen_sd, "socket() failed");

   /*************************************************************/
   /* Allow socket descriptor to be reuseable                   */
   /*************************************************************/
   rc = setsockopt(this->listen_sd, SOL_SOCKET,  SO_REUSEADDR,
                   (char *)&on, sizeof(on));
   this->check_errors(rc, "setsockopt() failed", 1);

   /*************************************************************/
   /* Set socket to be nonblocking. All of the sockets for      */
   /* the incoming connections will also be nonblocking since   */
   /* they will inherit that state from the listening socket.   */
   /*************************************************************/
   rc = ioctl(this->listen_sd, FIONBIO, (char *)&on);
   this->check_errors(rc, "ioctl() failed", 1);

   /*************************************************************/
   /* Bind the socket                                           */
   /*************************************************************/
   memset(&(this->addr), 0, sizeof(this->addr));
   this->addr.sin6_family      = AF_INET6;
   memcpy(&(this->addr.sin6_addr), &in6addr_any, sizeof(in6addr_any));
   this->addr.sin6_port        = htons(SERVER_PORT);
   rc = bind(this->listen_sd,
             (struct sockaddr *)&(this->addr), sizeof(this->addr));
   this->check_errors(rc, "bind() failed", 1);

   /*************************************************************/
   /* Set the listen back log                                   */
   /*************************************************************/
   rc = listen(this->listen_sd, 32);
   this->check_errors(rc, "listen() failed", 1);

   /*************************************************************/
   /* Initialize the master fd_set                              */
   /*************************************************************/
   FD_ZERO(&(this->master_set));
   this->max_sd = this->listen_sd;
   FD_SET(this->listen_sd, &master_set);

   /*************************************************************/
   /* Initialize the timeval struct to 3 minutes.  If no        */
   /* activity after 3 minutes this program will end.           */
   /*************************************************************/
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
    int desc_ready;
    //int i;
    int end_server = FALSE;
    int close_conn;
    

    do
   {
      /**********************************************************/
      /* Copy the master fd_set over to the working fd_set.     */
      /**********************************************************/
      memcpy(&(this->working_set), &(this->master_set), sizeof(this->master_set));

      /**********************************************************/
      /* Call select() and wait 3 minutes for it to complete.   */
      /**********************************************************/
      std::cout << "Waiting on select()...\n";
      rc = select(this->max_sd + 1, &(this->working_set), NULL, NULL, &(this->timeout));

      /**********************************************************/
      /* Check to see if the select call failed.                */
      /**********************************************************/
      if (rc < 0)
      {
         perror("  select() failed");
         break;
      }

      /**********************************************************/
      /* Check to see if the 3 minute time out expired.         */
      /**********************************************************/
      if (rc == 0)
      {
         std::cout << "  select() timed out.  End program.\n";
         break;
      }

      /**********************************************************/
      /* One or more descriptors are readable.  Need to         */
      /* determine which ones they are.                         */
      /**********************************************************/
      //desc_ready = rc;
      this->check_descriptors(rc, end_server, close_conn);

   } while (end_server == FALSE);
}

void    Webserver::check_descriptors(int desc_ready,
    int& end_server, int& close_conn)
{
    for (int i=0; i <= this->max_sd  &&  desc_ready > 0; ++i)
      {
         /*******************************************************/
         /* Check to see if this descriptor is ready            */
         /*******************************************************/
         if (FD_ISSET(i, &(this->working_set)))
         {
            /****************************************************/
            /* A descriptor was found that was readable - one   */
            /* less has to be looked for.  This is being done   */
            /* so that we can stop looking at the working set   */
            /* once we have found all of the descriptors that   */
            /* were ready.                                      */
            /****************************************************/
            desc_ready -= 1;

            /****************************************************/
            /* Check to see if this is the listening socket     */
            /****************************************************/
            if (i == this->listen_sd)
            {
               std::cout << "  Listening socket is readable\n";
               /*************************************************/
               /* Accept all incoming connections that are      */
               /* queued up on the listening socket before we   */
               /* loop back and call select again.              */
               /*************************************************/
               this->accept_connections(end_server);
            }

            /****************************************************/
            /* This is not the listening socket, therefore an   */
            /* existing connection must be readable             */
            /****************************************************/
            else
            {
               std::cout << "  Descriptor " << i << " is readable\n";
               close_conn = FALSE;
               /*************************************************/
               /* Receive all incoming data on this socket      */
               /* before we loop back and call select again.    */
               /*************************************************/
               
                this->receive_data(i, close_conn);
               /*************************************************/
               /* If the close_conn flag was turned on, we need */
               /* to clean up this active connection.  This     */
               /* clean up process includes removing the        */
               /* descriptor from the master set and            */
               /* determining the new maximum descriptor value  */
               /* based on the bits that are still turned on in */
               /* the master set.                               */
               /*************************************************/
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
    int new_sd;
    do
    {
                  /**********************************************/
                  /* Accept each incoming connection.  If       */
                  /* accept fails with EWOULDBLOCK, then we     */
                  /* have accepted all of them.  Any other      */
                  /* failure on accept will cause us to end the */
                  /* server.                                    */
                  /**********************************************/
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

                  /**********************************************/
                  /* Add the new incoming connection to the     */
                  /* master read set                            */
                  /**********************************************/
        std::cout << "  New incoming connection - " << new_sd << "\n";
        FD_SET(new_sd, &(this->master_set));
        if (new_sd > this->max_sd)
            this->max_sd = new_sd;

                  /**********************************************/
                  /* Loop back up and accept another incoming   */
                  /* connection                                 */
                  /**********************************************/
    } while (new_sd != -1);
}

void    Webserver::receive_data(int i, int& close_conn)
{
    int len;
    char   buffer[80];
    int rc;

    do
    {
                  /**********************************************/
                  /* Receive data on this connection until the  */
                  /* recv fails with EWOULDBLOCK.  If any other */
                  /* failure occurs, we will close the          */
                  /* connection.                                */
                  /**********************************************/
        rc = recv(i, buffer, sizeof(buffer), 0);
        if (rc < 0)
        {
            if (errno != EWOULDBLOCK)
            {
                perror("  recv() failed");
                close_conn = TRUE;
            }
            break;
        }

                  /**********************************************/
                  /* Check to see if the connection has been    */
                  /* closed by the client                       */
                  /**********************************************/
        if (rc == 0)
        {
            std::cout << "  Connection closed\n";
            close_conn = TRUE;
            break;
        }

                  /**********************************************/
                  /* Data was received                          */
                  /**********************************************/
        len = rc;
        std::cout << "  " << len << " bytes received\n";

                  /**********************************************/
                  /* Echo the data back to the client           */
                  /**********************************************/
        rc = send(i, buffer, len, 0);
        if (rc < 0)
        {
            perror("  send() failed");
            close_conn = TRUE;
            break;
        }
    } while (TRUE);
}