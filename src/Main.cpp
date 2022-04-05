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
#include <Constants.h>


/* ------------------------------------------------------------------------------------ */
// Protottypes
/* ------------------------------------------------------------------------------------ */
void readINI();
GLFWwindow* initGLFWandGLEW();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void setPointLightShaderParameters(Shader& shader, string pointLightNumber, glm::vec3 postion);
void computeTimeLogic();
void drawGameObject(Model* gameObj, glm::vec3 translation, float angle, glm::vec3 rotationAxes, glm::vec3 scale, Shader* shader);
void activateShader(Shader* shader);


/* ------------------------------------------------------------------------------------ */
// Create Objects and make settings
/* ------------------------------------------------------------------------------------ */
// camera & physics
Camera camera(glm::vec3(-10.0f, 18.0f, 50.0f));
Physics* pHandler;
KinematicPlayer* playerController;

// lighting
glm::vec3 directLightPos(30.f, 36.0f, 10.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
double lastHUDPress = glfwGetTime();
double lastShotPress = glfwGetTime();

//HUD
float HUDstart;


int main(void)
{
    /* ------------------------------------------------------------------------------------ */
    // load setting.ini and inititalize openGL & bullet
    /* ------------------------------------------------------------------------------------ */
    readINI();
    HUDstart = SCR_HEIGHT - HUDyOffset;
    GLFWwindow* window = initGLFWandGLEW();
    pHandler = new Physics(debug);
  
    /* ------------------------------------------------------------------------------------ */
    // load shader
    /* ------------------------------------------------------------------------------------ */
    Shader modelShader(modelVertPath, modelFragPath);
    Shader lightShader(lightVertPath, lightFragPath);
    Shader playerShader(snowBallVertPath, snowBallFragPath);

    /* ------------------------------------------------------------------------------------ */
    // load models related physics objects
    /* ------------------------------------------------------------------------------------ */
    Model heart(heartPath);
    Model player(playerPath, true, PNG);
    Model largeBlock(largeBlockPath);

    btRigidBody* groundBody = pHandler->addMeshShape(&largeBlock, glm::vec3(0, 0, 0), 0);                    
    btBvhTriangleMeshShape* groundShape = ( (btBvhTriangleMeshShape*) (groundBody->getCollisionShape()) );  // so 2 lines of code for extracting and scaling the colShape of a rigid body
    pHandler->addScaledMeshShape(groundShape, glm::vec3(0, 0, 0), 0, glm::vec3(4, 1, 4)); 

    playerController = new KinematicPlayer(pHandler, camera.pos, &camera, &player);

    /* ------------------------------------------------------------------------------------ */
    // load HUD
    /* ------------------------------------------------------------------------------------ */
    Shader HUDShader(HUDVertPath, HUDFragPath);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    HUDShader.use();
    glUniformMatrix4fv(glGetUniformLocation(HUDShader.ID, "proj"), 1, GL_FALSE, glm::value_ptr(projection)); 
    HUD hud(fontPath);
    hud.update(&camera, FPS, msPerFrame, pHandler);


    /* ------------------------------------------------------------------------------------ */
    // main & render loop
    /* ------------------------------------------------------------------------------------ */
    while (!glfwWindowShouldClose(window))
    {
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


        // DRAW GAME OBJECTS
        drawGameObject(&largeBlock, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(4.0f, 1.0f, 4.0f), &modelShader);


        // PHYSICS
        pHandler->stepSim(deltaTime);
        pHandler->setDebugMatrices(view, projection);  // set debug draw matrices
        pHandler->debugDraw();                         // call the debug drawer

        playerController->update(pNONE, deltaTime);
        activateShader(&playerShader);
        playerController->drawPlayer(&playerShader);

        
        /* ------------------------------------------------------------------------------------ */
        // DRAW HUD
        // HUD to render last so blending works properly, try it out in the beginning of the render loop ;)
        /* ------------------------------------------------------------------------------------ */
        hud.update(&camera, FPS, msPerFrame, pHandler);  //--> updates all HUD messages
        if (showHUD)
        {   
            hud.renderAll(HUDShader, HUDxOffset, HUDstart);
        }


        // GLFW: renew buffers and check all I/O events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /* ------------------------------------------------------------------------------------ */
    // proper termintaion
    // NOTE: missing delete of objects in kinematicPlayer, that were dynamically allocated
    /* ------------------------------------------------------------------------------------ */
    glfwTerminate();
    pHandler->deleteAll();
    return 0;
}


// read information from the settings.ini file
void readINI()
{

    INIReader iniReader(iniPath);
    SCR_WIDTH = iniReader.GetInteger("window", "width", 800);
    SCR_HEIGHT = iniReader.GetInteger("window", "height", 600);
    fullscreen = iniReader.GetBoolean("window", "fullscreen", false);
    refreshRate = iniReader.GetInteger("window", "refresh_rate", 60);
    brightness = float(iniReader.GetReal("window", "brightness", 1.0f));

    debug = iniReader.GetBoolean("physics", "debug", false);
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
        btRigidBody* sphere = pHandler->addSphere(playerController->getPos() + glm::vec3(0.0, 3.0, 0.0), 1.0, 1.0);

        glm::vec3 look = (camera.front) * 20.0f;
        sphere->setLinearVelocity(btVector3(look.x, look.y, look.z));
        
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


// renders a model, by specifiyng the shader as well as transformation matrices 
void drawGameObject(Model* gameObj, glm::vec3 translation, float angle, glm::vec3 rotationAxes, glm::vec3 scale, Shader *shader)    
{
    shader->use();
    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translation);
    model = glm::rotate(model, glm::radians(angle), rotationAxes);
    model = glm::scale(model, scale);
    shader->setMat4("model", model);

    gameObj->draw(*shader);
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