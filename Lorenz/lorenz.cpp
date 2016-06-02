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

#define GL_GLEXT_PROTOTYPES

float pts[200000][3];

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

int indexVal = 0;

#define LEN 8192  //  Maximum amount of text
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


void ErrCheck(char* where)
{
  int err = glGetError();

  if(err)
    fprintf(stderr, "ERROR: %s [%s]\n", gluErrorString(err), where);
}


void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
		
	glRotatef(view_rotx, 1.0, 0.0, 0.0);
	glRotatef(view_roty, 0.0, 1.0, 0.0);
	glRotatef(view_rotz, 0.0, 0.0, 1.0);

	glLineWidth(1.5);
	glBegin(GL_LINE_STRIP);
	int i = 0;

	for(int i = 0; i<indexVal;)
	  {
	    glColor3ub(rand()%256,rand()%256,rand()%256);
	    glVertex3fv(pts[i]);
	    i++;
	  }
	  

	glEnd();
	
	/*while( i < iter && i < numPts ) {
		glColor3fv(pts[i]);
		glVertex3fv(pts[i++]);
	}
	glEnd();
	
	if( iter < numPts ) {
		if( iter + iterInc > numPts ) iter = numPts;
		else iter+=iterInc;
		}*/

	//glWindowPos2i(5,5);
	Print("Angle=%.1f",view_roty);
	ErrCheck("display");
	//glFlush();
	glutSwapBuffers();
	
	glPopMatrix();
	
}


static void reshape(int width, int height) {

  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-5.0, 5.0, -h*2, h*2, 1.0, 300.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -60.0);

}

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
  glutPostRedisplay();
}


static void key(unsigned char k, int x, int y) {
	switch (k) {
		case 'q':
			view_rotx += 5.0;
			break;
		case 'a':
			view_rotx -= 5.0;
			break;
		case 'w':
			view_roty += 5.0;
			break;
		case 's':
			view_roty -= 5.0;
			break;
		case 'e':
			view_rotz += 5.0;
			break;
		case 'd':
			view_rotz -= 5.0;
			break;
		case 'r':
			indexVal = 0;
			break;
        	case 'y':
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			view_posz -= 1;
			gluLookAt(0,0,view_posz,0.0,0.0,0.0,0.0,1.0,0.0);
			break;
		case 'h':
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			view_posz += 1;
			gluLookAt(0,0,view_posz,0.0,0.0,0.0,0.0,1.0,0.0);
			break;
		default:
			return;
	}
	glutPostRedisplay();
}


static void idle(void) {

      double dx = s*(y-x);
      double dy = x*(r-z)-y;
      double dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;

      pts[indexVal+1][0] = x;
      pts[indexVal+1][1] = y;
      pts[indexVal+1][2] = z;

      indexVal++;
      
      //printf("%f %f %f\n", x,y,z);
      glutPostRedisplay();
}

int main(int argc,char* argv[]) {
 	
	glutInit(&argc,argv);
	
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	//size of the initial window
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 500);
	//Window title
	glutCreateWindow("Lorenz Attractor");

	//special call backs
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(key);
	//Idle function
	glutIdleFunc(idle);
	
	glutMainLoop();
	
	return 0;
}
