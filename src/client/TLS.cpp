#include "network.h"

int tls_send(std::string &hostname, int portnum,
             std::string &data)
{
    struct sockaddr_in srvaddr;
    struct hostent *hst;
    WOLFSSL_CTX *wsctx;
    WOLFSSL *wssl;
    int sockfd;

    //DEBUG//
    std::cout<<hostname<<':'<<portnum<<std::endl;
    std::cout<<data<<std::endl;

    wolfSSL_Init();

    if (portnum < 1 || portnum > 65535)
    {
        std::cout<<"Invalid port"<<std::endl;
        return -1;
    }

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        std::cout<<"Failed to create socket"<<std::endl;
        return -2;
    }

    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(portnum);

    if ( (hst = gethostbyname(hostname.c_str())) == NULL )
    {
        std::cout<<"Failed to resolve hostname"<<std::endl;
        return -3;
    }

    memcpy(&srvaddr.sin_addr.s_addr, hst->h_addr, hst->h_length);

    if (connect(sockfd,(struct sockaddr *)&srvaddr,sizeof(srvaddr)) < 0)
    {
        std::cout<<"Failed to connect to server"<<std::endl;
        return -4;
    }

    if ( (wsctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL )
    {
        std::cout<<"Failed to create new wolfSSL CTX"<<std::endl;
        return -5;
    }

    if ( (wssl = wolfSSL_new(wsctx)) == NULL )
    {
        std::cout<<"Failed to create new wolfSSL object"<<std::endl;
        return -5;
    }

    wolfSSL_set_fd(wssl,sockfd);

    if (wolfSSL_connect(wssl) == SSL_SUCCESS)
    {
        //TODO: create functions for REGISTER, UPLOAD, DOWNLOAD
        /*
         * Format of std::string data:
         *
         * REGISTER:username:secretkey\n
         * UPLOAD:username:secretkey:b64_userdb_string\n
         * DOWNLOAD:username:secretkey\n
         *
         */
        return 0;
        
    }

    wolfSSL_free(wssl);
    wolfSSL_CTX_free(wsctx);
    wolfSSL_Cleanup();

    return 0;
}
