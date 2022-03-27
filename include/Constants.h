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

// LIGHT POSITIONS
glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f, -20.0f, 5.0f),
        glm::vec3(60.f, -20.0f, 5.0f),
        glm::vec3(30.0f,  -20.0f, 40.0f),
        glm::vec3(30.0f,  -20.0f, -25.0f),
        glm::vec3(30.0f,  -50.0f, 0.0f)
};


/* ----------------------------------------------------------------- */
// FILE PATH
/* ----------------------------------------------------------------- */
// OBJECTS
const char* worldPath             = "C:\\Users\\dittm\\Documents\\maya\\projects\\default\\scenes\\cube-world.fbx";
const char* playerPath            = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-block-characters\\player.fbx";
const char* collectPointPath      = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-space-kit\\heart.fbx";      //fix here
const char* heartPath             = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-platformer-kit\\heart.fbx";
const char* treePath              = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-platformer-kit\\tree.fbx";
const char* lightpostPath         = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\lightpost.fbx";
const char* snowmanPath           = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\snowmanFancy.fbx";
const char* snowPatchPath         = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\snowPatch.fbx";
const char* treePinePath          = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\treePine.fbx";
const char* treePineSnowPath      = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\treePineSnow.fbx";
const char* treePineSnowedPath    = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\treePineSnowed.fbx";
const char* treePineSnowRoundPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\treePineSnowRound.fbx";
const char* snowBallPath          = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\snowball\\snowball.fbx";

// TEST OBJECTS
const char* isoGroundSnowPath     = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\isometric-cubes\\groundSnow\\groundSnow.fbx";
const char* matchBoxPath          = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\matchbox\\veb.fbx";
const char* reptileMagePath       = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\reptile-mage\\LizPosed.obj";
const char* campfirePath          = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\campfire\\campfire.blend";
const char* plinthPath            = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\dungeon-plinth\\plinth.fbx";

const char* birdPath              = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\animals\\bird.fbx";
const char* chickPath             = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\animals\\chick.fbx";
const char* fishPath              = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\animals\\fish.fbx";
const char* foxPath               = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\animals\\fox.fbx";
const char* whalePath             = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\animals\\whale.fbx";

const char* stoneArrayPath        = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\stone-array\\SP2.obj";
const char* stoneArkPath          = "C:\\Users\\dittm\\Downloads\\stone-arch-in-low-poly-style\\source\\ark.fbx";

// SHADER
const char* modelVertPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\vertexShader.vert";
const char* modelFragPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\fragmentShader.frag";
const char* lightVertPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\directLight.vert";
const char* lightFragPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\directLight.frag";
const char* HUDVertPath   = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\HUD.vert";
const char* HUDFragPath   = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\HUD.frag";
const char* snowBallVertPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\snowBall.vert";
const char* snowBallFragPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\snowBall.frag";

// FONTS
const char* fontPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\fonts\\datcub\\datcub.ttf";

// INI 
const char* iniPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\settings.ini";


// FILE TYPES
const char* PNG = ".png";
const char* JPG = ".jpg";
const char* GIF = ".gif";

#endif // !CONSTANTS_H

