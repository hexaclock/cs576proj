#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "keylocker.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h>
#include <wolfssl/ssl.h>

int tls_send(std::string &hostname, int portnum,
             std::string &data);

#endif
