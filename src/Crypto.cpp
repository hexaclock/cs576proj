#include "keylocker.h"
#include "network.h"

bool KLCrypto::dbEncrypt(std::string filepath, std::string pass)
{
  int retcode;
  std::string cmd = "/usr/bin/openssl aes-256-cbc -salt -pass pass:"
    +pass+" -in "+filepath+" -out "+filepath+".enc"+" >/dev/null 2>&1";

  //std::cout<<cmd<<std::endl;

  retcode = system(cmd.c_str());

  std::remove(filepath.c_str());

  if (retcode == 0)
    return true;
  else
    return false;
}

bool KLCrypto::dbDecrypt(std::string filepath, std::string pass)
{
  int retcode;
  std::string cmd = "/usr/bin/openssl aes-256-cbc -d -pass pass:"
    +pass+" -in "+filepath+".enc -out "+filepath+" >/dev/null 2>&1";

  //std::cout<<cmd<<std::endl;

  retcode = system(cmd.c_str());

  if (retcode == 0)
    return true;
  else
    {
      std::remove(filepath.c_str());
      return false;
    }
}

