#ifndef _KEYLOCKER_H_
#define _KEYLOCKER_H_

/*C stuff*/
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
/*C++ stuff*/
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
/*wolfSSL*/
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/pwdbased.h>
#include <wolfssl/wolfcrypt/coding.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
/*
#include <openssl/aes.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
*/
#include <linux/limits.h>
#include "json/json.h"
#include "base64.h"

/* class for creating/editing clients
 */
class Client
{
    public:
        int connect(std::string servname);
        bool auth();
        std::string newEntry();
        bool editEntry(std::string euuid);
        bool deleteEntry(std::string euuid);
        bool checkpass(std::string curpass);
    private:

};

/* class for creating/editing server
 */
/*
class Server
{
    public:
        int start();
        bool addUser(std::string username, std::string pubkey)
        {
            return true;
        };
        bool deleteUser(std::string username)
        {
            return true;
        };

    private:
        bool started;
};
*/
/* class for reading and writing to a JSON database file
 */
class JsonParsing
{
public:
    static bool readJson(Json::Value* root, std::string dbName, std::string key);
    static bool writeJson(Json::Value* root, std::string dbName, std::string key);
};

/*
 *class for encrypting and decrypting JSON database file
 */
class KLCrypto
{
public:
    static byte* getRandBytes(int nbytes);
    static std::string dbEncrypt(std::string ptxt, std::string pass);
    static std::string dbDecrypt(std::string ctxt, std::string pass);
    static std::string genpwd(int len);
};

#endif
