#include "network.h"
#include "keylocker.h"
#include "json/json.h"

#include <termios.h>

/* pre: takes in an std::string msg and int code
 * post: prints msg to stdout and exits with error code code
 */
void panic(std::string msg, int code)
{
    std::cout << msg << std::endl;
    exit(code);
}

/* pre: none
 * post: uses termcaps to hide user input
 */
void hideterm()
{
    termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

/* pre: none
 * post: uses termcaps to unhide user input
 */
void showterm()
{
    termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

/* pre: takes in a Json::Value* passdb
 * post: reads in user input and creates a new keylocker database entry with the
 *      provided input in passdb
 */
void add_entry(Json::Value *passdb)
{

    std::string service;
    std::string username;
    std::string password;
    std::string notes;
    std::string entry;

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

    (*passdb)["dbentry"][entry]["service"] = service;
    (*passdb)["dbentry"][entry]["username"] = username;
    (*passdb)["dbentry"][entry]["password"] = password;
    (*passdb)["dbentry"][entry]["notes"] = notes;
}

/* pre: takes in a Json::Value* passdb and a std::string request
 * post: if request is not empty prints out the keylocker database entry for key
 *      request, else prints out all the keylocker database entries in passdb
 */
void get_entry(Json::Value *passdb, std::string request)
{
    Json::Value val;
    Json::Value::iterator it;
    Json::StreamWriterBuilder builder;
    std::string line;

    //This line was recommended on the internet but I don't think it changes
    //  output
    //builder.settings_["indentation"] = "";

    if (!request.empty()) /* get where key=request */
    {
        val = (*passdb)["dbentry"][request]["service"];
        line = "Service:\t" + Json::writeString(builder, val);

        val = (*passdb)["dbentry"][request]["username"];
        line += "\n\tUsername:\t" + Json::writeString(builder, val);

        val = (*passdb)["dbentry"][request]["password"];
        line += "\n\tPassword:\t" + Json::writeString(builder, val);

        val = (*passdb)["dbentry"][request]["notes"];
        line += "\n\tNotes:\t\t" + Json::writeString(builder, val);

        std::cout << line << std::endl;
    }
    else /* get all */
    {
        it = (*passdb)["dbentry"].begin();
        for (; it != (*passdb)["dbentry"].end(); it++)
        {
            //this gets the key from the iterator, but returns it in quotes
            request = Json::writeString(builder, it.key());

            //this removes the quotes
            request.erase(remove(request.begin(), request.end(), '\"'), request.end());

            val = (*passdb)["dbentry"][request]["service"];
            line = "Service:\t" + Json::writeString(builder, val);

            val = (*passdb)["dbentry"][request]["username"];
            line += "\n\tUsername:\t" + Json::writeString(builder, val);

            val = (*passdb)["dbentry"][request]["password"];
            line += "\n\tPassword:\t" + Json::writeString(builder, val);

            val = (*passdb)["dbentry"][request]["notes"];
            line += "\n\tNotes:\t\t" + Json::writeString(builder, val);

            std::cout << line << std::endl;
        }
    }
}

/* pre: takes in int argc and char** argv command line arguments
 * post: runs the client side keylocker program
 * returns: 0 on success, something else on failure
 */
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
     * we'd use secure_getenv, but linux lab is out of date
     * and doesn't have latest
     * glibc :(
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
    std::cout << "Found KeyLocker directory at: " << kldir << std::endl;
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
        add_entry(&passdb);
    else if (!strcmp(argv[1], "get")) /* else if 'get' is the user's command */
    {
        if (argc == 4) /* prg get service username */
            get_entry(&passdb, (std::string)argv[2] + "_" + (std::string)argv[3]);
        else if (argc == 2) /* prg get */
            get_entry(&passdb, "");
        else
            panic("usage: " + (std::string)argv[0] + " get [<service> <username>]", 1);
    }

    if (!JsonParsing::writeJson(&passdb,dbpath))
        panic("[-] Failed to write to user's password database file!", 3);

    return 0;
}
