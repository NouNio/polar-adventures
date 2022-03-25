#ifndef MODEL_H
#define MODEL_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Mesh.h>
#include <Shader.h>
#include <Constants.h>

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
    Model(string const& path, bool withTextures = false, const char* texFileType = "")
    {
        this->withTextures = withTextures;
        this->texFileType = texFileType;
        loadModel(path);
    }


    void draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].draw(shader);
    }


    bool hasTextures()
    {
        return this->withTextures;
    }

private:
    vector<Mesh> meshes;          // all the meshes of the model, usually our models have aroudn 2-3 meshes
    string directory;
    Assimp::Importer importer;
    const aiScene* scene;
    bool withTextures;
    const char* texFileType;
    const string TEX_DIFF = "texture_diffuse";
    // add datastructure like map/hash to not load the same texture multiple times for each model, when all/some of the meshes use this texture


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
        Texture texture;

        // fill it with the data from the assimp meshes
        parseVertices(&vertices, mesh);   // 1. retrieve the vertice data from the assimp mesh
        parseIndices(&indices, mesh);     // 2. retrieve the indice data from the assimp mesh
        parseMaterials(&material, mesh);  // 3. retrieve material data

        if (this->hasTextures())
        {
            parseTextures(&texture);   // 4. retireve the (correctly named) textures
            return Mesh(vertices, indices, material, texture, this->hasTextures());
        }
        else
            return Mesh(vertices, indices, material, this->hasTextures());
    }


    void parseVertices(vector<Vertex>* vertices, aiMesh* mesh)
    {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            
            vertex.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

            //TODO: if considering textures, would need TexCoords parsing here
            
            if (this->hasTextures() && mesh->mTextureCoords[0]) {
                vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else
                vertex.texCoords = glm::vec2(0.0f, 0.0f); 


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


    void parseTextures(Texture* texture)
    {
        texture->ID = textureFromFile();
        texture->type = TEX_DIFF;
    }


    unsigned int textureFromFile()
    {
        string filename = directory + '\\' + TEX_DIFF + texFileType;

        unsigned int textureID;
        glGenTextures(1, &textureID);
        std::cout << "trying to load file: " << filename << std::endl;
        
        int width, height, nComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nComponents, 0);
        if (data)
        {
            glTextureConfig(nComponents,width,height, textureID, data);
        }
        else
        {
            std::cout << "There was an error loading the texture file: " << filename << std::endl;
        }
        
        stbi_image_free(data);

        return textureID;
    }


    void glTextureConfig(int nComponents, int width, int height, unsigned int textureID, unsigned char* data)
    {
        GLenum format = GL_RGB;
        if (nComponents == 1)
            format = GL_RED;
        else if (nComponents == 3)
            format = GL_RGB;
        else if (nComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // can also move these 4 loc below glGenTextures() in textureFromFile()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
};
#endif
