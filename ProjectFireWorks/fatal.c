/*
 *  Print message to stderr and exit
 */
#include "CSCI5229.h"

void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}
