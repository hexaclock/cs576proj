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
    Json::Value root;   // will contains the root value after parsing.
    Json::Reader reader;
    std::ifstream passdb_file;

    if (argc < 2)
        panic("usage: " + (std::string)argv[0] + " <username>", 1);

    passdb_file.open("bob.db", std::ifstream::binary);
    if ( !reader.parse( passdb_file, root, false ) )
      {
	std::cout  << reader.getFormattedErrorMessages() << std::endl;
      }

    std::string encoding = root.get("testkey", "UTF-8" ).asString();
    std::cout << encoding << std::endl;

    return 0;
}
