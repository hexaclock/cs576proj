#ifndef _KEYLOCKER_H_
#define _KEYLOCKER_H_

/*C stuff*/
#include <stdlib.h>
/*C++ stuff*/
#include <fstream>
#include <iostream>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <linux/limits.h>
#include "json/json.h"

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
    private:

};

/* class for creating/editing server
 */
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

/* class for reading and writing to a JSON database file
 */
class JsonParsing
{
    public:
        static bool readJson(Json::Value* root, std::string dbName);
        static bool writeJson(Json::Value* root, std::string dbName);
};

#endif
