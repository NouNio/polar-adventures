#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Shader.h>

#include <string>
#include <vector>
using namespace std;

// this code is inspired by learnopengl.com as this is my main resource for opengl information
// other resources my have been influential as well, this file howver should describes my way of implementing the theoretical concepts

// a mesh represents a single drawable shape
// since there can be arbitrary many textures, we use a naming convention to make iteration possible (s. learnopengl.com
// name of texture: texture_typeN so e.g. texture_diffuse1 or texture_specular2

struct MeshVertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct MeshTexture {
    unsigned int id;
    string type;
    string path;
};

const string DIFF = "texture_diffuse";
const string SPEC = "texture_specular";
const string MATERIAL = "material.";     // materli. since in the shaders, we store textures in structs, so we need to set the uniforms using this prefix

class Mesh {
public:
    // mesh Data
    vector<MeshVertex> vertices;    // all the vertices that form the mesh
    vector<unsigned int> indices;   // indices for
    vector<MeshTexture> textures;   // texture data for the mesh


    Mesh(vector<MeshVertex> vertices, vector<unsigned int> indices, vector<MeshTexture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        setupMesh();   // use the mesh data to make OpenGL calls, i.e. set the vertex buffers and its attribute pointers
    }


    void draw(Shader& shader)
    {
        extractAndActivateTexs(shader);

        drawMesh();
    }

private: 
    unsigned int VAO, VBO, EBO;  // vertex array / buffer object | element buffer object

    void drawMesh() 
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }


    void extractAndActivateTexs(Shader& shader)
    {
        unsigned int nDiffTextures = 1;
        unsigned int nSpecTextures = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);  // activate proper, i.e. i-th, texture unit before binding
            string N;
            string name = textures[i].type;
            if (name == DIFF)
                N = std::to_string(nDiffTextures++);  // first retrieve N, then increment
            else if (name == SPEC)
                N = std::to_string(nSpecTextures++);

            shader.setFloat((MATERIAL + name + N).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);
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
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(MeshVertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                     &indices[0], GL_STATIC_DRAW);
    }


    void enableAndSetPtr() 
    {
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, texCoords));
    }
};
#endif
