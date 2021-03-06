// lander.h
//
// Unit of length is the meter


#ifndef LANDER_H
#define LANDER_H


#include "headers.h"


class Lander {

  static float landerVerts[];	// lander segments as vertex pairs
  int numSegments;		// number of line segments in the lander model
  
  GLuint VAO;			// VAO for lander geometry

  vec3 position;		// position in world coordinates (m)
  vec3 velocity;		// velocity in world coordinates (m/s)
  float fuel = 1000;
  float orientation;		// orientation (radians CCW)
  float angularVelocity;	// angular velocity (radians/second CCW)

  float worldMaxX, worldMaxY;	// world dimensions

  float horizSpeed;
  float vertSpeed;

  bool collided = false;

  float altitude;


 public:
 void setCollided() {
   collided = true;
 }
 bool getCollided() {
   return collided;
 }
 void unSetCollided() {
  collided = false;
 }
 void setAltitude(float alt){
  altitude = alt;
 }
 float getAltitude(){
  return altitude;
 }

  Lander( float maxX, float maxY ) {
    worldMaxX = maxX;
    worldMaxY = maxY;
    reset();
    setupVAO();
  };

  void setupVAO();  

  void draw( mat4 &worldToViewTransform );

  void updatePose( float deltaT );

  void reset() {
    position = vec3( 0.05 * worldMaxX, 0.7 * worldMaxY, 0.0  );
    velocity = vec3( 30.0f, 0.0f, 0.0f );

    orientation = 0;
    angularVelocity = 0;
  }

  void rotateCW( float deltaT );
  void rotateCCW( float deltaT );
  void addThrust( float deltaT );

  vec3 centrePosition() { return position; }
  float getFuel() { return fuel; }
  float speed() { return velocity.length(); }
  float getHorizSpeed() { return velocity[0];}
  float getVertSpeed() { return velocity[1];}
};


#endif
