#include "server.h"

char *get_time(char *format)
{
  char *ret;
  time_t rawtime;
  struct tm *tmp;
  ret = (char *)malloc(128*sizeof(char));

  time(&rawtime);
  tmp = localtime(&rawtime);

  strftime(ret,128,format,tmp);
  return ret;
}
