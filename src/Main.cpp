#pragma once
// open gl related libs
#include <GL/glew.h>
#include <gl/GLU.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//assimp included manually ...

#include <ft2build.h>
#include FT_FREETYPE_H
#include <INIReader.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <BulletViewer.h>

// std libs
#include <iostream>
#include <string>

// self made libs
#include <Shader.h>
#include <Camera.h>
#include <Model.h>
#include <HUD.h>
#include <Physics.h>
#include <KinematicPlayer.h>
#include <Snowball.h>
#include <FileManager.h>
#include <Constants.h>


/* ------------------------------------------------------------------------------------ */
// Protottypes
/* ------------------------------------------------------------------------------------ */
void readINI();
GLFWwindow* initGLFWandGLEW();
bool hasWon();
bool hasLost();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void setPointLightShaderParameters(Shader& shader, string pointLightNumber, glm::vec3 postion);
void computeTimeLogic();
void activateShader(Shader* shader);

/* ------------------------------------------------------------------------------------ */
// Create Objects and make settings
/* ------------------------------------------------------------------------------------ */
// gameplay
double maxGameTime = 300.0;  // time in seconds until player looses

// camera & physics
Camera camera(0.0f, 0.0f, 0.0f, 0.0f, glm::vec3(-30.0f, 58.0f, 30.0f));
Physics* pHandler;
KinematicPlayer* playerController;
map<unsigned int, Snowball*> snowballs;
vector<Snowball*> collectedSnowballs;
vector<Snowball*> savedSnowballs;

// lighting
glm::vec3 directLightPos(30.f, 90.0f, 10.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
double lastHUDPress = glfwGetTime();
double lastShotPress = glfwGetTime();

//HUD
float HUDstart;

// file manager
FileManager* fm;

glm::vec3 worldMiddle = glm::vec3(-30, 15, 25);  //test world model  middle coordinate: (-30, 15, 25)
string worldPath = "C:\\Users\\dittm\\Documents\\Computergraphik-UE\\maya-new-objects\\world.fbx";

int main(void)
{
    /* ------------------------------------------------------------------------------------ */
    // load setting.ini and inititalize openGL & bullet as well as physics handler and file manager
    /* ------------------------------------------------------------------------------------ */
    fm = new FileManager();
    readINI();
    HUDstart = SCR_HEIGHT - HUDyOffset;
    GLFWwindow* window = initGLFWandGLEW();
    pHandler = new Physics(debug);
  
    /* ------------------------------------------------------------------------------------ */
    // load shader
    /* ------------------------------------------------------------------------------------ */
    Shader modelShader(fm->getShaderPath("modelVert"), fm->getShaderPath("modelFrag"));
    Shader directLightShader(fm->getShaderPath("directLightVert"), fm->getShaderPath("directLightFrag"));
    Shader playerShader(fm->getShaderPath("playerVert"), fm->getShaderPath("playerFrag"));

    /* ------------------------------------------------------------------------------------ */
    // load models related physics objects
    /* ------------------------------------------------------------------------------------ */
    // world
    Model world(worldPath, &camera);                                                                    // load world model
    btRigidBody* worldBody = pHandler->addMeshShape(&world, glm::vec3(0, 0, 0), 0);                     // create a physics obj
    btBvhTriangleMeshShape* worldShape = ((btBvhTriangleMeshShape*)(worldBody->getCollisionShape()));   // now create an easily scalable version of that body
    pHandler->addScaledMeshShape(worldShape, glm::vec3(-30.0f, 10.0f, 30.0f), 0, glm::vec3(3.0f));      // add to world
    pHandler->getWorld()->removeRigidBody(worldBody);                                                   // removw the inital physcics obj

    // player
    Model player(fm->getObjPath("player"), &camera, true, PNG);
    playerController = new KinematicPlayer(pHandler, camera.pos, &camera, &player);

    // snowballs
    Snowball snowball2(PALM_TREE, fm->getObjPath("snowball"), glm::vec3(-25.0f, 50.0f, 80.0f), 0.2, 1.0, glm::vec3(0.0, 0.0, -9.81), pHandler, &camera);  // on FRONT side (palm tree)
    snowballs.insert(std::pair<unsigned int, Snowball*>(PALM_TREE, &snowball2));
    Snowball snowball3(LABYRINTH, fm->getObjPath("snowball"), glm::vec3(-22.0f, 30.0f, 25.0f), 0.2, 1.0, glm::vec3(-9.81, 0.0, 0.0), pHandler, &camera);  // on RIGHT side (labyrinth)
    snowballs.insert(std::pair<unsigned int, Snowball*>(LABYRINTH, &snowball3));
    Snowball snowball1(PLATFORM, fm->getObjPath("snowball"), glm::vec3(-47.0f, 52.0f, -18.0f), 0.2, 1.0, glm::vec3(0, 0, 9.81f), pHandler, &camera);      // on BACK side (platform)
    snowballs.insert(std::pair<unsigned int, Snowball*>(PLATFORM, &snowball1));
    Snowball snowball4(LEVER, fm->getObjPath("snowball"), glm::vec3(-40.0f, 58.0f, 20.0f), 0.2, 1.0, glm::vec3(0.0, -9.81, 0.0), pHandler, &camera);      // test
    snowballs.insert(std::pair<unsigned int, Snowball*>(LEVER, &snowball4));

    // collection point place holder
    Model collectionPoint(fm->getObjPath("obelisk"), &camera);
    btRigidBody* colPntBody = pHandler->addCylinder(glm::vec3(-37.5f, 53.5f, 20.0f), 0, glm::vec3(0.5, 2, 0.5));
    pHandler->activateColCallBack(colPntBody);
    colPntBody->setUserPointer(&cpPtr);


    /* ------------------------------------------------------------------------------------ */
    // load HUD
    /* ------------------------------------------------------------------------------------ */
    HUD hud(fm->getFontPath("arial"));
    Shader HUDShader(fm->getShaderPath("HUDvert"), fm->getShaderPath("HUDfrag"));
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    HUDShader.use();
    glUniformMatrix4fv(glGetUniformLocation(HUDShader.ID, "proj"), 1, GL_FALSE, glm::value_ptr(projection)); 
    hud.update(&camera, FPS, msPerFrame, pHandler, playerController);


    /* ------------------------------------------------------------------------------------ */
    // main & render loop
    /* ------------------------------------------------------------------------------------ */
    do {
        // per-frame time logic
        computeTimeLogic();

        // input & player
        processInput(window);

        // render
        glClearColor(0.6f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        activateShader(&modelShader);
        

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);


        /* ------------------------------------------------------------------------------------ */
        // GAME OBJECTS
        /* ------------------------------------------------------------------------------------ */
        world.draw(modelShader, glm::vec3(-30.0f, 10.0f, 30.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(3.0f));
        collectionPoint.draw(modelShader, glm::vec3(-37.5f, 51.5f, 20.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f));
        
        for (const auto& item : snowballs) {  // item.second == Snowball*
            item.second->shrink(deltaTime);
            item.second->s_draw(&modelShader, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
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
            }
        }

        pHandler->stepSim(deltaTime);
        pHandler->setDebugMatrices(view, projection);  // set debug draw matrices
        pHandler->debugDraw();                         // call the debug drawer

        playerController->update(pNONE, deltaTime);
        activateShader(&playerShader);
        playerController->drawPlayer(&playerShader);

        
        /* ------------------------------------------------------------------------------------ */      
        // HUD
        /* ------------------------------------------------------------------------------------ */
        hud.update(&camera, FPS, msPerFrame, pHandler, playerController);  //--> updates all HUD messages
        if (showHUD)
        {   
            hud.renderAll(HUDShader, HUDxOffset, HUDstart);
        }


        // GLFW: renew buffers and check all I/O events
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (!glfwWindowShouldClose(window) && !hasWon() && !hasLost());

    /* ------------------------------------------------------------------------------------ */
    // TERMINATE
    /* ------------------------------------------------------------------------------------ */
    glfwTerminate();
    playerController->~KinematicPlayer();
    pHandler->deleteAll();
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

    // physics
    debug = iniReader.GetBoolean("physics", "debug", false);

    // gameplay
    maxGameTime = iniReader.GetReal("gameplay", "maxGameTime", 300.0);
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


bool hasWon() {
    if (snowballs.size() == 0 && collectedSnowballs.size() == 0)
        return true;
    
    return false;
}


bool hasLost() {
    if (glfwGetTime() > maxGameTime)
        return true;
 
    return false;
}


// process all input that is triggered by the keyboard
void processInput(GLFWwindow* window)
{   
    

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && (glfwGetTime() - lastShotPress) >= 0.2)
    {
        camera.processKeyboard(FORWARD, deltaTime);
        playerController->update(pFORWARD, deltaTime);
    }
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
    {
        camera.processKeyboard(BACKWARD, deltaTime);
        playerController->update(pBACKWARD, deltaTime);
    }
        
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.processKeyboard(LEFT, deltaTime);
        playerController->update(pLEFT, deltaTime);
    }


    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.processKeyboard(RIGHT, deltaTime);
        playerController->update(pRIGHT, deltaTime);
    }
    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.processKeyboard(UP, deltaTime);
        playerController->update(pUP, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (glfwGetTime() - lastShotPress) >= 0.2)
    {
        playerController->shootSnowball();
        lastShotPress = glfwGetTime();
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && (glfwGetTime() - lastHUDPress) >= 0.2)
    {
        showHUD = !showHUD;
        lastHUDPress = glfwGetTime();
    }
        
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
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    msPerFrame = deltaTime * 1000;  // deltaTime is the time in s from frame to frame, i.e. taking to compute 1 frame; 1s = 1000ms
    FPS = 1.0 / deltaTime;
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
    shader->setVec3("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
    shader->setVec3("directionalLight.diffuse", 1.0f*brightness, 1.0f*brightness, 1.0f*brightness);              // change here for scene brightness
    shader->setVec3("directionalLight.specular", 0.5f, 0.5f, 0.5f);

    for (unsigned int i = 0; i < nPointLights; i++)
    {
        setPointLightShaderParameters(*shader, to_string(i), pointLightPositions[i]);
    }
}


// set the pointlights[] uniform values in the specified shader
void setPointLightShaderParameters(Shader& shader, string pointLightNumber, glm::vec3 postion)
{
    shader.setVec3("pointLights[" + pointLightNumber + "].pos", postion);
    shader.setVec3("pointLights[" + pointLightNumber + "].ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("pointLights[" + pointLightNumber + "].diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("pointLights[" + pointLightNumber + "].specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kc", 1.0f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kl", 0.027f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kq", 0.0028f);
}