#include "network.h"
#include "keylocker.h"

class Server
{
public:
  int start();
  bool addUser(std::string username, std::string pubkey)
  {
    return true;
  };
  bool deleteUser(std::string username)
  {
    return true;
  };
  
private:
  bool started;
};

int main(int argc, char **argv)
{
  return 0;
}
