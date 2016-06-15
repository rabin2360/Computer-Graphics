#include "CSCI5229.h"

//drawing cone using radius and height parameters
/*void drawCone(double h, double r, float tx, float ty, float tz, float sx, float sy, float sz)
{
  glPushMatrix();

  int i;

  glTranslatef(tx, ty, tz);
  glScalef(sx, sy, sz);

  //draw the outside of the cone
  glBegin(GL_TRIANGLES);

  int inc = 20;
  
  for(i = 0; i<=360; i+=inc)
    {
      glNormal3d(Sin(i),1, -Cos(i));
      glTexCoord2f(r*Sin(i),-r*Cos(i)); glVertex3f(r*Sin(i),0,-r*Cos(i));

      glNormal3d(Sin(i+(inc)/2), h, -Cos(i+(inc)/2));
      glTexCoord2f(r*Sin(i+(inc)/2),-r*Sin(i+(inc)/2)); glVertex3f(0,h,0);

      glNormal3d(Sin(i+inc),1, -Cos(i+inc));
      glTexCoord2f(r*Sin(i+(inc)),-r*Sin(i+(inc))); glVertex3f(r*Sin(i+inc),0,-r*Cos(i+inc));
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
  
  }*/

void drawCylinder(double h, double r, float tx, float ty, float tz, float sx, float sy, float sz)
{
  glPushMatrix();
  
  glTranslatef(tx,ty,tz);
  glScalef(sx,sy,sz);

  glBegin(GL_QUADS);
  int i;
  int inc = 20;
  
  for(i = 0; i<=360; i+=inc)
    {
      glNormal3d(Sin(i),1, -Cos(i));
      glVertex3f(r*Sin(i),0,-r*Cos(i));

      glNormal3d(Sin(i),h, -Cos(i));
      glVertex3f(r*Sin(i),h,-r*Cos(i));

      glNormal3d(Sin(i+inc),h, -Cos(i+inc));
      glVertex3f(r*Sin(i+inc),h,-r*Cos(i+inc));

      glNormal3d(Sin(i+inc),1, -Cos(i+inc));
      glVertex3f(r*Sin(i+inc),0,-r*Cos(i+inc));
    }

  glEnd();
  
  glPopMatrix();
}
