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

/*values for r,g,b*/
int rVal = 255;
int gVal = 255;
int bVal = 255;

/*keeps count of all the calculated co-ordinates in the x-y-z plane*/
int indexVal = 0;

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

	glLineWidth(1.5);
	glBegin(GL_LINE_STRIP);
	
	int i;

	for(int i = 0; i<indexVal;)
	  {
	    glColor3ub(rVal,gVal,bVal);
	    glVertex3fv(calculatedPts[i]);
	    i++;
	  }
	  

	glEnd();
	
	glRasterPos2i(10,10);
	view_rotx = fmod(view_rotx,360);
	view_roty = fmod(view_roty,360);
	view_rotz = fmod(view_rotz,360);
	
	Print("X-Angle=%.1f Y-Angle=%.1f Z-Angle=%.1f",view_rotx,view_roty,view_rotz);

	ErrCheck("display");

	glFlush();	
	glutSwapBuffers();	
	glPopMatrix();
	
}

//reshaping the screen and adjusting the coordinates
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
  glutPostRedisplay();
}


//special key functions
static void key(unsigned char k, int x, int y) {
	switch (k) {
		case 'r':
		         rVal = 255;
		         gVal = 0;
		         bVal = 0;
		         break;

	        case 'g':
		         rVal = 0;
		         gVal = 255;
		         bVal = 0;
		         break;

               case 'b':
	                 rVal = 0;
		         gVal = 0;
		         bVal = 255;
		         break;
		     
        	case 'q':
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			view_posz -= 1;
			gluLookAt(0,0,view_posz,0.0,0.0,0.0,0.0,1.0,0.0);
			break;
		case 'a':
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			view_posz += 1;
			gluLookAt(0,0,view_posz,0.0,0.0,0.0,0.0,1.0,0.0);
			break;
	       case 'z':
		        rVal = 255;
		        gVal = 255;
		        bVal = 255;
		        indexVal = 0;
		        break;
		default:
			return;
	}
	glutPostRedisplay();
}

//idle function
static void idle(void) {

      double dx = s*(y-x);
      double dy = x*(r-z)-y;
      double dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;

      calculatedPts[indexVal+1][0] = x;
      calculatedPts[indexVal+1][1] = y;
      calculatedPts[indexVal+1][2] = z;

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
