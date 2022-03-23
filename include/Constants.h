#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <glm/glm.hpp>

// LIGHT POSITIONS
glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f, -20.0f, 5.0f),
        glm::vec3(60.f, -20.0f, 5.0f),
        glm::vec3(30.0f,  -20.0f, 40.0f),
        glm::vec3(30.0f,  -20.0f, -25.0f),
        glm::vec3(30.0f,  -50.0f, 0.0f)
};


//FILE PATH'
//OBJECTS
const char* worldPath = "C:\\Users\\dittm\\Documents\\maya\\projects\\default\\scenes\\cube-world.fbx";
const char* heartPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-platformer-kit\\heart.fbx";
const char* treePath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-platformer-kit\\tree.fbx";
const char* lightpostPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\lightpost.fbx";
const char* snowmanPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\snowmanFancy.fbx";
const char* snowPatchPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\snowPatch.fbx";
const char* treePinePath          = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\treePine.fbx";
const char* treePineSnowPath      = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\treePineSnow.fbx";
const char* treePineSnowedPath    = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\treePineSnowed.fbx";
const char* treePineSnowRoundPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\objects\\kenny-holiday-kit\\treePineSnowRound.fbx";


// SHADER
const char* modelVertPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\vertexShader.vert";
const char* modelFragPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\fragmentShader.frag";
const char* lightVertPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\directLight.vert";
const char* lightFragPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\directLight.frag";
const char* HUDVertPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\HUD.vert";
const char* HUDFragPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\shader\\HUD.frag";


// FONTS
const char* fontPath = "C:\\Users\\dittm\\Dev\\polar-adventures-test\\assets\\fonts\\datcub\\datcub.ttf";

#endif // !CONSTANTS_H

