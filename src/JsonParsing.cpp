#include "network.h"
#include "keylocker.h"
#include "json/json.h"

//change this to a 0 for fast non-readable json output
#define READABLE 1

/* class for reading and writing to a JSON database file
 */
class JsonParsing
{
    public:
        bool readJson(Json::Value* root, std::string dbName);
        bool writeJson(Json::Value* root, std::string dbName);
};

/* pre: takes in a Json::Value* root and std::string dbName
 * post: reads the data in the Json::Value pointed to by root into a file called
 *      dbName
 * returns: true on success, false otherwise
 */
bool readJson(Json::Value* root, std::string dbName)
{
    Json::Reader reader;
    std::ifstream passdb_file;

    passdb_file.open(dbName, std::ifstream::binary);
    if (!passdb_file.is_open())
        return false;

    if (!reader.parse(passdb_file, *root, false))
    {
        std::cout  << reader.getFormattedErrorMessages() << std::endl;
        passdb_file.close();
        return false;
    }

    passdb_file.close();
    return true;
}

/* pre: takes in a Json::Value* root and std::string dbName
 * post: writes the data in the Json::Value pointed to by root into a file
 *      called dbName
 * returns: true on success, false otherwise
 */
bool writeJson(Json::Value* root, std::string dbName)
{
#ifdef READABLE
    Json::StyledWriter writer;
#else
    Json::FastWriter writer;
#endif
    std::ofstream outFile;

    outFile.open(dbName, std::ofstream::binary);
    if (!outFile.is_open())
        return false;

    //maybe we need to error check this later?
    outFile << writer.write(*root);

    outFile.close();

    return true;
}
