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


struct VertexBoneData {
    unsigned int BoneIDs[MAX_NUM_BONES_PER_VERTEX] = { 0 };
    float Weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

    VertexBoneData() {}

    void AddBoneData(unsigned int BoneID, float Weight) {
        for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(BoneIDs); i++) {
            if (Weights[i] == 0.0) {
                BoneIDs[i] = BoneID;
                Weights[i] = Weight;
                printf("bone %d  with weigth %.2f and index %i\n", BoneID, Weight, i);
                return;
            }
        }
        // should never get here, i.e. have more bones than we defined space for
        //assert(0);
    }
};

struct BoneInfo {
    glm::mat4 offsetMatrix;
    glm::mat4 finalTransformation;

    BoneInfo(const glm::mat4& offset) {
        offsetMatrix = offset;
        finalTransformation = glm::mat4(0.0f);
    }
};

struct BoneInfo2{
    int id;             // idx in final BoneMatrix in for the shaders

    glm::mat4 offset; // transforms vertex from model to bone space

};


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


    auto& GetBoneInfoMap() { return m_boneInfoMap; }
    
    
    int& GetBoneCount() { return m_boneCounter; }


    void get_bone_transforms(float timeInSeconds, std::vector<glm::mat4>& transforms) {
        glm::mat4 eye = glm::mat4(1.0f);  // creates identity matrix

        float tps = (float)(scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f);
        float timeInTicks = timeInSeconds * tps;  // how far in the ticks are we 
        float animTimeTicks = fmod(timeInTicks, (float)scene->mAnimations[0]->mDuration);  // scale this down to the range of the animation via modulo --> loop

        parse_node_hierarchy(animTimeTicks, scene->mRootNode, eye);
        transforms.resize(boneInfos.size());

        for (int i = 0; i < boneInfos.size(); i++) {
            transforms[i] = boneInfos[i].finalTransformation;
        }
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


protected:
    Assimp::Importer importer;
    const aiScene* scene = NULL;
    int space_count = 0;
    int total_vertices = 0;
    int total_indices = 0;
    int total_bones = 0;
    std::vector<VertexBoneData> vertex_to_bones;				// bones
    std::vector<int> mesh_base_vertex;							// indices
    std::map<std::string, unsigned int> bone_name_to_idx_map;
    std::vector<BoneInfo> boneInfos;
    glm::mat4 globalInverseTransform;

    std::map<std::string, BoneInfo2> m_boneInfoMap;
    int m_boneCounter = 0;


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

        globalInverseTransform = glm::inverse(ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation));

        printf("**************************************************\n");
        parseMeshes();
        printf("**************************************************\n\n\n");

        //parse_hierarchy();  // this is printing the node hierarchy
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

        mesh_base_vertex.resize(scene->mNumMeshes);

        for (unsigned int mesh_idx = 0; mesh_idx < scene->mNumMeshes; mesh_idx++) {
            aiMesh* mesh = scene->mMeshes[mesh_idx];

            int num_vertices = mesh->mNumVertices;
            int num_indices = mesh->mNumFaces * 3;
            int num_bones = mesh->mNumBones;

            mesh_base_vertex[mesh_idx] = total_vertices;

            printf("  Mesh %d '%s': vertices %d   indices %d   bones %d\n\n", mesh_idx, mesh->mName.C_Str(), num_vertices, num_indices, num_bones);
            total_vertices += num_vertices;
            total_indices += num_indices;
            total_bones += num_bones;

            vertex_to_bones.resize(total_vertices);

            meshes.push_back(parseMesh(mesh_idx, mesh));

            printf("\n");
        }
        printf("\nTotal vertices %d   total indices %d   total bones %d", total_vertices, total_indices, total_bones);
    }


    Mesh parseMesh(unsigned int meshIdx, aiMesh* mesh) {// create the data to call the Mesh constructor
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Material material{};
        Texture texture;

        if (this->animated) { // mesh->HasBones()
            parse_mesh_bones(meshIdx, mesh);
        }

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
		for (unsigned int i = 0; i < MAX_BONES_PER_VERTEX; ++i) {
			// find the first free spot and insert the bone there, then break out
			if (vertex.boneIDs[i] < 0) {
				vertex.boneIDs[i] = boneID;
				vertex.boneWeights[i] = weight;
				foundFreeSlot = true;
				break;
			}
		}

		if (!foundFreeSlot) {
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
		}
	
	}


    std::unordered_map<int, int> vertexIDToNumBonesSet;
    void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        auto& boneInfoMap = m_boneInfoMap;
        int& boneCount = m_boneCounter;

        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo2 newBoneInfo;
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

                if (weight >= 0.01) {
                    assert(vertexId <= vertices.size());
                    SetVertexBoneData(vertices[vertexId], boneID, weight);
                }
            }
        }
    }


    void parse_mesh_bones(int mesh_idx, aiMesh* mesh) {
        for (int i = 0; i < mesh->mNumBones; i++) {
            parse_single_bone(mesh_idx, mesh->mBones[i]);
        }
    }


    void parse_single_bone(int mesh_idx, aiBone* bone) {
        printf("      Bone %d: '%s' affetcs %d vertices\n", mesh_idx, bone->mName.C_Str(), bone->mNumWeights);
        int bone_id = get_bone_id(bone);
        printf("Bone id %d\n", bone_id);

        if (bone_id == boneInfos.size()) {
            BoneInfo bi(ConvertMatrixToGLMFormat(bone->mOffsetMatrix));
            boneInfos.push_back(bi);
        }


        for (int i = 0; i < bone->mNumWeights; i++) {
            if (i == 0) printf("\n");
            const aiVertexWeight& vw = bone->mWeights[i];

            unsigned int global_vertex_id = mesh_base_vertex[mesh_idx] + vw.mVertexId;
            printf("Vertex id %d  ", global_vertex_id);

            assert(global_vertex_id < vertex_to_bones.size());
            vertex_to_bones[global_vertex_id].AddBoneData(bone_id, vw.mWeight);
        }

        printf("\n");
    }


    int get_bone_id(aiBone* bone) {
        int bone_id = 0;
        std::string bone_name(bone->mName.C_Str());

        if (bone_name_to_idx_map.find(bone_name) == bone_name_to_idx_map.end()) {
            // allocate idx for new bone
            bone_id = bone_name_to_idx_map.size();
            bone_name_to_idx_map[bone_name] = bone_id;
        }
        else {
            bone_id = bone_name_to_idx_map[bone_name];
        }

        return bone_id;
    }


    void parse_node_hierarchy(float animTimeTicks, const aiNode* node, const glm::mat4 parentTransform) {
        std::string nodeName(node->mName.data);

        const aiAnimation* animation = scene->mAnimations[0];

        glm::mat4 nodeTransformation = ConvertMatrixToGLMFormat(node->mTransformation);

        const aiNodeAnim* nodeAnim = findNodeAnim(animation, &nodeName);

        if (nodeAnim) {
            // interpolate scaling and generate scaling transformation matrix
            aiVector3D scaling;
            calcInterpolatedScaling(scaling, nodeAnim, animTimeTicks);
            glm::mat4 scalingMat = glm::mat4(1.0);
            scalingMat = glm::scale(scalingMat, glm::vec3(scaling.x, scaling.y, scaling.z));

            // interpolate rotation and generate rotation transformation matrix
            aiQuaternion rotation;
            calcInterpolatedRotation(rotation, nodeAnim, animTimeTicks);
            glm::mat4 rotationMat = glm::mat4(1.0);
            rotationMat = convertAssimp3x3ToGLM4x4Matrix(rotation.GetMatrix());

            // interpolate translation and generate translation transformation matrix
            aiVector3D translation;
            calcInterpolatedTranslation(translation, nodeAnim, animTimeTicks);
            glm::mat4 translationMat = glm::mat4(1.0);
            translationMat = glm::translate(translationMat, glm::vec3(translation.x, translation.y, translation.z));


            // combine all three together
            nodeTransformation = translationMat * rotationMat * scalingMat;
        }

        //printf("%s -\n", nodeName.c_str());

        glm::mat4 globalTransformation = parentTransform * nodeTransformation;

        if (bone_name_to_idx_map.find(nodeName) != bone_name_to_idx_map.end()) {  // this is an acutal bone, some nodes dont have a bone 
            unsigned int bone_idx = bone_name_to_idx_map[nodeName];
            boneInfos[bone_idx].finalTransformation = globalInverseTransform * globalTransformation * boneInfos[bone_idx].offsetMatrix;
        }

        // traverse node tree recursively
        for (int i = 0; i < node->mNumChildren; i++) {
            parse_node_hierarchy(animTimeTicks, node->mChildren[i], globalTransformation);
        }
    }


    const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string* nodeName) {
        for (int i = 0; i < animation->mNumChannels; i++) {
            const aiNodeAnim* nodeAnim = animation->mChannels[i];

            if (std::string(nodeAnim->mNodeName.data) == *nodeName) {
                return nodeAnim;
            }
        }
        return NULL;
    }


    void calcInterpolatedScaling(aiVector3D& scaling, const aiNodeAnim* nodeAnim, float animTimeTicks) {
        // check if at least two values
        if (nodeAnim->mNumScalingKeys == 1) {
            scaling = nodeAnim->mScalingKeys[0].mValue; return;
        }

        unsigned int scalingIdx = findScaling(animTimeTicks, nodeAnim);  // idx of previous time step scale
        unsigned int nextScalingIdx = scalingIdx + 1;					 // idx of next time step scale
        assert(nextScalingIdx < nodeAnim->mNumScalingKeys);


        // interpolate
        float time1 = (float)nodeAnim->mScalingKeys[scalingIdx].mTime;
        float time2 = (float)nodeAnim->mScalingKeys[nextScalingIdx].mTime;
        float dt = time2 - time1;
        float factor = (animTimeTicks - time1) / dt;  // in (0,1) based on how close we are to the next anim --> factor ~ 1 or the befor anim --> factor ~ 0
        assert(factor <= 1.0f && factor >= 0.0f);

        const aiVector3D& startScale = nodeAnim->mScalingKeys[scalingIdx].mValue;
        const aiVector3D& endScale = nodeAnim->mScalingKeys[nextScalingIdx].mValue;
        aiVector3D deltaScale = endScale - startScale;
        scaling = startScale + factor * deltaScale;
    }


    unsigned int findScaling(float animTimeTicks, const aiNodeAnim* nodeAnim) {
        assert(nodeAnim->mNumScalingKeys > 0);

        for (int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++) {
            if (animTimeTicks < (float)nodeAnim->mScalingKeys[i + 1].mTime) {
                return i;
            }
        }
        return 0;
    }


    void calcInterpolatedTranslation(aiVector3D& translation, const aiNodeAnim* nodeAnim, float animTimeTicks) {
        // check if at least two values
        if (nodeAnim->mNumPositionKeys == 1) {
            translation = nodeAnim->mPositionKeys[0].mValue; return;
        }

        unsigned int translationIdx = findTranslation(animTimeTicks, nodeAnim);  // idx of previous time step scale
        unsigned int nextTranslationIdx = translationIdx + 1;					 // idx of next time step scale
        assert(nextTranslationIdx < nodeAnim->mNumPositionKeys);


        // interpolate
        float time1 = (float)nodeAnim->mPositionKeys[translationIdx].mTime;
        float time2 = (float)nodeAnim->mPositionKeys[nextTranslationIdx].mTime;
        float dt = time2 - time1;
        float factor = (animTimeTicks - time1) / dt;  // in (0,1) based on how close we are to the next anim --> factor ~ 1 or the befor anim --> factor ~ 0
        assert(factor <= 1.0f && factor >= 0.0f);

        const aiVector3D& startTranslation = nodeAnim->mPositionKeys[translationIdx].mValue;
        const aiVector3D& endTranslation = nodeAnim->mPositionKeys[nextTranslationIdx].mValue;
        aiVector3D deltaTranslation = endTranslation - startTranslation;
        translation = startTranslation + factor * deltaTranslation;
    }


    unsigned int findTranslation(float animTimeTicks, const aiNodeAnim* nodeAnim) {
        assert(nodeAnim->mNumPositionKeys > 0);

        for (int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++) {
            if (animTimeTicks < (float)nodeAnim->mPositionKeys[i + 1].mTime) {
                return i;
            }
        }
        return 0;
    }


    void calcInterpolatedRotation(aiQuaternion& rotation, const aiNodeAnim* nodeAnim, float animTimeTicks) {
        // check if at least two values
        if (nodeAnim->mNumRotationKeys == 1) {
            rotation = nodeAnim->mRotationKeys[0].mValue; return;
        }

        unsigned int rotationIdx = findRotation(animTimeTicks, nodeAnim);  // idx of previous time step scale
        unsigned int nextRotationIdx = rotationIdx + 1;					 // idx of next time step scale
        assert(nextRotationIdx < nodeAnim->mNumRotationKeys);


        // interpolate
        float time1 = (float)nodeAnim->mRotationKeys[rotationIdx].mTime;
        float time2 = (float)nodeAnim->mRotationKeys[nextRotationIdx].mTime;
        float dt = time2 - time1;
        float factor = (animTimeTicks - time1) / dt;  // in (0,1) based on how close we are to the next anim --> factor ~ 1 or the befor anim --> factor ~ 0
        assert(factor <= 1.0f && factor >= 0.0f);

        const aiQuaternion& startRotaion = nodeAnim->mRotationKeys[rotationIdx].mValue;
        const aiQuaternion& endRotation = nodeAnim->mRotationKeys[nextRotationIdx].mValue;
        aiQuaternion::Interpolate(rotation, startRotaion, endRotation, factor);
        rotation = startRotaion;
        rotation.Normalize();
    }


    unsigned int findRotation(float animTimeTicks, const aiNodeAnim* nodeAnim) {
        assert(nodeAnim->mNumRotationKeys > 0);

        for (int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++) {
            if (animTimeTicks < (float)nodeAnim->mRotationKeys[i + 1].mTime) {
                return i;
            }
        }
        return 0;
    }


    void parse_hierarchy() {
        printf("\n*****************************************\n");
        printf("Parsing the node hierarchy.\n");

        parse_node(scene->mRootNode);
    }


    void parse_node(const aiNode* node) {
        print_space(space_count);  printf("Node name: '%s'  num children %d  num meshes %d\n", node->mName.C_Str(), node->mNumChildren, node->mNumMeshes);
        print_space(space_count);  printf("Node transformation:\n");
        print_assimp_matrix(node->mTransformation, space_count);

        space_count += 4;

        for (int i = 0; i < node->mNumChildren; i++) {
            printf("\n");
            print_space(space_count); printf("--- %d ---\n", i);
            parse_node(node->mChildren[i]);
        }

        space_count -= 4;
    }
};
#endif
