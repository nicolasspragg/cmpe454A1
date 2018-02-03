// world.cpp


#include "world.h"
#include "lander.h"
#include "ll.h"
#include "gpuProgram.h"
#include "strokefont.h"

#include <time.h> 

#include <sstream>
#include <iomanip>

bool endGame = false;
bool gameWon = false;

float alienX = world->newAlienX();
float alienDrop = 0;
float oneMoreLoop = false;

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
  if (closestDistance <= 4.5){
    // cout << "collision";
    // use absolute value to cover both left and right directions
    if (abs(lander->getHorizSpeed()) <= 0.5 && lander->getVertSpeed() <= 1) {
      // cout << "WIN!!!";
      gameWon = true;
    }
    else {
      // cout << "LOSE!!!";
      gameWon = false;
    }
    lander->setCollided();
  }
  // if the lander has collided, pause execution
  if (lander->getCollided()) {
    if (endGame){ // this allows one more loop of the program to run to draw changes (so we see the win/lose message)
      sleep(2);
      lander->reset();
      endGame = false;
      lander->unSetCollided();
      // exit(0);
    }
    else {
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
  drawStrokeString( ss.str(), -0.95, 0.75, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  // YOUR CODE HERE (modify the above code, too)

  stringstream ss2;
  ss2 << "SCORE: " << world->getPoints();
  drawStrokeString( ss2.str(), 0.6, 0.75, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  stringstream fuelStream;
  float fuelRemaining = lander->getFuel();
  if (fuelRemaining < 0) { fuelRemaining = 0; }
  fuelStream << "Fuel amount: " << fuelRemaining << " L!";
  drawStrokeString( fuelStream.str(),-0.95, 0.65, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );

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
  horizSpeedStream <<"Horizontal Speed " << abs(lander->getHorizSpeed()) << " M/S" << hArrowState;
  drawStrokeString( horizSpeedStream.str(),-0.95, 0.55, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  stringstream vertSpeedStream;
  vertSpeedStream <<"Vertical Speed " <<lander->getVertSpeed() << " M/S" << vArrowState;
  drawStrokeString( vertSpeedStream.str(),-0.95, 0.45, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );

//------------------------MARK TIMER----------------------------------
  clock_t timer = clock();

  int realSeconds = timer / CLOCKS_PER_SEC;
  int seconds = realSeconds%60;

  int minutes = realSeconds/60;
 

  stringstream timerStream;

  timerStream <<"Time elapsed " << minutes << " : " << setfill('0') << setw(2) << seconds;

  drawStrokeString( timerStream.str(),-0.95, 0.35, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  stringstream altitudeStream;
  float altitude = landscape->findAltitude(lander->centrePosition()[0],lander->centrePosition()[1]) - 5.0;
  if (altitude < 0) { altitude = 0; } // it can sometimes go slightly below due to the -5 offset
  altitudeStream << "Altitude: " << altitude;
  drawStrokeString( altitudeStream.str(), -0.95, 0.25, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );

  stringstream neutralize;
  neutralize << "neutralize: [*]";
  drawStrokeString( neutralize.str(), 0.55, 0.85, 0.04, glGetUniformLocation( myGPUProgram->id(), "MVP") );


  if (endGame == true) {
    stringstream gameOver;
    if (gameWon) {
      gameOver << "Landed safely! +5 points";
      world->addPoint();
      world->addPoint();
      world->addPoint();
      world->addPoint();
      world->addPoint();
    } else {
      gameOver << "Crash landing! +0 points";
    }
    // drawStrokeString( test.str(), lander->centrePosition()[0], lander->centrePosition()[1], 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP") );
    drawStrokeString( gameOver.str(), -0.95, -0.75, 0.09, glGetUniformLocation( myGPUProgram->id(), "MVP") );
  }

  stringstream alienBarrier;
  alienBarrier << "atmosphere---------------------------------------------------------------------------------------------------";
  
  // I have added an additional feature to the game where a virus/alien drops onto the planet.
  // If the virus gets past the atmosphere, some of the planet gets infected and your score drops by 1.
  // If you navigate the ship to the neutralize spot (the asterisk in the top right), the virus gets
  // destroyed and your score increases by 1 and the lander resets. It is up to the player to decide if 
  // it is more beneficial to focus on destroying viruses or landing the ship.
  stringstream alienString;
  alienString << "<(X)>";
  if (!zoomView) {
    drawStrokeString(alienBarrier.str(), -1, 0, 0.03, glGetUniformLocation( myGPUProgram->id(), "MVP") );
    drawStrokeString(alienString.str(), alienX, 0.65-alienDrop, 0.015, glGetUniformLocation( myGPUProgram->id(), "MVP") );
  }
  // backwards logic because it needs one more loop to display text
  if (oneMoreLoop == true){
    oneMoreLoop = false;
    sleep(2);
    if (alienDrop >= 0.65){
      world->removePoint();
    } else {
      world->addPoint();
    }
    alienDrop = 0;
  }
  if (alienDrop >= 0.65){ //alien entered atmosphere
    oneMoreLoop = true;
    alienX = newAlienX();
    stringstream msg;
    msg << "Virus entered atmosphere! -1 point";
    drawStrokeString( msg.str(), -0.95, -0.1, 0.07, glGetUniformLocation( myGPUProgram->id(), "MVP") );
  }
  cout <<lander->centrePosition()[0] << "   " << lander->centrePosition()[1] << "\n";
  if (lander->centrePosition()[0]>=(960) && lander->centrePosition()[0]<=(964)){
    if (lander->centrePosition()[1]>=(874) && lander->centrePosition()[1]<=(878)){
      stringstream msg;
      msg << "Virus destroyed! +1 point";
      drawStrokeString( msg.str(), -0.95, -0.1, 0.07, glGetUniformLocation( myGPUProgram->id(), "MVP") );
      oneMoreLoop = true;
      lander->reset();
    }
  }
  alienDrop +=0.0002;

}
