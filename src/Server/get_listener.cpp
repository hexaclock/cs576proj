#include "server.h"

/*
 *given a port number
 *creates listen socket on port, returns a socket 
 file descriptor
 */
int get_listener(unsigned short portnum)
{
  struct sockaddr_in srv;
  int sockfd;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&srv, 0, sizeof(srv));
  srv.sin_addr.s_addr = INADDR_ANY;
  srv.sin_family = AF_INET;
  srv.sin_port = htons(portnum);

  if (bind(sockfd, (struct sockaddr *)&srv,sizeof(srv)) < 0)
    panic("Can't bind to port!\n",3);
  listen(sockfd,5);
  return sockfd;
}
