#include "network.h"

WOLFSSL* tls_connect(std::string hostname, int portnum)
{
    struct sockaddr_in srvaddr;
    struct hostent *hst;
    WOLFSSL_CTX *wsctx;
    WOLFSSL *wssl;
    int sockfd;

    wolfSSL_Init();

    if (portnum < 1 || portnum > 65535)
    {
        std::cout<<"Invalid port"<<std::endl;
        return NULL;
    }

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        std::cout<<"Failed to create socket"<<std::endl;
        return NULL;
    }

    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(portnum);

    if ( (hst = gethostbyname(hostname.c_str())) == NULL )
    {
        std::cout<<"Failed to resolve hostname"<<std::endl;
        return NULL;
    }

    memcpy(&srvaddr.sin_addr.s_addr, hst->h_addr, hst->h_length);

    if (connect(sockfd,(struct sockaddr *)&srvaddr,sizeof(srvaddr)) < 0)
    {
        std::cout<<"Failed to connect to server"<<std::endl;
        return NULL;
    }

    if ( (wsctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL )
    {
        std::cout<<"Failed to create new wolfSSL CTX"<<std::endl;
        return NULL;
    }

    if ( (wssl = wolfSSL_new(wsctx)) == NULL )
    {
        std::cout<<"Failed to create new wolfSSL object"<<std::endl;
        return NULL;
    }

    wolfSSL_set_fd(wssl,sockfd);

    if (wolfSSL_connect(wssl) == SSL_SUCCESS)
    {
        return wssl;
    }

    return NULL;
}
