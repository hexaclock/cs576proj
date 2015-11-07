#include "server.h"

/*sends data between proxy client (clisockfd) and server specified by hostname:port sockets*/
int start_proxy(char *hostname, unsigned short port, int clisockfd, char *cliipaddr, char *pcertpath, char *pprivpath)
{
  /*need to open connection to server at hostname:port*/
  /*clisockfd is file descriptor for host connecting to this proxy server*/
  /*hostfd will be file descriptor for server we're connecting to*/
  int hostfd;
  int br,bw;
  int act;
  int maxfd;
  char toggledir;
  char reqbuf[REQMAX];
  fd_set sockfds;
  char *fname;
  FILE *logfile;
  const char *respmsg = "HTTP/1.0 200 Connection established\nProxy-agent: Netscape-Proxy/1.1\n\n";
  /* SSL/TLS stuff */
  SSL *hosttls;
  SSL *ctls;
  int ssl_accept_err;
  SSL_load_error_strings();
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  /* end SSL/TLS stuff */
  toggledir = 0;
  /*send plaintext response to client*/
  /*if ( (bw = write(clisockfd,respmsg,strlen(respmsg))) < 0 )
    panic("[-] Failed to send response to client",-2);
    printf("[+] Sent HTTP/1.0 200 response to client\n");*/

  /*start speaking TLS to proxy client*/
  ctls = add_tls_srv(clisockfd,pcertpath,pprivpath);
  if ( (ssl_accept_err = SSL_accept(ctls)) <= 0)
    {
      ERR_print_errors_fp(stdout);
      printf("[-] Failed to negotiate SSL session with client\n");
      close(clisockfd);
      SSL_shutdown(ctls);
      SSL_free(ctls);
      exit(11);
    }
  printf("[+] Successfully negotiated SSL session with client\n");

  /* connect to server */
  printf("[+] Got CONNECT request from %s\n",cliipaddr);
  printf("[+] Trying to connect to %s on port %d\n",hostname,port);
  hostfd = connect_to_host(hostname,port);
  hosttls = add_tls_cli(hostfd);
  if (SSL_connect(hosttls) <= 0)
    {
      ERR_print_errors_fp(stdout);
      printf("[-] Failed to negotiate SSL session with server\n");
      close(clisockfd);
      close(hostfd);
      SSL_shutdown(ctls);
      SSL_free(ctls);
      SSL_shutdown(hosttls);
      SSL_free(hosttls);
      exit(11);
    }
  printf("[+] Connected to %s:%d\n",hostname,port);

  /*make filename, and open file for logging*/
  fname = (char *)malloc(255*sizeof(char));
  fname = get_time("%F-%H.%M.%SEST");
  strcat(fname,"_");
  strcat(fname,cliipaddr);
  strcat(fname,"-");
  strcat(fname,hostname);
  strcat(fname,".log");
  if ( (logfile = fopen(fname,"w")) == NULL )
    panic("Could not open logfile for writing",-5);
  else
    printf("[+] Opened logfile %s for writing\n",fname);

  /*start reading/writing from/to sockets*/
  while (1)
    {
      /* setup sockets for select */
      FD_ZERO(&sockfds);
      FD_SET(hostfd,&sockfds);
      FD_SET(clisockfd,&sockfds);
      maxfd = hostfd;
      if (clisockfd > hostfd)
	maxfd = clisockfd;
      /* wait for input on a socket */
      act = select(maxfd+1,&sockfds,NULL,NULL,NULL);
      if (act<0)
	printf("Problem with select\n");
      /* read data from client, send it to server */
      if (FD_ISSET(clisockfd,&sockfds))
	{
	  br = SSL_read(ctls,reqbuf,REQMAX-1);
	  if (br <= 0)
	    {
	      fflush(logfile);
	      fclose(logfile);
	      close(clisockfd);
	      close(hostfd);
	      free(fname);
	      SSL_shutdown(ctls);
	      SSL_free(ctls);
	      SSL_shutdown(hosttls);
	      SSL_free(hosttls);
	      panic("[-] Failed to read data from proxy client",-3);
	    }
	  if (br > 0)
	    {
	      if ( (bw = SSL_write(hosttls,reqbuf,br)) < 0 )
		{
		  fflush(logfile);
		  fclose(logfile);
		  close(clisockfd);
		  close(hostfd);
		  free(fname);
		  SSL_shutdown(ctls);
		  SSL_free(ctls);
		  SSL_shutdown(hosttls);
		  SSL_free(hosttls);
		  panic("[-] Failed to write data from proxy client to server",-4);
		}
	      /* log the data */
	      if (toggledir == 0 || toggledir == 2)
		if (fprintf(logfile,":::%s to %s:%d:::\n",cliipaddr,hostname,port) < 0)
		  printf("[-] Failed to log to file %s\n",fname);
	      if ( (bw = fwrite(reqbuf,1,bw,logfile)) < 0 )
		printf("[-] Failed to log to file %s\n",fname);
	      toggledir = 1;
	    }
	}
      /* read data from server, send it to client */
      if (FD_ISSET(hostfd,&sockfds))
	{
	  br = SSL_read(hosttls,reqbuf,REQMAX-1);
	  if (br <= 0)
	    {
	      fflush(logfile);
	      fclose(logfile);
	      close(clisockfd);
	      close(hostfd);
	      free(fname);
	      SSL_shutdown(ctls);
	      SSL_free(ctls);
	      SSL_shutdown(hosttls);
	      SSL_free(hosttls);
	      panic("[-] Failed to read data from server",-3);
	    }
	  if (br > 0)
	    {
	      if ( (bw = SSL_write(ctls,reqbuf,br)) < 0 )
		{
		  fflush(logfile);
		  fclose(logfile);
		  close(clisockfd);
		  close(hostfd);
		  free(fname);
		  SSL_shutdown(ctls);
		  SSL_free(ctls);
		  SSL_shutdown(hosttls);
		  SSL_free(hosttls);
		  panic("[-] Failed to write data from server to proxy client",-4);
		}
	      /* log the data */
	      if (toggledir == 0 || toggledir == 1)
		if (fprintf(logfile,":::%s:%d to %s:::\n",hostname,port,cliipaddr) < 0)
		  printf("[-] Failed to log to file %s\n",fname);
	      if ( (bw = fwrite(reqbuf,1,bw,logfile)) < 0 )
		printf("[-] Failed to log to file %s\n",fname);
	      toggledir = 2;
	    }
	}
      /* flush buffer */
      fflush(logfile);
    }
  return 0;
}

