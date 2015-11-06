#ifndef _SERVER_H_
#define _SERVER_H_

/*c stuff + network*/
#define HEADERMAX 4096
#define REQMAX 8192
#define PROXYCERT "../../include/proxy_cert.pem"
#define PROXYPRIV "../../include/proxy_private.pem"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
/*openssl*/
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <linux/limits.h>

/*c++ stuff*/
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

/*wolfSSL*/
/*#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/pwdbased.h>
#include <wolfssl/wolfcrypt/coding.h>
#include <wolfssl/wolfcrypt/error-crypt.h>*/
/*json, base64*/
#include "json/json.h"
#include "base64.h"

int main(int argc, char **argv);
int connect_to_host(char *hostname, unsigned short port);
int get_listener(unsigned short portnum);
int start_proxy(char *hostname, unsigned short port, 
		int clisockfd, char *cliipaddr,
		char *pcertpath, char *pprivpath);
SSL *add_tls_cli(int sockfd);
SSL *add_tls_srv(int clisockfd, char *cert, char *priv);
char *get_time(char *format);
void panic(std::string s, unsigned int n);


#endif
