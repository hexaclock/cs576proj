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
  int result;
  std::string username;
  std::string homepath;
  std::string servname;
  std::string dbname;
  std::string dbpath;
  std::string kldir;
  Json::Value passdb;

  if (argc < 2)
    panic("usage: " + (std::string)argv[0] + " <command> <options>", 1);

  /*
   *we'd use secure_getenv, but linux lab is out of date 
   *and doesn't have latest
   *glibc :( 
   */
  if ( (tmp = getenv("USER")) != NULL )
    username = std::string(tmp);
  else
    panic("Invalid $USER defined in env",2);
  
  if ( (tmp = getenv("HOME")) != NULL )
    homepath = std::string(tmp);
  else
    panic("Invalid $HOME defined in env",2);

  dbname = username + "_keylocker" + ".db";
  kldir  = homepath + "/" + ".keylocker";
  std::cout << kldir << std::endl;
  if ( (result = mkdir(kldir.c_str(),0700)) == -1 )
    if (errno != EEXIST)
      panic("Failed to create .keylocker directory in home directory",-2);
  dbpath = kldir + "/" + dbname;

  if (!JsonParsing::readJson(&passdb,dbpath))
    {
      std::cout<<"User's password database file not found... Creating new file"<<std::endl;
      passdb["dbuser"] = username;
    }
  
  /*detect if new file, set username if so*/
  if (!passdb.isMember("dbuser"))
    passdb["dbuser"] = username;

  


  if (!JsonParsing::writeJson(&passdb,dbpath))
    panic("[-] Failed to write to user's password database file!", 3);

  return 0;
}
