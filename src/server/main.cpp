#include "server.h"

int main(int argc, char **argv)
{
  struct sockaddr_in cli;
  int sockfd,clisockfd;
  unsigned short portnum;
  int pid;
  int n;
  char *connectbuf = NULL;
  char *srvhost = NULL;
  char *cliipaddr = NULL;
  char *pcertpath = NULL;
  char *pprivpath = NULL;
  socklen_t clilen;
  unsigned short srvport;
  /*don't use buffer for printf*/
  setvbuf(stdout, NULL, _IONBF, 0);
  connectbuf = (char *)malloc(HEADERMAX*sizeof(char));
  pcertpath = (char *)malloc(PATH_MAX*sizeof(char));
  pprivpath = (char *)malloc(PATH_MAX*sizeof(char));
  connectbuf[0] = '\0';
  n = 0;
  srvport = 0;
  clilen = sizeof(cli);
  /* end variable setup */
  if (argc < 2)
    {
      printf("Usage: %s <port #> <certfile> <privkey>\n",argv[0]);
      exit(1);
    }
  if (argc == 3)
    {
      printf("Usage: %s <port #> <certfile> <privkey>\n",argv[0]);
      exit(1);
    }
  portnum = atoi(argv[1]);
  if (portnum < 1 || portnum > 65535)
    panic("Port must be between 1 and 65535",2);
  if (argc > 3)
    {
      strcpy(pcertpath,argv[2]);
      strcpy(pprivpath,argv[3]);
    }
  else
    {
      printf("[*] Using default certificate and private key\n");
      strcpy(pcertpath,PROXYCERT);
      strcpy(pprivpath,PROXYPRIV);
    }
  sockfd = get_listener(portnum);
  printf("[+] Proxy server started on port %d\n",portnum);
  /* infinite loop that accepts new connections from clients */
  /* forks on new connection */
  while(1)
    {
      /* wait for connection */
      if ( (clisockfd = accept(sockfd,(struct sockaddr *)&cli,&clilen)) < 0 )
	panic("Failed to accept connection on socket\n",4);
      /* fork */
      if ( (pid=fork()) < 0 )
	panic("Fork failed\n",5);
      else if (pid > 0)
	{
	  /* parent */
	  close(clisockfd);
	  continue;
	}
      else
	{
	  /* child */
	  close(sockfd);
	  cliipaddr = inet_ntoa(cli.sin_addr);
	  printf("[+] Client connected from IP address: %s\n",cliipaddr);
	  /*parse CONNECT HOST:PORT*/
	  while (1)
	    {
	      n = read(clisockfd,connectbuf,HEADERMAX-1);
	      if (n < 0)
		panic("Error reading from client.\n",6);
	      if (n > 0 && srvhost == NULL)
		{
		  connectbuf[n] = '\0';
		  if (!strncmp(connectbuf,"CONNECT",7))
		    {
		      connectbuf += 8;
		      connectbuf[strchr(connectbuf,':')-connectbuf] = '\0';
		      srvhost = (char *)malloc(HEADERMAX*sizeof(char));
		      strcpy(srvhost,connectbuf);
		      srvport = (unsigned short)atoi(connectbuf+strlen(srvhost)+1);
		      connectbuf -= 8;
		    }
		}
	    }
	  free(connectbuf);
	  connectbuf = NULL;
	  if (srvport < 1 || srvport > 65535)
	    panic("Port out of range",8);
          
	  printf("[/] Launching start_proxy routine\n");
	  /*
	   *srvhost is host to connect to
	   *srvport is port on host to connect to
	   *clisockfd is socket that we opened to proxy client
	   *cliipaddr is client's ip address as string
	   */
	  //start_proxy(srvhost,srvport,clisockfd,cliipaddr,pcertpath,pprivpath);
	}
      usleep(3000);
    } 
  return 0;
}
