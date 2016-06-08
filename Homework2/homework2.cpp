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
//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415927/180*(th))
#define Sin(th) sin(3.1415927/180*(th))

static GLfloat view_rotx = 10.0, view_roty = -10.0, view_rotz = 0.0;
//static GLfloat view_posz = 60.0;

bool draw_axis = true;

//dimension of the world
double dim = 2.0;
GLfloat h = 1;

//variables for perspective projections
double fov = 55;
double aspectRatio = 1;
int projectionMode = 0;

void Project(int mode)
{
  glMatrixMode(GL_PROJECTION);

  glLoadIdentity();

  if(mode)
    gluPerspective(fov, aspectRatio, dim, -dim);
  else
    glOrtho(-aspectRatio*dim,+aspectRatio*dim,-dim,+dim,-dim,+dim);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}

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

void drawBuilding()
{
  glPushMatrix();
  glColor3f(1.0,1.0,0.0);
  //body of the house
  glutSolidCube(0.5);
  glPopMatrix();
    
  //roof
  glPushMatrix();
  glColor3f(1.0,1.0,1.0);
  glTranslatef(0,0,0.2);
 
  glutSolidCone(0.5, 0.6, 50, 5);
  glPopMatrix();

}

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

  //draw a 3-D object
  //glPointSize(6);

  //drawing the floor
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_QUADS);
  glVertex3f(-1.8,0.0,-1.2);  
  glVertex3f(1.5,0.0,-1.2);
  glVertex3f(1.5,0.0,1.2);
  glVertex3f(-1.8,0.0,1.2);
  glEnd();
  
  glPopMatrix();   

}

/*void drawText()
{
  glPushMatrix();
  glWindowPos2i(5,5);
  Print("Angle=%d,%d",view_rotx,view_roty);
  glPopMatrix();
}
*/

//code that draws the torus
static void torus(int numc, int numt)
{
   int i, j, k;
   double s, t, x, y, z, twopi;

   twopi = 2 * (double)M_PI;
   for (i = 0; i < numc; i++) {
      glBegin(GL_QUAD_STRIP);
      for (j = 0; j <= numt; j++) {
         for (k = 1; k >= 0; k--) {
            s = (i + k) % numc + 0.5;
            t = j % numt;

            x = (0.2+.1*cos(s*twopi/numc))*cos(t*twopi/numt);
            y = (0.2+.1*cos(s*twopi/numc))*sin(t*twopi/numt);
            z = .1 * sin(s * twopi / numc);
            glVertex3f(x, y, z);
         }
      }
      glEnd();
   }
}

//display function
void display() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  
  //loading identity - otherwise, rotate function is very choppy
  glLoadIdentity();

  if(projectionMode)
    {
      double Ex = -2*dim*Sin(view_roty)*Cos(view_rotx);
      double Ey = +2*dim        *Sin(view_rotx);
      double Ez = +2*dim*Cos(view_roty)*Cos(view_rotx);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(view_rotx),0);
    }
  else
    {
      //rotation functions
      glRotatef(view_rotx, 1.0, 0.0, 0.0);
      glRotatef(view_roty, 0.0, 1.0, 0.0);
      glRotatef(view_rotz, 0.0, 0.0, 1.0);
    }
  
  //drawing the torus1
  glPushMatrix();
  glTranslatef(-0.8,0.29,0);
  glColor3f(1,0,0);
  torus(5, 20);
  glPopMatrix();

  //drawing the torus2
  glPushMatrix();
  glTranslatef(-1.5,0.29,0);
  glRotatef(90,0,1,0);
  glScalef(0.5,0.5,0.5);
  glColor3f(1,0,0);
  torus(5, 20);
  glPopMatrix();

  //drawing building1
  glPushMatrix();
  glTranslatef(0,0.26,0);
  glRotatef(-90, 1, 0,0); 
  drawBuilding();
  glPopMatrix();

  //drawing building2
  glPushMatrix();
  glTranslatef(1,0.14,0);
  glRotatef(-90, 1, 0,0);
  glScalef(1,1,0.5);
  drawBuilding();
  glPopMatrix();

  //draw the floor
  drawFloor();

  //toggle the axis display
  if(draw_axis)
    drawAxes();
  
  //display the text
  //drawText();
  
  ErrCheck("display");

  glFlush();	
  glutSwapBuffers();	
	
}

//reshaping the screen and adjusting the coordinates
static void reshape(int width, int height) {


  aspectRatio = (height>0)?(GLfloat) width / (GLfloat) height : 1;

  glViewport(0, 0, (GLint) width, (GLint) height);

  Project(projectionMode);
  
  /*glMatrixMode(GL_PROJECTION);

  glLoadIdentity();
  //glOrtho - left, right, bottom, top,zNear, zfar)
  glOrtho(-h*dim,+h*dim,-dim,+dim,-dim,+dim);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  */
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
  
  case 'o':
    projectionMode = 0;
    
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //view_posz -= 1;
    //gluLookAt(0,0,view_posz,0.0,0.0,0.0,0.0,1.0,0.0);
    break;
    
  case 'p':
    projectionMode = 1;

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
