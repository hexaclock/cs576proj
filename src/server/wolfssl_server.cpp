#include <wolfssl_server.h>

bool write_user_db(Json::Value *root, const std::string &dbpath)
{
    Json::StyledWriter writer;
    std::ofstream userdb_file;
    std::string userdb_json = writer.write(*root);

    userdb_file.open(dbpath,std::ofstream::trunc);
    if (!userdb_file.is_open())
        return false;

    userdb_file << userdb_json;
    userdb_file.close();

    return true;
    
}

bool read_user_db(Json::Value *root, const std::string &dbpath)
{
    Json::Reader reader;
    std::ifstream userdb_file;

    userdb_file.open(dbpath);

    if (!userdb_file.is_open())
        return false;

    std::string userdb_json( (std::istreambuf_iterator<char>(userdb_file)),
                             std::istreambuf_iterator<char>() );

    if (!reader.parse(userdb_json, *root, false))
    {
        userdb_file.close();
        return false;
    }

    userdb_file.close();
    
    return true;
}

bool register_user(const std::string &user, const std::string &secret,
                   const std::string &dbpath)
{
    //Json::StyledWriter writer;
    Json::Value root;
    byte *salt;
    std::string hash;


    if (!read_user_db(&root,dbpath))
    {
        std::cout<<"[-] Could not parse user database"<<std::endl;
        return false;
    }

    //client needs to pick a shorter username
    if (user.size() > 64)
    {
        std::cout<<"[-] Failed to register "<<user
                 <<" because their username is >64 characters."<<std::endl;
        return false;
    }

    for (int i=0; i<user.size(); ++i)
    {
        //prevent arbitrary file read/write with "../" in username
        if (!isalnum(user[i]) && user[i] != '-' && user[i] != '_')
        {
            std::cout<<"[-] Failed to register "<<user
                     <<" because the username is not valid."<<std::endl;
            return false;
        }
    }

    /* fail if user already exists! */
    if (root["users"].isMember(user))
    {
        std::cout<<"[-] Failed to register "<<user
                 <<" because user already exists"<<std::endl;
        return false;
    }

    salt = KLCrypto::getRandBytes(16);
    hash = KLCrypto::pbkdf2hash(secret,salt,16);
    if (hash == "")
    {
        std::cout<<"[-] Failed to compute password hash for new user: "<<user
                 <<std::endl;
        return false;
    }
    root["users"][user]["secret"] = hash;
    root["users"][user]["salt"]   = base64_encode(salt, 16);

    if (!write_user_db(&root,dbpath))
    {
        std::cout<<"[-] Failed to write to user database"<<std::endl;
        return false;
    }

    return true;
}

bool change_user_pass(const std::string &user, const std::string newpass,
                      const std::string &dbpath)
{
    //Json::StyledWriter writer;
    Json::Value root;
    byte *newsalt = KLCrypto::getRandBytes(16);
    std::string newhash = KLCrypto::pbkdf2hash(newpass,newsalt,16);

    if (!read_user_db(&root,dbpath))
    {
        std::cout<<"[-] Could not parse user database"<<std::endl;
        return false;
    }
    /* fail if user does not exist! */
    if (!root["users"].isMember(user))
    {
        std::cout<<"[-] Failed to update password for "<<user
                 <<" because they do not exist"<<std::endl;
        return false;
    }

    if (newhash == "")
    {
        std::cout<<"[-] Failed to compute new password hash for user: "<<user
                 <<std::endl;
        return false;
    }

    root["users"][user]["secret"] = newhash;
    root["users"][user]["salt"]   = base64_encode(newsalt, 16);

    if (!write_user_db(&root,dbpath))
    {
        std::cout<<"[-] Failed to write to user database"<<std::endl;
        return false;
    }

    return true;
}

bool auth_user(const std::string &user, const std::string &secret,
               const std::string &dbpath)
{
    //Json::Reader reader;
    Json::Value root;
    Json::Value val;
    Json::StreamWriterBuilder builder;
    std::string tmp = "";
    int loginattempts = 0;

    if (!read_user_db(&root,dbpath))
    {
        std::cout<<"[-] Could not parse user database"<<std::endl;
        return false;
    }    

    /* check if user exists */
    if (root["users"].isMember(user))
    {
        /* make sure user is not locked out */
        if (root["users"][user].isMember("loginattempts"))
        {
            val = root["users"][user]["loginattempts"];
            tmp = Json::writeString(builder,val);
            loginattempts = atoi(tmp.c_str());

            if (loginattempts > 10)
            {
                std::cout<<"[-] "<<user<<" is locked out"<<std::endl;
                return false;
            }
        }
        std::string storedsecret;
        std::string storedsalt;
        const byte *salt_bytes;

        val = root["users"][user]["secret"];
        storedsecret = Json::writeString(builder,val);
        val = root["users"][user]["salt"];
        storedsalt   = Json::writeString(builder,val);
        //strip quotes
        storedsecret = storedsecret.substr(1,storedsecret.size()-2);
        storedsalt   = storedsalt.substr(1,storedsalt.size()-2);
        
        salt_bytes = (const byte*)(base64_decode(storedsalt).c_str());
        std::string hash = KLCrypto::pbkdf2hash(secret,salt_bytes,16);

        //DEBUG
        //std::cout<<"Stored secret: "<<storedsecret<<" "<< storedsecret.size() <<std::endl;
        //std::cout<<"Supplied secret: "<<hash<<" "<< hash.size() <<std::endl;        

        if ( storedsecret == hash )
        {
            return true;
        }
        
    }

    return false;
}

bool send_file(const std::string &user, WOLFSSL *sslconn)
{
    std::string filename = user + "_keylocker.db";
    std::ifstream fin(filename);
    const char *b64dat_c;
    int nwritten;
    int b64datc_len;

    if (!fin.is_open())
        return false;

    std::string b64dat((std::istreambuf_iterator<char>(fin)),
                        std::istreambuf_iterator<char>());

    b64dat_c = b64dat.c_str();
    b64datc_len = strlen(b64dat_c);

    if ( (nwritten = wolfSSL_write(sslconn, b64dat_c, b64datc_len)) < b64datc_len )
    {
        fin.close();
        return false;
    }
    
    fin.close();

    return true;    
}

bool update_file(const std::string &user, const std::string &b64dat)
{
    std::string filename = user + "_keylocker.db";
    std::ofstream fout(filename);

    if (!fout.is_open())
        return false;
    
    fout << b64dat;
    fout.close();

    return true;
}

void process_data(const std::string &data, const std::string &dbpath, WOLFSSL *sslconn)
{
    //format is cmd:user:secretkey:<possible b64 string>
    std::vector<std::string> cmdparts;
    std::stringstream ss(data);
    std::string part;
    std::string cmd;
    std::string user;
    std::string secret;
    std::string b64dat;
    char reply;

    while(std::getline(ss,part,':'))
    {
        if (part != "" && part != " ")
            cmdparts.push_back(part);
    }

    if (cmdparts.size() < 3)
    {
        std::cout<<"[-] Client sent an invalid command"<<std::endl;
        return;
    }

    if (cmdparts.size() >= 3)
    {
        cmd = cmdparts[0];
        user = cmdparts[1];
        secret = cmdparts[2];
    }
    if (cmdparts.size() == 4)
    {
        b64dat = cmdparts[3];
    }

    //std::cout<<"[DEBUG] "<<data<<std::endl;

    /* client wishes to register */
    if (cmd == "REGISTER")
    {
        if (cmdparts.size() == 3)
        {
            if (!register_user(user,secret,dbpath))
            {
                std::cout<<"[-] "<<"Failed to register user: "<<user<<std::endl;
                reply = 0;
            }
            else
            {
                std::cout<<"[+] "<<"Successfully registered user: "<<user<<std::endl;
                reply = 1;
            }
        }
        else
        {
            std::cout<<"[-] Client sent an invalid command"<<std::endl;
        }

        if (wolfSSL_write(sslconn, &reply, sizeof(char)) != sizeof(char))
            std::cout<< "[-] " << "Failed to notify client of registration status"
                     << std::endl;

        return;
    }

    bool authsuccess = auth_user(user,secret,dbpath);
    Json::Value root;
    Json::Value val;
    Json::StreamWriterBuilder builder;
    std::string tmp = "";
    int loginattempts = 0;

    if (!read_user_db(&root,dbpath))
        return;
        
    /* authenticate user before any command processing */
    if (!authsuccess)
    {
        std::cout<<"[-] "<<user<<" failed to authenticate"<<std::endl;

        if (root["users"].isMember(user))
        {
            if (root["users"][user].isMember("loginattempts"))
            {
                val = root["users"][user]["loginattempts"];
                tmp = Json::writeString(builder,val);
                loginattempts = atoi(tmp.c_str());
                //DEBUG
                //std::cout << tmp << std::endl;
                //std::cout << loginattempts << std::endl;
            }

            loginattempts += 1;

            root["users"][user]["loginattempts"] = loginattempts;

            if (!write_user_db(&root, dbpath))
                return;
        }

        return;
    }
    else
    {
        if (root["users"].isMember(user))
            root["users"][user]["loginattempts"] = 0;

        if (write_user_db(&root, dbpath))
            std::cout<<"[+] "<<user<<" authenticated successfully"<<std::endl;
        else
            std::cout << "[-] Failed to write to user database!" << std::endl;
    }

    /* client wishes to upload their database */
    if (cmd == "UPLOAD")
    {
        if (cmdparts.size() == 4)
        {
            if (!update_file(user,b64dat))
			{
				std::cout<<"[-] Failed to update "<<user<<"'s"<<" database"<<std::endl;
				reply = 0;
			}
            else
			{
                std::cout<<"[+] Updated "<<user<<"'s"<<" database"<<std::endl;
				reply = 1;
			}
        }
        else
        {
            std::cout<<"[-] Client sent an invalid command"<<std::endl;
        }
		if (wolfSSL_write(sslconn, &reply, sizeof(char)) != sizeof(char))
			std::cout<< "[-] " << "Failed to notify client of chpass status" << std::endl;    
    }

    /* client wishes to download their database */
    else if (cmd == "DOWNLOAD")
    {
        if (cmdparts.size() == 3)
        {
            if (!send_file(user,sslconn))
                std::cout<<"[-] Failed to send "<<user<<"'s"<<" database"<<std::endl;
            else
                std::cout<<"[+] Sent "<<user<<"'s"<<" database"<<std::endl;
        }
        else
        {
            std::cout<<"[-] Client sent an invalid command"<<std::endl;
        }
    }

    else if (cmd == "CHPASS")
    {
        //CHPASS:user:curpass:newpass\n
        if (cmdparts.size() == 4)
        {
            //it isn't actually b64-encoded, it's hex..
            std::string newpass = b64dat;
            if (!change_user_pass(user,newpass,dbpath))
            {
                reply = 0;
                std::cout<<"[-] Failed to change "<<user<<"'s"<<" password"<<std::endl;
            }
            else
            {
                reply = 1;
                std::cout<<"[+] Successfully changed "<<user<<"'s"<<" password"<<std::endl;
            }
        }
        else
        {
            std::cout<<"[-] Client sent an invalid command"<<std::endl;
        }

        if (wolfSSL_write(sslconn, &reply, sizeof(char)) != sizeof(char))
            std::cout<< "[-] " << "Failed to notify client of chpass status"
                     << std::endl;
    }

    else if (cmd == "TIMESTAMP")
    {
        //TIMESTAMP:USER:PASS:time_t value
        if (cmdparts.size() == 4)
        {
            struct stat filestats;
            std::string userdbpath = user + "_keylocker.db";
            //time_t gotmodtime;
            time_t modtime;
            
            std::cout << "[*] Got timestamp request from " << user
                      << std::endl;

            //file not found, send timestamp of 0
            if (stat(userdbpath.c_str(), &filestats) == -1)
            {
                std::cout << "[*] " << user << " does not have a database on this server"
                          << std::endl;
                modtime = 0;
            }
            else
                modtime = filestats.st_mtime;

            if (wolfSSL_write(sslconn, &modtime, sizeof(time_t)) != sizeof(time_t))
            {
                std::cout << "[-] Failed to notify " << user << " of timestamp"
                          << std::endl;
            }
        }
        else
            std::cout<<"[-] Client sent an invalid command"<<std::endl;
    }

    /* client sent invalid command */
    else
    {
        std::cout<<"[-] Client sent an invalid command"<<std::endl;
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
        memset(tempbuf,0,sizeof(tempbuf));
        if ( (bytes_read = wolfSSL_read(sslconn,tempbuf,sizeof(tempbuf)-2)) > 0 )
        {
            cli_data += std::string(tempbuf);
            if (tempbuf[bytes_read-1] == '\n')
            {
                //remove '\n' from string
                cli_data.erase(cli_data.size()-1,1);
                break;
            }
            if (cli_data.size() > MAX_DATA_SIZE)
            {
                break;
            }
        }
        else if (bytes_read < 0)
        {
            break;
        }
    }
    
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

void sighandler(int sig)
{
    //exit child
    if (sig == SIGALRM)
    {
        std::cout << "[-] Timeout exceeded" << std::endl;
        exit(2);
    }
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
    std::string dbpath;
    pid_t pid;

    clilen = sizeof(cliaddr);
    wolfSSL_Init();

    if (argc == 4)
    {
        if (prompt_y_n("Create new user database?", ""))
        {
            do
            {
                std::cout << "Please specify a filename for the new database: ";
                std::getline(std::cin, dbpath);
                
                if (!access(dbpath.c_str(), F_OK))
                {
                    if (prompt_y_n("File already exists, overwrite?", ""))
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            } while (true);
                
            std::string jsondat = "{ \"users\" : { } }";
            std::ofstream outputfile;
            outputfile.open(dbpath);
         
            if (outputfile.is_open())
            {
                outputfile << jsondat;
                outputfile.close();
                std::cout << "Created new database file!"
                          << std::endl;
            }
            else
            {
                std::cout << "Failed to create new database file!"
                          << std::endl;
            }
        }
        else
        {
            std::cout << "Ok, please specify an existing user database" << std::endl;
			std::cout<<"Usage: "<<argv[0]<<" <port #> <certfile> <privkey> [userdb]"<<std::endl;
            return -1;
        }
    }

    else if (argc < 5)
    {
        std::cout<<"Usage: "<<argv[0]<<" <port #> <certfile> <privkey> [userdb]"<<std::endl;
        std::cout<<"If [userdb] is unspecified, we will create a new one"<<std::endl;
        return 1;
    }

    portnum  = atoi(argv[1]);
    certpath = argv[2];
    privpath = argv[3];
    if (argc == 5)
        dbpath = std::string(argv[4]);

    if (portnum < 1 || portnum > 65535)
    {
        std::cout<<"Please choose a port in the range: 1-65535"<<std::endl;
        return 1;
    }

    /*userdb_file.open(dbpath);
    if (!userdb_file.is_open())
    {
        std::cout<<"[-] Could not open user database"<<std::endl;
        return 1;
        }*/

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
            waitpid(pid, 0, 0);
            continue;
        }
        else
        {
            /* child */
            close(socketfd);
            //15 second timeout
            signal(SIGALRM,sighandler);
            alarm(15);
            cliipaddr = std::string(inet_ntoa(cliaddr.sin_addr));
            std::cout<<"[+] Client connected from IP address: "<<cliipaddr
                     <<std::endl;
            sslconn = start_ssl(wsslctx,clisocketfd,cliaddr);
            data = get_cli_data(sslconn);

            //shut alarm off
            alarm(0);
            process_data(data,dbpath,sslconn);

            close(clisocketfd);
            break;
        }

        usleep(1000);
    }
    //close(clisocketfd);
    wolfSSL_free(sslconn);
    wolfSSL_CTX_free(wsslctx);
    wolfSSL_Cleanup();

    return 0;
}
