#include "CSCI5229.h"

//axes for display
void drawAxes(double axesLen)
{
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(axesLen,0.0,0.0);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(0.0,axesLen,0.0);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(0.0,0.0,axesLen);
  glEnd();

  //giving names to the axes
  glRasterPos3d(axesLen, 0.0, 0.0);
  Print("X");
  glRasterPos3d(0.0, axesLen, 0.0);
  Print("Y");
  glRasterPos3d(0.0, 0.0, axesLen);
  Print("Z");
}
