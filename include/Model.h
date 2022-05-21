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


class Model
{
public:
    std::vector<Mesh> meshes;          // all the meshes of the model, usually our models have aroudn 2-3 meshes


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
            meshes[i].transformBound(modelMatrix);
            if (viewFrustumCulling) {
                if (isInFrustum(meshes[i]))
                    meshes[i].draw(shader);
            }
            else {
                meshes[i].draw(shader);
                camera.frustum->increaseRenderedObjects();
            }
            meshes[i].resetBound();
           
        }
    }


    void draw(Shader& shader) {

        bool viewFrustumCulling = camera.getVFCEnabled();
        for (unsigned int i = 0; i < meshes.size(); i++) {
            if (viewFrustumCulling) {
                if (isInFrustum(meshes[i]))
                    meshes[i].draw(shader);
            }
            else {
                meshes[i].draw(shader);
                camera.frustum->increaseRenderedObjects();
            }
            meshes[i].resetBound();

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


    bool isInFrustum (Mesh m) const {
        return 
            //true;
            camera.frustum->isInside(m.bound.getPoints());
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


    void getVertexToNumBones() {
        int vertToNumBones[4322] = {};  // all zeros
        for (int i = 0; i < scene->mNumMeshes; i++) {
            for (int j = 0; j < scene->mMeshes[i]->mNumBones; j++) {
                for (int k = 0; k < scene->mMeshes[i]->mBones[j]->mNumWeights; k++) {
                    int idx = scene->mMeshes[i]->mBones[j]->mWeights[k].mVertexId;
                    vertToNumBones[idx] += 1;
                }
            }
        }

        for (int i = 0; i < 4322; i++) {
            if (vertToNumBones[i] > 4) {
                printf("vertex with idx %d has %d bones\n", i, vertToNumBones[i]);
            }
        }
    }

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }


protected:
    Assimp::Importer importer;
    const aiScene* scene = NULL;

    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;


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
            extractBoneWeightForVertices(vertices, mesh, scene);
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
            SetVertexBoneDataToDefault(vertex);
            vertex.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            
            if (vertex.pos.x > xBound[1]) {
                xBound[1] = vertex.pos.x;
            }
            if (vertex.pos.y >yBound[1]) {
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


    // ANIM RELATED 
    void SetVertexBoneDataToDefault(Vertex& vertex)
    {
        for (int i = 0; i < MAX_BONES_PER_VERTEX; i++)
        {
            vertex.boneIDs[i] = -1;
            vertex.boneWeights[i] = 0.0f;
        }
    }


    void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
	{
		bool foundFreeSlot = false;
        for (int i = 0; i < MAX_BONES_PER_VERTEX; ++i) {
            if (vertex.boneIDs[i] < 0){
                vertex.boneWeights[i] = weight;
                vertex.boneIDs[i] = boneID;
                break;
            }
        }

		/*if (!foundFreeSlot) {
			float min_weight = 1.1f;
			int min_idx = -1;
			for (unsigned int i = 0; i < MAX_BONES_PER_VERTEX; ++i) {
				if (vertex.boneWeights[i] < min_weight) {
					min_weight = vertex.boneIDs[i];
					min_idx = vertex.boneWeights[i];
				}
			}

			if (min_weight < weight) {
				vertex.boneIDs[min_idx] = boneID;
				vertex.boneWeights[min_idx] = weight;
			}
		}*/
	
	}


    void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        auto& boneInfoMap = m_BoneInfoMap;
        int& boneCount = m_BoneCounter;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCount;
                newBoneInfo.offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCount;
                boneCount++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }
};
#endif
