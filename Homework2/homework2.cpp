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
#include <math.h>

#define GL_GLEXT_PROTOTYPES

float calculatedPts[200000][3];

static GLfloat view_rotx = 0.0, view_roty = 0.0, view_rotz = 0.0;
static GLfloat view_posz = 60.0;

/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;

double dt = 0.007;
double x = 1;
double y = 1;
double z = 1;

/*height and width*/
int gHeight=0;
int gWidth=0;

#define LEN 8192  //  Maximum amount of text

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


//display function
void display() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();

  //rotation functions
  glRotatef(view_rotx, 1.0, 0.0, 0.0);
  glRotatef(view_roty, 0.0, 1.0, 0.0);
  glRotatef(view_rotz, 0.0, 0.0, 1.0);

  //draw a 3-D object
  //glPointSize(6);
  glColor3f(0, 1, 0);
  glBegin(GL_QUADS);
  
  glVertex3f(0,0,0);  
  glVertex3f(0.9,0,0);
  glVertex3f(0.9,0.9,0);
  glVertex3f(0,0.9,0);
  //glVertex3f(-1.0f, 1.0f, 1.0f);
  //glVertex3f(-1.0f, 1.0f,  1.0f);
  //glVertex3f( 1.0f, 1.0f,  1.0f);

  glEnd();
  glPopMatrix();   
  ErrCheck("display");

  glFlush();	
  glutSwapBuffers();	

	
}

//reshaping the screen and adjusting the coordinates
static void reshape(int width, int height) {
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glOrtho - left, right, bottom, top,zNear, zfar)
  glOrtho(1,1,1,1,-1,1);
  //glFrustum(-5.0, 5.0, -h*2, h*2, 1.0, 300.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //glTranslatef(0.0, 0.0, -60.0);

}

//special characters function
static void special(int k, int x, int y) {
  switch (k) {
  case GLUT_KEY_UP:
    view_rotx += 5.0;
    break;
    
  case GLUT_KEY_DOWN:
    view_rotx -= 5.0;
    break;
    
  case GLUT_KEY_LEFT:
    view_roty += 5.0;
    break;
    
  case GLUT_KEY_RIGHT:
    view_roty -= 5.0;
    break;
    
  default:
    return;
    
  }

  view_roty = fmod(view_roty, 360);
  view_rotx = fmod(view_rotx, 360);
  glutPostRedisplay();
}

//special key functions
static void key(unsigned char k, int x, int y) {
  switch (k) {
  case 'q':
    s++;
    break;

  case 'w':
    s--;
    break;

  case 'a':
    b += 0.1;
    break;

  case 's':
    b-= 0.1;
    break;

  case 'z':
    r++;
    break;

  case 'x':
    r--;
    break;
  
  case 'e':
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //view_posz -= 1;
    //gluLookAt(0,0,view_posz,0.0,0.0,0.0,0.0,1.0,0.0);
    break;
    
  case 'r':
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //view_posz += 1;
    //gluLookAt(0,0,view_posz,0.0,0.0,0.0,0.0,1.0,0.0);
    break;

  case 'd':
    view_rotx = 0;
    view_roty = 0;
    view_rotz = 0;
    break;
    
  default:
    return;
  }
  
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
  //enabling the z-buffer depth test
  glEnable(GL_DEPTH_TEST);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
	
  glutMainLoop();
	
  return 0;
}
