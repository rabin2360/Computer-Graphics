#ifndef CSCI5229
#define CSCI5229

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>


#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif




#define Cos(th) cos(3.1415926/180 *(th))
#define Sin(th) sin(3.1415926/180 *(th))

#ifdef __cplusplus
extern "C"{
#endif
  
void Print(const char* format, ...);
void ErrCheck(const char* where);
unsigned int LoadTexBMP(const char* file, int mode);
void Fatal(const char* format, ...);
void drawAxes(double axesLen);
int LoadOBJ(const char* file);

void InitNoise (void);
//Perlin noise
float Noise (const float, const float, const float, const float);

#ifdef __cplusplus
}
#endif

#endif
