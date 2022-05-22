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
#include <animation.h>
#include <animator.h>
#include <Camera.h>
#include <Constants.h>
#include <FileManager.h>
#include <Framebuffer.h>
#include <HUD.h>
#include <KinematicPlayer.h>
#include <Model.h>
#include <Physics.h>
#include <Shader.h>
#include <Skybox.h>
#include <Snowball.h>


/* ------------------------------------------------------------------------------------ */
// Protottypes
/* ------------------------------------------------------------------------------------ */
void readINI();
GLFWwindow* initGLFWandGLEW();
//bool hasWon();
static void initialize(int window_width, int window_height, unsigned int& color, unsigned int& normal, unsigned int& depth, unsigned int& edge, unsigned int& handle, unsigned int& postprocessor, unsigned int& rbo, GLenum attachments[]);
static void doImageProcessing(unsigned int& color, unsigned int& normal, unsigned int& depth, unsigned int& edge, unsigned int& handle, unsigned int& postprocessor, Shader& processor, Shader& combination);
void clearAll(unsigned int& fbo, unsigned int& postprocessor);
bool hasLost();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void setPointLightShaderParameters(Shader& shader, std::string pointLightNumber, glm::vec3 postion);
void computeTimeLogic();
void activateShader(Shader* shader);
void delay(double seconds);
void playWalkSound();
void playEndOfGameSound();
void transitionToEndOfGameScreen(GLFWwindow* window);
void addGHandlers();
void renderQuad();





/* ------------------------------------------------------------------------------------ */
// Create Objects and make settings
/* ------------------------------------------------------------------------------------ */
// gameplay 
bool sound = false;
double startTimeSec = 0.0;
float animTimeSec = 0.0;
double maxGameTime = 300.0;  // time in seconds until player looses

// sound
irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();


// camera & physics
Camera camera(0.0f, 0.0f, 0.0f, 0.0f, glm::vec3(-30.0f, 58.0f, 30.0f));
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
double lastHUDPress, lastShotPress, lastVFCPress, lastWalkSound, lastESCPress, lastCPress = glfwGetTime();
Skybox* skybox;
float fov, near, far;


float HUDstart;
int currRenderedObjects = 0;

FileManager* fm;

// debug
float xx = 0.0f;
float yy = 0.0f;
float zz = 0.0f;
float step_size = 0.1;


int main(void)
{
    
    stbi_set_flip_vertically_on_load(false);
    /* ------------------------------------------------------------------------------------ */
    // load setting.ini and inititalize openGL & bullet as well as physics handler and file manager
    /* ------------------------------------------------------------------------------------ */
    fm = new FileManager();
    readINI();
    camera = Camera( fov,  float(SCR_WIDTH)/float(SCR_HEIGHT), near, far, glm::vec3(-30.0f, 58.0f, 30.0f));
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

    /* ------------------------------------------------------------------------------------ */
    // load models related physics objects
    /* ------------------------------------------------------------------------------------ */
    // world
    Model newWorld(fm->getWorldPath("game-world"));
    btRigidBody* newWorldBody = pHandler->addMeshShape(&newWorld, glm::vec3(0), 0);                     // create a physics obj
    btBvhTriangleMeshShape* newWorldShape = ((btBvhTriangleMeshShape*)(newWorldBody->getCollisionShape()));   // now create an easily scalable version of that body
    pHandler->addScaledMeshShape(newWorldShape, WORLD_POS, WORLD_MASS, WORLD_SCALE);      // add to world
    pHandler->getWorld()->removeRigidBody(newWorldBody);

    // permeable wall
    Model permWall(fm->getObjPath("perm-wall"));


    /* ------------------------------------------------------------------------------------ */
    // ANIMATED PLAYER MODEL - still in testing phase
    /* ------------------------------------------------------------------------------------ */
    Model animPlayer(fm->getPlayerPath("player"), true, true, PNG);
    Animation walkAnim(fm->getPlayerPath("player"), &animPlayer);
    Animator animator(&walkAnim);
    playerController = new KinematicPlayer(pHandler, camera.pos, &camera, &animPlayer);


    // snowballs
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


    // collection point place holder
    Model collectionPoint(fm->getObjPath("obelisk"));
    btRigidBody* colPntBody = pHandler->addCylinder(COLLECTION_POINT_POS, COLLECTION_POINT_MASS, COLLECTION_POINT_BODY_SCALE);
    pHandler->activateColCallBack(colPntBody);
    colPntBody->setUserPointer(&cpPtr);

    /* ------------------------------------------------------------------------------------ */
    // GRAVITIY CHANGING COLLIDERS
    /* ------------------------------------------------------------------------------------ */
    addGHandlers();


    /* ------------------------------------------------------------------------------------ */
    // HUD
    /* ------------------------------------------------------------------------------------ */
    HUD hud(fm->getFontPath("arial"));
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    HUDShader.use();
    glUniformMatrix4fv(glGetUniformLocation(HUDShader.ID, "proj"), 1, GL_FALSE, glm::value_ptr(projection)); 
    hud.update(&camera, FPS, msPerFrame, pHandler, playerController);

    
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
    unsigned int handle;

    unsigned int color;
    unsigned int normal;
    unsigned int depth;
    unsigned int postprocessor;
    unsigned int edge;
    unsigned int rbo;
  
  
    Shader combination(fm->getShaderPath("passOn.vert", true), fm->getShaderPath("screen.frag", true));
    combination.use();
    combination.setFloat("brightness", brightness);
    Shader processor(fm->getShaderPath("passOn.vert", true), fm->getShaderPath("sobel.frag", true));
    GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
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
        
        if (maxGameTime - glfwGetTime() < 30.0 && !playedAlarm  && sound) {
            soundEngine->play2D(fm->getAudioPath("alarm").c_str(), false);
            playedAlarm = true;
        }

        /* ------------------------------------------------------------------------------------ */
        // INPUT 
        /* ------------------------------------------------------------------------------------ */
        processInput(window);
        animator.UpdateAnimation(deltaTime);

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
        permWall.draw(modelShader, glm::vec3(-49, 18.265, 33.35), 0, glm::vec3(1, 0, 0), glm::vec3(0.9f, 0.01f, 0.53f));
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

            auto transforms = animator.GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i) {
                animModelShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
            }

            // render the loaded model
            playerController->draw(&animModelShader, 1.0);
        }
        else {
            idleShader.use();
            playerController->draw(&idleShader, 0.01);
        }


        /* ------------------------------------------------------------------------------------ */
        // SKYBOX
        /* ------------------------------------------------------------------------------------ */
        skybox->draw(&skyboxShader);
        doImageProcessing(color, normal, depth, edge, handle,postprocessor,  processor, combination);

        currRenderedObjects = camera.frustum->getRenderedObjects();
        /* ------------------------------------------------------------------------------------ */      
        // HUD
        /* ------------------------------------------------------------------------------------ */
        hud.update(&camera, FPS, msPerFrame, pHandler, playerController);  //--> updates all HUD messages
        if (showHUD)
        {   
            hud.renderAll(HUDShader, HUDxOffset, HUDstart);
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

        hud.renderEndOfGame(HUDShader, SCR_WIDTH/2.0f-100.0f, SCR_HEIGHT/2.0f);

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
    glfwSetScrollCallback(window, scrollCallback);

   
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
        
    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        camera.processKeyboard(UP, deltaTime);
        playerController->update(pUP, deltaTime);
    }
        
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.processKeyboard(LEFT, deltaTime);
        playerController->update(pLEFT, deltaTime);
    }


    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.processKeyboard(RIGHT, deltaTime);
        playerController->update(pRIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && (glfwGetTime() - lastHUDPress) >= BUTTON_PAUSE){
        showHUD = !showHUD;
        lastHUDPress = glfwGetTime();
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (glfwGetTime() - lastShotPress) >= BUTTON_PAUSE){
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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        playerController->setWalking(true);
        camera.processKeyboard(FORWARD, deltaTime);
        playerController->update(pFORWARD, deltaTime);
        playWalkSound();
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE) {
        playerController->setWalking(false);
    }


    // increase / decrease x translation of model
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        xx += step_size;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        xx -= step_size;
    // increase / decrease y translation of model
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        yy += step_size;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        yy -= step_size;
    // increase / decrease z translation of model
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        zz += step_size;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        zz -= step_size;
}


// necessary for resizing the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
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


// except scrolling, which is handled here
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
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
void activateShader(Shader *shader)
{
    // be sure to activate shader when setting uniforms/drawing objects
    shader->use();
    shader->setVec3("viewPos", camera.pos);

    /*
       Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
       the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
       by defining light types as classes and set their values in there, or by using a more efficient uniform approach
       by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
    */
    // directional light
    shader->setVec3("directionalLight.direction", -20.2f, -21.0f, -20.3f);
    shader->setVec3("directionalLight.color", 0.5f, 0.5f, 0.5f);              // change here for scene brightness


    for (unsigned int i = 0; i < nPointLights; i++)
    {
        setPointLightShaderParameters(*shader, std::to_string(i), pointLightPositions[i]);
    }
}


// set the pointlights[] uniform values in the specified shader
void setPointLightShaderParameters(Shader& shader, std::string pointLightNumber, glm::vec3 postion)
{
    shader.setVec3("pointLights[" + pointLightNumber + "].pos", postion);
    shader.setVec3("pointLights[" + pointLightNumber + "].color", 0.75f, 0.75f, 0.75f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kc", 1.0f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kl", 0.007f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kq", 0.0002f);
}


void addGHandlers() {
    float vertAxesScale = 19.f;

    // LEFT
    btRigidBody* leftGHandler = pHandler->addBox(LEFT_CUBE_MIDDLE, 0.0f, glm::vec3(10.0, vertAxesScale, vertAxesScale));
    pHandler->activateColCallBack(leftGHandler);
    pHandler->makePermeable(leftGHandler);
    leftGHandler->setUserPointer(&leftGHandlerPtr);

    // RIGHT
    btRigidBody* rightGHandler = pHandler->addBox(RIGHT_CUBE_MIDDLE, 0.0f, glm::vec3(10.0, vertAxesScale, vertAxesScale));
    pHandler->activateColCallBack(rightGHandler);
    pHandler->makePermeable(rightGHandler);
    rightGHandler->setUserPointer(&rightGHandlerPtr);

    // FRONT
    btRigidBody* frontGHandler = pHandler->addBox(FRONT_CUBE_MIDDLE, 0.0f, glm::vec3(vertAxesScale, vertAxesScale, 10.0));
    pHandler->activateColCallBack(frontGHandler);
    pHandler->makePermeable(frontGHandler);
    frontGHandler->setUserPointer(&frontGHandlerPtr);

    // BACK
    btRigidBody* backGHandler = pHandler->addBox(BACK_CUBE_MIDDLE, 0.0f, glm::vec3(vertAxesScale, vertAxesScale, 10.0));
    pHandler->activateColCallBack(backGHandler);
    pHandler->makePermeable(backGHandler);
    backGHandler->setUserPointer(&backGHandlerPtr);

    // TOP
    btRigidBody* topGHandler = pHandler->addBox(TOP_CUBE_MIDDLE, 0.0f, glm::vec3(vertAxesScale, 10.0, vertAxesScale));
    pHandler->activateColCallBack(topGHandler);
    pHandler->makePermeable(topGHandler);
    topGHandler->setUserPointer(&topGHandlerPtr);

    // BOTTOM
    btRigidBody* bottomGHandler = pHandler->addBox(BOTTOM_CUBE_MIDDLE, 0.0f, glm::vec3(vertAxesScale, 10.0, vertAxesScale));
    pHandler->activateColCallBack(bottomGHandler);
    pHandler->makePermeable(bottomGHandler);
    bottomGHandler->setUserPointer(&bottomGHandlerPtr);
}









static void initialize(int window_width, int window_height, unsigned int& color, unsigned int& normal, unsigned int& depth, unsigned int& edge, unsigned int& handle, unsigned int& postprocessor, unsigned int& rbo, GLenum attachments[]) //fm(&fm),
{
    //this->fm = &fm;

   
    
    glGenTextures(1, &depth);
  
  
    
    

    glGenFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    //setup color texture
    glGenTextures(1, &color);
    {
        glBindTexture(GL_TEXTURE_2D, color);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
    }
    //setup normal texture
    glGenTextures(1, &normal);
    {
        glBindTexture(GL_TEXTURE_2D, normal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_SNORM, window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the edge filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);
    }
    //setup depth texture
    //*
    {
        glBindTexture(GL_TEXTURE_2D, depth);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8_SNORM,
            window_width, window_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the edge filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, depth, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

    }//*/
    //glBindTexture(GL_TEXTURE_2D, 0);
    //GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    //glDrawBuffers(2, attachments);
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);


    //finish fbo initialization
    

       
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "fbo init failed for fbo1" << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        }
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDrawBuffers(2, attachments);

    


    glGenFramebuffers(1, &postprocessor);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocessor);
    glGenTextures(1, &edge);
    //init edge texture
    

        glBindTexture(GL_TEXTURE_2D, edge);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, edge, 0);
        GLenum attachment[] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, attachment);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "fbo init failed for fbo2" << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::endl;
        }




        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//*/

static void doImageProcessing(unsigned int& color, unsigned int& normal, unsigned int& depth, unsigned int& edge, unsigned int& handle, unsigned int& postprocessor, Shader& processor, Shader& combination) 
{
    //*
    glBindFramebuffer(GL_FRAMEBUFFER, postprocessor);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    processor.use();
    glUniform1i(glGetUniformLocation(processor.ID, "diffuseTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normal);
    // set the appropriate texture sampler variable in the fragment shader
    glUniform1i(glGetUniformLocation(processor.ID, "depthText"), 1);
    glActiveTexture(GL_TEXTURE1);
    // set the appropriate texture sampler variable in the fragment shader
    glBindTexture(GL_TEXTURE_2D,depth);
    renderQuad();
    //*/
    //*
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    combination.use();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);


    glUniform1i(glGetUniformLocation(combination.ID, "diffuseTexture"), 0);
    glActiveTexture(GL_TEXTURE0);
  // set the appropriate texture sampler variable in the fragment shader
    glBindTexture(GL_TEXTURE_2D, color);
    glUniform1i(glGetUniformLocation(combination.ID, "edge"), 1);
    glActiveTexture(GL_TEXTURE1);
   // set the appropriate texture sampler variable in the fragment shader

    glBindTexture(GL_TEXTURE_2D, edge);
    renderQuad();
    glActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE1);
    //glUseProgram(0);
    //bindBuffer();
    //glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    //bindPostProcessor();
    //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //unbind();
    //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //*/

}
void clearAll(unsigned int& fbo, unsigned int& postprocessor)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, postprocessor);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}
static void deleteBuffers(unsigned int& color, unsigned int& normal, unsigned int& depth, unsigned int& edge, unsigned int& handle, unsigned int& postprocessor, unsigned int& rbo) 
{
    glDeleteTextures(1, &color);
    glDeleteTextures(1, &normal);
    glDeleteTextures(1, &depth);
    glDeleteTextures(1, &edge);
    glDeleteFramebuffers(1, &handle);
    glDeleteFramebuffers(1, &postprocessor);
    glDeleteRenderbuffers(1, &rbo);

}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

