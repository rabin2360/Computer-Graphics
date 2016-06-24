//
// Authors: Rabin Ranabhat
//
/////////////////////////////////////////////////////////////////

#include "ex1.h"

extern "C"
{
#include "CSCI5229.h"
}

#define RENDERING_TRAIL 1

list<Particle *> particles;
int frames;
//smaller than one?
list<Sphere *> spheres;
float SPHERES_PER_FRAME;
float GRAVITY_POWER;
int SPHERE_TTL;
int SPHERE_PARTICLES;
int PARTICLE_TTL;
float POINT_LINE_SIZE;

int renderingType = 0;


//lightning in openGL
int light = 0;
//light azimuth
int zh = 90;
//ambient light in percentage
int ambient = 20;
//diffuse light in percentage
int diffuse = 100;
//specular light in percentage
int specular = 100;
//dist of light
float dist = 380.0;
//elevation of light
float ylight = 0.0;
//shininess value
float shinyvec[1];
//emission intensity
int emission = 30;
//ball increment
int inc = 10;
//local viewer model
int local = 0;
//angle
int angle = 0;

//camera position
double Lx = -0.174311;
double Ly = 50;//500.0;
double Lz = -1.992389;

double Ex = 1;
double Ey = 50;//100;
double Ez = 600;//1000;

//skybox variables
int skyTex[2];
int dim = 100;

//object variables
int objBarrel;

//terrain
float z[65][65];       //  DEM data
float zmin=+1e8;       //  DEM lowest location
float zmax=-1e8;       //  DEM highest location
float zmag=0.6;          //  DEM magnification

  int countVal = 0;
///////////////////////////////////////////////

void initData(void) {
  frames = 0;
  SPHERES_PER_FRAME = 0.03;
  GRAVITY_POWER = 0.05;
  SPHERE_TTL = 150;
  SPHERE_PARTICLES = 200;
  PARTICLE_TTL = 100;
  POINT_LINE_SIZE = 3.0;

}

///////////////////////////////////////////////

/*
 *  Read DEM from file
 */
void ReadDEM(char* file)
{
   int i,j;
   FILE* f = fopen(file,"r");
   if (!f) Fatal("Cannot open file %s\n",file);
   for (j=0;j<=64;j++)
      for (i=0;i<=64;i++)
      {
         if (fscanf(f,"%f",&z[i][j])!=1) Fatal("Error reading saddleback.dem\n");
         if (z[i][j] < zmin) zmin = z[i][j];
         if (z[i][j] > zmax) zmax = z[i][j];
      }
   fclose(f);
}


void animate(void)
{
  //zh +=1;
  //zh %= 360;

  frames++;

  //creating the spheres for the fireworks display
  if(SPHERES_PER_FRAME > myRandom()){
  //if(countVal < 1){
    spheres.push_back(new Sphere());
    countVal++;
  }
  auto spheres_iterator = spheres.begin();

  //decrementing the lifetime for the particle
  while(spheres_iterator != spheres.end()) {
    ((*spheres_iterator)->center).ttl--;

    //if lifetime of the particles expired, explode
    if(((*spheres_iterator)->center).ttl <= 0 || ((*spheres_iterator)->center).location.y > 500) {
    //if(((*spheres_iterator)->center).speed.y == 0){
      particles.splice(particles.end(), (*spheres_iterator)->explode());
      delete *spheres_iterator;
      spheres_iterator = spheres.erase(spheres_iterator); 
    }
    else {
      (*spheres_iterator)->move();
      spheres_iterator++;
    }
  }
  
  auto particles_iterator = particles.begin();

  while(particles_iterator != particles.end()) {
    (*particles_iterator)->ttl--;
    if((*particles_iterator)->ttl <= 0) {
      delete *particles_iterator;
      particles_iterator = particles.erase(particles_iterator);
    } else {
      (*particles_iterator)->move();
      particles_iterator++;
    }
  }
  glutPostRedisplay();
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

void skyBox(double D)
{
  glColor3f(1,1,1);
  glEnable(GL_TEXTURE_2D);

   //  Sides
   glBindTexture(GL_TEXTURE_2D,skyTex[0]);

   glBegin(GL_QUADS);
   glTexCoord2f(0.00,0); glVertex3f(-D,-0.1*D,-D);
   glTexCoord2f(0.25,0); glVertex3f(+D,-0.1*D,-D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.00,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(0.25,0); glVertex3f(+D,-0.1*D,-D);
   glTexCoord2f(0.50,0); glVertex3f(+D,-0.1*D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);

   glTexCoord2f(0.50,0); glVertex3f(+D,-0.1*D,+D);
   glTexCoord2f(0.75,0); glVertex3f(-D,-0.1*D,+D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);

   glTexCoord2f(0.75,0); glVertex3f(-D,-0.1*D,+D);
   glTexCoord2f(1.00,0); glVertex3f(-D,-0.1*D,-D);
   glTexCoord2f(1.00,1); glVertex3f(-D,+D,-D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glEnd();

      //  Top and bottom
   glBindTexture(GL_TEXTURE_2D,skyTex[1]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.5,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.5,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.0,1); glVertex3f(-D,+D,-D);
   glEnd();
   
     int i,j;
   double z0 = (zmin+zmax)/2;
   //glColor3f(1,1,0);
   for (i=0;i<64;i++)
         for (j=0;j<64;j++)
         {
	  
            float x=32*i-1024;
            float y=32*j-1024;
	    glNormal3d(0,1,0);
            glBegin(GL_QUADS);
	    glTexCoord2f((i+0)/64.,(j+0)/64.); glVertex3d(x+ 0,zmag*(z[i+0][j+0]-z0),y+ 0);
            glTexCoord2f((i+1)/64.,(j+0)/64.); glVertex3d(x+32,zmag*(z[i+1][j+0]-z0),y+ 0);
            glTexCoord2f((i+1)/64.,(j+1)/64.); glVertex3d(x+32,zmag*(z[i+1][j+1]-z0),y+32);
            glTexCoord2f((i+0)/64.,(j+1)/64.); glVertex3d(x+ 0,zmag*(z[i+0][j+1]-z0),y+32);
            glEnd();
	 }
   
   //glTexCoord2f(1.0,1); glVertex3f(-D,-0.01*D,+D);
   //glTexCoord2f(0.5,1); glVertex3f(+D,-0.01*D,+D);
   //glTexCoord2f(0.5,0); glVertex3f(+D,-0.01*D,-D);
   //glTexCoord2f(1.0,0); glVertex3f(-D,-0.01*D,-D);
   //glEnd();

   glDisable(GL_TEXTURE_2D);

}

void display()
{
   
  // Clear the screen
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glAccum(GL_RETURN, 0.95);
  //glClear(GL_ACCUM_BUFFER_BIT);
  //glMatrixMode(GL_MODELVIEW);
  
  glLoadIdentity();

  gluLookAt(Ex, Ey, Ez,
            Ex+Lx, Ly, Ez+Lz,
            0.0, 1.0, 0.0);

  skyBox(8*dim);
  drawAxes(60);

  glPushMatrix();
  //glScalef(50,50,50);
  //glColor3f(1,1,0);
  glCallList(objBarrel);
  glPopMatrix();
  
  //determine light
  if(light)
    {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};

      //Light position
      float Position[]  = {dist*Cos(zh),ylight,dist*Sin(zh),1.0};
      //color for the rotating light
      glColor3f(1,1,1);
      Position[0] = 0;
      Position[1] = 380;
      Position[2] = 0;
      
      ball(Position[0],Position[1],Position[2],15);

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

  
  //creating the spheres
  for(auto it = spheres.begin(); it != spheres.end(); it++) {
    glPushMatrix();

    	double alpha = 0;
	
	//for each particle, reference the prevLocations
	glLineWidth(POINT_LINE_SIZE);
	glBegin(GL_LINE_STRIP);
	for(auto sphereIterator =  ((*it)->center).prevLocations.begin(); sphereIterator !=  ((*it)->center).prevLocations.end(); sphereIterator++)
	  {
	    glColor4f(1, 1, 0, alpha);
	    //glColor4f(((*it)->center).color.x, ((*it)->center).color.y, ((*it)->center).color.z, alpha);
	    glVertex3f((*sphereIterator).x, (*sphereIterator).y,(*sphereIterator).z);

	    
	    alpha += 0.15;
	    }

	glEnd();

	glTranslatef(((*it)->center).location.x, ((*it)->center).location.y, ((*it)->center).location.z);
	glutSolidSphere(1, 18, 18);
	
    //glColor3f(((*it)->center).color.x, ((*it)->center).color.y, ((*it)->center).color.z);
    //glColor3f(1.0, 1.0, 0);
    //glutSolidSphere(2/*(*it)->radius*/, 18, 18);
    
    glPopMatrix();
  }

  switch(renderingType){
  case RENDERING_TRAIL:
    glLineWidth(POINT_LINE_SIZE);

    
      for(auto it = particles.begin(); it != particles.end(); it++) {
        
	double alpha = 0;
	
	glBegin(GL_LINE_STRIP);	
	//for each particle, reference the prevLocations
	for(auto particleIterator =  (*it)->prevLocations.begin(); particleIterator !=  (*it)->prevLocations.end(); particleIterator++)
	  {
	    glColor4f((*it)->color.x, (*it)->color.y, (*it)->color.z, alpha);
	    glVertex3f( (*particleIterator).x,(*particleIterator).y, (*particleIterator).z);

	    alpha += 0.1;
	  }
        //glVertex3f((*it)->location.x, (*it)->location.y, (*it)->location.z);
        //glVertex3f((*it)->previousLocation.x, (*it)->previousLocation.y, (*it)->previousLocation.z);
	glEnd();
      }
    
    break;
    
  default:
    glPointSize(POINT_LINE_SIZE);
    glBegin(GL_POINTS);
      for(auto it = particles.begin(); it != particles.end(); it++) {
        glColor3f((*it)->color.x, (*it)->color.y, (*it)->color.z);
        glVertex3f((*it)->location.x, (*it)->location.y, (*it)->location.z);
      }
    glEnd();
  }

  glFlush();
  //glAccum(GL_ACCUM, 0.90); 
  glutSwapBuffers(); 
  
  ErrCheck("display");
}

///////////////////////////////////////////////

void keyboard(unsigned char key, int x, int y)
{
  if(key == 27)
    exit(0);
  // If space restart program??
  if(key == 32) 
    exit(0);

  if(key == 'l' || key == 'L')
    {
      if(light)
	light = 0;
      else
	light = 1;
    }
  
  glutPostRedisplay();
}

///////////////////////////////////////////////

//special characters function
static void special(int k, int x, int y) {
  GLfloat speed = 1.2;
 
  switch (k) {   
  case GLUT_KEY_UP:
    
    Ex += Lx * speed;
    Ez += Lz * speed;
     break;
    
  case GLUT_KEY_DOWN:
    Ex -= Lx * speed;
    Ez -= Lz * speed;
    break;

  case GLUT_KEY_LEFT:
    	//rotate left
    // if(angle>-40){
	angle -= 5;
	Lx = 2*Sin(angle);
	Lz = -2*Cos(angle);
	//}
	break;

  case GLUT_KEY_RIGHT:
    	//rotate right
    // if(angle<40){
	angle += 5;
	Lx = 2*Sin(angle);
	Lz = -2*Cos(angle);
	// }
	break;

  case GLUT_KEY_PAGE_DOWN:
    Ly += 0.1;
    break;

  case GLUT_KEY_PAGE_UP:
    Ly -= 0.1;
    break;
  
  default:
    return;
    
  }

  //printf("Lx %f, Ly%f, Lz %f, angle %d\n", Ex,Ey,Ez, angle);
  glutPostRedisplay();
}
////////////////////////////////////////////////////

void reshape(int width, int height)
{
  glClearColor(0.9, 0.9, 0.9, 1.0);
  glViewport(0, 0, (GLsizei)width, (GLsizei)height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, (GLfloat)width / (GLfloat)height, 1, 32*dim);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void change_sphere_rate(int value) {
  SPHERES_PER_FRAME = value/100.0;
}

void change_gravity(int value) {
  GRAVITY_POWER = value*0.025;
}

void change_sphere_ttl(int value) {
  SPHERE_TTL = value;
}

void change_sphere_particles(int value) {
  SPHERE_PARTICLES = value;
}

void change_particle_ttl(int value) {
  PARTICLE_TTL = value;
}

void change_rendering_type(int value) {
  renderingType = value;
}



///////////////////////////////////////////////
void initGraphics(int argc, char *argv[])
{
  initData();
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);
  glutInitWindowPosition(100, 100);

  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow("Project: Particles");

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutReshapeFunc(reshape);
  glutIdleFunc(animate);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearAccum(0.0f, 0.0f, 0.0f, 1.0f);
  
  auto sphere_rate_submenu = glutCreateMenu(change_sphere_rate);
    glutAddMenuEntry("Very few", 1);
    glutAddMenuEntry("Few", 2);
    glutAddMenuEntry("Normal", 3);
    glutAddMenuEntry("Many", 4);
    glutAddMenuEntry("A lot ", 5);
    
  auto gravity_power_submenu = glutCreateMenu(change_gravity);
    glutAddMenuEntry("No Gravity", 0);
    glutAddMenuEntry("Low", 1);
    glutAddMenuEntry("Medium", 2);
    glutAddMenuEntry("High", 3);
    glutAddMenuEntry("Very Hight", 4);
    
  auto sphere_ttl_submenu = glutCreateMenu(change_sphere_ttl);
    glutAddMenuEntry("Very Short", 50);
    glutAddMenuEntry("Short", 100);
    glutAddMenuEntry("Normal", 150);
    glutAddMenuEntry("Long", 200);
    glutAddMenuEntry("Very Long", 250);
    
  auto sphere_particles_submenu = glutCreateMenu(change_sphere_particles);
    glutAddMenuEntry("Very few", 50);
    glutAddMenuEntry("Few", 100);
    glutAddMenuEntry("Normal", 150);
    glutAddMenuEntry("Many", 200);
    glutAddMenuEntry("A lot", 250);
    
  auto particle_ttl_submenu = glutCreateMenu(change_particle_ttl);
    glutAddMenuEntry("Very Shorts", 50);
    glutAddMenuEntry("Short", 100);
    glutAddMenuEntry("Normal", 150);
    glutAddMenuEntry("Long", 200);
    glutAddMenuEntry("Very Long", 250);
    
  auto rendering_type_submenu = glutCreateMenu(change_rendering_type);
    glutAddMenuEntry("Points", 0);
    glutAddMenuEntry("Trails", 1);
    
  auto main_menu = glutCreateMenu(NULL);
    glutAddSubMenu("Number of Spheres", sphere_rate_submenu);
    glutAddSubMenu("Gravity", gravity_power_submenu);
    glutAddSubMenu("Lifetime of Spheres", sphere_ttl_submenu);
    glutAddSubMenu("Number of Particles", sphere_particles_submenu);
    glutAddSubMenu("Lifetime of Particles", particle_ttl_submenu);
    glutAddSubMenu("Rendering Type", rendering_type_submenu);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  
  //loading textures for the sky box with mipmap
  skyTex[0] = LoadTexBMP("sky0.bmp",1);
  skyTex[1] = LoadTexBMP("sky1.bmp",1);

  objBarrel = LoadOBJ("Tree.obj");
  ReadDEM("saddleback.dem");

  ErrCheck("init");
}

/////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  // double f;
  srand(time(NULL));
  initGraphics(argc, argv);
  glEnable(GL_POINT_SMOOTH);
  glutMainLoop();
  return 0;
}
