// open gl related libs
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//assimp included manually ...

#include <ft2build.h>
#include FT_FREETYPE_H

#include <bullet/btBulletDynamicsCommon.h>

// std libs
#include <iostream>
#include <string>

// self made libs
#include <Shader.h>
#include <Camera.h>
#include <Model.h>
#include <HUD.h>
#include <Constants.h>


// testing
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


// prototypes
GLFWwindow* initGLFWandGLEW();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, glm::vec3* playerTranslation);
void setPointLightShaderParameters(Shader& shader, string pointLightNumber, glm::vec3 postion);
void computeTimeLogic();
void drawGameObject(Model* gameObj, glm::vec3 translation, float angle, glm::vec3 rotationAxes, glm::vec3 scale, Shader* shader);
void drawPlayer(Model* gameObj, glm::vec3 translation, float angle, glm::vec3 rotationAxes, glm::vec3 scale, Shader* shader);
void activateShader(Shader* shader);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
bool showHUD = false;

// camera
Camera camera(glm::vec3(75.0f, 7.0f, 20.0f));
glm::vec3 playerOffset(0.0, -6.0, -8.0); // the player is a little bit below and in front of the camera
// lighting
glm::vec3 directLightPos(30.f, 36.0f, 10.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
double lastHUDPress = glfwGetTime();

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
double FPS = 0.0;
double msPerFrame = 0.0;


int main(void)
{
    GLFWwindow* window = initGLFWandGLEW();

    //stbi_set_flip_vertically_on_load(true);  // tell stb_image.h to flip loaded texture's on the y-axis (before loading model); NOTE turn off for most the textures I saw don't need it
    glEnable(GL_DEPTH_TEST);
    
    // load Shader, Models and HUD
    Shader lightShader(lightVertPath, lightFragPath);
    Shader modelShader(modelVertPath, modelFragPath);
    Shader snowBallShader(snowBallVertPath, snowBallFragPath);
    Model player(playerPath, true, PNG);
    Model heart(heartPath);
    Model tree(treePath);
    Model lightpost(lightpostPath);
    Model snowman(snowmanPath);
    Model snowPatch(snowPatchPath);
    Model treePine(treePinePath);
    Model treePineSnow(treePineSnowPath);
    Model treePineSnowed(treePineSnowedPath);
    Model treePineSnowRound(treePineSnowRoundPath);
    Model snowBall(snowBallPath, true, PNG);
    Model cubeWorld(worldPath);

    Model isoGroundSnow(isoGroundSnowPath, true, GIF);
    Model matchBox(matchBoxPath, true, PNG);
    Model plinth(plinthPath, true, PNG);
    Model campfire(campfirePath);
    Model reptileMage(reptileMagePath, true, PNG);
    Model bird(birdPath);
    Model chick(chickPath);
    Model fish(fishPath);
    Model fox(foxPath);
    Model whale(whalePath);
    //Model stoneArray(stoneArrayPath, true, JPG);
    //Model stoneArk(stoneArkPath);

    // do HUD stuff 
    Shader HUDShader(HUDVertPath, HUDFragPath);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    HUDShader.use();
    glUniformMatrix4fv(glGetUniformLocation(HUDShader.ID, "proj"), 1, GL_FALSE, glm::value_ptr(projection)); 
    HUD hud(fontPath);


    // do physics stuff with bullet3
    // TODO


    // render loop
    // -----------
    glm::vec3 playerPos = camera.pos + playerOffset;
    float playerRotAngle = 0.0f;
    glm::vec3 playerRotationAxes(0.0f, 1.0f, 0.0f);
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        computeTimeLogic();

        // input
        // -----
        processInput(window, &playerPos);

        // render
        // ------
        glClearColor(0.6f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        activateShader(&modelShader);
        

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);


        // DRAW GAME OBJECTS
        drawGameObject(&heart, glm::vec3(15.0f, 3.0f, 0.0f), 200.0f * float(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), &modelShader);

        drawGameObject(&tree, glm::vec3(15.0f, -0.5f, 2.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), &modelShader);
        drawGameObject(&tree, glm::vec3(50.0f, -0.5f, 3.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f), &modelShader);
        drawGameObject(&tree, glm::vec3(40.0f, -0.5f, 17.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.9f, 1.1f, 0.9f), &modelShader);
        drawGameObject(&tree, glm::vec3(30.0f, -0.5f, 20.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), &modelShader);

        drawGameObject(&treePine, glm::vec3(18.0f, 0.0f, 5.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), &modelShader);
        drawGameObject(&treePineSnow, glm::vec3(15.0f, 0.0f, 22.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.9f, 1.9f, 1.9f), &modelShader);
        drawGameObject(&treePineSnowed, glm::vec3(22.0f, 0.0f, -5.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.9f, 1.9f, 1.9f), &modelShader);
        drawGameObject(&treePineSnowRound, glm::vec3(47.0f, 0.0f, -2.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.1f, 1.6f, 1.1f), &modelShader);

        drawGameObject(&cubeWorld, glm::vec3(18.0f, -56.0f, 8.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(10.0f, 10.0f, 10.0f), &modelShader);

        drawGameObject(&lightpost, glm::vec3(7.0f, -20.0f, 5.0f), 90.0f, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), &modelShader);
        drawGameObject(&snowman, glm::vec3(26.0f, 0.0f, 3.0f), 90.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), &modelShader);
        drawGameObject(&snowPatch, glm::vec3(25.0f, 0.0f, 5.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f), &modelShader);

        
        activateShader(&snowBallShader);
        snowBallShader.setMat4("projection", projection);
        snowBallShader.setMat4("view", view);
        float scale = 0.05f;
        drawGameObject(&plinth, glm::vec3(28.0f, 0.0f, 16.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(scale, scale, scale), &snowBallShader);

        activateShader(&snowBallShader);
        float scale2 = 0.4f;
        playerOffset = camera.front * glm::length(playerOffset);
        playerPos = camera.pos + playerOffset + glm::vec3(0.0f, -5.0f, -1.0f);
        playerRotAngle = -camera.yaw;
        drawPlayer(&player, playerPos, playerRotAngle, playerRotationAxes, glm::vec3(scale2, scale2, scale), &snowBallShader);

        activateShader(&snowBallShader);
        snowBallShader.setMat4("projection", projection);
        snowBallShader.setMat4("view", view);
        drawGameObject(&snowBall, glm::vec3(28.0f, 4.0f, 16.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.01f, 0.01f, 0.01f), &snowBallShader);

        // PHYSICS
        // TO DO
        

        // DRAW HUD
        // HUD to render last so blending works properly, try it out in the beginning of the render loop ;)
        if (showHUD)
        {
            hud.render(HUDShader, "Snowballs 0/4", 10.0f, 560.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "--Data--", 10.0f, 530.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "Camera.pos X: " + to_string(camera.pos.x), 10.0f, 510.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "Camera.pos Y: " + to_string(camera.pos.y), 10.0f, 490.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "Camera.pos Z: " + to_string(camera.pos.z), 10.0f, 470.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "Camera.front X: " + to_string(camera.front.x), 10.0f, 450.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "Camera.front Y: " + to_string(camera.front.y), 10.0f, 430.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "Camera.front Z: " + to_string(camera.front.z), 10.0f, 410.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "Camera.front len: " + to_string(glm::length(camera.front)), 10.0f, 390.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "playerOffset len: " + to_string(glm::length(playerOffset)), 10.0f, 370.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "FPS: " + to_string(FPS), 10.0f, 350.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
            hud.render(HUDShader, "ms/frame: " + to_string(msPerFrame), 10.0f, 330.0f, 0.5f, glm::vec3(0.1f, 0.6f, 0.9f));
        }


        // GLFW: renew buffers and check all I/O events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}


// usual GLFW and GLEW initialization and some config settings
GLFWwindow* initGLFWandGLEW()
{
    // init and config of glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // creat the window, set as context and activate necessary callbacks
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "There was an error creating the GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

   
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);   // make GLFW measure our mouse


    // init GLEW
    if (glewInit() != GLEW_OK) {
        std::cout << "There was an error creating initializing GLEW" << std::endl;
        glfwTerminate();
    }

    return window;
}


// process all input that is triggered by the keyboard
void processInput(GLFWwindow* window, glm::vec3* playerTranslation)
{   
    

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime, playerTranslation);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime, playerTranslation);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime, playerTranslation);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime, playerTranslation);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(UP, deltaTime, playerTranslation);


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
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
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


// player rendering is differtent, as the player obj needs to be rotated around the camera object space y-axes (not around the player obj space y-axes
void drawPlayer(Model* gameObj, glm::vec3 translation, float angle, glm::vec3 rotationAxes, glm::vec3 scale, Shader* shader)
{
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader->setMat4("projection", projection);
    
    glm::mat4 view = camera.GetViewMatrix();
    shader->setMat4("view", view);

    shader->use();
    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translation);
    model = glm::rotate(model, glm::radians(angle+90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
    shader->setVec3("directionalLight.diffuse", 1.0f, 1.0f, 1.0f);              // change here for scene brightness
    shader->setVec3("directionalLight.specular", 0.5f, 0.5f, 0.5f);

    setPointLightShaderParameters(*shader, "0", pointLightPositions[0]);  // point light 1
    setPointLightShaderParameters(*shader, "1", pointLightPositions[1]);  // point light 2
    setPointLightShaderParameters(*shader, "2", pointLightPositions[2]);  // point light 3
    setPointLightShaderParameters(*shader, "3", pointLightPositions[3]);  // point light 4
    setPointLightShaderParameters(*shader, "4", pointLightPositions[4]);  // point light 5
}


// set the pointlights[] uniform values in the specified shader
void setPointLightShaderParameters(Shader& shader, string pointLightNumber, glm::vec3 postion)
{
    shader.setVec3("pointLights[" + pointLightNumber + "].pos", postion);
    shader.setVec3("pointLights[" + pointLightNumber + "].ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("pointLights[" + pointLightNumber + "].diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("pointLights[" + pointLightNumber + "].specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kc", 1.0f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kl", 0.09f);
    shader.setFloat("pointLights[" + pointLightNumber + "].Kq", 0.032f);
}