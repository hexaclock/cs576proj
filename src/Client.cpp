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
  std::string username;
  std::string servname;
  std::string dbname;
  Json::Value passdb;

  if (argc < 2)
    panic("usage: " + (std::string)argv[0] + " <username>", 1);
  username = (std::string)argv[1];
  dbname = username + ".db";

  if (!JsonParsing::readJson(&passdb,dbname))
    panic("Could not open user's password database file", 2);

  /*detect if new file, set username if so*/
  if (passdb.get("username","BLARGH").asString() == "BLARGH")
    passdb["username"] = username;

  if (!JsonParsing::writeJson(&passdb,dbname))
    panic("Failed to write to user's password database file", 3);

  return 0;
}
