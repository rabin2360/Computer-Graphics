#include "CSCI5229.h"

//printing the error check values
void ErrCheck(const char* where)
{
  int err = glGetError();

  if(err)
    fprintf(stderr, "ERROR: %s [%s]\n", gluErrorString(err), where);
}
