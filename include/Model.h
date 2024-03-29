#pragma once
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
#include <Camera.h>
#include <Constants.h>
#include <Utils.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <unordered_map>


// this code is inspired by learnopengl.com as this is my main resource for opengl information
// other resources my have been influential as well, this file howver should describes my way of implementing the theoretical concepts

// here as well as in the Mesh class, we use Kenney Artwork so far, e.g. we do not have textures (yet)

extern Camera camera;


#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs)
#define MAX_NUM_BONES_PER_VERTEX 4


struct BoneData {
    int id;				// idx in finalBoneMatrices
    glm::mat4 offset;	// this transfrom vertex from model to bone space

    BoneData() {};

    BoneData(int initID, glm::mat4 initOffset) {
        id = initID;
        offset = initOffset;
    }
};


class Model
{
public:
    std::vector<Mesh> meshes;          // all the meshes of the model, usually our models have aroudn 2-3 meshes
    glm::mat4 model = glm::mat4();


    static unsigned int textureFromFile(std::string name)
    {
        std::string filename = name;

        unsigned int textureID;
        glGenTextures(1, &textureID);
        //std::cout << "trying to load file: " << filename << std::endl;

        int width, height, nComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nComponents, 0);
        if (data)
        {
            glTextureConfig(nComponents, width, height, textureID, data);
        }
        else
        {
            std::cout << "There was an error loading the texture file: " << filename << std::endl;
        }

        stbi_image_free(data);

        return textureID;
    }


    Model(std::string const& path, bool withTextures = false, bool animated = false, const char* texFileType = "")
    {
        this->withTextures = withTextures;
        this->texFileType = texFileType;
        this->animated = animated;
        loadModel(path);
    }


    void draw(Shader& shader, glm::vec3 translation, float angle, glm::vec3 rotationAxes, glm::vec3 scale)
    {   
        shader.setMat4("projection", glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f));
        shader.setMat4("view", camera.GetViewMatrix());

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, translation);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), rotationAxes);
        modelMatrix = glm::scale(modelMatrix, scale);

        shader.setMat4("model", modelMatrix);

        bool viewFrustumCulling = camera.getVFCEnabled();
        for (unsigned int i = 0; i < meshes.size(); i++){
           // meshes[i].transformBound(modelMatrix);
            if (viewFrustumCulling) {
                if (isInFrustum(meshes[i], modelMatrix)) {
                    meshes[i].draw(shader);
                    //camera.frustum->increaseRenderedObjects();
                }
            }
            else {
                meshes[i].draw(shader);
                camera.frustum->increaseRenderedObjects();
            }
       
           
        }
    }

    //only used for playe3r so yeah im ignoring that
    void draw(Shader& shader, glm::mat4 model) {

        bool viewFrustumCulling = camera.getVFCEnabled();
        for (unsigned int i = 0; i < meshes.size(); i++) {
            if (viewFrustumCulling) {
               
           
                
            }
            camera.frustum->increaseRenderedObjects();
            meshes[i].draw(shader);
           // meshes[i].resetBound();

        }
        
    }


    void drawUnculled(Shader& shader) {
        for (unsigned int i = 0; i < meshes.size(); i++) {
            meshes[i].draw(shader);
            shader.setMat4("projection", glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f));
            shader.setMat4("view", camera.GetViewMatrix());
            
        }
    }


    bool hasTextures() const{
        return this->withTextures;
    }


    bool isInFrustum (Mesh m, glm::mat4 transform) const {
        return 
            //true;
            camera.frustum->isInside(m.bound, //camera.GetProjection() * 
                camera.GetViewMatrix()*transform);
    }


    size_t getNumMeshes() const
    {
        return this->meshes.size();
    }


    int getNumVertices() const {
        int nVertices = 0;
        for (Mesh mesh : meshes) {
            nVertices += mesh.getNumVertices();
        }
        return nVertices;
    }


    void printVerticesData(int meshIdx) {
        Mesh mesh = this->meshes[meshIdx];
        int countVerticesWithGreaterFourAffectingBones = 0;
        for (int i = 0; i < mesh.vertices.size(); i++) {
            Vertex vertex = mesh.vertices[i];

            for (int j = 0; j < 4; j++) {
                printf("Bone %d has weight %.2f\n", vertex.boneIDs[j], vertex.boneWeights[j]);
            }

            printf("\n\n\n");
        }

        printf("\n\n");
    }


    auto& getBoneInfo() { 
        return boneInfo; 
    }
    
    
    int& getNumBones() { 
        return nBones; 
    }


protected:
    // these are protected since there once was the idea to let an animated model class inherit from this class, so they could be made private again as well
    Assimp::Importer importer;
    const aiScene* scene = NULL;
    std::map<std::string, BoneData> boneInfo;
    int nBones = 0;


private:
    std::string directory, fileName;
    bool withTextures, animated; 
    const char* texFileType;
    const std::string TEX_DIFF = "texture_diffuse";

    //boundaries initalized with max / min values to make sure max/min-search runs properly
    std::vector<float>xBound = { std::numeric_limits<float>::max(),std::numeric_limits<float>::min() };
    std::vector<float>yBound = { std::numeric_limits<float>::max(),std::numeric_limits<float>::min() };
    std::vector<float>zBound = { std::numeric_limits<float>::max(),std::numeric_limits<float>::min() };

    // add datastructure like map/hash to not load the same texture multiple times for each model, when all/some of the meshes use this texture

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const& path){
        scene = importer.ReadFile(path, ASSIMP_LOAD_FLAGS);

        directory = path.substr(0, path.find_last_of('\\'));
        fileName = path.substr(path.find_last_of('\\') + 1);

        checkLoadErros();

        printf("**************************************************\n");
        
        parseMeshes();
        
        printf("**************************************************\n\n\n");
    }


    void checkLoadErros(){
        if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)  // check if scene or rooNode is null or if the data is incomplete
        {
            printf("There was an error loading the model '%s': '%s' \n", fileName.c_str(), importer.GetErrorString());
            return;
        }
    }


    void parseMeshes() {
        printf("*****************************************\n");
        printf("Parsing %d meshes for '%s'\n\n", scene->mNumMeshes, fileName.c_str());

        for (unsigned int mesh_idx = 0; mesh_idx < scene->mNumMeshes; mesh_idx++) {
            aiMesh* mesh = scene->mMeshes[mesh_idx];

            meshes.push_back(parseMesh(mesh));

            printf("\n");
        }
    }


    Mesh parseMesh(aiMesh* mesh) {// create the data to call the Mesh constructor
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Material material{};
        Texture texture;

        // fill it with the data from the assimp meshes
        parseVertices(&vertices, mesh);             // 1. retrieve the vertice data from the assimp mesh

        parseIndices(&indices, mesh);               // 2. retrieve the indice data from the assimp mesh
        parseMaterials(&material, mesh);            // 3. retrieve material data

        if (this->animated) {
            parseBones(vertices, mesh, scene);
        }

        if (this->hasTextures()){
            parseTextures(&texture);                // 4. retrieve the (correctly named) textures
            return Mesh(vertices, indices, material, texture, xBound, yBound, zBound, this->hasTextures(), this->animated);
        }
        else
            return Mesh(vertices, indices, material, xBound, yBound, zBound, this->hasTextures(), this->animated);
    }


    void parseVertices(std::vector<Vertex>* vertices, aiMesh* mesh)
    {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.setAllBoneDataToDefault();
            vertex.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            
            checkVertexBounds(vertex);
            
            if (this->hasTextures() && mesh->mTextureCoords[0]) {
                vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }
            else
                vertex.texCoords = glm::vec2(0.0f, 0.0f);

            vertices->push_back(vertex);
        }
    }


    void parseIndices(std::vector<unsigned int>* indices, aiMesh* mesh)
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
        std::string filename = directory + '\\' + TEX_DIFF + texFileType;

        unsigned int textureID;
        glGenTextures(1, &textureID);
        //std::cout << "trying to load file: " << filename << std::endl;
        
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


    static void glTextureConfig(int nComponents, int width, int height, unsigned int textureID, unsigned char* data)
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


    void checkVertexBounds(Vertex vertex) {
        if (vertex.pos.x > xBound[1]) {
            xBound[1] = vertex.pos.x;
        }
        if (vertex.pos.y > yBound[1]) {
            yBound[1] = vertex.pos.y;
        }
        if (vertex.pos.z > zBound[1]) {
            zBound[1] = vertex.pos.z;
        }
        if (vertex.pos.x < xBound[0]) {
            xBound[0] = vertex.pos.x;
        }
        if (vertex.pos.y < yBound[0]) {
            yBound[0] = vertex.pos.y;
        }
        if (vertex.pos.z < zBound[0]) {
            zBound[0] = vertex.pos.z;
        }
    }


    void parseBones(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
        for (int boneIdx = 0; boneIdx < mesh->mNumBones; ++boneIdx) {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIdx]->mName.C_Str();
            
            if (boneInfo.find(boneName) == boneInfo.end()) {
                BoneData newBoneData(nBones, ConvertMatrixToGLMFormat(mesh->mBones[boneIdx]->mOffsetMatrix));
                boneInfo[boneName] = newBoneData;
                boneID = nBones++;  // assign first, then increase number of bones
            }
            else {
                boneID = boneInfo[boneName].id;
            }

            setVertexBoneData(boneID, mesh->mBones[boneIdx]->mWeights, mesh->mBones[boneIdx]->mNumWeights, vertices);
        }
    }


    void setVertexBoneData(int boneID, aiVertexWeight* weights, int nWeights, std::vector<Vertex>& vertices) {
        // given the data for a bone from assimp set the accroding vertex to hold the bone data, i.e. reverse the mapping boneID -> (vertID, weight) to vertID -> (boneID, weight)
        for (int weightIdx = 0; weightIdx < nWeights; ++weightIdx) {
            int vertexId = weights[weightIdx].mVertexId;
            float weight = weights[weightIdx].mWeight;
            vertices[vertexId].setBoneData(boneID, weight);
        }
    }
};
#endif
