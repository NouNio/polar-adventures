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



const string TEX_DIFF = "texture_diffuse";
const string TEX_SPEC = "texture_specular";

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
    // model data 
    vector<Mesh> meshes;
    vector<MeshTexture> textureCache;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    string directory;
    Assimp::Importer importer;
    const aiScene* scene;
    bool inCache = false;


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
        vector<MeshVertex> vertices;
        vector<unsigned int> indices;
        vector<MeshTexture> textures;

        // fill it with the data from the assimp meshes
        parseVertices(&vertices, mesh);   // 1. retrieve the vertice data from the assimp mesh
        parseIndices(&indices, mesh);     // 2. retrieve the indice data from the assimp mesh
        parseMaterials(&textures, mesh);  // 3. retrieve texture data
        return Mesh(vertices, indices, textures);
    }

    void parseVertices(vector<MeshVertex>* vertices, aiMesh* mesh)
    {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            MeshVertex vertex;
            
            vertex.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);  // positions 
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);  // normals

            // texture coordinates - first check if the mesh contains them at all
            // NOTE that atm this only allows for one, i.e. the first texture to be loaded
            if (mesh->mTextureCoords[0])
            {
                vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else
                vertex.texCoords = glm::vec2(0.0f, 0.0f);  // set to zero, if no texture coords provided

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

    void parseMaterials(vector<MeshTexture>* textures, aiMesh* mesh)  //TODO: add other materials here, e.g. normal or height maps
    {
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];  // again here the mesh only contains an idx which points to the specific pos in scene->mMaterials

            // diffuse texture
            vector<MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TEX_DIFF);
            textures->insert(textures->end(), diffuseMaps.begin(), diffuseMaps.end());

            // specular texture
            vector<MeshTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TEX_SPEC);
            textures->insert(textures->end(), specularMaps.begin(), specularMaps.end());
        }
    }


    vector<MeshTexture> loadMaterialTextures(aiMaterial* material, aiTextureType texType, string typeName)
    {
        vector<MeshTexture> textures;
        for (unsigned int i = 0; i < material->GetTextureCount(texType); i++)
        {   
            inCache = false;    // assume this texture is not in the cache yet
            aiString textureFilename;
            material->GetTexture(texType, i, &textureFilename);
            
            checkCache(&textures, textureFilename);

            if (!inCache) // if inCache the texture was already pushed to textures in the checkCache() call
            {
                MeshTexture texture;
                texture.id = textureFromFile(textureFilename.C_Str(), directory);
                texture.type = typeName;
                texture.path = textureFilename.C_Str();
                textures.push_back(texture);
                textureCache.push_back(texture);
            }
        }

        return textures;
    }

    void checkCache(vector<MeshTexture>* textures, aiString textureFilename)
    {
        for (unsigned int j = 0; j < textureCache.size(); j++)  // check for all textures currently in the cache, if one of them equals the new one
        {
            if (std::strcmp(textureCache[j].path.data(), textureFilename.C_Str()) == 0)
            {
                textures->push_back(textureCache[j]);
                inCache = true;
                break;
            }
        }
    }

    // retrieve the id from a texture file and also call the corresponding OpenGL functions
    unsigned int textureFromFile(const char* path, const string& directory)
    {
        string filename = directory + '\\' + string(path);
        unsigned int textureID;
        int width, height, nColorChannels;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nColorChannels, 0);

        if (data)
        {
            setupGLTextures(&textureID, width, height, nColorChannels, data);
        }
        else
            std::cout << "There was an error loading the texture file: " << path << std::endl;

        stbi_image_free(data);

        return textureID;
    }

    void setupGLTextures(unsigned int* textureID, int width, int height, int nColorChannels, unsigned char* data)
    {
        glGenTextures(1, textureID);
        glBindTexture(GL_TEXTURE_2D, *textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                       // wrapping params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                       //        ||
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);     // filtering params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);                   //        ||

        GLenum format = GL_RGBA;
        if (nColorChannels == 1)
            format = GL_RED;
        else if (nColorChannels == 3)
            format = GL_RGB;
        else if (nColorChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
};
#endif
