#pragma once
// open gl related libs
#include <GL/glew.h>
#include <gl/GLU.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext.hpp"


#include <ft2build.h>
#include FT_FREETYPE_H
#include <INIReader.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <BulletViewer.h>
#include <irrklang/irrKlang.h>

// std libs
#include <iostream>
#include <string>

// self made libs
#include <glm/gtx/rotate_vector.hpp>
#include <Animation.h>
#include <Animator.h>
#include <Camera.h>
#include <Constants.h>
#include <FileManager.h>
#include <Framebuffer.h>
#include <GravityModel.h>
#include <HUD.h>
#include <KinematicPlayer.h>
#include <Model.h>
#include <Physics.h>
#include <ParticleSystem.h>
#include <Shader.h>
#include <Skybox.h>
#include <Snowball.h>


/* ------------------------------------------------------------------------------------ */
// Protottypes
/* ------------------------------------------------------------------------------------ */
void readINI();
GLFWwindow* initGLFWandGLEW();
//bool hasWon();

bool hasLost();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void setPointLightShaderParameters(Shader& shader, std::string pointLightNumber, glm::vec3 postion);
void computeTimeLogic();
void activateShader(Shader* shader);
void delay(double seconds);
void playWalkSound();
void playEndOfGameSound();
void transitionToEndOfGameScreen(GLFWwindow* window);
glm::vec3 hsv2rgb(float h, float s, float v);
void setCubeSides();
std::vector<glm::vec3> determineColours(std::vector<int> numbers);
glm::vec3 determineColour(int i);
void drawMap(Shader& s, Mesh& a, Mesh& b, float transparency, glm::vec2 pos, float scale, glm::vec3 colour, glm::vec3 c2, std::vector <glm::vec2> offsets);
void drawonHUd(Shader& s, Mesh& m);
void drawCharge(Shader& s, Mesh& out, Mesh& quad, float transparency, glm::vec2 pos, float scale, glm::vec3 colour);

/* ------------------------------------------------------------------------------------ */
// Create Objects and make settings
/* ------------------------------------------------------------------------------------ */
// gameplay 
bool sound = false;
bool spaceWasDown = false;
bool renderMap = false;
float currJumpForce = 2.0;
const float START_JUMP_FORCE = 2.0;
const float MAX_JUMP_FORCE = 20.0;
double startTimeSec = 0.0;
float animTimeSec = 0.0;
double maxGameTime = 300.0;  // time in seconds until player looses

// sound
irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();


// camera & physics
Camera camera(0.0f, 0.0f, 0.0f, 0.0f, glm::vec3(0.0f, 22.0f, 3.0f));
Physics* pHandler;
KinematicPlayer* playerController;
std::map<unsigned int, Snowball*> snowballs;
std::vector<Snowball*> collectedSnowballs;
std::vector<Snowball*> savedSnowballs;

// lighting & background
glm::vec3 directLightPos(30.f, 90.0f, 10.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
double lastHUDPress, lastShotPress, lastVFCPress, lastWalkSound, lastESCPress, lastCPress, lastMPress = glfwGetTime();
Skybox* skybox;
float fov, near, far;

// new gravity model
CubeGravtiy cubeGravity(HALF_SIDES);


float HUDstart;
int currRenderedObjects = 0;

FileManager* fm;
ParticleSystem* parSys;
ParticleSystem* snowBottom;

unsigned int handle;

unsigned int color;
unsigned int normal;
unsigned int depth;
unsigned int postprocessor;
unsigned int edge;
unsigned int rbo;
//top, front right, bottom, back, left sum of opposite sides = 5
std::vector< int> sides = { 0, 1, 2 ,5, 4, 3 };
std::vector< int> OGsides = { 0, 1, 2 ,5, 4, 3 };
std::vector<glm::vec2> offsets{ glm::vec2(0,0), glm::vec2(0,1) ,glm::vec2(1,0) ,glm::vec2(1,1) ,glm::vec2(0,-1) ,glm::vec2(-1,0) };





GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

int main(void)
{

    stbi_set_flip_vertically_on_load(false);
    /* ------------------------------------------------------------------------------------ */
    // load setting.ini and inititalize openGL & bullet as well as physics handler and file manager
    /* ------------------------------------------------------------------------------------ */
    fm = new FileManager();
    readINI();
    camera = Camera(fov, float(SCR_WIDTH) / float(SCR_HEIGHT), near, far, glm::vec3(0.0f, 22.0f, 3.0f));
    HUDstart = SCR_HEIGHT - HUDyOffset;
    GLFWwindow* window = initGLFWandGLEW();

    pHandler = new Physics(debug);

    /* ------------------------------------------------------------------------------------ */
    // load shader
    /* ------------------------------------------------------------------------------------ */
    Shader modelShader(fm->getShaderPath("modelVert"), fm->getShaderPath("modelFrag"));
    Shader directLightShader(fm->getShaderPath("directLightVert"), fm->getShaderPath("directLightFrag"));
    Shader idleShader(fm->getShaderPath("idleVert"), fm->getShaderPath("idleFrag"));
    Shader animModelShader(fm->getShaderPath("animModelVert"), fm->getShaderPath("animModelFrag"));
    Shader skyboxShader(fm->getShaderPath("skyboxVert"), fm->getShaderPath("skyboxFrag"));
    Shader HUDShader(fm->getShaderPath("HUDvert"), fm->getShaderPath("HUDfrag"));
    Shader particleShader(fm->getShaderPath("particleVert"), fm->getShaderPath("particleFrag"));
    Shader snowBottomShader(fm->getShaderPath("particleVert"), fm->getShaderPath("particleFrag"));


    Shader hud2(fm->getShaderPath("hud2.vert", true), fm->getShaderPath("hud2.frag", true));
    /* ------------------------------------------------------------------------------------ */
    // load models related physics objects
    /* ------------------------------------------------------------------------------------ */
    // world
    Model newWorld(fm->getWorldPath("game-world-centered"));
    btRigidBody* newWorldBody = pHandler->addMeshShape(&newWorld, glm::vec3(0), 0);                     // create a physics obj
    btBvhTriangleMeshShape* newWorldShape = ((btBvhTriangleMeshShape*)(newWorldBody->getCollisionShape()));   // now create an easily scalable version of that body
    pHandler->addScaledMeshShape(newWorldShape, WORLD_POS, WORLD_MASS, WORLD_SCALE);      // add to world
    pHandler->getWorld()->removeRigidBody(newWorldBody);

    // permeable wall
    Model permWall(fm->getObjPath("perm-wall"));

    //models for map and shit
    Model outline(fm->getObjPath("map_outline.fbx", true));

    Model map(fm->getObjPath("map.fbx", true));
    Model Quad(fm->getObjPath("plane.fbx", true));
    Model quadline(fm->getObjPath("plane_edge.fbx", true));
    /* ------------------------------------------------------------------------------------ */
    // ANIMATED PLAYER MODEL
    /* ------------------------------------------------------------------------------------ */
    Model animPlayer(fm->getPlayerPath("player"), true, true, PNG);
    Animation walkAnim(fm->getPlayerPath("player"), &animPlayer);
    Animator animator(&walkAnim);
    playerController = new KinematicPlayer(pHandler, camera.pos, &camera, &animPlayer);



    /* ------------------------------------------------------------------------------------ */
    // SNOWBALLS
    /* ------------------------------------------------------------------------------------ */
    Snowball snowball_left(SNOWBALL_LEFT_ID, fm->getObjPath("snowball"), SNOWBALL_LEFT_POS, SNOWBALL_RADIUS, SNOWBALL_MASS, G_LEFT, pHandler, &camera);     // on LEFT side (cave)
    snowballs.insert(std::pair<unsigned int, Snowball*>(SNOWBALL_LEFT_ID, &snowball_left));

    Snowball snowball_right(SNOWBALL_RIGHT_ID, fm->getObjPath("snowball"), SNOWBALL_RIGHT_POS, SNOWBALL_RADIUS, SNOWBALL_MASS, G_RIGHT, pHandler, &camera);  // on RIGHT side (labyrinth)
    snowballs.insert(std::pair<unsigned int, Snowball*>(SNOWBALL_RIGHT_ID, &snowball_right));

    Snowball snowball_front(SNOWBALL_FRONT_ID, fm->getObjPath("snowball"), SNOWBALL_FRONT_POS, SNOWBALL_RADIUS, SNOWBALL_MASS, G_FRONT, pHandler, &camera);  // on FRONT side (palm tree)
    snowballs.insert(std::pair<unsigned int, Snowball*>(SNOWBALL_FRONT_ID, &snowball_front));

    Snowball snowball_back(SNOWBALL_BACK_ID, fm->getObjPath("snowball"), SNOWBALL_BACK_POS, SNOWBALL_RADIUS, SNOWBALL_MASS, G_BACK, pHandler, &camera);      // on BACK side (platform)
    snowballs.insert(std::pair<unsigned int, Snowball*>(SNOWBALL_BACK_ID, &snowball_back));

    Snowball snowball_top(SNOWBALL_TOP_ID, fm->getObjPath("snowball"), SNOWBALL_TOP_POS, SNOWBALL_RADIUS, SNOWBALL_MASS, G_TOP, pHandler, &camera);          // on TOP side (crater)
    snowballs.insert(std::pair<unsigned int, Snowball*>(SNOWBALL_TOP_ID, &snowball_top));

    Snowball snowball_bottom(SNOWBALL_BOTTOM_ID, fm->getObjPath("snowball"), SNOWBALL_BOTTOM_POS, SNOWBALL_RADIUS, SNOWBALL_MASS, G_BOTTOM, pHandler, &camera);  // on BOTTOM side (pyramid)
    snowballs.insert(std::pair<unsigned int, Snowball*>(SNOWBALL_BOTTOM_ID, &snowball_bottom));


    /* ------------------------------------------------------------------------------------ */
    // Collection point
    /* ------------------------------------------------------------------------------------ */
    Model collectionPoint(fm->getObjPath("snowman"));
    btRigidBody* colPntBody = pHandler->addCylinder(COLLECTION_POINT_POS, COLLECTION_POINT_MASS, COLLECTION_POINT_BODY_SCALE);
    pHandler->activateColCallBack(colPntBody);
    colPntBody->setUserPointer(&cpPtr);


    /* ------------------------------------------------------------------------------------ */
    // Particle System
    /* ------------------------------------------------------------------------------------ */
    snowBottom = new ParticleSystem(&snowBottomShader, G_BOTTOM, PARTICLE_SYSTEM_MIDDLE, PARTICLE_SYSTEM_SPEED);

    /* ------------------------------------------------------------------------------------ */
    // HUD
    /* ------------------------------------------------------------------------------------ */
    HUD hud(fm->getFontPath("arial"));
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    HUDShader.use();
    glUniformMatrix4fv(glGetUniformLocation(HUDShader.ID, "proj"), 1, GL_FALSE, glm::value_ptr(projection));
    hud.update(&camera, FPS, msPerFrame, pHandler, playerController);

    hud2.use();
    glUniformMatrix4fv(glGetUniformLocation(hud2.ID, "proj"), 1, GL_FALSE, glm::value_ptr(projection));


    /* ------------------------------------------------------------------------------------ */
    // skybox
    /* ------------------------------------------------------------------------------------ */
    skybox = new Skybox("galaxy", TGA);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    modelShader.use();
    modelShader.setInt("skybox", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->skyboxTex);

    animModelShader.use();
    animModelShader.setInt("skybox", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->skyboxTex);
    /* ------------------------------------------------------------------------------------ */
    // edge detection
    /* ------------------------------------------------------------------------------------ */



    Shader combination(fm->getShaderPath("passOn.vert", true), fm->getShaderPath("screen.frag", true));
    combination.use();
    glm::vec3 edgeCol(1.0,1,1.0);
    combination.setFloat("brightness", brightness);
    combination.setVec3("edgeCol", edgeCol);
    Shader processor(fm->getShaderPath("passOn.vert", true), fm->getShaderPath("sobel.frag", true));

    initialize(SCR_WIDTH, SCR_HEIGHT, color, normal, depth, edge, handle, postprocessor, rbo, attachments);

    /* ------------------------------------------------------------------------------------ */
    // sound
    /* ------------------------------------------------------------------------------------ */
    if (sound)soundEngine->play2D(fm->getAudioPath("background1").c_str(), true);


    /* ------------------------------------------------------------------------------------ */
    // main render loop
    /* ------------------------------------------------------------------------------------ */

    startTimeSec = glfwGetTime();
    do {
        glBindFramebuffer(GL_FRAMEBUFFER, handle);
        /* ------------------------------------------------------------------------------------ */
        // TIME LOGIC
        /* ------------------------------------------------------------------------------------ */
        computeTimeLogic();
        ///double currTimeSeconds = glfwGetTime();
        //float animTimeSec = (float)(currTimeSeconds - startTimeSec);

        if (maxGameTime - glfwGetTime() < 30.0 && !playedAlarm && sound) {
            soundEngine->play2D(fm->getAudioPath("alarm").c_str(), false);
            playedAlarm = true;
        }

        /* ------------------------------------------------------------------------------------ */
        // INPUT 
        /* ------------------------------------------------------------------------------------ */
        processInput(window);
        animator.updateAnim(deltaTime);

        // render
        //glClearColor(0.6f, 0.7f, 0.9f, 1.0f);;
        animModelShader.use();
        animModelShader.setVec3("viewPos", camera.pos);
        activateShader(&modelShader);


        // view/projection transformations
        projection = camera.GetProjection();
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        /* ------------------------------------------------------------------------------------ */
        // GAME OBJECTS
        /* ------------------------------------------------------------------------------------ */
        newWorld.draw(modelShader, WORLD_POS, WORLD_ROT_ANGLE, WORLD_ROT_AXES, WORLD_SCALE);
        permWall.draw(modelShader, glm::vec3(-21.5, 0.725, -1.35), 0, glm::vec3(1, 0, 0), glm::vec3(0.9f, 0.01f, 0.53f));
        collectionPoint.draw(modelShader, COLLECTION_POINT_POS, COLLECTION_POINT_ROT_ANGLE, COLLECTION_POINT_ROT_AXES, COLLECTION_POINT_SCALE);


        if (snowballs.size() == 1)
            bottomSnowballActive = true;

        for (const auto& item : snowballs) {  // item.second == Snowball*
            if (item.second->getID() != SNOWBALL_BOTTOM_ID) {
                item.second->shrink(deltaTime);
                item.second->s_draw(&modelShader, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            }
            else if (item.second->getID() == SNOWBALL_BOTTOM_ID && bottomSnowballActive) {
                item.second->shrink(deltaTime);
                item.second->s_draw(&modelShader, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }


        /* ------------------------------------------------------------------------------------ */
        // PHYSICS & PLAYER
        /* ------------------------------------------------------------------------------------ */
        // remove the rigidBodies of the collected and saved snowballs from the world to avoid null ptr exceptions (i.e. crashs) when collision callbacks are called

        for (Snowball* snowball : collectedSnowballs) {
            if (snowball->getBody()->isInWorld()) {
                btRigidBody* tempBody = snowball->getBody();
                pHandler->getWorld()->removeCollisionObject(tempBody);
            }
        }

        for (Snowball* snowball : savedSnowballs) {  // here also add the score directly to the record
            if (snowball->getBody()->isInWorld()) {
                btRigidBody* tempBody = snowball->getBody();
                pHandler->getWorld()->removeCollisionObject(tempBody);
                score += snowball->getBodyScale().x() * 100;

                if (snowballs.size() == 0 && collectedSnowballs.size() == 0) {
                    hasWon = true;
                }
            }
        }

        pHandler->stepSim(deltaTime);
        pHandler->setDebugMatrices(view, projection);  // set debug draw matrices
        pHandler->debugDraw();                         // call the debug drawer


        /* ------------------------------------------------------------------------------------ */
        // ANIMATED MODEL
        /* ------------------------------------------------------------------------------------ */
        playerController->update(pNONE, deltaTime);
        // get anim data
        if (playerController->isWalking()) {
            animModelShader.use();

            auto boneTransforms = animator.getBoneTransforms();
            for (int i = 0; i < boneTransforms->size(); ++i) {
                animModelShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]",  (*boneTransforms)[i]);
            }

            // render the loaded model
            playerController->draw(&animModelShader, 1.0);
        }
        else {
            idleShader.use();
            playerController->draw(&idleShader, 0.01);
        }

        /* ------------------------------------------------------------------------------------ */
        // PARTICLE SYSTEM
        /* ------------------------------------------------------------------------------------ */
        if (!bottomSnowballActive)
            snowBottom->update(deltaTime, projection, view);


        /* ------------------------------------------------------------------------------------ */
        // SKYBOX
        /* ------------------------------------------------------------------------------------ */
        skybox->draw(&skyboxShader);
        combination.use();
        combination.setBool("edgeActive", true);
        //combination.setVec3("edgeCol", hsv2rgb( 0.0, 0.0, 1.0));
        doImageProcessing(color, normal, depth, edge, handle, postprocessor, processor, combination);

        currRenderedObjects = camera.frustum->getRenderedObjects();
        /* ------------------------------------------------------------------------------------ */
        // HUD
        /* ------------------------------------------------------------------------------------ */
        hud.update(&camera, FPS, msPerFrame, pHandler, playerController);  //--> updates all HUD messages
        if (showHUD)
        {
            hud.renderAll(HUDShader, HUDxOffset, HUDstart);
        }
        hud.renderLine(HUDShader, ("TIME: "  + std::to_string((int)glm::floor(maxGameTime-glfwGetTime()))), 1000, 700, glm::vec3(1, 0, 0));
        setCubeSides();
        if (renderMap) {
            drawMap(hud2, outline.meshes[0], map.meshes[0], 0.5, glm::vec2(1015.0, 100.0f), 75.0, glm::vec3(0.8), glm::vec3(), offsets);
            hud.renderNumbers(HUDShader, 1000.0f, 100.0f, sides, offsets, determineColours(sides), 50.f);
            drawCharge(hud2, quadline.meshes[0], Quad.meshes[0], 0.2, glm::vec2(105.0, 100.0f), 75.0, glm::vec3(0.0));
        }

        camera.frustum->resetRenderedObjects();

        // GLFW: renew buffers and check all I/O events
        glfwSwapBuffers(window);
        glfwPollEvents();
        clearAll(handle, postprocessor);
    } while (!firstWindowClose && !hasWon && !hasLost());

    transitionToEndOfGameScreen(window);

    /* ------------------------------------------------------------------------------------ */
    // end of game render loop
    /* ------------------------------------------------------------------------------------ */
    do {
        computeTimeLogic();
        processInput(window);

        // render
        glClearColor(0.6f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        hud.renderEndOfGame(HUDShader, SCR_WIDTH / 2.0f - 100.0f, SCR_HEIGHT / 2.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window));


    /* ------------------------------------------------------------------------------------ */
    // TERMINATE
    /* ------------------------------------------------------------------------------------ */
    pHandler->deleteAll();
    playerController->~KinematicPlayer();
    glfwTerminate();
    return 0;
}


// read information from the settings.ini file
void readINI()
{

    INIReader iniReader(fm->getIniPath());
    // window
    SCR_WIDTH = iniReader.GetInteger("window", "width", 800);
    SCR_HEIGHT = iniReader.GetInteger("window", "height", 600);
    fullscreen = iniReader.GetBoolean("window", "fullscreen", false);
    refreshRate = iniReader.GetInteger("window", "refresh_rate", 60);
    brightness = float(iniReader.GetReal("window", "brightness", 1.0f));
    //camera
    fov = float(iniReader.GetReal("camera", "fov", 30.0f));
    near = float(iniReader.GetReal("camera", "near", 0.1f));
    far = float(iniReader.GetReal("camera", "far", 100.0f));
    // physics
    debug = iniReader.GetBoolean("physics", "debug", false);

    // gameplay
    maxGameTime = iniReader.GetReal("gameplay", "maxGameTime", 300.0);

    // sound 
    sound = iniReader.GetBoolean("sound", "sound", true);
}


// usual GLFW and GLEW initialization and some config settings
GLFWwindow* initGLFWandGLEW()
{
    // init and config of glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);  // use GLFW_OPENGL_COMPAT_PROFILE when using the BulletViewer for debugging, else GLFW_OPENGL_CORE_PROFILE
    glfwWindowHint(GLFW_REFRESH_RATE, refreshRate);


    // creat the window, set as context and activate necessary callbacks; the monitor is needed for fullscreen mode
    GLFWmonitor* monitor = nullptr;

    if (fullscreen)
        monitor = glfwGetPrimaryMonitor();

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Polar Adventures Test", monitor, NULL);

    if (window == NULL)
    {
        std::cout << "There was an error creating the GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);   // make GLFW measure our mouse
    glEnable(GL_DEPTH_TEST);

    // init GLEW
    if (glewInit() != GLEW_OK) {
        std::cout << "There was an error creating initializing GLEW" << std::endl;
        glfwTerminate();
    }

    return window;
}


bool hasLost() {
    if (glfwGetTime() > maxGameTime) {
        return true;
    }

    return false;
}


// process all input that is triggered by the keyboard
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && (glfwGetTime() - lastESCPress) >= BUTTON_PAUSE) {
        if (!firstWindowClose)
            firstWindowClose = !firstWindowClose;
        else
            glfwSetWindowShouldClose(window, true);

        lastESCPress = glfwGetTime();
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && (glfwGetTime() - lastMPress) >= BUTTON_PAUSE) {
     
        renderMap = !renderMap;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (currJumpForce < MAX_JUMP_FORCE)
            currJumpForce += 0.1;
        else
            currJumpForce = MAX_JUMP_FORCE;

        playerController->setJumpForce(currJumpForce);
        spaceWasDown = true;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && spaceWasDown) {
        camera.processKeyboard(UP, deltaTime);
        playerController->update(pUP, deltaTime);
        currJumpForce = START_JUMP_FORCE;
        spaceWasDown = false;
    }



    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.processKeyboard(LEFT, deltaTime);
        playerController->update(pLEFT, deltaTime);
    }


    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.processKeyboard(RIGHT, deltaTime);
        playerController->update(pRIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && (glfwGetTime() - lastHUDPress) >= BUTTON_PAUSE) {
        showHUD = !showHUD;
        lastHUDPress = glfwGetTime();
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        playerController->setWalking(true);
        camera.processKeyboard(BACKWARD, deltaTime);
        playerController->update(pBACKWARD, deltaTime);
        playWalkSound();
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && (glfwGetTime() - lastVFCPress) >= BUTTON_PAUSE) {
        camera.setVFCEnabled(!camera.getVFCEnabled());
        lastVFCPress = glfwGetTime();
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        playerController->setWalking(true);
        camera.processKeyboard(FORWARD, deltaTime);
        playerController->update(pFORWARD, deltaTime);
        playWalkSound();
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
        playerController->setWalking(false);
    }
}


// necessary for resizing the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    deleteBuffers(color, normal, depth, edge, handle, postprocessor, rbo);
    initialize(width, height, color, normal, depth, edge, handle, postprocessor, rbo, attachments);

    glViewport(0, 0, width, height);
}


// process all input that is triggered by the mouse
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}


// compute deltaTime, FPS and ms/Frame
void computeTimeLogic()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    animTimeSec = (float)(glfwGetTime() - startTimeSec);
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    msPerFrame = deltaTime * 1000;  // deltaTime is the time in s from frame to frame, i.e. taking to compute 1 frame; 1s = 1000ms
    FPS = 1.0 / deltaTime;
}


// plays a sound for walking with some interval between the sounds and only if player on ground
void playWalkSound() {
    if (glfwGetTime() - lastWalkSound >= WALK_SOUND_PAUSE && playerController->onGround()) {

        if (sound) {
            switch (walkSound) {
            case WALK_SOUND_A:
                soundEngine->play2D(fm->getAudioPath("walk1").c_str(), false);
                break;
            case WALK_SOUND_B:
                soundEngine->play2D(fm->getAudioPath("walk2").c_str(), false);
                break;
            }
        }
        walkSound = !walkSound;
        lastWalkSound = glfwGetTime();
    }
}


void playEndOfGameSound() {

    if (sound) {
        if (hasWon) {
            soundEngine->play2D(fm->getAudioPath("win").c_str(), false);
        }
        else {
            soundEngine->play2D(fm->getAudioPath("lose").c_str(), false);
        }
    }
}


void transitionToEndOfGameScreen(GLFWwindow* window) {
    glClearColor(0.6f, 0.7f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);
    delay(1);
    playEndOfGameSound();
    delay(2.1);
    soundEngine->drop();
}


void delay(double seconds) {
    double delayStartTime = glfwGetTime();
    do {
        // nothing
    } while (glfwGetTime() - seconds <= delayStartTime);
}


// set some prominent vaues for the shader used fr most of the models in the scene
void activateShader(Shader* shader)
{
    // be sure to activate shader when setting uniforms/drawing objects
    shader->use();
    //for efficiency reasons
    shader->setFloat("pi", glm::pi<float>());
    shader->setVec3("viewPos", camera.pos);

    /*
       Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
       the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
       by defining light types as classes and set their values in there, or by using a more efficient uniform approach
       by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
    */
    // directional light
    shader->setVec3("directionalLight.direction", -20.2f, -21.0f, -20.3f);
    shader->setVec3("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
    shader->setVec3("directionalLight.diffuse", 1.0f, 1.0f, 1.0f);              // change here for scene brightness
    shader->setVec3("directionalLight.specular", 0.5f, 0.5f, 0.5f);

    for (unsigned int i = 0; i < nPointLights; i++)
    {
        setPointLightShaderParameters(*shader, std::to_string(i), pointLightPositions[i]);
    }
}


// set the pointlights[] uniform values in the specified shader
void setPointLightShaderParameters(Shader& shader, std::string pointLightNumber, glm::vec3 postion)
{
    shader.setVec3("pointLights[" + pointLightNumber + "].pos", postion);
    shader.setVec3("pointLights[" + pointLightNumber + "].ambient", 0.5f, 0.5f, 0.5f);
    shader.setVec3("pointLights[" + pointLightNumber + "].diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("pointLights[" + pointLightNumber + "].specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kc", 1.0f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kl", 0.007f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kq", 0.0002f);
}









std::vector<glm::vec3> determineColours(std::vector<int> numbers) {
    // I top, front, right, bottom, back, left
    // snowballs: left, right, front, back, top bottom
    std::vector<glm::vec3> result;
    for (size_t i = 0; i < numbers.size(); i++)
    {

        result.push_back(determineColour(numbers[i]));

    }
    return result;
}
glm::vec3 determineColour(int i) {
    int var = i;
    switch (i) {
    case 0:
        var = 4;
        break;
    case 1:
        var = 2;
        break;
    case 2:
        var = 1;
        break;
    case 3:
        var = 0;
        break;
    case 4:
        var = 3;
        break;
    case 5:
        var = 5;
        break;
    }
    /*
                    if (snowballs.size() == 0 && collectedSnowballs.size() == 0) {
                    hasWon = true;
                }
    */

    /*for (const auto& item : snowballs) {  // item.second == Snowball*
        if (item.second->getID() != SNOWBALL_BOTTOM_ID) {

        } */

        if (i== 5&& !bottomSnowballActive) {
            return glm::vec3(1, 0, 0);
        }
        for (size_t i = 0; i < savedSnowballs.size(); i++)
        {
            Snowball* v = savedSnowballs[i];
            if (v->getID() == var) { return glm::vec3(0.5f, 0.5f, 0.5f); }
        }
        for (size_t i = 0; i < collectedSnowballs.size(); i++)
        {
            Snowball* v = collectedSnowballs[i];
            if (v->getID() == var) { return glm::vec3(0, 1, 0); }
        }
        return glm::vec3(1, 1, 0);
    }
    void drawMap(Shader &s, Mesh &a , Mesh &b , float transparency, glm::vec2 pos, float scale, glm::vec3 colour, glm::vec3 c2 ,std::vector <glm::vec2> offsets) {
        s.use();
        s.setVec2("scale", glm::vec2(scale));
        s.setVec2("xyoffset", pos);

        s.setBool("front", true);
        s.setVec3("colour", c2);
        s.setFloat("transparency", 0.0f);
        drawonHUd(s, a);

        s.setBool("front", false);
        s.setFloat("transparency", transparency);
        s.setVec3("colour", colour);
        drawonHUd(s, b);

    }
    //apparently this doesnt like me so yay, bullshitting it is
   void drawonHUd(Shader &s, Mesh  &m) {
       s.use();
       m.drawMesh();

   }
   void drawCharge(Shader&s, Mesh &out, Mesh &quad, float transparency, glm::vec2 pos, float scale, glm::vec3 colour) {
       float percentage = currJumpForce/MAX_JUMP_FORCE;
       scale /= 3;
       s.use();
       s.setVec2("scale", glm::vec2(scale));
       s.setVec2("xyoffset", pos);

       s.setBool("front", true);
       s.setVec3("colour", colour);
  
       s.setFloat("transparency", 0.0f);
       drawonHUd(s, out);
       colour = glm::vec3();
       colour.r = glm::min(1.0f, 2 * percentage);
       colour.g = 2*             glm::max(0.0f,  (percentage - 0.5f));
       pos.y -= (1-percentage)*3 * scale;
       s.setBool("front", false);;
       s.setVec3("colour", colour);
       s.setFloat("transparency", transparency);
       s.setVec2("xyoffset", pos);
       s.setVec2("scale", glm::vec2(scale,scale* percentage * 3));
       drawonHUd(s, quad);

   }
void setCubeSides() {

    int min = 0;

    glm::vec3 front = glm::normalize(playerController->getPlayerFront(camera.front));
    // glm::vec3 front = camera.front;
    switch (playerController->cubeSide) {
    case CUBE_TOP:
        min = 0;
        break;
    case CUBE_FRONT:
        min = 1;
        break;
    case CUBE_RIGHT:
        min = 2;
        break;
    case CUBE_BOTTOM:
        min = 5;
        break;
    case CUBE_BACK:
        min = 4;
        break;
    case CUBE_LEFT:
        min = 3;
        break;
    };
    sides[0] = min;
    sides[3] = 5 - min;

    glm::vec3 right = camera.right;
    //std::vector< int> OGsides = { 0, 1, 2 ,5, 4, 3 };
    std::vector <glm::vec3> referenceValues = { glm::vec3(0,1,0),glm::vec3(0,0,1) ,glm::vec3(1,0,0) ,glm::vec3(0,-1,0) ,glm::vec3(0,0,-1) ,glm::vec3(-1,0,0) };

    //right = glm::rotate(front,glm::radians(-90.0f), referenceValues[sides[0]]);

    min = 0;
    for (size_t i = 0; i < referenceValues.size(); i++)
    {
        if (glm::dot(front, referenceValues[i]) > glm::dot(front, referenceValues[min])) {
            min = i;
        }
    }
    sides[1] = OGsides[min];
    sides[4] = 5 - OGsides[min];

    min = 0;
    for (size_t i = 0; i < referenceValues.size(); i++)
    {
        if (glm::dot(right, referenceValues[i]) > glm::dot(right, referenceValues[min])) {
            min = i;
        }
    }

    sides[2] = OGsides[min];
    sides[5] = 5 - OGsides[min];

}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        {
            if (sound) {
                if (playerController->getSnowBallAmmo() > 0) {
                    soundEngine->play2D(fm->getAudioPath("throw").c_str(), false);
                }
                else {
                    soundEngine->play2D(fm->getAudioPath("noAmmo").c_str(), false);
                }
            }
            playerController->shootSnowball();
            lastShotPress = glfwGetTime();
        }

    }
}
glm::vec3 hsv2rgb(float h, float s, float v) {
    glm::vec3 result = glm::vec3(1.0,0.0,0.0);
    float pi = glm::pi<float>();

    while (h >= (2 * pi)) {
        h = h - 2 * pi;
    }
    while (h < 0) {
        h = h + 2 * pi;
    }

    float hi = (glm::floor(h / (1.0 / 3.0 * pi)));//why floor doesnt cast to int is a mystery to me but whatever
//float hi=0;
    float f = (h / (1.0 / 3.0)) - hi;
    float p = v * (1.0 - s);
    float q = v * (1.0 - s * f);
    float t = v * (1.0 - s * (1.0 - f));
    
    if ((hi >= 0 && hi < 1) || (hi >= 6 && hi < 7))result = glm::vec3(v, t, p);
    else if (hi >= 1 && hi < 2)result = glm::vec3(q, v, p);
    else if (hi >= 2 && hi < 3)result = glm::vec3(p, v, t);
    else if (hi >= 3 && hi < 4)result = glm::vec3(p, q, v);
    else if (hi >= 4 && hi < 5)result = glm::vec3(t, p, v);
    else if (hi >= 5 && hi < 6)result = glm::vec3(v, p, q);

    return result;
};