#include "network.h"
#include "keylocker.h"

std::string HELP_TEXT = "Commands can be any of the following:\
\n\t'add [<length>]':\t\tAdds a new entry to the database with a random password \
of specified length (or prompts user for password if length was 0 or not included).\
\n\t'gen <length>':\t\t\tGenerates a random password of specified length.\
\n\t'get [<service> <username>]':\tRetrieves the entry for \
key: '<service>_<username>' from the database if they were provided,\
else returns a list of all entries. Reports error message if no such key exists.\
\n\t'edit <service> <username>':\tEdits an existing entry for key: \
'<service>_<username>' with new values provided by user. Reports error message \
if no such key exists.\
\n\t'delete <service> <username>':\tDeletes an existing entry for key: \
'<service>_<username>'. Reports error message if no such key exists.\
\n\t'quit':\t\t\t\tExits the program.\
\n\t'help':\t\t\t\tDisplays this list of commands.";

/* pre: takes in an std::string msg and int code
 * post: prints msg to stdout and exits with error code code
 */
void panic(std::string msg, int code)
{
    std::cout << msg << std::endl;
    exit(code);
}

/* pre: takes in an integer len
 * post: returns a random string of len characters, encoded in b64
 */
std::string gen(int len)
{
    return KLCrypto::genpwd(len);
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
void add_entry(Json::Value *passdb, int randlen)
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

    std::cout<<"Notes:    ";
    std::getline(std::cin,notes);
    std::cin.sync();

    if (randlen > 0)
    {
        password = gen(randlen);
        std::cout<<"Password :"<<std::endl<<password;
    }
    else
    {
        std::cout<<"Password: ";
        hideterm();
        std::getline(std::cin,password);
        showterm();
        std::cin.sync();
    }

    entry = service + "_" + username;
    // If an entry is exist, user cannot overwrite it. This will prevent some fault operation.
    if ((*passdb)["dbentry"].isMember(entry))
    {
        panic("Entry already exists", 4);
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

bool checkpass(std::string curpass)
{
    std::string testpass;
    //std::string newpass;
    std::cout<<"Current password: ";
    hideterm();
    std::getline(std::cin,testpass);
    showterm();

    if (testpass == curpass)
    {
        return true;
        //return newpass;
    }
    return false;
}

/* pre: takes in int argc and char** argv command line arguments
 * post: runs the client side keylocker program
 * returns: 0 on success, something else on failure
 */
int main()
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
    Json::StreamWriterBuilder builder;
    std::string srvname;
    std::string srvport;

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

        std::cout<<"Server hostname: ";
        std::getline(std::cin, srvname);
        
        std::cout<<"Server port: ";
        std::getline(std::cin, srvport);
        
        /*set JSON key:value pairs*/
        passdb["dbuser"]  = username;
        passdb["srvname"] = srvname;
        passdb["srvport"] = srvport;

        /*generate secret key*/
        passdb["secret"] = KLCrypto::genpwd(64);
    }
    else
    {
        std::cout << "Found KeyLocker directory at: " << kldir << std::endl;
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

    /*set server hostname variable from database, strip quotes*/
    srvname = Json::writeString(builder,passdb["srvname"]);
    srvname.erase(remove(srvname.begin(), srvname.end(), '\"'), srvname.end());

    /* set server portnum variable from database, strip quotes */
    /* note srvport is a string */
    srvport = Json::writeString(builder,passdb["srvport"]);
    srvport.erase(remove(srvport.begin(), srvport.end(), '\"'), srvport.end());

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

        if (argcnt == 0)
            continue;
        else if (args[0] == "add")
        {
            if (argcnt > 1 && atoi(args[1].c_str()))
                add_entry(&passdb, atoi(args[1].c_str()));
            else
                add_entry(&passdb, 0);
        }
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
        else if (args[0] == "chpass")
        {
            std::string newpass;
            std::string confirmnewpass;

            if (checkpass(dbpass))
            {
                std::cout<<std::endl<<"New password: ";
                hideterm();
                std::getline(std::cin,newpass);
                showterm();
                std::cout<<std::endl<<"Retype new password: ";
                hideterm();
                std::getline(std::cin,confirmnewpass);
                showterm();
                if (newpass == confirmnewpass)
                {
                    dbpass = newpass;
                    if (!JsonParsing::writeJson(&passdb,dbpath,dbpass))
                    {
                        panic("[-] Failed to update user's password", 3);
                    }
                    else
                        std::cout<<std::endl<<"Password updated successfully"<<std::endl;
                }
                else
                {
                    std::cout<<std::endl<<"Typo detected! Password not changed"<<std::endl;
                }
            }
            else
                std::cout<<std::endl<<"Incorrect password"<<std::endl;
        }
        else if (args[0] == "gen")
        {
            if (argcnt == 2)
                if (atoi(args[1].c_str()) > 0)
                    std::cout<<gen(atoi(args[1].c_str()))<<std::endl;
                else
                    std::cout<<"usage: gen <password length>"<<std::endl;
        }
        else if (args[0] == "quit")
            break;
        else if (args[0] == "help")
            std::cout<<HELP_TEXT<<std::endl;
		else if (args[0] == "register" || args[0] == "upload" || args[0] == "download")
		{
			std::string reqType;
			std::string data;

			//TODO: Need to replace secretKey with the secretkey from database
			std::string secretKey = "";
			if (args[0] == "register")
			{
				reqType = "REGISTER";
				data = reqType + ":" + "username" + ":" + secretKey + "\n"; 
			}
			else if (args[0] == "download")
			{
				reqType = "DOWNLOAD";
				data = reqType + ":" + "username" + ":" + secretKey + "\n";
			}
			else if (args[0] == "upload")
			{
				reqType = "UPLOAD";
				/*I'm not sure if I can use passdb here directly??*/
				//TODO: Need to modify the database format
				data = reqType + ":" + "username" + ":" + secretKey + ":" + "passdb" + "\n";
			}

			int ret = tls_send(srvname, atoi(srvport.c_str()), data, dbpath);
			if (ret != 0)
				std::cout << "TLS_send error code: " << ret << std::endl;
		}
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
