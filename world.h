// world.h


#ifndef WORLD_H
#define WORLD_H


#include "headers.h"
#include "landscape.h"
#include "lander.h"
#include "ll.h"


#define BOTTOM_SPACE 0.1f // amount of blank space below terrain (in viewing coordinates) 


class World {

  Landscape *landscape;
  Lander    *lander;
  bool       zoomView; // show zoomed view when lander is close to landscape
  GLFWwindow *window;
  int points = 0;

 public:

  World( GLFWwindow *w ) {
    landscape = new Landscape();
    lander    = new Lander( maxX(), maxY() ); // provide world size to help position lander
    zoomView  = false;
    window    = w;
  }

  void draw();

  void updateState( float elapsedTime );

  void resetLander() {
    lander->reset();
  }

  // World extremes (in world coordinates)

  float minX() { return 0; }
  float maxX() { return landscape->maxX(); }

  float minY() { return 0; }
  float maxY() { return (landscape->maxX() - landscape->minX()) / SCREEN_ASPECT * (2 - BOTTOM_SPACE) / 2; }
  void addPoint() { points += 1; }
  void removePoint() { points -= 1; }
  int getPoints() { return points; }

  vec3 alien;
  float newAlienX() {
    srand(time(NULL));
    return ((float)rand()/RAND_MAX);
  }
};


#endif
