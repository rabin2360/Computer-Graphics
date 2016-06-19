//Name: Rabin Ranabhat Assignment3: Lighting and Textures

#ifdef USEGLEW
#include <GL/glew.h>
#endif


#include <time.h>
#include <stdio.h>
#include <stdbool.h>

#define GL_GLEXT_PROTOTYPES

#include "CSCI5229.h"

static GLfloat view_rotx = 10.0, view_roty = -10.0, view_rotz = 0.0;

bool draw_axis = true;

//dimension of the world
double dim = 3;
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

//lightning in openGL
int light = 0;
//light azimuth
int zh = 90;
//ambient light in percentage
int ambient = 45;
//diffuse light in percentage
int diffuse = 100;
//specular light in percentage
int specular = 10;
//distance of light
float distance = 3.0;
//elevation of light
float ylight = 1.0;
//shininess value
float shinyvec[1];
//emission intensity
int emission = 0;
//ball increment
int inc = 10;
//local viewer model
int local = 0;

//pause variable that controls the auto rotation of the light
int pause = 0;

//variables for texture
unsigned int texture[5];  //texture names

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

void drawCylinder(double h, double r,
		  float tx, float ty, float tz,
		  float sx, float sy, float sz,
		  int texType)
{
  glPushMatrix();
  
  glTranslatef(tx,ty,tz);
  glScalef(sx,sy,sz);

  
  //enable texture
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

  //texType 1 - tree bark
  //else - castle
  if(texType)
    glBindTexture(GL_TEXTURE_2D,texture[0]);
  else
    glBindTexture(GL_TEXTURE_2D,texture[3]);
    
  glBegin(GL_QUADS);
  int i;
  int inc = 40;

  double texMap = 0.0;
  
  for(i = 0; i<=360; i+=inc)
    {
      glNormal3d(Sin(i),1, -Cos(i));
      glTexCoord2d(texMap,0); glVertex3f(r*Sin(i),0,-r*Cos(i));

      glNormal3d(Sin(i),h, -Cos(i));
      glTexCoord2d(texMap,0.8); glVertex3f(r*Sin(i),h,-r*Cos(i));

      glNormal3d(Sin(i+inc),h, -Cos(i+inc));
      glTexCoord2d(texMap+0.1,0.8); glVertex3f(r*Sin(i+inc),h,-r*Cos(i+inc));

      glNormal3d(Sin(i+inc),1, -Cos(i+inc));
      glTexCoord2d(texMap+0.1,0); glVertex3f(r*Sin(i+inc),0,-r*Cos(i+inc));

      texMap += 0.1;//1.0/(360/inc);
      
    }

  glEnd();

  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}

//drawing cone using radius and height parameters
void drawCone(double h, double r,
	      float tx, float ty, float tz,
	      float sx, float sy, float sz,
	      int texMode)
{
  glPushMatrix();

  int i;

  glTranslatef(tx, ty, tz);
  glScalef(sx, sy, sz);

  
  //enable texture
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

  //if texMode is 1, draw the castle roof
  //otherwise, draw the tree texture
  if(texMode)
    glBindTexture(GL_TEXTURE_2D,texture[2]);
  else
    glBindTexture(GL_TEXTURE_2D,texture[4]);
 
  //draw the outside of the cone
  glBegin(GL_TRIANGLES);

  int inc = 20;
  double texVal = 0;
  
  for(i = 0; i<=360; i+=inc)
    {
      glNormal3d(Sin(i),1, -Cos(i));
      glTexCoord2f(texVal, 0); glVertex3f(r*Sin(i),0,-r*Cos(i));

      glNormal3d(Sin(i+(inc)/2), h, -Cos(i+(inc)/2));
      glTexCoord2f(texVal+0.1,1); glVertex3f(0,h,0);

      glNormal3d(Sin(i+inc),1, -Cos(i+inc));
      glTexCoord2f(texVal+0.2, 0); glVertex3f(r*Sin(i+inc),0,-r*Cos(i+inc));
    }

  glEnd();

  glDisable(GL_TEXTURE_2D);

  //draw the base of the cone
  glBegin(GL_TRIANGLE_STRIP);
  for(i = 0; i<=360; i+=inc)
    {
      glNormal3d(0,-1, 0);
      
      glVertex3f(r*Sin(i),0,-r*Cos(i));
      glVertex3f(0,0,0);
      glVertex3f(r*Sin(i+inc),0,-r*Cos(i+inc));
    }

  glEnd();
  glPopMatrix();
  
}

//draw the floor for the display
void drawFloor()
{
  glPushMatrix();

  //enable texture
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[1]);


  //drawing the floor
  glColor3f(0.0, 1.0, 0.0);
  glBegin(GL_QUADS);
  glNormal3f(0,1,0);

  /*  float xValStart = -1.8;
  float xValEnd = 1.5;
  float yValStart = -1.5;
  float yValEnd = 1.2;
  */
  float xVal, zVal;
  
  for(zVal = -1.5; zVal<=1.2; zVal += 0.1)
        for(xVal = -1.8; xVal<=1.5; xVal+=0.1)
      {
	glTexCoord2f(0,0); glVertex3f(xVal,0.0,zVal);  
	glTexCoord2f(1,0); glVertex3f(xVal+0.1,0.0,zVal);
	glTexCoord2f(1,1); glVertex3f(xVal+0.1,0.0,zVal+0.1);
	glTexCoord2f(0,1); glVertex3f(xVal,0.0,zVal+0.1);

      }
  /*
  glTexCoord2f(0,0); glVertex3f(-1.8,0.0,-1.5);  
  glTexCoord2f(1,0); glVertex3f(1.5,0.0,-1.5);
  glTexCoord2f(1,1); glVertex3f(1.5,0.0,1.2);
  glTexCoord2f(0,1); glVertex3f(-1.8,0.0,1.2);
  */
  glEnd();

  glDisable(GL_TEXTURE_2D);
   
  glPopMatrix();   
}

void drawText()
{
  glPushMatrix();
  glWindowPos2i(5,5);

  if(projectionMode == 1)
    {
      Print("Angle=%g, Projection: Traverse, Light: %s",angle, light ? "On": "Off");
      Print("Ambient=%d, Specular=%d, Diffuse=%d",ambient, specular, diffuse);
    }
  else
    {
      Print("Angle=%g,%g, Projection: %s, Light: %s\n",view_rotx,view_roty, projectionMode?"Perspective":"Orthographic", light ? "On": "Off");
    }

  glWindowPos2i(5,25);
  Print("Ambient=%d, Specular=%d, Diffuse=%d",ambient, specular, diffuse);

  glPopMatrix();
}

void drawTower(double h, double r)
{
  glPushMatrix();

  //drawing the roof
  glColor3f(0.15, 0.15, 0.15);
  drawCone(1.2*h,1*r,
	   0.0,1.0,0.0,
	   0.5,1,0.5,
	   1);

  //body
  glColor3f(0.66, 0.66, 0.66);
  drawCylinder(2*h,0.5*r,
	       0.0,0.0,0.0,
	       0.5,1,0.5,
	       1);

  glPopMatrix();
}

void drawWall(double h)
{
  glPushMatrix();

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[0]);

  glColor3f(0.86, 0.86, 0.86);

  glBegin(GL_QUADS);
  glNormal3f(0,0,1);

  /*double yValues;
  double xValues;
  double texVals = 0;
  double texValt = 0;
  
  for(yValues = 0; yValues<=h; yValues+= 0.1){
    for(xValues = 0; xValues<=h; xValues+=0.1)
      {
	glTexCoord2f(texVals,texValt);   glVertex3f(xValues,yValues,0.0);
	glTexCoord2f(texVals,texValt+0.1);   glVertex3f(xValues,yValues+0.1,0.0);
	glTexCoord2f(texVals+0.1,texValt+0.1);   glVertex3f(xValues+0.1,yValues+0.1,0.0);
	glTexCoord2f(texVals+0.1,texValt);   glVertex3f(xValues+0.1,yValues,0.0);

	texVals += 0.1;
      }
    texValt +=0.1;
  }
  */
  
  glTexCoord2f(0,0);   glVertex3f(0.0,0.0,0.0);
  glTexCoord2f(0,1);   glVertex3f(0.0,h,0.0);
  glTexCoord2f(1,1);   glVertex3f(h,h,0.0);
  glTexCoord2f(1,0);   glVertex3f(h,0.0,0.0);
  glEnd();

  glDisable(GL_TEXTURE_2D);
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
	   0.5,1,0.5,
	   0);

  //drawing a cone-2
  drawCone(0.9*h,0.6*r,
	   0.0,0.8,0.0,
	   0.5,1,0.5,
	   0);


  //drawing a cone-3
  drawCone(0.9*h,0.6*r,
	   0.0,1.0,0.0,
	   0.5,1,0.5,
	   0);

  glColor3f(0.54,0.27,0.074);
  //drawing stalk of the tree
  drawCylinder(1.6*h,0.2*r,
	       0.0,0.0,0.0,
	       0.5,1,0.5,
	       0);
 
}

void drawScene()
{
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
}

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
  double x = Sin(th)*Cos(ph);
  double y = Cos(th)*Cos(ph);
  double z =         Sin(ph);
  //  For a sphere at the origin, the position
  //  and normal vectors are the same
  glNormal3d(x,y,z);
  glVertex3d(x,y,z);
}

static void ball(double x,double y,double z,double r)
{
  int th,ph;
  float yellow[] = {1.0,1.0,0.0,1.0};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  //  Save transformation
  glPushMatrix();

  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glScaled(r,r,r);

  //  White ball
  glColor3f(1,1,1);
  glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
  glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
  
  //  Bands of latitude
  for (ph=-90;ph<90;ph+=inc)
    {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
	{
	  Vertex(th,ph);
	  Vertex(th,ph+inc);
	}
      glEnd();
    }
  //  Undo transofrmations
  glPopMatrix();
}


//display function
void display() {

  //redraw the window and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //Enable the z-buffer
  glEnable(GL_DEPTH_TEST);
  //reload the transformation matrix - make it identity
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

  
  //determine light
  if(light)
    {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};

      //Light position
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      //color for the rotating light
      glColor3f(1,1,1);
      ball(Position[0],Position[1],Position[2],0.2);

      //keeps the normal vector unit length
      glEnable(GL_NORMALIZE);
      
      //Enable lighting
      glEnable(GL_LIGHTING);

      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
	
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);

	
      //Enable light 0
      glEnable(GL_LIGHT0);

      //settings for light  0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0, GL_POSITION, Position);
    }
  else
    {
      glDisable(GL_LIGHTING);
    }

  drawScene();

  glDisable(GL_LIGHTING);
  //toggle the axis display
  if(draw_axis)
    drawAxes(1.5);
  
  //display the text
  drawText();
  
  ErrCheck("display");

  glFlush();	
  glutSwapBuffers();	
	
}

static void idle()
{
  
  //if not paused, then only increment the ball rotation degree
  if(!pause && light)
    {
      zh +=1;
      zh %= 360;
    }
  
  glutPostRedisplay();
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

  case 'a':
    if (light && ambient <100)
      ambient += 1;
    break;

  case 'A':
    if(light && ambient >0)
      ambient -= 1;
    break;

  case 's':
    if (light && specular <100)
      specular += 1;
    break;

  case 'S':
    if(light && specular >0)
      specular -= 1;
    break;

  case 'd':
    if (light && diffuse <100)
      diffuse += 1;
    break;

  case 'D':
    if(light && diffuse >=0)
      diffuse -= 1;
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

  case 'q':
    //if the light is turned on
    //then pause the light and move is clockwise by 1.5
    if(light)
      {
	if(!pause)
	  pause = 1;
    
	zh += 1.5;
      }
    break;
    
  case 'Q':

    if(light)
      {
    if(!pause)
      pause = 1;
    
    zh -= 1.5;
    }
    break;

  case 'w':
    if(light){
    if(!pause)
      pause = 1;
    
    ylight += 0.1;
    }
    break;
    
  case 'W':
    if(light){
    if(!pause)
      pause = 1;
    
    ylight -= 0.1;
    }
    break;

  case 'e':
    if(light){
    if(!pause)
      pause = 1;
    
    distance += 0.1;
    }
    break;
    
  case 'E':
    if(light){
    if(!pause)
      pause = 1;
    
    distance -= 0.1;
    }
    break;
    
  case 'l':
    light = 1;
    break;
  case 'L':
    light = 0;
    break;

  case 'z':
  case 'Z':
    if(pause)
      {
	pause = 0;
	distance = 3.0;
      }
    else
      pause = 1;
    
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
  glutCreateWindow("Assignment 3: Rabin Ranabhat");

  //special call backs
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);
  glutIdleFunc(idle);

  //load texture
  texture[0] = LoadTexBMP("stoneWall.bmp");
  texture[1] = LoadTexBMP("grass.bmp");
  texture[2] = LoadTexBMP("castleRoof.bmp");
  texture[3] = LoadTexBMP("treeBark.bmp");
  texture[4] = LoadTexBMP("tree.bmp");
  
  ErrCheck("init");
  glutMainLoop();
	
  return 0;
}
