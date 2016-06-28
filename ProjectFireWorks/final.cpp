//////////////////////////////////////////////////////////////////
// Author: Rabin Ranabhat
//
/////////////////////////////////////////////////////////////////



#include "final.h"

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
int light = 1;
//light azimuth
int zh = 90;
//ambient light in percentage
int ambient = 31;//45;
//diffuse light in percentage
int diffuse = 0;//100;
//specular light in percentage
int specular = 6;//10;
//dist of light
float dist = 380.0;
//elevation of light
float ylight = 0.0;
//shininess value
float shinyvec[1];
//emission intensity
int emission = 0;
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
int skyTex[6];
int dim = 100;

//object variables
int objTree;
int objBlocks;

//terrain
float z[65][65];       //  DEM data
float zmin=+1e8;       //  DEM lowest location
float zmax=-1e8;       //  DEM highest location
float zmag=0.6;          //  DEM magnification

//water variables
int waterTex;
int waterTexture = 1;

//pool variables
int marbleTex;

//global view variables
int gWidth;
int gHeight;
  
#define RESOLUTION 40 //water ripples rate
static float surface[6 * RESOLUTION * (RESOLUTION + 1)];
static float normal[6 * RESOLUTION * (RESOLUTION + 1)];

//shader variables
int refractShader = 0;
//////
int movX = 0;
int movZ = 0;
int movY = 0;

int wsize = 3200;
bool paused = false;
int squaresize = 16;
GLubyte square[16][16][2];
GLuint textureObj;
std::vector<int> arrayOfSquares;  // contains x- and y-coords, x- and y- velocity, current rotation, rotation velocity, and frames since creation
int smokeAmt=5,varAmt=0;  // control amount and placement of smoke
int wind=0;

void initData(void) {
  frames = 0;
  SPHERES_PER_FRAME = 0.03;
  GRAVITY_POWER = 0.05;
  SPHERE_TTL = 150;
  SPHERE_PARTICLES = 200;
  PARTICLE_TTL = 80;
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

  frames++;

  //creating the spheres for the fireworks display
  if(SPHERES_PER_FRAME > myRandom()){
    spheres.push_back(new Sphere());
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

static void ball(double x,double y,double z,double r)
{
  float yellow[] = {1.0,1.0,0.0,1.0};
  float Emission[]  = {0.0,0.0,0.01*emission,1.0};
  //  Save transformation
  glPushMatrix();

  //  Offset, scale and rotate
  glTranslated(x,y,z);
  //glScaled(r,r,r);

  //  White ball
  glColor3f(1,1,1);
  glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
  glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
  glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
  
  glutSolidSphere(r, 18, 18);

  //  Undo transofrmations
  glPopMatrix();
}


void drawTopSkyBox(double D)
{
  glBegin(GL_QUADS);
  glTexCoord2f(0,0); glVertex3f(+D,+D,-D);
  glTexCoord2f(1,0); glVertex3f(+D,+D,+D);
  glTexCoord2f(1,1); glVertex3f(-D,+D,+D);
  glTexCoord2f(0,1); glVertex3f(-D,+D,-D);
  glEnd();

}

void skyBox(double D)
{
  //left,right,front,back,up and down
  glColor3f(1,1,1);
  glEnable(GL_TEXTURE_2D);

  //  Sides
  glBindTexture(GL_TEXTURE_2D,skyTex[2]);

  //front
  glBegin(GL_QUADS);
  glTexCoord2f(0,0); glVertex3f(-D,-0.1*D,-D);
  glTexCoord2f(1,0); glVertex3f(+D,-0.1*D,-D);
  glTexCoord2f(1,1); glVertex3f(+D,+D,-D);
  glTexCoord2f(0,1); glVertex3f(-D,+D,-D);
  glEnd();


  //right
    glBindTexture(GL_TEXTURE_2D,skyTex[0]);
 glBegin(GL_QUADS);
 
  glTexCoord2f(0,0); glVertex3f(+D,-0.1*D,-D);
  glTexCoord2f(1,0); glVertex3f(+D,-0.1*D,+D);
  glTexCoord2f(1,1); glVertex3f(+D,+D,+D);
  glTexCoord2f(0,1); glVertex3f(+D,+D,-D);
  glEnd();

  //back
    glBindTexture(GL_TEXTURE_2D,skyTex[3]);
 glBegin(GL_QUADS);
 
  glTexCoord2f(0,0); glVertex3f(+D,-0.1*D,+D);
  glTexCoord2f(1,0); glVertex3f(-D,-0.1*D,+D);
  glTexCoord2f(1,1); glVertex3f(-D,+D,+D);
  glTexCoord2f(0,1); glVertex3f(+D,+D,+D);
 glEnd();

 //left
    glBindTexture(GL_TEXTURE_2D,skyTex[1]);
 glBegin(GL_QUADS);
 
  glTexCoord2f(0,0); glVertex3f(-D,-0.1*D,+D);
  glTexCoord2f(1,0); glVertex3f(-D,-0.1*D,-D);
  glTexCoord2f(1,1); glVertex3f(-D,+D,-D);
  glTexCoord2f(0,1); glVertex3f(-D,+D,+D);
  glEnd();

  //  Top
  glBindTexture(GL_TEXTURE_2D,skyTex[4]);
  drawTopSkyBox(D);

  //Bottom
  /*int i,j;
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
  */

    glBindTexture(GL_TEXTURE_2D,skyTex[5]);

  glBegin(GL_QUADS);
  glTexCoord2f(0,0); glVertex3f(-D,-0.01*D,+D);
  glTexCoord2f(1,0); glVertex3f(+D,-0.01*D,+D);
  glTexCoord2f(1,1); glVertex3f(+D,-0.01*D,-D);
  glTexCoord2f(0,1); glVertex3f(-D,-0.01*D,-D);
  glEnd();

  glDisable(GL_TEXTURE_2D);

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

  if(waterTexture){
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
  }
  /* The water */
  GLfloat texVertices [] = {0,0,1,0,0.5,0.5};
   

  glColor4f(0.39,0.58,0.92,0.5);
  //glColor3f(1,1,1);
  glEnableClientState (GL_NORMAL_ARRAY);
  glEnableClientState (GL_VERTEX_ARRAY);
  //glEnableClientState (GL_TEXTURE_COORD_ARRAY_EXT);


  if(waterTexture)
    {
      glNormalPointer (GL_FLOAT, 0, normal);
        glEnable (GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, waterTex);
    
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

    }
  
  glVertexPointer (3, GL_FLOAT, 0, surface);
  glTexCoordPointer(1, GL_FLOAT, 0, texVertices);


  
  for (i = 0; i < RESOLUTION; i++)
    {
      glDrawArrays (GL_TRIANGLE_STRIP, i * length, length);
    }

  if(waterTexture){
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  
  glDisable(GL_TEXTURE_2D);
  glDisableClientState (GL_NORMAL_ARRAY);
  }
  glDisableClientState(GL_VERTEX_ARRAY);

  ErrCheck("water");
  glPopMatrix();
}

// creates random, smoothed noise texture to use as the basis of the smoke
void makeSquare(){
  int i,j,k,coeffs[6];
  double offsets[6];
  for(i=0;i<6;i++){
    offsets[i] = 2*M_PI * (rand()%1000)/1000.0;
  }

  for(i=0;i<squaresize;i++){
    for(j=0;j<squaresize;j++){
      square[i][j][0] = 0;
      for(k=0;k<6;k++) square[i][j][0]+=40*sin(sin(i+offsets[k])*(j+1)*2);
      square[i][j][1] = 255*(1-pow(hypot(squaresize/2-i,squaresize/2-j)/hypot(squaresize/2,squaresize/2),.5));
    }
  }

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  glGenTextures(1,&textureObj);
  glBindTexture(GL_TEXTURE_2D,textureObj);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE_ALPHA,squaresize,squaresize,0,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,square);

  
}

void makeSmoke()
{
    glBindTexture(GL_TEXTURE_2D,textureObj);
    //glMatrixMode(GL_MODELVIEW_MATRIX);
  glLoadIdentity();

  int i;
    double alpha,size;
  for(i=arrayOfSquares.size()-7;i>=0;i-=7){
    // place
    glLoadIdentity();
    glTranslatef(arrayOfSquares[i],arrayOfSquares[i+1],0.0);
    glRotatef(arrayOfSquares[i+4],0.0,0.0,1.0);

    // set apparent alpha based on time since creation
    alpha = 1.0 - arrayOfSquares[i+6]/60.0;
    glColor4f(alpha,alpha,alpha,alpha);

    // draw with size based on time
    size = .075 * (1+arrayOfSquares[i+6]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.0);
    glVertex2f(-squaresize*size,-squaresize*size);
    glTexCoord2f(1.0,0.0);
    glVertex2f(squaresize*size,-squaresize*size);
    glTexCoord2f(1.0,1.0);
    glVertex2f(squaresize*size,squaresize*size);
    glTexCoord2f(0.0,1.0);
    glVertex2f(-squaresize*size,squaresize*size);
    glEnd();

    // reset location
    glLoadIdentity;
  }
}

// runs every 50 ms; moves the smoke squares
void increment(int t){
  int i,j;
  while(rand()%(smokeAmt+1)){  // add square
    arrayOfSquares.push_back(wsize/2+rand()%(2*varAmt+1)-varAmt);  // X
    arrayOfSquares.push_back(wsize/4+rand()%(2*varAmt+1)-varAmt);  // Y
    arrayOfSquares.push_back(rand()%5-2);  // X-vel
    arrayOfSquares.push_back(rand()%3);  // Y-vel
    arrayOfSquares.push_back(rand()%360);  // rotation
    arrayOfSquares.push_back(rand()%11-5);  // rotation-vel
    arrayOfSquares.push_back(0);  // timer
  }

  for(i=0;i<arrayOfSquares.size();i+=7){
    // delete if out of screen or at time of 3 sec
    if(arrayOfSquares[i]<(-squaresize*4) || arrayOfSquares[i]>wsize+squaresize*4 || arrayOfSquares[i+1]>wsize+squaresize*4 || arrayOfSquares[i+6]>60){
      for(j=0;j<7;j++){
	arrayOfSquares.erase(arrayOfSquares.begin()+i);
      }
      i -= 7;
      continue;
    }

    // add velocities to current position
    arrayOfSquares[i] += arrayOfSquares[i+2];
    arrayOfSquares[i+1] += arrayOfSquares[i+3];
    arrayOfSquares[i+4] += arrayOfSquares[i+5];

    // accelerate
    if(rand()%5==0) arrayOfSquares[i+3]++;
    if(wind && rand()%(abs(wind)+1) && rand()%5==0) arrayOfSquares[i+2]+=(wind)/abs(wind);

    // add to timer
    arrayOfSquares[i+6]++;
  }

  // reset timer
  if(!paused) glutTimerFunc(50,increment,0);
  glutPostRedisplay();
}

void smokeReset()
{
  arrayOfSquares.clear();
  makeSquare();
  paused = false;
  wind = 0;
  smokeAmt = 5;
  varAmt = 0;
}

void frontWall()
{
  //  Draw the wall
  double i = 0;
  double j = 0;
  int inc = 1;
  glColor3f(0.33,0.33,0.33);
   glNormal3f(0,0,1); 
   glBegin(GL_QUADS);
   for (i=0;i<41;i+=inc)
      for (j=0;j<41;j+=inc)
      {
         glVertex2d(i,j);
         glVertex2d(i+inc,j);
         glVertex2d(i+inc,j+inc);
         glVertex2d(i,j+inc);
      }
   glEnd();
   
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

   /*glPushMatrix();
   glTranslatef(0,0,0);
   glScalef(100,100,100);
   makeSmoke();
   glPopMatrix();
   */
   
  /*glPushMatrix();
  glTranslatef(-293, 44, 305);
  glScalef(20, 27, 26);
  glCallList(objBlocks);
  glPopMatrix();
  */
  
  //tree-1: right
  glPushMatrix();
  glTranslatef(319,46,-184);
  glScalef(2,2,2);
  glCallList(objTree);
  glPopMatrix();
  
  //tree-3: left
  glPushMatrix();
  glTranslatef(-310,43,45);
  glScalef(1,1.15,1);
  glCallList(objTree);
  glPopMatrix();

  /*
  //tree-2: left
  glPushMatrix();
  glTranslatef(-312,0,133);
  glCallList(objTree);
  glPopMatrix();
 
  
  //tree-1: left
  glPushMatrix();
  glTranslatef(-332,0,199);
  glCallList(objTree);
  glPopMatrix();
  */
  //tree in the middle
  glPushMatrix();
  glTranslatef(0,46,-305);
  glCallList(objTree);
  glPopMatrix();

  //glMatrixMode (GL_TEXTURE);
  //glLoadIdentity ();
  //glMatrixMode(GL_PROJECTION);
 
   //back wall
   glPushMatrix();
   glColor3f(1, 1, 1);
   glBindTexture (GL_TEXTURE_2D, marbleTex);
   glTranslatef(1,26,-326);
   glScalef(16,1,1);
   glutSolidCube(41);
  glPopMatrix();

  
  //left wall
  glPushMatrix();

   glColor3f(1, 1, 1);
  glTranslatef(-306,26,-13);
  glRotatef(-90,0,1,0);
  glScalef(15.3,1,1);
  glutSolidCube(41);

  glPopMatrix();
 
  //right wall
  glPushMatrix();
   
   glColor3f(1, 1, 1);
  glTranslatef(308,26,-13);
  glRotatef(-90,0,1,0);
  glScalef(15.3,1,1);
  glutSolidCube(41);

 glPopMatrix();

   
 
  glDisable(GL_BLEND);  


  glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT|GL_DEPTH_BUFFER_BIT);
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
  //important, this will make sure that the inverted trees do not appear below the surface but just on the water
  glClear(GL_STENCIL_BUFFER_BIT);
  
  //draw the water
  //glPushAttrib(GL_DEPTH_BUFFER_BIT);
  //glEnable(GL_DEPTH_TEST);
  drawWaterRipples(300,300,300,0,38,0);
  //glEnable(GL_DEPTH_TEST);
  //glPopAttrib();
  //glColorMask(1,1,1,1);
  //glEnable(GL_DEPTH_TEST);
  
  glStencilFunc(GL_EQUAL, 1, 0xFF);
  glStencilMask(0x00);
  glDepthMask(GL_TRUE);
  glStencilOp(GL_KEEP,GL_KEEP, GL_KEEP);
   
  glDisable(GL_DEPTH_TEST);
  glPushMatrix();
  glTranslatef(0,0,-296);
  glScalef(0.8,-1,4);
   glCallList(objTree);
  glPopMatrix();
  
    //tree-3: left
  glPushMatrix();
  glScalef(1,-1.15,1);
  glTranslatef(-310,0,45);
  glCallList(objTree);
  glPopMatrix();
  /*
  //tree-2: left
  glPushMatrix();
    glScalef(1,-1,1);
  glTranslatef(-312,0,133);
  glCallList(objTree);
  glPopMatrix();
 
  
  //tree-1: left
  glPushMatrix();
    glScalef(1,-1,1);
  glTranslatef(-332,0,199);
  glCallList(objTree);
  glPopMatrix();
  */
  
  //tree-1: right
  glPushMatrix();
  glEnable(GL_DEPTH_TEST);
  glTranslatef(323,0,-184);
  glScalef(2,-2,2);
  glDisable(GL_DEPTH_TEST);
  glCallList(objTree);
 
  glPopMatrix();

  glDisable(GL_STENCIL_TEST);
 
  glPopMatrix();
  
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glPopAttrib();

  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //determine light
  if(light)
    {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      float Emission[] =  {0.01*emission,0.01*emission,0.01*emission,1.0};
      float Shininess[] =  {0};
      
      //Light position
      //float Position[]  = {dist*Cos(zh),ylight,dist*Sin(zh),1.0};
      float Position[] = {-306,234,301};
      //color for the rotating light
      glColor3f(1,1,1);

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

      glPushMatrix();
      //settings for light  0
      glMaterialfv(GL_FRONT, GL_SHININESS, Shininess);
      glMaterialfv(GL_BACK, GL_SHININESS, Shininess);
      glPopMatrix();
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
      glPushMatrix();
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
      glPopMatrix();
    }
    
    break;
    
  default:
    glPushMatrix();
    glPointSize(POINT_LINE_SIZE);
    glBegin(GL_POINTS);
    for(auto it = particles.begin(); it != particles.end(); it++) {

      glColor3f((*it)->color.x, (*it)->color.y, (*it)->color.z);
      glVertex3f((*it)->location.x, (*it)->location.y, (*it)->location.z);
    }
    glEnd();
    glPopMatrix();
  }
  

 
 //drawAxes(60);
 
 //front wall
  glPushMatrix();
  glColor4f(0.49, 0.61, 0.75, 0.8);
  glTranslatef(1,25,280);
  glScalef(15.9,1,1);
  glutSolidCube(41);
  glPopMatrix();
  glDisable(GL_BLEND);  

  /* 
     glUseProgram(refractShader);
      int id;
 
      id = glGetUniformLocation(refractShader,"MVMatrix");
      if (id>=0) glUniform1f(id,X);
      id = glGetUniformLocation(refractShader,"MVPMatrix");
      if (id>=0) glUniform1f(id,Y);
      id = glGetUniformLocation(refractShader,"NormalMatrix");
      if (id>=0) glUniform1f(id,Z);
      id = glGetUniformLocation(refractShader,"TextureMatrix");
      if (id>=0) glUniform1f(id,time);

   //  Draw the teapot or cube

   glEnable(GL_TEXTURE_2D);
   glutSolidTeapot(1.0);
   glDisable(GL_TEXTURE_2D);
   //  No shader for what follows
   glUseProgram(0);
  */
  //differentAngle();
   
  glFlush();
  glutSwapBuffers(); 
  
  ErrCheck("display");
}

///////////////////////////////////////////////

void keyboard(unsigned char key, int x, int y)
{
  switch(key)
    {
    case 27:
      exit(0);
      break;

    case 'a':
      if (ambient < 100) ambient += 1;
      break;

    case 'A':
      if (ambient > 0) ambient -= 1;
      break;

    case 's':
      if (specular < 100) specular += 1;
      break;

    case 'S':
      if (specular > 0) specular -= 1;
      break;

    case 'd':
      if (diffuse < 100) diffuse += 1;
      break;

    case 'D':
      if (diffuse > 0) diffuse -= 1;
      break;

    case 'L':
    case 'l':
      if(light)
	light = 0;
      else
	light = 1;
      break;

    case 'q':
      Ey -= 3;
      Ly -= 3;

      break;
    case 'Q':
      Ey += 3;
      Ly += 3;
      break;
    case 't':
    case 'T':
      if(waterTexture)
	waterTexture = 0;
      else
	waterTexture = 1;
      break;
      
    case 'x':
      movX += 1;
      break;

    case 'X':
      movX -= 1;
      break;

   case 'y':
      movY += 1;
      break;

    case 'Y':
      movY -= 1;
      break;

    case 'z':
      movZ += 1;
      break;

    case 'Z':
      movZ -= 1;
      break;

         
    default:
      break;
    }

  if(light)
    printf("ambient %d specular %d diffuse %d\n", ambient, specular, diffuse);

  //printf("movX %d movY %d movZ %d\n",movX, movY,movZ);
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

  case GLUT_KEY_PAGE_UP:
    Ly += 0.1;
    break;

  case GLUT_KEY_PAGE_DOWN:
    Ly -= 0.1;
    break;
  
  default:
    return;
    
  }

  printf("Ex %f Ey %f Ez %f Lx %f, Ly%f, Lz %f, angle %d\n", Ex,Ey,Ez, Lx, Ly, Lz, angle);
  glutPostRedisplay();
}
////////////////////////////////////////////////////

void reshape(int width, int height)
{
  gWidth = width;
  gHeight = height;
  
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

///////////////////////////Reading the shader files////////////////////////////
/*
 *  Read text file
 */
char* ReadText(char *file)
{
   int   n;
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Print Shader Log
 */
void PrintShaderLog(int obj,char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

/*
 *  Print Program Log
 */
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}

/*
 *  Create Shader
 */
int CreateShader(GLenum type,char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file);
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 */
int CreateShaderProg(char* VertFile,char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
    int vert = CreateShader(GL_VERTEX_SHADER  ,VertFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   // Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;

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

  if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
      
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutReshapeFunc(reshape);
  glutIdleFunc(animate);
  
  glEnable(GL_DEPTH_TEST);
  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  
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
  
  //loading textures -left,right,front,back,up and down
  skyTex[0] = LoadTexBMP("darkcity_lf.bmp",0);
  skyTex[1] = LoadTexBMP("darkcity_rt.bmp",0);
  skyTex[2] = LoadTexBMP("darkcity_ft.bmp",0);
  skyTex[3] = LoadTexBMP("darkcity_bk.bmp",0);
  skyTex[4] = LoadTexBMP("darkcity_up.bmp",0);
  skyTex[5] = LoadTexBMP("darkcity_dn.bmp",0);

      //texture for water
  waterTex = LoadTexBMP("reflection.bmp",1);  
  marbleTex = LoadTexBMP("marbleTex.bmp",1);
  
      //loading objects
  objTree = LoadOBJ("Tree.obj");
  objBlocks = LoadOBJ("predator.obj");
  //ReadDEM("saddleback.dem");

  refractShader = CreateShaderProg("refract.vert","refract.frag");
  
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
