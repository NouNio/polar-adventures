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

// so far we use .fbx objects from the artist Kenny from OpenGameArt
// these have vertices with positions and normals and 
// each mesh has one simple material with the respective color values (diff, spec, ambi, shini)

struct Vertex {             
    glm::vec3 pos;
    glm::vec3 normal;
};

struct Material {
    glm::vec3 diffuseCol;
    glm::vec3 specularCol;
    glm::vec3 ambientCol;
    float shininess;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex> vertices;        // all the vertices that form the mesh
    vector<unsigned int> indices;   // indices for the mesh data
    Material material;              // material data for the mesh

    const string MAT_DIFF = "material.diffuse";
    const string MAT_SPEC = "material.specular";
    const string MAT_AMBI = "material.ambient";
    const string MAT_SHIN = "material.shininess";


    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material material)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->material = material;
        setupMesh();   // use the mesh data to make OpenGL calls, i.e. set the vertex buffers and its attribute pointers
    }


    void draw(Shader& shader)
    {
        setMaterialValues(shader);  // set the materials color values in the fragment shader

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


    void setMaterialValues(Shader& shader)
    {
        // material properties, the properties of the Light in the fragment shader are set once globally
        shader.setVec3(MAT_DIFF, this->material.diffuseCol);
        shader.setVec3(MAT_SPEC, this->material.specularCol);
        shader.setVec3(MAT_AMBI, this->material.ambientCol);
        shader.setFloat(MAT_SHIN, this->material.shininess);
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


    void enableAndSetPtr() 
    {
        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    }
};
#endif
