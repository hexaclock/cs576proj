#include <wolfssl_server.h>

void process_data(const std::string &data)
{
    //format is cmd:user:secretkey:<possible b64 string>
    std::vector<std::string> cmdparts;
    std::stringstream ss(data);
    std::string part;

    while(std::getline(ss,part,':'))
    {
        cmdparts.push_back(part);
    }

    if (cmdparts[0] == "UPLOAD")
    {
        std::cout<<"got UPLOAD"<<std::endl;
    }
    else if (cmdparts[0] == "DOWNLOAD")
    {
        std::cout<<"got DOWNLOAD"<<std::endl;
    }
    else if (cmdparts[0] == "REGISTER")
    {
        std::cout<<"got REGISTER"<<std::endl;
    }

    return;
}

std::string get_cli_data(WOLFSSL *sslconn)
{
    int bytes_read;
    char tempbuf[4096];
    std::string cli_data;

    while(1)
    {
        memset(&tempbuf,0,sizeof(tempbuf));
        if ( (bytes_read = wolfSSL_read(sslconn,tempbuf,sizeof(tempbuf)-2)) > 0 )
        {
            cli_data += std::string(tempbuf);
        }
        else if (bytes_read < 0)
        {
            break;
        }
    }
    wolfSSL_free(sslconn);
    return cli_data;
}

WOLFSSL *start_ssl(WOLFSSL_CTX *wsslctx, socklen_t clisocketfd, struct sockaddr_in cliaddr)
{
    socklen_t casize;
    WOLFSSL *wssl;

    casize = sizeof(cliaddr);

    if ( (wssl = wolfSSL_new(wsslctx)) == NULL )
    {
        std::cout<<"Failed to create new SSL object"<<std::endl;
        return NULL;
    }

    wolfSSL_set_fd(wssl,clisocketfd);

    return wssl;    
}

int main(int argc, char **argv)
{
    struct sockaddr_in srvaddr, cliaddr;
    socklen_t socketfd,clisocketfd;
    socklen_t clilen;
    WOLFSSL_CTX *wsslctx;
    WOLFSSL *sslconn;
    int portnum;
    const char *certpath;
    const char *privpath;
    std::string cliipaddr;
    std::string data;
    pid_t pid;

    clilen = sizeof(cliaddr);
    wolfSSL_Init();

    if (argc < 4)
    {
        std::cout<<"Usage: "<<argv[0]<<" <port #> <certfile> <privkey>"<<std::endl;
        return 1;
    }

    portnum  = atoi(argv[1]);
    certpath = argv[2];
    privpath = argv[3];

    if (portnum < 1 || portnum > 65535)
    {
        std::cout<<"Please choose a port in the range: 1-65535"<<std::endl;
        return 1;
    }

    if ( (socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        std::cout<<"Failed to initialize socket"<<std::endl;
        return -1;
    }

    memset((void*)&srvaddr,0,sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = INADDR_ANY;
    srvaddr.sin_port = htons(portnum);

    if ( (wsslctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method())) == NULL )
    {
        std::cout<<"Failed to create new WolfSSL CTX"<<std::endl;
        return -1;
    }
    
    if (wolfSSL_CTX_use_PrivateKey_file(wsslctx,privpath,SSL_FILETYPE_PEM) != SSL_SUCCESS)
    {
        std::cout<<"Failed to load SSL private key file"<<std::endl;
        return -2;
    }

    if (wolfSSL_CTX_use_certificate_file(wsslctx,certpath,SSL_FILETYPE_PEM) != SSL_SUCCESS)
    {
        std::cout<<"Failed to load SSL certificate file"<<std::endl;
        return -2;
    }

    if (bind(socketfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) != 0)
    {
        std::cout<<"Failed to bind to port "<<portnum<<std::endl;
        return -3;
    }
    
    listen(socketfd,10);
    std::cout<<"[+] KeyLocker server started. Waiting for connections..."<<std::endl;

    while(1)
    {
        if ( (clisocketfd = accept(socketfd,(struct sockaddr *)&cliaddr,&clilen)) == -1 )
        {
            std::cout<<"Failed to accept connection on socket"<<std::endl;
            //return -3;
        }

        if ( (pid=fork()) < 0 )
        {
            std::cout<<"Fork failed"<<std::endl;
            return -4;
        }
        else if (pid > 0)
        {
            /* parent */
            close(clisocketfd);
            continue;
        }
        else
        {
            /* child */
            close(socketfd);
            cliipaddr = std::string(inet_ntoa(cliaddr.sin_addr));
            std::cout<<"[+] Client connected from IP address: "<<cliipaddr
                     <<std::endl;
            sslconn = start_ssl(wsslctx,clisocketfd,cliaddr);
            data = get_cli_data(sslconn);
            process_data(data);

            close(clisocketfd);
            break;
        }

        usleep(1000);
    }

    wolfSSL_CTX_free(wsslctx);
    wolfSSL_Cleanup();
    
    return 0;
}
