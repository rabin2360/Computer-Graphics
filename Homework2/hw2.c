//Name: Rabin Ranabhat
//Assignment2: 3D scene

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define GL_GLEXT_PROTOTYPES

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

static GLfloat view_rotx = 10.0, view_roty = -10.0, view_rotz = 0.0;

bool draw_axis = true;

//dimension of the world
double dim = 3.0;
GLfloat h = 1;

//variables for perspective projections
double fov = 55;
double aspectRatio = 1;
int projectionMode = 0;

//movement variables in Perspective Mode
double Lx = 0, Ly = 0, Lz = 0;
double Ex = 0.0, Ey=0.0, Ez=0.0;

//camera angle for navigation
GLfloat angle = 0;

//  Maximum amount of text
#define LEN 8192

void Project(int mode)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if(mode)
    //dimension math ensures that only 3 z-buffer bits are lost
    gluPerspective(fov, aspectRatio, dim/4, 4*dim);
  else
    glOrtho(-aspectRatio*dim,+aspectRatio*dim,-dim,+dim,-dim,+dim);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}

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


//printing the error check values
void ErrCheck(char* where)
{
  int err = glGetError();

  if(err)
    fprintf(stderr, "ERROR: %s [%s]\n", gluErrorString(err), where);
}

//axes for display
void drawAxes()
{
  //drawing the axes
  GLfloat axesLen = 1.5;
  
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

//draw the floor for the display
void drawFloor()
{
  glPushMatrix();

  //drawing the floor
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_QUADS);

  glVertex3f(-1.8,0.0,-1.5);  
  glVertex3f(1.5,0.0,-1.5);
  glVertex3f(1.5,0.0,1.2);
  glVertex3f(-1.8,0.0,1.2);
  
  glEnd();
  
  glPopMatrix();   
}

void drawText()
{
  glPushMatrix();
  glWindowPos2i(5,5);

  if(projectionMode == 1)
    {
      Print("Angle=%g, Projection: Traverse",angle);
    }
  else
    {
      Print("Angle=%g,%g, Projection: %s",view_rotx,view_roty, projectionMode?"Perspective":"Orthographic");
    }

  glPopMatrix();
}



//drawing cone using radius and height parameters
void drawCone(double h, double r, float tx, float ty, float tz, float sx, float sy, float sz)
{
  glPushMatrix();

  int i;

  glTranslatef(tx, ty, tz);
  glScalef(sx, sy, sz);
 
  glBegin(GL_TRIANGLES);
  
  for(i = 0; i<=360; i+=5)
    {
      glVertex3f(r*Sin(i),0,-r*Cos(i));
      glVertex3f(0,h,0);
      glVertex3f(r*Sin(i+20),0,-r*Cos(i+20));
    }

  glEnd();
  glPopMatrix();
  
}

void drawCylinder(double h, double r, float tx, float ty, float tz, float sx, float sy, float sz)
{
  glPushMatrix();
  
  glTranslatef(tx,ty,tz);
  glScalef(sx,sy,sz);

  glBegin(GL_QUADS);
  int i = 0;
  for(i = 0; i<=360; i+=5)
    {
      glVertex3f(r*Sin(i),0,-r*Cos(i));
      glVertex3f(r*Sin(i),h,-r*Cos(i));
      glVertex3f(r*Sin(i+20),h,-r*Cos(i+20));
      glVertex3f(r*Sin(i+20),0,-r*Cos(i+20));
    }

  glEnd();
  
  glPopMatrix();
}

void drawTower(double h, double r)
{
  glPushMatrix();

  //drawing the roof
  glColor3f(0.18, 0.30, 0.30);
  drawCone(1.2*h,1*r,
	   0.0,1.0,0.0,
	   0.5,1,0.5);

  //underside of the roof
  glColor3f(0.41, 0.41, 0.41);
  drawCone(0,1*r,
	   0.0,1.0,0.0,
	   0.5,1,0.5);

  //body
  glColor3f(0.66, 0.66, 0.66);
  drawCylinder(2*h,0.5*r,
	       0.0,0.0,0.0,
	       0.5,1,0.5);

  glPopMatrix();
}

void drawWall(double h)
{
  glPushMatrix();
  glColor3f(0.86, 0.86, 0.86);

  glBegin(GL_QUADS);
  glVertex3f(0.0,0.0,0.0);
  glVertex3f(0.0,h,0.0);
  glVertex3f(h,h,0.0);
  glVertex3f(h,0.0,0.0);
  glEnd();
  
  glPopMatrix();
}

//draw castle
void drawCastle()
{
  //front tower - left
  glPushMatrix();
  glTranslatef(0.5,0.0,0.0);
  drawTower(0.5, 0.5);
  glPopMatrix();

  //front tower - right
  glPushMatrix();
  glTranslatef(1.3,0.0,0.0);
  //glScalef(1.3,1.0,1.3);
  drawTower(0.5, 0.5);
  glPopMatrix();

  //back tower - left
  glPushMatrix();
  glTranslatef(0.5,0.0,-1.1);
  drawTower(0.5, 0.5);
  glPopMatrix();

  //back tower - right
  glPushMatrix();
  glTranslatef(1.3,0.0,-1.1);
  //glScalef(1.3,1.0,1.3);
  drawTower(0.5, 0.5);
  glPopMatrix();

  //front wall
  glPushMatrix();
  glTranslatef(0.4,0.0,0.0);
  drawWall(1);
  glPopMatrix();

  //left wall
  glPushMatrix();
  glTranslatef(0.4,0.0,-1.0);
  glRotatef(270,0,1,0);
  drawWall(1);
  glPopMatrix();

  //right wall
  glPushMatrix();
  glTranslatef(1.3,0.0,0.0);
  glRotatef(90,0,1,0);
 
  drawWall(1);
  glPopMatrix();

  //right wall
  glPushMatrix();
  glTranslatef(1.4,0.0,-1.1);
  glRotatef(180,0,1,0);
  drawWall(1);
  glPopMatrix();

  //roof
  glPushMatrix();
  glTranslatef(0.4,1.0,0.0);
  glRotatef(-90,1,0,0);
  glScalef(0.85,1,1);
  drawWall(1.1);
  glPopMatrix();
}

void drawTree(double h, double r)
{
  //color of the tree cones
  glColor3f(0.13,0.54,0.13);

  //drawing a cone-1
  drawCone(0.9*h,0.6*r,
	   0.0,0.6,0.0,
	   0.5,1,0.5);

  //drawing a cone-2
  drawCone(0.9*h,0.6*r,
	   0.0,0.8,0.0,
	   0.5,1,0.5);


  //drawing a cone-3
  drawCone(0.9*h,0.6*r,
	   0.0,1.0,0.0,
	   0.5,1,0.5);

  //color for the underside of the trees
  glColor3f(0.19,0.80,0.19);
  //base of cone-1
  drawCone(0,0.6*r,
	   0.0,0.6,0.0,
	   0.5,1,0.5);

  //base of cone-2
  drawCone(0,0.6*r,
	   0.0,0.8,0.0,
	   0.5,1,0.5);

  //base of cone-3
  drawCone(0,0.6*r,
	   0.0,1.0,0.0,
	   0.5,1,0.5);


  glColor3f(0.54,0.27,0.074);
  //drawing stalk of the tree
  drawCylinder(1.6*h,0.2*r,
	       0.0,0.0,0.0,
	       0.5,1,0.5);
 
}

//display function
void display() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  
  glLoadIdentity();

  //determining the projection mode
  if(projectionMode == 1)
    {
      //projection for FP mode
      gluLookAt(Ex,Ey,Ez ,Ex+Lx,Ly,Ez+Lz, 0,1,0);
    }
  else if(projectionMode == 2)
    {
      //projection for Perspective mode
      Ex = -2*dim*Sin(view_roty)*Cos(view_rotx);
      Ey = +2*dim        *Sin(view_rotx);
      Ez = +2*dim*Cos(view_roty)*Cos(view_rotx);
      gluLookAt(Ex,Ey,Ez ,0,0,0, 0,Cos(view_rotx),0);
    }

  else
    {
      //rotation functions
      glRotatef(view_rotx, 1.0, 0.0, 0.0);
      glRotatef(view_roty, 0.0, 1.0, 0.0);
      glRotatef(view_rotz, 0.0, 0.0, 1.0);
    }

  //draw the floor
  drawFloor();
  
  //draw tree
  glPushMatrix();
  glTranslatef(0,0,-1.3);
  glScalef(0.7,0.8,0.7);
  drawTree(0.5, 0.9);
  glPopMatrix();

  //draw tree
  glPushMatrix();
  glTranslatef(-0.3,0,-1.3);
  glScalef(0.7,0.4,0.7);
  drawTree(0.5, 0.9);
  glPopMatrix();

  //draw tree
  glPushMatrix();
  glTranslatef(-0.7,0,-1.3);
  glScalef(0.7,0.6,0.7);
  drawTree(0.5, 0.9);
  glPopMatrix();
  
  //draw tree
  glPushMatrix();
  glTranslatef(-1.15,0,-1.3);
  glScalef(0.7,0.8,0.7);
  drawTree(0.5, 0.9);
  glPopMatrix();


  //draw tree
  glPushMatrix();
  glTranslatef(-0.2,0,-0.9);
  glScalef(0.7,0.6,0.7);
  drawTree(0.5, 0.9);
  glPopMatrix();

  //draw tree
  glPushMatrix();
  glTranslatef(-0.5,0,-0.9);
  glScalef(0.7,0.3,0.7);
  drawTree(0.7, 0.9);
  glPopMatrix();

  //draw castle
  drawCastle();

  glPushMatrix();
  
  glTranslatef(-1.3,0.0,1.3);
  glRotatef(90,0,1,0);
  glScalef(0.3,0.3,0.3);
  drawCastle();
  glPopMatrix();

  //toggle the axis display
  if(draw_axis)
    drawAxes();
  
  //display the text
  drawText();
  
  ErrCheck("display");

  glFlush();	
  glutSwapBuffers();	
	
}

//reshaping the screen and adjusting the coordinates
static void reshape(int width, int height) {
  aspectRatio = (height>0)?(GLfloat) width / (GLfloat) height : 1;
  glViewport(0, 0, (GLint) width, (GLint) height);
  Project(projectionMode);
}

//special characters function
static void special(int k, int x, int y) {
  GLfloat speed = 0.25;
 
  switch (k) {   
  case GLUT_KEY_UP:
    //if traverse mode, move forward, else, rotate about X-axis
    if(projectionMode == 1)
      {
	Ex += Lx * speed;
	Ez += Lz * speed;
      }
    else
      view_rotx += 5.0;
    
    break;
    
  case GLUT_KEY_DOWN:
    //if traverse mode, move back, else, rotate about X-axis
    if(projectionMode == 1)
      {
	Ex -= Lx * speed;
	Ez -= Lz * speed;
      }
    else
      view_rotx -= 5.0;

    break;

  case GLUT_KEY_LEFT:
    //if traverse mode, rotate left, else, rotate about Y-axis
    if(projectionMode == 1)
      {
	//rotate left
	angle -= 5;
	Lx = 2*Sin(angle);
	Lz = -2*Cos(angle);
      }
    else
      view_roty += 5.0;

    break;
    
  case GLUT_KEY_RIGHT:
    //if traverse mode, rotate right, else, rotate about Y-axis
    if(projectionMode == 1)
      {
	//rotate right
	angle += 5;
	Lx = 2*Sin(angle);
	Lz = -2*Cos(angle);
      }
    else
      view_roty -= 5.0;
    break;
    
  default:
    return;
    
  }

  view_roty = fmod(view_roty, 360);
  view_rotx = fmod(view_rotx, 360);
  angle = fmod(angle,360);
  
  Project(projectionMode);
  glutPostRedisplay();
}

//special key functions
static void key(unsigned char k, int x, int y) {
  switch (k) {
    //pressing escape key
  case 27:
    exit(0);
    break;
    
  case 'X':
  case 'x':
    if(draw_axis)
      draw_axis = false;
    else
      draw_axis = true;
    break;

  case 'O':
  case 'o':
    projectionMode = 0;
    break;

  case 'P':
  case 'p':
    projectionMode = 2;

    //setting my eye position
    Ex = -2*dim*Sin(view_roty)*Cos(view_rotx);
    Ey = +2*dim        *Sin(view_rotx);
    Ez = +2*dim*Cos(view_roty)*Cos(view_rotx);

    //setting the lookAt position
    Lx = -Ex;
    Ly = Ey;
    Lz = -Ex;
    break;

  case 'R':
  case 'r':
    //reset the rotation angles only in either projection or ortho mode but not
    //in traverse mode
    if(projectionMode !=1){
      view_rotx = 0;
      view_roty = 0;
      view_rotz = 0;
    }
    break;


  case 'I':
  case 'i':
    projectionMode = 1;
    angle = 0;
    
    //setting my eye position
    Ex = -2*dim*Sin(0)*Cos(10);
    Ey = +2*dim        *Sin(10);
    Ez = +2*dim*Cos(0)*Cos(10);

    //setting the lookAt position
    Lx = sin(angle);
    Lz = -cos(angle);
    Ly = Ey;
    break;
  default:
    return;
  }

  Project(projectionMode);
  glutPostRedisplay();
}

int main(int argc,char* argv[]) {

  glutInit(&argc,argv);	
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  //size of the initial window
  glutInitWindowPosition(50, 50);
  glutInitWindowSize(600, 600);
  //Window title
  glutCreateWindow("Assignment 2: Rabin Ranabhat");

  //special call backs
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
	
  glutMainLoop();
	
  return 0;
}
