#ifndef CSCI5229
#define CSCI5229

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define GL_GLEXT_PROTOTYPES

#define Cos(th) cos(3.1415926/180 *(th))
#define Sin(th) sin(3.1415926/180 *(th))

#ifdef __cplusplus
extern "C"{
#endif
  
void Print(const char* format, ...);
void ErrCheck(const char* where);
unsigned int LoadTexBMP(const char* file);
void Fatal(const char* format, ...);
void drawAxes(double axesLen);


#ifdef __cplusplus
}
#endif

#endif
