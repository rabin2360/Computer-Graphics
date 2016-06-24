#include "ex1.h"

const float defaultRadius = 10;
const float XRange = 1000;

Sphere::Sphere() {
  //center of the sphere for fireworks
  set(&center.location, /*(myRandom() - 0.5)*XRange*/0, 0.0, 0.0);

  //speed of the ball
  center.speed.x = (myRandom() - 0.5)*5;
  center.speed.y = (myRandom() + 0.5)*7;
  center.speed.z = (myRandom() - 0.5)*5;

  //setting the acceleration for the ball
  set(&center.acceleration, 0, 0-GRAVITY_POWER, 0);

  //making the spheres of various sizes
  radius = (myRandom() + 0.5)*defaultRadius;

  //setting the expiration date for the radius
  center.ttl = int((myRandom() + 0.5)*SPHERE_TTL);
  
  //setting the number of particles the sphere is made up of
  number_of_particles = int((myRandom() + 0.5)*SPHERE_PARTICLES);

  //setting the color
  set(&(center.color), myRandom(), myRandom(), myRandom());
}

void Sphere::move() {
  center.move();
  //add(&center.speed, &center.acceleration);

  //add(&center.location, &center.speed);
}

list<Particle *> Sphere::explode() {
  list<Particle *> toReturn;
  for(int i=0;i<number_of_particles;i++)
    toReturn.push_back(new Particle(this));
  return toReturn;
}
