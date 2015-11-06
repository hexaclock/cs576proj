#include "network.h"
#include "keylocker.h"

class User
{
public:
  std::string getUserName(int uid);
  std::string getPublicKey(int uid);
  int getUIDByName(std::string username);
  int getUIDByKey(std::string pubkey);
  
private:
  std::string username;
  std::string pubkey;
  bool locked;
  
};
