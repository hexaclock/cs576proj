#include "network.h"

#define MAXDATASIZE 20480		/*20k bytes*/

int regist (WOLFSSL* ssl, std::string &data) 
{
	char sendBuf[MAXDATASIZE], rcvBuf[MAXDATASIZE] = {0};
	int ret = 0;

	strcpy(sendBuf, data.c_str());
	if (wolfSSL_write(ssl, sendBuf, strlen(sendBuf)) != strlen(sendBuf))
	{
		ret = wolfSSL_get_error(ssl, 0);
		std::cout << "WolfSSL write error. Error: " << ret << std::endl;
		return -1;
	}

	if (wolfSSL_read(ssl, rcvBuf, MAXDATASIZE) < 0)
	{
		ret = wolfSSL_get_error(ssl, 0);
		std::cout << "WolfSSL read error. Error: " << ret << std::endl;
	}
	std::cout << "Recieved: " << rcvBuf << std::endl;
	
	return ret;
}

int upload (WOLFSSL* ssl, std::string &data)
{
	char sendBuf[MAXDATASIZE], rcvBuf[MAXDATASIZE] = {0};
	int ret = 0;

	strcpy(sendBuf, data.c_str());

	if (wolfSSL_write(ssl, sendBuf, strlen(sendBuf)) != strlen(sendBuf))
	{
		ret = wolfSSL_get_error(ssl, 0);
		std::cout << "WolfSSL write error. Error: " << ret << std::endl;
		return -1;
	}

	if (wolfSSL_read(ssl, rcvBuf, MAXDATASIZE) < 0)
	{
		ret = wolfSSL_get_error(ssl, 0);
		std::cout << "WolfSSL read error. Error: " << ret << std::endl;
	}
	std::cout << "Recieved: " << rcvBuf << std::endl;

	return ret;
}

/* pre:  pass request data; pass the path of database (~/.keyloker)
 * post: update the database file with the one from server.
 * return: true on success; false on failed
 */
bool download (WOLFSSL* ssl, std::string &data, std::string &dbpath)
{
	char sendBuf[MAXDATASIZE], rcvBuf[MAXDATASIZE] = {0};
	int ret;
	std::string username;
	std::string msg;
	std::string filename;
	std::string delimiter = ":";
    std::string b64dat;

    /*parse the data for file update*/
	msg.erase(0, msg.find(delimiter) + delimiter.length());
	username = msg.substr(0, msg.find(delimiter));
	filename = username + "_keylocker.db";
	std::ofstream fout(dbpath + '/' + filename);

	strcpy(sendBuf, data.c_str());

	if (wolfSSL_write(ssl, sendBuf, strlen(sendBuf)) != strlen(sendBuf))
	{
		ret = wolfSSL_get_error(ssl, 0);
		std::cout << "WolfSSL write error. Error: " << ret << std::endl;
		return false;
	}

	if (wolfSSL_read(ssl, rcvBuf, MAXDATASIZE) < 0)
	{
		ret = wolfSSL_get_error(ssl, 0);
		std::cout << "WolfSSL read error. Error: " << ret << std::endl;
	}
	
	std::cout << "Message recieved" <<std::endl;
	std::string rcv= rcvBuf;
    b64dat = rcv.substr(rcv.find_last_of(delimiter) + 1);

	if (!fout.is_open())
		return false;

	fout << b64dat;
	fout.close();
	return true;
}

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
		if (data contains upload)
				upload(wssl,
        return 0;
        
    }

    wolfSSL_free(wssl);
    wolfSSL_CTX_free(wsctx);
    wolfSSL_Cleanup();

    return 0;
}
