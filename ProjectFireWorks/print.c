#include "CSCI5229.h"

#define LEN 8192

//printing the text on the screen
void Print(const char* format , ...)
{
  char    buf[LEN]; // Text storage
  char*   ch=buf;   // Text pointer
   
  //  Create text to be display
  va_list args;
  va_start(args,format);
  vsnprintf(buf,LEN,format,args);
  va_end(args);

  //  Display text string
  while (*ch)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}
