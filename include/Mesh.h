#pragma once
#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Shader.h>
#include <Model.h>

#include <string>
#include <vector>
#include <Boundary.h>

// this code is inspired by learnopengl.com as this is my main resource for opengl information
// other resources my have been influential as well, this file howver should describes my way of implementing the theoretical concepts

// a mesh represents a single drawable shape
// since there can be arbitrary many textures, we use a naming convention to make iteration possible (s. learnopengl.com
// name of texture: texture_typeN so e.g. texture_diffuse1 or texture_specular2

// so far we use .fbx objects from the artist Kenny from OpenGameArt
// these have vertices with positions and normals and 
// each mesh has one simple material with the respective color values (diff, spec, ambi, shini)


#define MAX_BONES_PER_VERTEX 4
#define BONE_ID_LOCATION 3
#define BONE_WEIGHT_LOCATION 4


struct Vertex {             
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoords;
    int boneIDs[MAX_BONES_PER_VERTEX];  // idx of the bones that influence this vertex
    float boneWeights[MAX_BONES_PER_VERTEX]; // weight of these bones
    Vertex() {
    }
    Vertex(glm::vec3 position, glm::vec2 uv) {
        pos = (position);
        texCoords = uv;
    }
};

struct Material {
    glm::vec3 diffuseCol;
    glm::vec3 specularCol;
    glm::vec3 ambientCol;
    float shininess;
};

struct Texture {
    unsigned int ID;
    std::string type;
};

class Mesh {
public:
    // mesh Data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;              
    Texture texture;   
    Boundary bound;
    bool withTexture;

    const std::string MAT_DIFF = "material.diffuse";
    const std::string MAT_SPEC = "material.specular";
    const std::string MAT_AMBI = "material.ambient";
    const std::string MAT_SHIN = "material.shininess";


    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material, 
         std::vector<float> xBound, std::vector<float> yBound,  std::vector<float> zBound, bool withTexture=false, bool animated = false)
        :bound(xBound, yBound, zBound) {
        this->vertices = vertices;
        this->indices = indices;
        this->material = material;
        this->withTexture = withTexture;
        this->animated = animated;
        setupMesh();   // use the mesh data to make OpenGL calls, i.e. set the vertex buffers and its attribute pointers
    }


    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material, Texture texture,
         std::vector<float> xBound, std::vector<float> yBound, std::vector<float> zBound, bool withTexture, bool animated)
    :bound(xBound,yBound,zBound) {
        this->vertices = vertices;
        this->indices = indices;
        this->material = material;
        this->texture = texture;
        this->withTexture = withTexture;
        this->animated = animated;
        setupMesh();   // use the mesh data to make OpenGL calls, i.e. set the vertex buffers and its attribute pointers
    }


    void draw(Shader& shader) 
    {
        //glDisable(GL_CULL_FACE);
        setMaterialValues(shader);  // set the materials color values in the fragment shader
        
        if (this->withTexture)
            setTextureValues(shader);   // set the texture coord values in the correspoiding shader
        
        drawMesh();
    }


    size_t getNumVertices() const { return this->vertices.size(); }
    
    
    void transformBound(glm::mat4 transformation)
    {
        bound.transform(transformation);
    }
    
    
    void resetBound()
    {
        bound.reset();
    }


private:
    unsigned int VAO, VBO, EBO;  // vertex array / buffer object | element buffer object
    bool animated;

    void drawMesh() 
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (this->withTexture)
            glActiveTexture(GL_TEXTURE0);  // set back to default after configuration is done
    }


    void setMaterialValues(Shader& shader)
    {
        // material properties, the properties of the Light in the fragment shader are set once globally
        shader.setVec3(MAT_DIFF, this->material.diffuseCol);
        shader.setVec3(MAT_SPEC, this->material.specularCol);
        shader.setVec3(MAT_AMBI, this->material.ambientCol);
        shader.setFloat(MAT_SHIN, this->material.shininess);
    }


    void setTextureValues(Shader& shader) {   
       // if we were to process multiple textures, we would need to loop through the textures vector and set the appropriate values in the shader
       glActiveTexture(GL_TEXTURE0);

       glUniform1i(glGetUniformLocation(shader.ID, (texture.type).c_str()), 0);  // set the appropriate texture sampler variable in the fragment shader
       glBindTexture(GL_TEXTURE_2D, texture.ID);
    }


    // init the necessary buffer objects/arrays via OpenGL calls
    void setupMesh()
    {
        genArraysAndBuffers();

        bindAndSetData();

        enableAndSetPtr();

        glBindVertexArray(0);  // unbind
    }


    void genArraysAndBuffers() 
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }


    void bindAndSetData()
    {
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }


    void enableAndSetPtr() {
        // vertex positions
        glEnableVertexAttribArray(0);
                             //(location in shader, size of vertex attrib, data type, normalize flag, space between consecutive vertices, offset in buffer)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        // vertex texture coordinates
        if (this->withTexture){
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        
            if (this->animated) {
                // bone ids
                glEnableVertexAttribArray(3);
                glVertexAttribIPointer(3, MAX_BONES_PER_VERTEX, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));

                // bone weights
                glEnableVertexAttribArray(4);
                glVertexAttribPointer(4, MAX_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeights));
            }
        }
    }
};
#endif
