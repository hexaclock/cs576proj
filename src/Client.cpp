#include "network.h"
#include "keylocker.h"
#include "json/json.h"

void panic(std::string msg, int code)
{
    std::cout << msg << std::endl;
    exit(code);
}

int main(int argc, char **argv)
{
  char *tmp;
  std::string username;
  std::string homepath;
  std::string servname;
  std::string dbname;
  std::string dbpath;
  Json::Value passdb;

  /*if (argc < 2)
    panic("usage: " + (std::string)argv[0] + " <username>", 1);*/
  
  /*
   *we'd use secure_getenv, but linux lab is out of date 
   *and doesn't have latest
   *glibc :( 
   */
  if ( (tmp = getenv("USER")) != NULL )
    username = std::string(tmp);
  if ( (tmp = getenv("HOME")) != NULL )
    homepath = std::string(tmp);

  dbname = "." + username + " _keylocker" + ".db";
  dbpath = homepath + "/" + dbname;

  if (!JsonParsing::readJson(&passdb,dbpath))
    {
      std::cout<<"User's password database file not found... Creating new file"<<std::endl;
      passdb["username"] = username;
    }
  /*detect if new file, set username if so*/
  if (!passdb.isMember("username"))
    passdb["username"] = username;

  if (!JsonParsing::writeJson(&passdb,dbpath))
    panic("[-] Failed to write to user's password database file!", 3);

  return 0;
}
