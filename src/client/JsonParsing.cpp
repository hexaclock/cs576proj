#include "network.h"
#include "keylocker.h"

//change this to a 0 for fast non-readable json output
#define READABLE 1

/* pre: takes in a Json::Value* root and std::string dbName
 * post: reads the data in the Json::Value pointed to by root into a file called
 *      dbName
 * returns: true on success, false otherwise
 */
bool JsonParsing::readJson(Json::Value* root, std::string dbName, std::string key)
{
    Json::Reader reader;
    std::ifstream passdb_file;
    std::string ptxtjson;
    
    passdb_file.open(dbName);    
    
    if (!passdb_file.is_open())
      return false;

    //read file into std::string ctxtjson//
    std::string ctxtjson( (std::istreambuf_iterator<char>(passdb_file)),
                          std::istreambuf_iterator<char>() );
    //decrypt into ptxtjson//
    if ( (ptxtjson = KLCrypto::dbDecrypt(ctxtjson,key)) == "" )
    {
        passdb_file.close();
        exit(-3);
        return false;
    }

    //DEBUG
    //std::cout<<ptxtjson<<std::endl;
    
    if (!reader.parse(ptxtjson, *root, false))
      {
        std::cout  << reader.getFormattedErrorMessages() << std::endl;
        passdb_file.close();
        exit(-3);
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
bool JsonParsing::writeJson(Json::Value* root, std::string dbName, std::string key)
{
#ifdef READABLE
    Json::StyledWriter writer;
#else
    Json::FastWriter writer;
#endif
    std::ofstream outFile;

    std::string ptxtjson = writer.write(*root);
    std::string ctxtjson = KLCrypto::dbEncrypt(ptxtjson,key);

    if (ctxtjson == "")
        return false;

    outFile.open(dbName);
    if (!outFile.is_open())
        return false;

    outFile << ctxtjson;
    outFile.close();

    return true;
}
