#include "network.h"
#include "keylocker.h"

void parse_tls_send(int argc, std::vector<std::string> argv);

std::string HELP_TEXT = "Commands can be any of the following:\
\n\t'add [<length>]':\t\tAdds a new entry to the database with a random password \
of specified length (or prompts user for password if length was 0 or not included).\
\n\t'gen <length>':\t\t\tGenerates a random password of specified length.\
\n\t'get [<service> <username>]':\tRetrieves the entry for \
key: '<service>_<username>' from the database if they were provided,\
else returns a list of all entries. Reports error message if no such key exists.\
\n\t'list [<service> <username>]':\tList is an alias for 'get'.\
\n\t'print [<service> <username>]':\tPrint is an alias for 'get'.\
\n\t'search <pattern>':\tRetrieves all entries that contain pattern in either \
their service, username or notes fields.\
\n\t'clip <service> <username>':\tCopies the password for key: '<service>_<username>' \
to the clipboard if the entry exists. Requires X window manager / xclip. \
\n\t'edit <service> <username>':\tEdits an existing entry for key: \
'<service>_<username>' with new values provided by user. Reports error message \
if no such key exists.\
\n\t'delete <service> <username>':\tDeletes an existing entry for key: \
'<service>_<username>'. Reports error message if no such key exists.\
\n\t'save':\t\t\t\tSave a local copy of the database without uploading to server.\
\n\t'register':\t\t\tAttempt to register with previously specified server.\
\n\t'chpass':\t\t\tChange database password and update record on server.\
\n\t'upload':\t\t\tSave database to disk and upload to server.\
\n\t'download':\t\t\tDownload the database file stored on the server.\
\n\t'quit':\t\t\t\tExits the program.\
\n\t'help':\t\t\t\tDisplays this list of commands.";

//remote things
std::string srvname; //the name of remote server
std::string srvport; //the port of the remote server
std::string srvuname; //username for communication with server
std::string reqType; //server request type
std::string data; //server request data
std::string secretKey; //server secret key

//local things
std::string username; //local username for creating files
std::string homepath; //local user home directory
std::string dbname; //name of the local database file
std::string dbpath; //path to the local database file
std::string dbpass; //password for the local database file
std::string kldir; //directory for keylocker files
Json::Value passdb; //the local database root

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

/* pre: takes in an integer len
 * post: returns a random string of len characters, encoded in b64
 */
std::string gen(int len)
{
    return KLCrypto::genpwd(len);
}

/* pre: takes in int argc and std::vector<std::string> argv, the first item of
 *      argv MUST be 'gen'
 * post: parses the command saved in argv and runs the appropriate function if
 *      one exists
 */
void parse_gen(int argc, std::vector<std::string> argv)
{
    if (argc == 2)
    {
        if (atoi(argv[1].c_str()) > 0)
            std::cout << gen(atoi(argv[1].c_str())) << std::endl;
    }
    else
        std::cout << "usage: gen <password length>" << std::endl;
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

    std::cout << "Service:  ";
    std::getline(std::cin,service);
    std::cin.sync();

    std::cout << "Username: ";
    std::getline(std::cin,username);
    std::cin.sync();

    std::cout << "Notes:    ";
    std::getline(std::cin,notes);
    std::cin.sync();

    if (randlen > 0)
    {
        password = gen(randlen);
        std::cout << "Password :" << std::endl << password;
    }
    else
    {
        std::cout << "Password: ";
        hideterm();
        std::getline(std::cin,password);
        showterm();
        std::cin.sync();
    }

    entry = service + "_" + username;
    // If an entry is exist, user cannot overwrite it.
    // This will prevent some fault operation.
    if ((*passdb)["dbentry"].isMember(entry))
    {
        panic("Entry already exists", 4);
    }

    (*passdb)["dbentry"][entry]["service"] = service;
    (*passdb)["dbentry"][entry]["username"] = username;
    (*passdb)["dbentry"][entry]["password"] = password;
    (*passdb)["dbentry"][entry]["notes"] = notes;
}

/* pre: takes in int argc and std::vector<std::string> argv, the first item of
 *      argv MUST be 'add'
 * post: parses the command saved in argv and runs the appropriate function if
 *      one exists
 */
void parse_add(int argc, std::vector<std::string> argv)
{
    if (argc > 2 && atoi(argv[1].c_str()))
        add_entry(&passdb, atoi(argv[1].c_str()));
    else
        add_entry(&passdb, 0);
}

/* pre: takes in a Json::Value* passdb an std::string dbentry_key a boolean
 *      show_pass, and an int num
 * post: prints out the dbentry in the keylocker database pointed to by passdb
 *      that has the key dbentry_key, if show_pass is true, prints the password
 *      in plaintext, else prints a string of *'s, if num is greater than zero
 *      prepend it to the information printed out
 */
void print_entry(Json::Value *passdb, std::string dbentry_key, bool show_pass, int num)
{
    Json::Value val;
    Json::StreamWriterBuilder builder;
    std::string line;

    line = "";
    val = (*passdb)["dbentry"][dbentry_key]["service"];
    if (num > 0)
        line += "[" + std::to_string(num) + "] ";
    line += "Service:\t" + Json::writeString(builder, val);

    val = (*passdb)["dbentry"][dbentry_key]["username"];
    line += "\n\tUsername:\t" + Json::writeString(builder, val);

    if (show_pass)
    {
        val = (*passdb)["dbentry"][dbentry_key]["password"];
        line += "\n\tPassword:\t" + Json::writeString(builder, val);
    }
    else
        line += "\n\tPassword:\t*****";

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
    int i;

    if (!request.empty()) /* get where key=request */
    {
        if ((*passdb)["dbentry"].isMember(request))
            print_entry(passdb, request, true, 0); //show passwords when requesting a specific entry
        else
            panic("No such entry, please check your input", 2);
    }
    else /* get all */
    {
        it = (*passdb)["dbentry"].begin();
        for (i = 1; it != (*passdb)["dbentry"].end(); it++, i++)
        {
            //this gets the key from the iterator, but returns it in quotes
            request = Json::writeString(builder, it.key());

            //this removes the quotes
            request.erase(remove(request.begin(), request.end(), '\"'), request.end());

            print_entry(passdb, request, false, i); //don't show passwords when printing the whole list
        }
    }
}

/* pre: takes in int argc and std::vector<std::string> argv, the first item of
 *      argv MUST be 'get' or 'list'
 * post: parses the command saved in argv and runs the appropriate function if
 *      one exists
 */
void parse_get(int argc, std::vector<std::string> argv)
{
    if (argc == 3) /* get service username */
        get_entry(&passdb, (std::string)argv[1] + "_" + (std::string)argv[2]);
    else if (argc == 1) /* get */
        get_entry(&passdb, "");
    else
        std::cout << "usage: get [<service> <username>]" << std::endl;
}

/* pre: takes in a Json::Value* passdb and a std::string pattern
 * post: if pattern is not empty prints out the keylocker database entries that
 *      match the pattern
 */
void search(Json::Value *passdb, std::string pattern)
{
    Json::Value val;
    Json::Value::iterator it;
    Json::StreamWriterBuilder builder;
    std::string key;
    std::string service;
    std::string username;
    std::string notes;
    int i;

    if (pattern.empty())
        return;

    it = (*passdb)["dbentry"].begin();
    for (i = 1; it != (*passdb)["dbentry"].end(); it++, i++)
    {
        key = Json::writeString(builder, it.key());
        key.erase(remove(key.begin(), key.end(), '\"'), key.end());

        val = (*passdb)["dbentry"][key]["service"];
        service = Json::writeString(builder, val);

        val = (*passdb)["dbentry"][key]["username"];
        username = Json::writeString(builder, val);

        val = (*passdb)["dbentry"][key]["notes"];
        notes = Json::writeString(builder, val);

        if (service.find(pattern) != std::string::npos ||
                username.find(pattern) != std::string::npos ||
                notes.find(pattern) != std::string::npos)
        {
            print_entry(passdb, key, false, i);
            continue;
        }
    }
}

/* pre: takes in int argc and std::vector<std::string> argv, the first item of
 *      argv MUST be 'search'
 * post: parses the command saved in argv and runs the appropriate function if
 *      one exists
 */
void parse_search(int argc, std::vector<std::string> argv)
{
    if (argc != 2)
        std::cout << "usage: search <pattern>" << std::endl;
    else
        search(&passdb, argv[1]);
}

/* pre: takes in a Json::Value* passdb and a std::string request
post: copies the pass for the request to the clipboard,
then overwrites clipboard when user is done
*/
void clip(Json::Value *passdb, std::string request)
{
    std::string pass;

    // check if xclip exists
    if (access("/usr/bin/xclip", F_OK))
    {
        std::cout << "Please install xclip in order to use this feature"
                  << std::endl;
        return;
    }

    if ((*passdb)["dbentry"].isMember(request))
    {
        pass = (*passdb)["dbentry"][request]["password"].asString();
        pass = "echo -n \"" + pass + "\" | xclip -selection clipboard";
        system((const char*)pass.c_str());

        std::cout << "Password copied to clipboard. "
                  << std::endl
                  << "Press enter to overwrite clipboard."
                  << std::endl;

        std::getline(std::cin, pass);
        pass = "echo -n \" \" | xclip -selection clipboard";
        system((const char*)pass.c_str());
    }
    else
        std::cout << "Entry doesn't exist!" << std::endl;
}

/* pre: takes in int argc and std::vector<std::string> argv, the first item of
 *      argv MUST be 'clip'
 * post: parses the command saved in argv and runs the appropriate function if
 *      one exists
 */
void parse_clip(int argc, std::vector<std::string> argv)
{
    if (argc == 3)
        clip(&passdb, (std::string)argv[1] + "_" + (std::string)argv[2]);
    else
        std::cout << "usage: clip [<service> <username>]" << std::endl;
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

/* pre: takes in int argc and std::vector<std::string> argv, the first item of
 *      argv MUST be 'delete'
 * post: parses the command saved in argv and runs the appropriate function if
 *      one exists
 */
void parse_delete(int argc, std::vector<std::string> argv)
{
    if (argc == 3)
    {
        if (prompt_y_n("Are you sure you wish to delete "
                    + (std::string)argv[1] + "_" + (std::string)argv[2] + "?",
                    ""))
        {
            int ret = delete_entry(&passdb, (std::string)argv[1] +
                    "_" + (std::string)argv[2]);
            if (ret == 0)
            {
                std::cout << "Entry deleted" << std::endl;
            }
            else if (ret == 1)
                std::cout << "No such entry, please check your input" << std::endl;
        }
    }
    else
        std::cout << "usage: delete [<service> <username>]" << std::endl;
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
        std::cout << "Password: ";
        hideterm();
        std::getline(std::cin,password);
        showterm();
        std::cin.sync();

        std::cout << "\nNotes:    ";
        std::getline(std::cin,notes);
        std::cout << "\n";
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

/* pre: takes in int argc and std::vector<std::string> argv, the first item of
 *      argv MUST be 'edit'
 * post: parses the command saved in argv and runs the appropriate function if
 *      one exists
 */
void parse_edit(int argc, std::vector<std::string> argv)
{
    if (argc == 3)
    {
        int ret = update_entry(&passdb, (std::string)argv[1] + "_" +
                (std::string)argv[2]);
        if (ret == 0)
        {
            std::cout << "Entry updated" << std::endl;
        }
        else if (ret == 1)
        {
            std::cout << "No such entry, please check your input" << std::endl;
        }
    }
    else
        std::cout << "usage: edit [<service> <username>]" << std::endl;
}

/* pre: takes in a std:string curpass
 * post: checks if the password is correct
 * return: returns true if curpass is correct, else false
 */
bool checkpass(std::string curpass)
{
    std::string testpass;
    //std::string newpass;
    std::cout << "Current password: ";
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

/* pre: takes in int argc and std::vector<std::string> argv, the first item of
 *      argv MUST be 'chpass'
 * post: parses the command saved in argv and runs the appropriate function if
 *      one exists
 */
void parse_chpass(int argc, std::vector<std::string> argv)
{
    std::string newpass;
    std::string confirmnewpass;
    int ret;

    if (checkpass(dbpass))
    {
        std::cout << std::endl << "New password: ";
        hideterm();
        std::getline(std::cin,newpass);
        showterm();
        std::cout << std::endl << "Retype new password: ";
        hideterm();
        std::getline(std::cin,confirmnewpass);
        showterm();
        if (newpass == confirmnewpass)
        {
            secretKey = KLCrypto::sha256sum(dbpass);
            std::string newSecretKey = KLCrypto::sha256sum(newpass);
            data = "CHPASS:" + srvuname + ":" + secretKey + ":" + newSecretKey + "\n";
            //try to update pass on server
            if ( (ret = tls_send(srvname, atoi(srvport.c_str()), data, dbpath)) == 0 )
            {
                //try to update db with newpass
                if (!JsonParsing::writeJson(&passdb,dbpath,newpass))
                {
                    std::cout << std::endl << "Failed to update local password" << std::endl;
                    std::cout << "Reverting change..." << std::endl;
                    //revert!
                    data = "CHPASS:" + srvuname + ":" + newSecretKey + ":" + secretKey + "\n";
                    if ( (ret = tls_send(srvname, atoi(srvport.c_str()), data, dbpath)) == 0 )
                        std::cout << "Reverted!" << std::endl;
                    //this should hopefully never happen...
                    else
                        std::cout << "Failed to revert! Please contact your local sysadmin"
                                  << std::endl
                                  << "or you may face issues syncing with the server."
                                  << std::endl;
                }
                else
                    std::cout << std::endl << "Password updated successfully" << std::endl;

            }
            else
            {
                std::cout << std::endl << "Failed to update password" << std::endl;
                return;
            }
            dbpass = newpass;
            secretKey = newSecretKey;
            //send re-encrypted database to server if everything went well
            std::vector<std::string> passargs;
            passargs.push_back("upload");
            parse_tls_send(3, passargs);
        }
        else
        {
            std::cout << std::endl << "Typo detected! Password not changed" << std::endl;
        }
    }
    else
        std::cout << std::endl << "Incorrect password" << std::endl;
}

/*
 returns: -1 if failed to connect
           0 if local file is newer
           1 if remote file is newer
*/
int timestamp_cmp()
{
    struct stat filestats;
    time_t modtime;
    stat(dbpath.c_str(), &filestats);
    modtime = filestats.st_mtime;
    secretKey = KLCrypto::sha256sum(dbpass);

    reqType = "TIMESTAMP";
    data = reqType + ":" + srvuname + ":" + secretKey + ":" + std::to_string(modtime) + "\n";

    int ret = tls_send(srvname, atoi(srvport.c_str()), data, dbpath);

    return ret;

}

/* pre: takes in int argc and std::vector<std::string> argv, the first item of
 *      argv MUST be 'register', 'upload' or 'download'
 * post: parses the command saved in argv and runs the appropriate function if
 *      one exists
 */
void parse_tls_send(int argc, std::vector<std::string> argv)
{
    /*std::string secretKey = Json::writeString(builder,passdb["secret"]);;
      secretKey.erase(remove(secretKey.begin(), secretKey.end(), '\"'), secretKey.end());*/
    int ret;
    secretKey = KLCrypto::sha256sum(dbpass);
    //DEBUG
    //std::cout << secretKey << std::endl;

    if (argv[0] == "register")
    {
        reqType = "REGISTER";
        data = reqType + ":" + srvuname + ":" + secretKey + "\n";
        if ( (ret = tls_send(srvname, atoi(srvport.c_str()), data, dbpath)) != 0 )
            std::cout<<"Failed to register with server. User may already exist."<<std::endl;
    }
    else if (argv[0] == "download")
    {
        reqType = "DOWNLOAD";
        data = reqType + ":" + srvuname + ":" + secretKey + "\n";
        if ( (ret = tls_send(srvname, atoi(srvport.c_str()), data, dbpath)) != 0 )
            std::cout<<"Failed to download database"<<std::endl;
    }
    else if (argv[0] == "upload")
    {
        /*write file before doing upload*/
        if (!JsonParsing::writeJson(&passdb,dbpath,dbpass))
            panic("[-] Failed to write to user's password database file!", 3);

        reqType = "UPLOAD";
        //Read database file into string
        Json::Reader reader;
        std::ifstream passdb_file;

        //DEBUG
        //std::cout << dbpath << std::endl;

        passdb_file.open(dbpath);
        if (!passdb_file.is_open())
            panic("Failed to open database file. TLS send failed!", -1);

        std::string db_b64((std::istreambuf_iterator<char>(passdb_file)), std::istreambuf_iterator<char>());

        data = reqType + ":" + srvuname + ":" + secretKey + ":" + db_b64 + "\n";
        passdb_file.close();

        if ( (ret = tls_send(srvname, atoi(srvport.c_str()), data, dbpath)) != 0 )
            std::cout<<"Failed to upload database to server"<<std::endl;
    }
}

void parse_save(int argc, std::vector<std::string> argv)
{
    if (!JsonParsing::writeJson(&passdb,dbpath,dbpass))
        std::cout << "Failed to save local database" << std::endl;
    else
        std::cout << "Successfuly saved local database" << std::endl;
}

/* pre: none
 * post: prints the Keylocker prompt
 */
void prompt()
{
    std::cout << "KeyLocker> ";
}

/* pre: none
 * post: gets the user input
 * return: a std::vector<std::string> of the user's command
 */
std::vector<std::string> read_input()
{
    std::string line;
    std::vector<std::string> vect;

    std::getline(std::cin, line);
    std::stringstream iss(line);
    copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(vect));

    return vect;
}

/* pre: takes in an int argc and std::vector<std::string> argv
 * post: parses the command saved in argv and runs the appropriate function if
 *      one exists
 * return: true if command was handled, false if we need to exit
 */
bool parse_command(int argc, std::vector<std::string> argv)
{
    if (argc == 0)
        return true;
    else if (argv[0] == "add")
        parse_add(argc, argv);
    else if (argv[0] == "get" ||
            argv[0] == "list" ||
            argv[0] == "print")
        parse_get(argc, argv);
    else if (argv[0] == "search")
        parse_search(argc, argv);
    else if (argv[0] == "clip")
        parse_clip(argc, argv);
    else if (argv[0] == "delete")
        parse_delete(argc, argv);
    else if (argv[0] == "edit")
        parse_edit(argc, argv);
    else if (argv[0] == "chpass")
        parse_chpass(argc, argv);
    else if (argv[0] == "gen")
        parse_gen(argc, argv);
    else if (argv[0] == "register" || argv[0] == "upload" || argv[0] == "download")
        parse_tls_send(argc, argv);
    else if (argv[0] == "save")
        parse_save(argc, argv);
    else if (argv[0] == "quit" || argv[0] == "exit" || argv[0] == "q")
        return false;
    else if (argv[0] == "help")
        std::cout << HELP_TEXT << std::endl;
    else
        std::cout << "Invalid command" << std::endl;

    return true;
}

bool local_db_exists(const std::string &kldir, const std::string &dbpath)
{
    bool ret = false;
    int result;

    //try to create a new directory
    if ( (result = mkdir(kldir.c_str(),0700)) == -1 )
    {
        //something is wrong if we can't create a directory
        if (errno != EEXIST)
        {
            panic("Failed to create .keylocker directory in home directory",-2);
        }
        //we know the directory exists now, but does the db file?
        else
        {
            struct stat buf;
            if ( stat(dbpath.c_str(), &buf) == 0 )
                ret = true;
            else
                ret = false;
        }
    }
    return ret;
}

bool validate_portnum(const std::string &portstr)
{
    int portnum = atoi(portstr.c_str());

    if (portstr.size() > 5)
        return false;

    for (int i=0; i<(int)portstr.size(); ++i)
    {
        if (!isdigit(portstr[i]))
            return false;
    }

    if (portnum < 1 || portnum > 65535)
        return false;
    return true;
}

bool validate_username(const std::string &uname)
{
    if (uname.size() > 64)
        return false;

    for (int i=0; i<(int)uname.size(); ++i)
    {
        if (!isalnum(uname[i]) && uname[i] != '-'
            && uname[i] != '_')
            return false;
    }
    return true;
}

/* pre: takes in int argc and char** argv command line arguments
 * post: runs the client side keylocker program
 * returns: 0 on success, something else on failure
 */
int main()
{
    char *tmp;
    //int result;

    std::vector<std::string> args; //the vector for the command the user types
    bool newfile; //flag to designate whether or not a new file has been created
    Json::StreamWriterBuilder builder; //a jsoncpp construct to help work with Json

    newfile = false;

    /* we'd use secure_getenv, but linux lab is out of date
     * and doesn't have latest glibc :(
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
    dbpath = kldir + "/" + dbname;

    newfile = !local_db_exists(kldir,dbpath);

    if (newfile)
    {
        int n;
        std::cout << "No local password database was found..." << std::endl;

        //check if user wants to download database from server
        if (prompt_y_n("Would you like to download a database from a server?", ""))
        {
            std::cout << "Server hostname: ";
            std::getline(std::cin, srvname);

            do
            {
                std::cout << "Server port: ";
                std::getline(std::cin, srvport);
                if (!validate_portnum(srvport))
                    std::cout << "Invalid port"
                              << std::endl;
            } while (!validate_portnum(srvport));

            do
            {
                std::cout << "Server username: ";
                std::getline(std::cin, srvuname);
                if (!validate_username(srvuname))
                    std::cout << "Username must be alphanumeric"
                              << std::endl;
            } while (!validate_username(srvuname));

            std::cout << "Password: ";
            hideterm();
            std::getline(std::cin, dbpass);
            showterm();
            std::cout << "\n";

            //download database
            secretKey =  KLCrypto::sha256sum(dbpass);
            data = "DOWNLOAD:" + srvuname + ":" + secretKey + "\n";
            n = tls_send(srvname, atoi(srvport.c_str()), data, dbpath);
            if (n != 0)
            {
                std::cout << "Failed to download database from server"
                          << std::endl;
                exit(-4);
            }

            //if we successfully downloaded database, try to load it//
            else if (!JsonParsing::readJson(&passdb,dbpath,dbpass))
                std::cout << "Failed to open downloaded database" << std::endl;
        }
        else //create new database
        {
            std::cout << "Creating new local database..." << std::endl;

            std::cout << "New database password: ";
            hideterm();
            std::getline(std::cin, dbpass);
            showterm();
            std::cout << std::endl;

            std::cout << "Server hostname: ";
            std::getline(std::cin, srvname);

            do
            {
                std::cout << "Server port: ";
                std::getline(std::cin, srvport);
                if (!validate_portnum(srvport))
                    std::cout << "Invalid port"
                              << std::endl;
            } while (!validate_portnum(srvport));

            do
            {
                std::cout << "Server username: ";
                std::getline(std::cin, srvuname);
                if (!validate_username(srvuname))
                    std::cout << "Username must be alphanumeric"
                              << std::endl;
            } while (!validate_username(srvuname));

            secretKey = KLCrypto::sha256sum(dbpass);

            //ask user if they want to register with the server
            if (prompt_y_n("Would you like to register with this server?", ""))
            {
                data = "REGISTER:" + srvuname + ":" + secretKey + "\n";
                n = tls_send(srvname, atoi(srvport.c_str()), data, dbpath);
                if (n != 0)
                    std::cout << "Failed to register with server. User may already exist." << std::endl;
            }
        }

        /*set JSON key:value pairs*/
        passdb["dbuser"]  = username;
        passdb["srvname"] = srvname;
        passdb["srvport"] = srvport;
        passdb["srvuname"] = srvuname;
    }
    else
    {
        std::cout << "Found KeyLocker directory at: " << kldir << std::endl;
        std::cout << "Database password: ";
        hideterm();
        std::getline(std::cin,dbpass);
        showterm();
        std::cout << std::endl;
    }

    if (!newfile)
    {
        if (!JsonParsing::readJson(&passdb,dbpath,dbpass))
        {
            std::cout << "User's password database file not found... Creating new file" << std::endl;
            passdb["dbuser"] = username;
            passdb["srvname"] = srvname;
            passdb["srvport"] = srvport;
            passdb["srvuname"] = srvuname;
        }
    }

    /*set server hostname variable from database, strip quotes*/
    srvname = Json::writeString(builder,passdb["srvname"]);
    srvname.erase(remove(srvname.begin(), srvname.end(), '\"'), srvname.end());

    /* set server portnum variable from database, strip quotes */
    /* note srvport is a string */
    srvport = Json::writeString(builder,passdb["srvport"]);
    srvport.erase(remove(srvport.begin(), srvport.end(), '\"'), srvport.end());
    if (!validate_portnum(srvport))
    {
        std::cout << "Invalid server port in database file!" << std::endl;
        exit(3);
    }

    /*set server username var from database, strip quotes*/
    srvuname = Json::writeString(builder,passdb["srvuname"]);
    srvuname.erase(remove(srvuname.begin(), srvuname.end(), '\"'), srvuname.end());
    if (!validate_username(srvuname))
    {
        std::cout << "Invalid server username in database file!" << std::endl;
        exit(3);
    }

    int tscheck = timestamp_cmp();

    if (tscheck == -1)
    {
        std::cout << "Server is offline. Continuing in offline mode." << std::endl;
    }

    else if (!newfile && (tscheck == 0 || tscheck == 1))
    {
        if (tscheck == 0)
        {
            if (prompt_y_n("Local database is newer, download from server anyway?", "no"))
            {
                std::cout << "Trying to download database from server..." << std::endl;
                //download copy of database from server
                std::vector<std::string> param;
                std::string cmd = "download";
                param.push_back(cmd);
                parse_tls_send(1,param);
                //parse downloaded database
                if (!JsonParsing::readJson(&passdb,dbpath,dbpass))
                    panic("Failed to open downloaded database file!", -5);

            }
        }
        else
        {
            std::cout << "Database on server is newer..." << std::endl;
            std::cout << "Trying to download database from server..." << std::endl;
            //download copy of database from server
            std::vector<std::string> param;
            std::string cmd = "download";
            param.push_back(cmd);
            parse_tls_send(1,param);
            //parse downloaded database
            if (!JsonParsing::readJson(&passdb,dbpath,dbpass))
                panic("Failed to open downloaded database file!", -5);

        }
    }

    while(1) /* main loop */
    {
        prompt();
        args = read_input();
        if (!parse_command(args.size(), args))
            break;
        std::cout << std::endl;
    }
    if (prompt_y_n("Would you like to save before exiting?", "yes"))
    {
        if (!JsonParsing::writeJson(&passdb,dbpath,dbpass))
            panic("[-] Failed to save the password database file!", 3);
        if (prompt_y_n("Would you like to upload the database to the server?", "yes"))
        {
            std::vector<std::string> param;
            std::string cmd = "upload";
            param.push_back(cmd);
            parse_tls_send(1,param);
        }
    }

    return 0;
}
