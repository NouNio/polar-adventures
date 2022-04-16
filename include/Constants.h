#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <glm/glm.hpp>


/* ----------------------------------------------------------------- */
// SETTINGS
/* ----------------------------------------------------------------- */
// FROM INI
// WINDOW 
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
bool fullscreen = false;
unsigned int refreshRate = 60;
float brightness = 1.0f;

// TIMING
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
double FPS = 0.0;
double msPerFrame = 0.0;

// HUD
bool showHUD = false;
const float HUDxOffset = 10.0;
const float HUDyOffset = 50.0;


// CUBE SIDES
#define CUBE_LEFT 0
#define CUBE_RIGHT 1
#define CUBE_FRONT 2
#define CUBE_BACK 3
#define CUBE_TOP 4
#define CUBE_BOTTOM 5


// SNOWBALL KEYS
unsigned int PALM_TREE = 0;
unsigned int LABYRINTH = 1;
unsigned int PLATFORM = 2;
unsigned int LEVER = 3;


// PHYSICS
bool debug = false;

// LIGHT POSITIONS
const unsigned int nPointLights = 5;  //5
glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f, -20.0f, 5.0f),
        glm::vec3(60.f, -20.0f, 5.0f),
        glm::vec3(30.0f,  -20.0f, 40.0f),
        glm::vec3(30.0f,  -20.0f, -25.0f),
        glm::vec3(30.0f,  -50.0f, 0.0f)
};

// FILE TYPES
const char* PNG = ".png";
const char* JPG = ".jpg";
const char* GIF = ".gif";

#endif // !CONSTANTS_H

