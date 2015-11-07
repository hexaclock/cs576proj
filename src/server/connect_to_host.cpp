#include "server.h"

/*
 *given a hostname and port number, tries to connect to host:port
 *returns a socket file descriptor on success
*/
int connect_to_host(char *hostname, unsigned short port)
{
  struct sockaddr_in srv;
  struct hostent *hst;
  int sockfd;
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if ((hst=gethostbyname(hostname)) == NULL)
    panic("[-] Failed to resolve hostname\n",-1);
  memset(&srv,0,sizeof(srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons(port);
  memcpy(&srv.sin_addr.s_addr, hst->h_addr, hst->h_length);
  if (connect(sockfd,(struct sockaddr *)&srv,sizeof(srv)) < 0)
    {
      close(sockfd);
      panic("[-] Failed to connect to host\n",-2);
    }
  return sockfd;
}

