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
bool firstWindowClose = false;

// WIN / LOOSE
double score = 0.0;
bool hasWon = false;

// SOUND
#define WALK_SOUND_A 0
#define WALK_SOUND_B 1
bool walkSound = 0;


// TIMING
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
double FPS = 0.0;
double msPerFrame = 0.0;
const double BUTTON_PAUSE = 0.2;
const double WALK_SOUND_PAUSE = 0.25;
bool playedAlarm = false;

// HUD
bool showHUD = false;
const float HUDxOffset = 10.0;
const float HUDyOffset = 50.0;


// CUBE SIDES, BOUNDS, GRAVITIES
#define CUBE_LEFT 0
#define CUBE_RIGHT 1
#define CUBE_FRONT 2
#define CUBE_BACK 3
#define CUBE_TOP 4
#define CUBE_BOTTOM 5

const struct CubeBounds {
	const float x_max = -6.0f;   // right boundary
	const float x_min = -50.0f;  // left boundary
	const float y_max = 40.0f;    // up boundary
	const float y_min = -4.0f;    // down boundary
	const float z_max = 57.0f;   // front boundary
	const float z_min = 13.0f;   // back boundary
};

CubeBounds cubeBounds;

const glm::vec3 G_LEFT(9.81f, 0.0f, 0.0f);
const glm::vec3 G_RIGHT(-9.81f, 0.0f, 0.0f);
const glm::vec3 G_FRONT(0.0f, 0.0f, -9.81f);
const glm::vec3 G_BACK(0.0f, 0.0f, 9.81f);
const glm::vec3 G_TOP(0.0f, -9.81f, 0.0f);
const glm::vec3 G_BOTTOM(0.0f, 9.81f, 0.0f);


// WORLD SCALE and POS
const glm::vec3 WORLD_SCALE(2.0f);
const glm::vec3 WORLD_POS(-30.0f, 10.0f, 30.0f);
const float WORLD_ROT_ANGLE = 0.0f;
const glm::vec3 WORLD_ROT_AXES(1.0f, 0.0f, 0.0f);
const float WORLD_MASS = 0.0f;

// SNOWBALL MASS, RADIUS, POS, KEYS
const float SNOWBALL_RADIUS = 0.2f;
const float SNOWBALL_MASS = 1.0f;

const glm::vec3 SNOWBALL_LEFT_POS(-49.0f, 20.0f, 33.0f);
const glm::vec3 SNOWBALL_RIGHT_POS(-6.0f, 12.0f, 34.0f);
const glm::vec3 SNOWBALL_FRONT_POS(-22.0f, 17.0f, 87.0f);
const glm::vec3 SNOWBALL_BACK_POS(-42.0f, 30.0f, -1.0f);
const glm::vec3 SNOWBALL_TOP_POS(-14.0f, 39.0f, 19.0f);
const glm::vec3 SNOWBALL_BOTTOM_POS(-30.0f, -12.0f, 35.0f);

unsigned int SNOWBALL_LEFT_ID = 0;
unsigned int SNOWBALL_RIGHT_ID = 1;
unsigned int SNOWBALL_FRONT_ID = 2;
unsigned int SNOWBALL_BACK_ID = 3;
unsigned int SNOWBALL_TOP_ID = 4;
unsigned int SNOWBALL_BOTTOM_ID = 5;


// COLLECTION POINT
std::string cpPtr = "collectionPoint";
const glm::vec3 COLLECTION_POINT_SCALE(0.5);
const glm::vec3 COLLECTION_POINT_POS(-26.0f, 39.0f, 37.0f);
const float COLLECTION_POINT_ROT_ANGLE = 0.0f;
const glm::vec3 COLLECTION_POINT_ROT_AXES(1.0f, 0.0f, 0.0f);
const float COLLECTION_POINT_MASS = 0.0f;
const glm::vec3 COLLECTION_POINT_BODY_SCALE(0.5, 2, 0.5);


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
const char* JPEG = ".jpeg";
const char* GIF = ".gif";
const std::string BMP = ".bmp";
const std::string TGA = ".tga";

// UTIL
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))  // this is taken from ogldev

#endif // !CONSTANTS_H

