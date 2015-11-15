#include "network.h"

#define MAXDATASIZE 20480       /*20k bytes*/

bool regist (WOLFSSL* ssl, std::string &data)
{
    //char rcvBuf[MAXDATASIZE] = {0};
    char success = 0;
    int ret = 0;

    //strcpy(sendBuf, data.c_str());
    if (wolfSSL_write(ssl, data.c_str(), data.size()) != (int)data.size())
    {
        ret = wolfSSL_get_error(ssl, 0);
        std::cout << "WolfSSL write error. Error: " << ret << std::endl;
        return false;
    }

    if (wolfSSL_read(ssl, &success, sizeof(char)) < 0)
    {
        ret = wolfSSL_get_error(ssl, 0);
        std::cout << "WolfSSL read error. Error: " << ret << std::endl;
    }   
    
    if (success)
        return true;
    else
        return false;
}

bool upload (WOLFSSL* ssl, std::string &data)
{
    char rcvBuf[MAXDATASIZE] = {0};
    int ret = 0;

    //strcpy(sendBuf, data.c_str());

    if (wolfSSL_write(ssl, data.c_str(), data.size()) != (int)data.size())
    {
        ret = wolfSSL_get_error(ssl, 0);
        std::cout << "WolfSSL write error. Error: " << ret << std::endl;
        return false;
    }

    if (wolfSSL_read(ssl, rcvBuf, MAXDATASIZE-1) < 0)
    {
        ret = wolfSSL_get_error(ssl, 0);
        std::cout << "WolfSSL read error. Error: " << ret << std::endl;
    }
    //std::cout << "Received: " << rcvBuf << std::endl;

    return true;
}

/* pre:  pass request data; pass the path of database (~/.keyloker)
 * post: update the database file with the one from server.
 * return: true on success; false on failed
 */
bool download (WOLFSSL* ssl, std::string &data, std::string &dbpath)
{
    char rcvBuf[MAXDATASIZE] = {0};
    int ret;
    int nread;
    std::string username;
    std::string msg;
    std::string filename;
    std::string delimiter = ":";
    std::string srvresponse;
    std::string b64dat;

    /*parse the data for file update*/
    std::ofstream fout(dbpath);

    //strcpy(sendBuf, data.c_str());

    if (wolfSSL_write(ssl, data.c_str(), data.size()) != (int)data.size())
    {
        ret = wolfSSL_get_error(ssl, 0);
        std::cout << "WolfSSL write error. Error: " << ret << std::endl;
        return false;
    }

    do
    {
        memset(rcvBuf, 0, sizeof(rcvBuf));

        nread = wolfSSL_read(ssl, rcvBuf, MAXDATASIZE-1);
        if (nread < 0)
        {
            ret = wolfSSL_get_error(ssl, 0);
            std::cout << "WolfSSL read error. Error: " << ret << std::endl;
        }
        else if (nread > 0)
        {
            srvresponse += std::string(rcvBuf);
        }
    } while (nread > 0);

    //std::cout << "Message received" <<std::endl;
    //std::string rcv = rcvBuf;
    b64dat = srvresponse.substr(srvresponse.find_last_of(delimiter) + 1);

    if (b64dat.size() < 2)
    {
        return false;
    }

    if (!fout.is_open())
    {
        std::cout << "Failed to open "<< dbpath << " for writing"
                  << std::endl;

        return false;
    }

    fout << b64dat;
    fout.close();

    return true;
}

bool chpass(WOLFSSL* ssl, std::string &data)
{
    char success = 0;
    int ret = 0;
    
    if (wolfSSL_write(ssl, data.c_str(), data.size()) != (int)data.size())
    {
        ret = wolfSSL_get_error(ssl, 0);
        std::cout << "WolfSSL write error. Error: " << ret << std::endl;
        return false;
    }

    if (wolfSSL_read(ssl, &success, sizeof(char)) < 0)
    {
        ret = wolfSSL_get_error(ssl, 0);
        std::cout << "WolfSSL read error. Error: " << ret << std::endl;
    }   
    
    if (success)
        return true;
    else
        return false;    
}

int tls_send(std::string &hostname, int portnum,
             std::string &data, std::string &dbpath)
{
    struct sockaddr_in srvaddr;
    struct hostent *hst;
    WOLFSSL_CTX *wsctx;
    WOLFSSL *wssl;
    int sockfd;
    int wssl_err;
    int wssl_result;
    char errorbuf[80];

    //DEBUG//
    /*std::cout<<hostname<<':'<<portnum<<std::endl;
      std::cout<<data<<std::endl;*/

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
        std::cout<<std::endl<<"Failed to connect to server"<<std::endl;
        return -4;
    }

    if ( (wsctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method())) == NULL )
    {
        std::cout<<"Failed to create new wolfSSL CTX"<<std::endl;
        return -5;
    }

    if (wolfSSL_CTX_load_verify_locations(wsctx, "../../include/server_cert.pem", 0) != SSL_SUCCESS)
    {
        std::cout<<"Error loading server's certificate for verification"<<std::endl;
        return EXIT_FAILURE;
    }

    if ( (wssl = wolfSSL_new(wsctx)) == NULL )
    {
        std::cout<<"Failed to create new wolfSSL object"<<std::endl;
        return -5;
    }

    wolfSSL_set_fd(wssl,sockfd);

    if ( (wssl_result = wolfSSL_connect(wssl)) == SSL_SUCCESS )
    {
        /*
         * Format of std::string data:
         *
         * REGISTER:username:secretkey\n
         * UPLOAD:username:secretkey:b64_userdb_string\n
         * DOWNLOAD:username:secretkey\n
         *
         */
        //std::cout<<"SSL connection success"<<std::endl;
        std::string delimiter = ":";
        std::string msg = data;
        std::string reqType = msg.substr(0, msg.find(delimiter));
        bool ret;

        if (reqType == "DOWNLOAD")
        {
            if ( (ret = download(wssl, data, dbpath)) )
                std::cout << "Successfully downloaded database" << std::endl;
        }
        else if (reqType == "UPLOAD")
        {
            if ( (ret = upload(wssl, data)) )
                std::cout << "Successfully uploaded database" << std::endl;
        }
        else if (reqType == "REGISTER")
        {
            if ( (ret = regist(wssl, data)) )
                std::cout << "Successfully registered with server" << std::endl;
        }
        else if (reqType == "CHPASS")
        {
            ret = chpass(wssl, data);
        }

        /*return 1 stands for failure of requests processing*/
        if (ret == false)
        {
            wolfSSL_free(wssl);
            close(sockfd);
            wolfSSL_CTX_free(wsctx);
            wolfSSL_Cleanup();
            return 1;
        }
    }
    else
    {
        wssl_err = wolfSSL_get_error(wssl,wssl_result);
        wolfSSL_ERR_error_string(wssl_err, errorbuf);
        return -10;
        //std::cout<<"SSL connection failed with: "<<std::string(errorbuf)<<std::endl;
    }

    wolfSSL_free(wssl);
    close(sockfd);
    wolfSSL_CTX_free(wsctx);
    wolfSSL_Cleanup();

    return 0;
}
