#include "network.h"
#include "keylocker.h"
#include "json/json.h"

#include <termios.h>

Json::Value passdb;

void panic(std::string msg, int code)
{
    std::cout << msg << std::endl;
    exit(code);
}

//edits termcaps to hide passwords as entered
void hideterm()
{
	termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	tty.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

//undoes hideterm()
void showterm()
{
	termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	tty.c_lflag |= ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void add_entry()
{

		std::string service;
		std::string username;
		std::string password;
		std::string notes;
		std::string entry;

		//JsonParsing::readJson(&passdb,dbpath);

		std::cout<<"Service:  ";
		std::getline(std::cin,service);
		std::cout<<"Username: ";
    std::getline(std::cin,username);
		std::cout<<"Password: ";
		hideterm();
    std::getline(std::cin,password);
		showterm();
		std::cout<<"\nNotes:    ";
    std::getline(std::cin,notes);
		std::cout<<"\n";

		entry = service + "_" + username;

		passdb["dbentry"][entry]["service"] = service;
		passdb["dbentry"][entry]["username"] = username;
		passdb["dbentry"][entry]["password"] = password;
		passdb["dbentry"][entry]["notes"] = notes;
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

  /*if 'add' is the user's command*/
	if (!strcmp(argv[1], "add"))
		add_entry();

  if (!JsonParsing::writeJson(&passdb,dbpath))
    panic("[-] Failed to write to user's password database file!", 3);

  return 0;
}
