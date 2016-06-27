#include "final.h"

Particle::Particle(Sphere *s) {

  //location
  location.x = s->center.location.x + 2*(myRandom() - 0.5);//*s->radius;
  location.y = s->center.location.y + 2*(myRandom() - 0.5);//*s->radius;
  location.z = s->center.location.z + 2*(myRandom() - 0.5);//*s->radius;

  //speed
  speed.x = (myRandom() - 0.5)*10;
  speed.y = (myRandom() - 0.5)*10;
  //speed.z = 0;
  speed.z = (myRandom() - 0.5)*10;

  //setting accelaration
  set(&acceleration, 0, 0-GRAVITY_POWER*0.5, 0);

  //setting color
  set(&color, s->center.color.x, s->center.color.y, s->center.color.z);

  //changing speed
  add(&speed, &(s->center.speed));

  //changing acceleration
  add(&acceleration, &(s->center.acceleration));
  //lifetime of the particle
  ttl = int((myRandom() + 0.5)*PARTICLE_TTL);

}

void Particle::move() {

  if(prevLocations.size()<10)
    {
      //storing the previous locations
      prevLocations.push_back(location);

    }
  else
    {
      prevLocations.pop_front();
      prevLocations.push_back(location);
    }
  
  
  previousLocation = location;

  add(&speed, &acceleration);

  //cout<<"speed x:"<<speed.x<<endl;
  add(&location, &speed);
  
}

