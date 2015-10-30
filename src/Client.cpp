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
    std::cin.sync();

    std::cout<<"Username: ";
    std::getline(std::cin,username);
    std::cin.sync();

    std::cout<<"Password: ";
    hideterm();
    std::getline(std::cin,password);
    showterm();
    std::cin.sync();

    std::cout<<"\nNotes:    ";
    std::getline(std::cin,notes);
    std::cout<<"\n";
    std::cin.sync();

    entry = service + "_" + username;
    // If an entry is exist, user cannot overwrite it. This will prevent some fault operation.
    if ((*passdb)["dbentry"].isMember(entry))
    {
        panic("Entry is already exist", 4);
    }

    (*passdb)["dbentry"][entry]["service"] = service;
    (*passdb)["dbentry"][entry]["username"] = username;
    (*passdb)["dbentry"][entry]["password"] = password;
    (*passdb)["dbentry"][entry]["notes"] = notes;
}

/* pre: takes in a Json::Value* passdb and an std::string dbentry_key
 * post: prints out the dbentry in the keylocker database pointed to by passdb
 *      that has the key dbentry_key
 */
void print_entry(Json::Value *passdb, std::string dbentry_key)
{
    Json::Value val;
    Json::StreamWriterBuilder builder;
    std::string line;

    val = (*passdb)["dbentry"][dbentry_key]["service"];
    line = "Service:\t" + Json::writeString(builder, val);

    val = (*passdb)["dbentry"][dbentry_key]["username"];
    line += "\n\tUsername:\t" + Json::writeString(builder, val);

    val = (*passdb)["dbentry"][dbentry_key]["password"];
    line += "\n\tPassword:\t" + Json::writeString(builder, val);

    val = (*passdb)["dbentry"][dbentry_key]["notes"];
    line += "\n\tNotes:\t\t" + Json::writeString(builder, val);

    std::cout << line << std::endl;
}

/* pre: takes in a Json::Value* passdb and a std::string request
 * post: if request is not empty prints out the keylocker database entry for key
 *      request, else prints out all the keylocker database entries in passdb
 */
void get_entry(Json::Value *passdb, std::string request)
{
    Json::Value::iterator it;
    Json::StreamWriterBuilder builder;

    //This line was recommended on the internet but I don't think it changes
    //  output
    //builder.settings_["indentation"] = "";

    if (!request.empty()) /* get where key=request */
    {
        if ((*passdb)["dbentry"].isMember(request))
            print_entry(passdb, request);
        else
            panic("No such entry, please check your input", 2);
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

            print_entry(passdb, request);
        }
    }
}

/* pre: takes in a Json::Value* passdb and a std::string request
 * post: delete the entry given by request
 * return: 0 on success, 1 on no such entry
 */
int delete_entry(Json::Value *passdb, std::string request)
{
    if ((*passdb)["dbentry"].isMember(request))
    {
        (*passdb)["dbentry"].removeMember(request);
        return 0;
    }
    else
        return 1;
}

/* pre: takes in a Json::Value* passdb and a std::string request
 * post: update the entry with new elements
 * return: 0 on success, 1 on no such entry
 */
int update_entry(Json::Value *passdb, std::string request)
{
    std::string service;
    std::string username;
    std::string password;
    std::string notes;
    std::string entry;
    std::string delimiter = "_";
    std::string temp;

    //split string request and parse the service and username
    temp = request;
    service = temp.substr(0, temp.find(delimiter));
    temp.erase(0, temp.find(delimiter) + delimiter.length());
    username = temp;

    if ((*passdb)["dbentry"].isMember(request))
    {
        std::cout<<"Password: ";
        hideterm();
        std::getline(std::cin,password);
        showterm();
        std::cin.sync();

        std::cout<<"\nNotes:    ";
        std::getline(std::cin,notes);
        std::cout<<"\n";
        std::cin.sync();

        entry = request;

        (*passdb)["dbentry"][entry]["service"] = service;
        (*passdb)["dbentry"][entry]["username"] = username;
        (*passdb)["dbentry"][entry]["password"] = password;
        (*passdb)["dbentry"][entry]["notes"] = notes;

        return 0;
    }
    else
        return 1;
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
    std::string dbpass;
    std::string kldir;
    std::string cmdline;
    std::vector<std::string> args;
    bool newfile;
    Json::Value passdb;

    newfile = false;

    /*if (argc < 2)
      panic("usage: " + (std::string)argv[0] + " <command> <options>", 1);*/

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
    {
        if (errno != EEXIST)
            panic("Failed to create .keylocker directory in home directory",-2);
    }
    else
        newfile = true;
    dbpath = kldir + "/" + dbname;

    if (newfile)
    {
        std::cout<<"User's password database file not found... Creating new file"<<std::endl;
        std::cout<<"New database password: ";
        hideterm();
        std::getline(std::cin,dbpass);
        showterm();
        std::cout<<std::endl;
        //std::cout<<"Generating RSA keypair..."<<std::endl;
        //KLCrypto::generateRSA(kldir+"/");
        passdb["dbuser"] = username;
    }
    else
    {
        std::cout<<"Database password: ";
        hideterm();
        std::getline(std::cin,dbpass);
        showterm();
        std::cout<<std::endl;
    }

    if (!newfile)
        if (!JsonParsing::readJson(&passdb,dbpath,dbpass))
        {
            std::cout<<"User's password database file not found... Creating new file"<<std::endl;
            passdb["dbuser"] = username;
        }

    /*detect if new file, set username if so*/
    /*   if (!passdb.isMember("dbuser"))
         passdb["dbuser"] = username;*/
    /*if 'add' is the user's command*/
    int argcnt;
    while(1)
    {
        std::cout << "KeyLocker> ";
        std::getline(std::cin,cmdline);
        std::stringstream iss(cmdline);
        copy(std::istream_iterator<std::string>(iss),
                std::istream_iterator<std::string>(),
                std::back_inserter(args));
        argcnt = args.size();

        if (args[0] == "add")
            add_entry(&passdb);
        else if (args[0] == "get") /* else if 'get' is the user's command */
        {
            if (argcnt == 3) /* prg get service username */
                get_entry(&passdb, (std::string)args[1] + "_" + (std::string)args[2]);
            else if (argcnt == 1) /* prg get */
                get_entry(&passdb, "");
            else
                std::cout<<"usage: get [<service> <username>]"<<std::endl;
        }
        else if (args[0] == "delete")
        {
            if (argcnt == 3)
            {
                std::string confirm;
                std::cout << "Are you sure wish to delete " +
                    (std::string)args[1] + "_" +
                    (std::string)args[2] + " entry? (yes/no): ";
                while (1)
                {
                    getline(std::cin, confirm);
                    std::cin.sync();
                    if (!strcmp(confirm.c_str(), "yes"))
                    {
                        int ret = delete_entry(&passdb, (std::string)args[1] +
                                "_" + (std::string)args[2]);
                        if (ret == 0)
                        {
                            std::cout << "Entry deleted" << std::endl;
                            break;
                        }
                        else if (ret == 1)
                            std::cout << "No such entry, please check your input" << std::endl;
                    }
                    else if (!strcmp(confirm.c_str(), "no"))
                        break;
                    else
                        std::cout << "Please type 'yes' or 'no': ";
                }
            }
            else
                std::cout<<"usage: delete [<service> <username>]"<<std::endl;
        }
        else if (args[0] == "edit")
        {
            if (argcnt == 3)
            {
                int ret = update_entry(&passdb, (std::string)args[1] + "_" +
                        (std::string)args[2]);
                if (ret == 0)
                {
                    std::cout << "Entry updated" << std::endl;
                }
                else if (ret == 1)
                {
                    std::cout<<"No such entry, please check your input"<<std::endl;
                }
            }
            else
                std::cout<<"usage: edit [<service> <username>]"<<std::endl;
        }
        else if (args[0] == "quit")
            break;
        else
            std::cout<<"Invalid command"<<std::endl;


        cmdline = "";
        args.clear();
        argcnt = 0;
        std::cout<<std::endl;
    }

    if (!JsonParsing::writeJson(&passdb,dbpath,dbpass))
        panic("[-] Failed to write to user's password database file!", 3);

    return 0;
}
