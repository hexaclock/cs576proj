#include "server.h"

/*
 *given a string and an int for error code
 *prints message and exits with error code
*/
void panic(std::string s, unsigned int n)
{
    std::cout<<s<<std::endl;
    exit(n);
}

