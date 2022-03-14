#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <Camera.h>
#include <Model.h>

#include <iostream>
// TEST
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(75.0f, 7.0f, 20.0f));
// lighting
glm::vec3 directLightPos(30.f, 36.0f, 10.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/material.h>


int main(void) {
    // BEGIN ASSIMP TESTING
    // ------------------------------
    std::cout << "Testing colored .fbx models in ASSIMP:\n" << endl;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("C:\\Users\\dittm\\source\\repos\\TestAssimp\\assets\\objects\\kenny-platformer\\heart.fbx",
                                aiProcess_Triangulate | aiProcess_FlipUVs);

    // print some general data
    std::cout << "Number of meshes: " << scene->mNumMeshes << endl;             // 2 or 1 
    std::cout << "Number of materials: " << scene->mNumMaterials << endl;      // 2 or 1, the rest of the attributes don't occur
    std::cout << "Number of textures: " << scene->mNumTextures << endl;
    std::cout << "Number of animations: " << scene->mNumAnimations << endl;     
    std::cout << "Number of lights: " << scene->mNumLights << endl;
    std::cout << "Number of materials: " << scene->mNumCameras << "\n" << endl;

    for(unsigned int i=0; i < scene->mNumMeshes; i++) 
    {
        aiMesh* mesh = scene->mMeshes[i];
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];  // get the material associated to this mesh, via global scene array, this is since meshes can have the same material
        aiColor4D diffCol;
        aiColor4D specCol;
        aiColor4D ambiCol;
        float shininess;

        aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffCol);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specCol);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambiCol);
        aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);

        std::cout << "Mesh " << i << " with " << mesh->mNumVertices << " vetices." << endl;
        std::cout << "It's color materials name: " << scene->mMaterials[i]->GetName().C_Str() << endl;
        std::cout << "The diff color values: r=" << diffCol.r << "  g=" << diffCol.g << "  b=" << diffCol.b << "  aplha=" << diffCol.a << endl;
        std::cout << "The spec color values: r=" << specCol.r << "  g=" << specCol.g << "  b=" << specCol.b << "  aplha=" << specCol.a << endl;
        std::cout << "The ambi color values: r=" << ambiCol.r << "  g=" << ambiCol.g << "  b=" << ambiCol.b << "  aplha=" << ambiCol.a << endl;
        std::cout << "The shinines values: " << shininess << endl;
        std::cout << "\n" << endl;
    
    
    
        // loop over all vertices of the mesh
        if (false)
        {
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                std::cout << "Vertex number: " << i << endl;
                std::cout << "x=" << mesh->mVertices[i].x << "  y=" << mesh->mVertices[i].y << "  z=" << mesh->mVertices[i].z << endl;  // positions 
                std::cout << "x=" << mesh->mNormals[i].x << "  y=" << mesh->mNormals[i].y << "  z=" << mesh->mNormals[i].z << endl;       // normals
                std::cout << "\n" << endl;

            }
        }
    }

    


    // ------------------------------
    // END ASSIMP TESTING (program stops here)

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    //// Initialize GLEW
    //glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    //// configure global opengl state
    //// -----------------------------
    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("C:\\Users\\dittm\\source\\repos\\TestAssimp\\assets\\shader\\vertexShader.vert", 
                            "C:\\Users\\dittm\\source\\repos\\TestAssimp\\assets\\shader\\fragmentShader.frag");
    Shader directLightShader("C:\\Users\\dittm\\source\\repos\\TestAssimp\\assets\\shader\\directLight.vert", 
                            "C:\\Users\\dittm\\source\\repos\\TestAssimp\\assets\\shader\\directLight.frag");

    Model object1("C:\\Users\\dittm\\source\\repos\\TestAssimp\\assets\\objects\\kenny-platformer\\heart.fbx");
    Model object2("C:\\Users\\dittm\\source\\repos\\TestAssimp\\assets\\objects\\kenny-platformer\\tree.fbx");
    Model object3("C:\\Users\\dittm\\Documents\\maya\\projects\\default\\scenes\\mini-level.fbx");
    Shader modelShader("C:\\Users\\dittm\\source\\repos\\TestAssimp\\assets\\shader\\vertexShader.vert",
                        "C:\\Users\\dittm\\source\\repos\\TestAssimp\\assets\\shader\\fragmentShader.frag");


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        
        lightingShader.setVec3("light.pos", directLightPos);
        lightingShader.setVec3("viewPos", camera.pos);

        // light properties
        glm::vec3 lightColor;
        lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0));
        lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7));
        lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3));
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
        lightingShader.setVec3("light.ambient", ambientColor);
        lightingShader.setVec3("light.diffuse", diffuseColor);
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
        lightingShader.setFloat("material.shininess", 32.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        // render the cube
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // also draw the lamp object
        directLightShader.use();
        directLightShader.setMat4("projection", projection);
        directLightShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, directLightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        directLightShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // also draw the loaded model1
        modelShader.use();
        modelShader.setVec3("light.pos", directLightPos);
        modelShader.setVec3("viewPos", camera.pos);

        // light properties
        modelShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        modelShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
        modelShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f); ;

        // view/projection transformations
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(24.0f, 3.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(50.0f * float(glfwGetTime())), glm::vec3(0.0f, 1.0f, 0.0f));  // rotate heart over time
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);

        object1.draw(modelShader);


        // also draw the loaded model2 4 times
        modelShader.use();
        // NUMBER 1 !!!!!!!!!!!!!!!!!!!
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(50.0f, 0.0f, -8.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	     // it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);

        object2.draw(modelShader);


        modelShader.use();
        // NUMBER 2 !!!!!!!!!!!!!!!!!!!
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(50.0f, 0.0f, 3.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));	     // it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);

        object2.draw(modelShader);


        modelShader.use();
        // NUMBER 3 !!!!!!!!!!!!!!!!!!!
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(40.0f, 0.0f, 20.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	     // it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);

        object2.draw(modelShader);


        modelShader.use();
        // NUMBER 4 !!!!!!!!!!!!!!!!!!!
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(20.0f, 0.0f, 20.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	     // it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);

        object2.draw(modelShader);


        // try to use an exported model, with multiple disconnected meshes
        modelShader.use();
        // world transformation
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(20.0f, -10.0f, -9.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	     // it's a bit too big for our scene, so scale it down
        modelShader.setMat4("model", model);

        object3.draw(modelShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
}