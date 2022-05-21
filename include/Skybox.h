#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>

#include <iostream>
#include <vector>
#include <string>

#include <Shader.h>
#include <FileManager.h>
#include <Constants.h>


extern FileManager* fm;
extern Camera camera;


/* ------------------------------------------------------------------------------------ */
// for the skybox implementation these turorials / resources have been evaluated 
// 1 https://learnopengl.com/Advanced-OpenGL/Cubemaps
// 2 https://www.ogldev.org/www/tutorial25/tutorial25.html
// 3 https://www.mbsoftworks.sk/tutorials/opengl3/10-skybox/
/* ------------------------------------------------------------------------------------ */


float skyboxVertices[] = {     
    // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
};


class Skybox {  // another name for a skybox --> cube map texture
public:
    unsigned int skyboxTex;
	Skybox(std::string name, std::string filetype) {
        this->name = name;
        this->filetype = filetype;
        glVAOandVBOSettings();
        this->skyboxTex = loadCubeMap();
	}


	void draw(Shader* shader) {
        glDepthFunc(GL_LEQUAL);  // depth test with eual values pass
        
        shader->use();
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));  // remove translation from the view matrix
        shader->setMat4("view", view);
        shader->setMat4("proj", glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f));

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
	}


private:
    unsigned int VAO, VBO;
   
    std::string name;
    std::string filetype;
	std::vector<std::string> fileNames = { "left", "right", "front", "back", "top", "bottom" };
    std::vector<GLenum>skyboxTypes = { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
                                  GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y };

	unsigned int loadCubeMap() {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nComponents;
        for (unsigned int i = 0; i < fileNames.size(); i++) {
            std::string filename = fm->getSkyBoxPath(name) + fileNames[i] + filetype;
            
            std::cout << "trying to load file: " << filename << std::endl;
            
            unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nComponents, 0);
            
            if (data){
                glTexImage2D(skyboxTypes[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }
            else {
                std::cout << "There was an error loading the cube map texture file: " << filename << std::endl;
            }

            stbi_image_free(data);

        }

        glTextureConfig();
        
        return textureID;
	}


    void glTextureConfig()
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }


    void glVAOandVBOSettings() {
        // gen arrays and buffer
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        // bind and set data
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        
        // enable and set ptr
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

};

#endif // !SKYBOX_H
