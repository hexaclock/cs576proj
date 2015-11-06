#ifndef _LAB1_H_
#define _LAB1_H_

#define HEADERMAX 4096
#define REQMAX 8192
#define PROXYCERT "../include/proxy_cert.pem"
#define PROXYPRIV "../include/proxy_private.pem"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <linux/limits.h>
int main(int argc, char **argv);
int connect_to_host(char *hostname, unsigned short port);
int get_listener(unsigned short portnum);
int start_proxy(char *hostname, unsigned short port, 
		int clisockfd, char *cliipaddr,
		char *pcertpath, char *pprivpath);
SSL *add_tls_cli(int sockfd);
SSL *add_tls_srv(int clisockfd, char *cert, char *priv);
char *get_time(char *format);
void panic(char *s, unsigned int n);

#endif
