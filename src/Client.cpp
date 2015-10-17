#include "network.h"
#include "keylocker.h"

class Client
{
public:
  int connect(std::string servname);
  bool auth();
  std::string newEntry();
  bool editEntry(std::string euuid);
  bool deleteEntry(std::string euuid);
private:
  
};

void panic(std::string msg, int code)
{
  std::cout << msg << std::endl;
  exit(code);
}

int main(int argc, char **argv)
{
  if (argc < 2)
    panic("usage: "+argv[0]+" <server hostname>",1);
  return 0;
}
