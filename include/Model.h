#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Mesh.h>
#include <Shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;


// this code is inspired by learnopengl.com as this is my main resource for opengl information
// other resources my have been influential as well, this file howver should describes my way of implementing the theoretical concepts

// NOTE: changed all appearances of '/' to '\\' as this is the only way that seems to work on my machine

// here as well as in the Mesh class, we use Kenney Artwork so far, e.g. we do not have textures (yet)

class Model
{
public:
    Model(string const& path)
    {
        loadModel(path);
    }


    void draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].draw(shader);
    }


private:
    vector<Mesh> meshes;          // all the meshes of the model, usually our models have aroudn 2-3 meshes
    string directory;
    Assimp::Importer importer;
    const aiScene* scene;


    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path)
    {
        scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

        checkLoadErros();

        directory = path.substr(0, path.find_last_of('\\'));

        parseNode(scene->mRootNode);

    }


    void checkLoadErros()
    {
        if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)  // check if scene or rooNode is null or if the data is incomplete
        {
            cout << "There was an error loading the model: " << importer.GetErrorString() << endl;
            return;
        }
    }


    // recursively go through all the nodes and parse the meshes they contain
    void parseNode(aiNode* node)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)  // go through all the meshes of a single node
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];  // obtain the node related mesh; node->mMeshes contains idx that point to actual respective mesh in the mesh array scene->mMeshes
            meshes.push_back(parseMesh(mesh));                // transform the assimp mesh into our won mesh class  
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)  // parse all of the nodes childres, which will then parse the childrens childrens etc... STOPs if a node has no children
        {
            parseNode(node->mChildren[i]);
        }
    }


    Mesh parseMesh(aiMesh* mesh)
    {
        // create the data to call the Mesh constructor
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        Material material{};

        // fill it with the data from the assimp meshes
        parseVertices(&vertices, mesh);   // 1. retrieve the vertice data from the assimp mesh
        parseIndices(&indices, mesh);     // 2. retrieve the indice data from the assimp mesh
        parseMaterials(&material, mesh);  // 3. retrieve material data
        
        return Mesh(vertices, indices, material);
    }

    void parseVertices(vector<Vertex>* vertices, aiMesh* mesh)
    {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            
            vertex.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

            //TODO: if considering textures, would need TexCoords parsing here

            vertices->push_back(vertex);
        }
    }

    void parseIndices(vector<unsigned int>* indices, aiMesh* mesh)
    {
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)       // each mesh consist of NumFaces faces
        {
            aiFace face = mesh->mFaces[i];                       // a face here is a triangle, since the model was loaded with aiProcess_Triangulate
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices->push_back(face.mIndices[j]);
            }
        }
    }

    void parseMaterials(Material* material, aiMesh* mesh)  //TODO: add other materials here, e.g. normal or height maps
    {
        aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];  // get the material associated to this mesh, via global scene array, this is since meshes can have the same material
        aiColor4D diffCol, specCol, ambiCol;
        float shini;

        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffCol);
        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_SPECULAR, &specCol);
        aiGetMaterialColor(assimpMaterial, AI_MATKEY_COLOR_AMBIENT, &ambiCol);
        aiGetMaterialFloat(assimpMaterial, AI_MATKEY_SHININESS, &shini);
        
        material->diffuseCol = glm::vec3(diffCol.r, diffCol.g, diffCol.b);
        material->specularCol = glm::vec3(specCol.r, specCol.g, specCol.b);
        material->ambientCol = glm::vec3(ambiCol.r, ambiCol.g, ambiCol.b);
        material->shininess = shini;
    }
};
#endif
