#include "network.h"
#include "keylocker.h"
#include "json/json.h"

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

void panic(std::string msg, int code)
{
    std::cout << msg << std::endl;
    exit(code);
}

int main(int argc, char **argv)
{
    if (argc < 2)
        panic("usage: " + (std::string)argv[0] + " <username>", 1);

    Json::Value root;
    Json::Reader reader;

    std::fstream db;
    std::cout << (std::string)argv[1] + ".db" << std::endl;
    db.open((std::string)argv[1] + ".db", std::fstream::binary);
    reader.parse(db, root, false);
    /*
    if (!reader.parse(db, root, false))
    {
        panic("Failed to parse Json!", 2);
    }
    */

    std::cout << root;

    db.close();
    return 0;
}
