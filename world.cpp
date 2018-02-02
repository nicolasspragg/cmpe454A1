// world.cpp


#include "world.h"
#include "lander.h"
#include "ll.h"
#include "gpuProgram.h"
#include "strokefont.h"

#include <time.h> 

#include <sstream>

bool endGame = false;
bool gameWon = false;

void World::updateState( float elapsedTime )

{

  // See if any keys are pressed for thrust

  if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS) // right arrow
    lander->rotateCW( elapsedTime );

  if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS) // left arrow
    lander->rotateCCW( elapsedTime );

  if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS) // down arrow
    lander->addThrust( elapsedTime );

  // Update the position and velocity

  lander->updatePose( elapsedTime );

  // See if the lander has touched the terrain

  vec3 closestTerrainPoint = landscape->findClosestPoint( lander->centrePosition() );
  float closestDistance = ( closestTerrainPoint - lander->centrePosition() ).length();
  // cout << closestDistance;
  // cout << "\n";

  // Find if the view should be zoomed

  zoomView = (closestDistance < ZOOM_RADIUS);

  // Check for landing or collision and let the user know

  // YOUR CODE HERE
  if (closestDistance <= 5){
    // cout << "collision";
    // use absolute value to cover both left and right directions
    if (abs(lander->getHorizSpeed()) <= 0.5 && lander->getVertSpeed() <= 1) {
      cout << "\n";
      cout << "WIN!!!";
      cout << "\n";
      gameWon = true;
    }
    else {
      cout << "\n";
      cout << "LOSE!!!";
      cout << "\n";
      gameWon = false;
    }
    lander->setCollided();
  }
  // if the lander has collided, pause execution
  if (lander->getCollided()) {
    if (endGame){ // this allows one more loop to draw changes
      sleep(2);
      lander->reset();
      endGame = false;
      lander->unSetCollided();
      // exit(0);
    }
    else {
      // this part is where we can notify the user of the result of the game

      
      endGame = true;
    }
  }
}



void World::draw()

{
  mat4 worldToViewTransform;

  if (!zoomView) {

    // Find the world-to-view transform that transforms the world
    // to the [-1,1]x[-1,1] viewing coordinate system, with the
    // left edge of the landscape at the left edge of the screen, and
    // the bottom of the landscape BOTTOM_SPACE above the bottom edge
    // of the screen (BOTTOM_SPACE is in viewing coordinates).

    float s = 2.0 / (landscape->maxX() - landscape->minX());

    worldToViewTransform
      = translate( -1, -1 + BOTTOM_SPACE, 0 )
      * scale( s, s, 1 )
      * translate( -landscape->minX(), -landscape->minY(), 0 );

  } else {
    // cout << "zoomed";
    // Find the world-to-view transform that is centred on the lander
    // and is ZOOM_WIDTH wide (in world coordinates).
    // YOUR CODE HERE
    float s = 0.25*ZOOM_RADIUS / (landscape->maxX() - landscape->minX());

    worldToViewTransform
      = scale( s, s, 1 )
      * translate( -lander->centrePosition()[0], -lander->centrePosition()[1], 0 );
 
  }

  // Draw the landscape and lander, passing in the worldToViewTransform
  // so that they can append their own transforms before passing the
  // complete transform to the vertex shader.

  landscape->draw( worldToViewTransform );
  lander->draw( worldToViewTransform );

  // Draw the heads-up display (i.e. all text).

  stringstream ss;

  drawStrokeString( "LUNAR LANDER", -0.2, 0.85, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  ss.setf( ios::fixed, ios::floatfield );
  ss.precision(1);

  ss << "SPEED " << lander->speed() << " m/s";
  drawStrokeString( ss.str(), -0.95, 0.75, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  // YOUR CODE HERE (modify the above code, too)

  stringstream ss2;
  ss2 << "POINTS: " << world->getPoints();
  drawStrokeString( ss2.str(), 0.6, 0.75, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  stringstream fuelStream;
  fuelStream << "Fuel amount: " << lander->getFuel() << " L!";
  drawStrokeString( fuelStream.str(),-0.95, 0.65, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  string hArrowState = "";

  if(lander->getHorizSpeed() < 0){
    hArrowState = " <--- ";
  } else {
    hArrowState = " ---> ";
  }


  string vArrowState = "";

  if(lander->getVertSpeed() > 0){
    vArrowState = " ^ ";
  } else {
    vArrowState = " V ";
  }


  stringstream horizSpeedStream;
  horizSpeedStream <<"Horizontal Speed " <<lander->getHorizSpeed() << " M/S" << hArrowState;
  drawStrokeString( horizSpeedStream.str(),-0.95, 0.55, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  stringstream vertSpeedStream;
  vertSpeedStream <<"Vertical Speed " <<lander->getVertSpeed() << " M/S" << vArrowState;
  drawStrokeString( vertSpeedStream.str(),-0.95, 0.45, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

//------------------------MARK TIMER----------------------------------
  clock_t timer = clock();

  int seconds = timer / CLOCKS_PER_SEC;
  
  if(seconds > 60 ) { seconds = 0;}
  int minutes = seconds/60;
  

  

 

  stringstream timerStream;

  timerStream <<"Seconds elapsed " << minutes << " : " << seconds;

  drawStrokeString( timerStream.str(),-0.95, 0.35, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  if (endGame == true) {
    stringstream gameOver;
    if (gameWon) {
      gameOver << "Landed safely! +1 point";
      world->addPoint();
    } else {
      gameOver << "Crash landing! +0 points";
    }
    // drawStrokeString( test.str(), lander->centrePosition()[0], lander->centrePosition()[1], 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );
    drawStrokeString( gameOver.str(), -0.95, 0.25, 0.09, glGetUniformLocation( myGPUProgram->id(), "MVP") );
  }
  

}
