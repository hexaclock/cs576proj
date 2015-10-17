#include "network.h"
#include "keylocker.h"
#include "json/json.h"

#define READABLE 1

class JsonParsing
{
    public:
        bool readJson(Json::Value* root, std::string dbName);
        bool writeJson(Json::Value* root, std::string dbName);
};

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
        return false;
    }

    return true;
}

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

    outFile << writer.write(*root);

    return true;
}
