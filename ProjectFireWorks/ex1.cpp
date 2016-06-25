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
int objTree;

//terrain
float z[65][65];       //  DEM data
float zmin=+1e8;       //  DEM lowest location
float zmax=-1e8;       //  DEM highest location
float zmag=0.6;          //  DEM magnification

  int countVal = 0;

//water variables
int waterTex;
#define RESOLUTION 30 //water ripples rate
static float surface[6 * RESOLUTION * (RESOLUTION + 1)];
static float normal[6 * RESOLUTION * (RESOLUTION + 1)];


///////////////////////////////////////////////
static float	zVal (const float x, const float y, const float t)
{
  const float x2 = x - 3;
  const float y2 = y + 1;
  const float xx = x2 * x2;
  const float yy = y2 * y2;
  return ((2 * sinf (20 * sqrtf (xx + yy) - 4 * t) +
	   Noise (10 * x, 10 * y, t, 0)) / 200);
}

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

void drawTopSkyBox(double D)
{
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.5,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.5,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.0,1); glVertex3f(-D,+D,-D);
   glEnd();

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

   //  Top
   glBindTexture(GL_TEXTURE_2D,skyTex[1]);
   drawTopSkyBox(D);

   //Bottom
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

void drawWater(int radius)
{
  glPushMatrix();
  glColor4f(0.39,0.58,0.92,0.5);
  glBegin(GL_TRIANGLE_STRIP);

  for(double i = 0; i< 2*M_PI; i+= M_PI/6)
    {
      glVertex3f(cos(i)*radius, -0.01, sin(i)*radius);
      glVertex3f(0,-0.01,0);
      glVertex3f(cos(i+(M_PI/6))*radius, -0.01, sin(i+(M_PI/6))*radius);
    }

  glEnd();

  glPopMatrix();
}

void drawWaterRipples(int sx, int sy, int sz,int tx, int ty, int tz)
{
  glPushMatrix();
  
   const float t = glutGet (GLUT_ELAPSED_TIME) / 1000.;
  const float delta = 2. / RESOLUTION;
  const unsigned int length = 2 * (RESOLUTION + 1);
  const float xn = (RESOLUTION + 1) * delta + 1;
  unsigned int i;
  unsigned int j;
  float x;
  float y;
  unsigned int indice;
  unsigned int preindice;

  /* Yes, I know, this is quite ugly... */
  float v1x;
  float v1y;
  float v1z;

  float v2x;
  float v2y;
  float v2z;

  float v3x;
  float v3y;
  float v3z;

  float vax;
  float vay;
  float vaz;

  float vbx;
  float vby;
  float vbz;

  float nx;
  float ny;
  float nz;

  float l;

    glTranslatef (tx, ty, tz);
    glScalef(sx,sy,sz);
  //glRotatef (rotate_y, 1, 0, 0);
  //glRotatef (rotate_x, 0, 1, 0);

  /* Vertices */
  for (j = 0; j < RESOLUTION; j++)
    {
      y = (j + 1) * delta - 1;
      for (i = 0; i <= RESOLUTION; i++)
	{
	  indice = 6 * (i + j * (RESOLUTION + 1));

	  x = i * delta - 1;
	  surface[indice + 3] = x;
	  surface[indice + 4] = zVal (x, y, t);
	  surface[indice + 5] = y;
	  if (j != 0)
	    {
	      /* Values were computed during the previous loop */
	      preindice = 6 * (i + (j - 1) * (RESOLUTION + 1));
	      surface[indice] = surface[preindice + 3];
	      surface[indice + 1] = surface[preindice + 4];
	      surface[indice + 2] = surface[preindice + 5];
	    }
	  else
	    {
	      surface[indice] = x;
	      surface[indice + 1] = zVal(x, -1, t);
	      surface[indice + 2] = -1;
	    }
	}
    }

  /* Normals */
  for (j = 0; j < RESOLUTION; j++)
    for (i = 0; i <= RESOLUTION; i++)
      {
	indice = 6 * (i + j * (RESOLUTION + 1));

	v1x = surface[indice + 3];
	v1y = surface[indice + 4];
	v1z = surface[indice + 5];

	v2x = v1x;
	v2y = surface[indice + 1];
	v2z = surface[indice + 2];

	if (i < RESOLUTION)
	  {
	    v3x = surface[indice + 9];
	    v3y = surface[indice + 10];
	    v3z = v1z;
	  }
	else
	  {
	    v3x = xn;
	    v3y = zVal(xn, v1z, t);
	    v3z = v1z;
	  }

	vax =  v2x - v1x;
	vay =  v2y - v1y;
	vaz =  v2z - v1z;

	vbx = v3x - v1x;
	vby = v3y - v1y;
	vbz = v3z - v1z;

	nx = (vby * vaz) - (vbz * vay);
	ny = (vbz * vax) - (vbx * vaz);
	nz = (vbx * vay) - (vby * vax);

	l = sqrtf (nx * nx + ny * ny + nz * nz);
	if (l != 0)
	  {
	    l = 1 / l;
	    normal[indice + 3] = nx * l;
	    normal[indice + 4] = ny * l;
	    normal[indice + 5] = nz * l;
	  }
	else
	  {
	    normal[indice + 3] = 0;
	    normal[indice + 4] = 1;
	    normal[indice + 5] = 0;
	  }


	if (j != 0)
	  {
	    /* Values were computed during the previous loop */
	    preindice = 6 * (i + (j - 1) * (RESOLUTION + 1));
	    normal[indice] = normal[preindice + 3];
	    normal[indice + 1] = normal[preindice + 4];
	    normal[indice + 2] = normal[preindice + 5];
	  }
	else
	  {
/* 	    v1x = v1x; */
	    v1y = zVal(v1x, (j - 1) * delta - 1, t);
	    v1z = (j - 1) * delta - 1;

/* 	    v3x = v3x; */
	    v3y = zVal(v3x, v2z, t);
	    v3z = v2z;

	    vax = v1x - v2x;
	    vay = v1y - v2y;
	    vaz = v1z - v2z;

	    vbx = v3x - v2x;
	    vby = v3y - v2y;
	    vbz = v3z - v2z;

	    nx = (vby * vaz) - (vbz * vay);
	    ny = (vbz * vax) - (vbx * vaz);
	    nz = (vbx * vay) - (vby * vax);

	    l = sqrtf (nx * nx + ny * ny + nz * nz);
	    if (l != 0)
	      {
		l = 1 / l;
		normal[indice] = nx * l;
		normal[indice + 1] = ny * l;
		normal[indice + 2] = nz * l;
	      }
	    else
	      {
		normal[indice] = 0;
		normal[indice + 1] = 1;
		normal[indice + 2] = 0;
	      }
	  }
      }


  /* Render wireframe? */
  //glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

  /* The water */
  //GLfloat texVertices [] = {0,0,1,0,0.5,0.5};
  // glEnable (GL_TEXTURE_2D);

   glColor4f(0.39,0.58,0.92,0.5);
 
   //glEnableClientState (GL_NORMAL_ARRAY);
  glEnableClientState (GL_VERTEX_ARRAY);
  //glEnableClientState (GL_TEXTURE_COORD_ARRAY_EXT);

  //glBindTexture(GL_TEXTURE_2D, waterTex);
  //glNormalPointer (GL_FLOAT, 0, normal);
  glVertexPointer (3, GL_FLOAT, 0, surface);
  //glTexCoordPointer(1, GL_FLOAT, 0, texVertices);
  
  for (i = 0; i < RESOLUTION; i++)
    {
      glDrawArrays (GL_TRIANGLE_STRIP, i * length, length);
    }
  
  //glDisable(GL_TEXTURE_2D);
  
  ErrCheck("water");
  glPopMatrix();
 }

void display()
{
   
  // Clear the screen
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
 
  glLoadIdentity();

  gluLookAt(Ex, Ey, Ez,Ex+Lx, Ly, Ez+Lz,0.0, 1.0, 0.0);

  glPushMatrix();
  skyBox(8*dim);
  glPopMatrix();

  //drawWaterRipples();
  
   //glCallList(objPineTree);
  
  glPushMatrix();
  glTranslatef(0,0,-100);
  glCallList(objTree);
  glPopMatrix();

  
  glPushMatrix();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  //glDisable(GL_DEPTH_TEST);
  //glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
  glEnable(GL_STENCIL_TEST);
  
  glStencilFunc(GL_ALWAYS, 1, 0xFF);
  glStencilOp(GL_REPLACE,GL_REPLACE, GL_REPLACE);
  glStencilMask(0xFF);
  glDepthMask(GL_FALSE);
  glClear(GL_STENCIL_BUFFER_BIT);
  
  //draw the water
  glPushAttrib(GL_DEPTH_BUFFER_BIT);
  //glEnable(GL_DEPTH_TEST);
  // drawWater(100);
  //int sx, int sy, int sz,int tx, int ty, int tz
    drawWaterRipples(300,300,300,0,1,0);
  //glEnable(GL_DEPTH_TEST);
  glPopAttrib();
  //glColorMask(1,1,1,1);
  //glEnable(GL_DEPTH_TEST);
  
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  glStencilMask(0x00);
  glDepthMask(GL_TRUE);
  glStencilOp(GL_KEEP,GL_KEEP, GL_KEEP);

  glPushMatrix();
  glTranslatef(0,0,-100);
  glScalef(0.8,-1,4);
  glCallList(objTree);
  glPopMatrix();

  glDisable(GL_STENCIL_TEST);
 
  glPopMatrix();
  
  //drawWater(200);
  //glDisable(GL_BLEND);

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
    //glutSolidSphere(2, 18, 18);
    
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
  
  
  glDisable(GL_BLEND);
  //drawAxes(60);
  glFlush();
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

  if(key == 'q')
    {
      Ey -= 3;
      Ly -= 3;
    }

  
  if(key == 'Q')
    {
      Ey += 3;
      Ly += 3;
    }
  
  glutPostRedisplay();
}

///////////////////////////////////////////////

//special characters function
static void special(int k, int x, int y) {
  GLfloat speed = 2.5;
 
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
  InitNoise ();
  
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

  //glShadeModel(GL_SMOOTH);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glClearAccum(0.0f, 0.0f, 0.0f, 1.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

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

  waterTex = LoadTexBMP("reflection.bmp",1);
  
  objTree = LoadOBJ("Tree.obj");
  
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
