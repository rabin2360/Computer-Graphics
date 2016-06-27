#include "final.h"
#include "CSCI5229.h"

void add(Triple *a, Triple *b) {
  a->x+=b->x;
  a->y+=b->y;
  a->z+=b->z;
}

void set(Triple* t, float x, float y, float z) {
  t->x = x;
  t->y = y;
  t->z = z;
}


//Return random double within range [0,1]
double myRandom()
{
  return (rand()/(double)RAND_MAX);
}


//used by the perlin noise methods
float zVal (const float x, const float y, const float t)
{
  const float x2 = x - 3;
  const float y2 = y + 1;
  const float xx = x2 * x2;
  const float yy = y2 * y2;
  return ((2 * sinf (20 * sqrtf (xx + yy) - 4 * t) +
	   Noise (10 * x, 10 * y, t, 0)) / 200);
}
