#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>

#include <Bone.h>
#include <Model.h>


struct AnimNode {
	std::string name;
	glm::mat4 transformation;
	int childrenCount;
	std::vector<AnimNode> children;

	void setValues(const char* initName, glm::mat4 initTrans, int initNumChildren) {
		name = initName;
		transformation = initTrans;
		childrenCount = initNumChildren;
	}
};

class Animation
{
public:
	Animation() = default;

	Animation(const std::string& animPath, Model* model) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animPath, aiProcess_Triangulate);
		//assert(scene && scene->mRootNode);
		auto animation = scene->mAnimations[0];
		duration = animation->mDuration;
		ticksPerSec = animation->mTicksPerSecond;
		
		parseAnimNodeData(rootNode, scene->mRootNode);

		parseMissingBones(animation, *model);
	}


	Bone* findBone(const std::string& name) {
		auto iter = std::find_if(bones.begin(), bones.end(), 
			[&](const Bone& Bone) {
				return Bone.getBoneName() == name;
			}
		);

		if (iter == bones.end()) 
			return nullptr;
		else 
			return &(*iter);
	}


	inline float getTPS() { 
		return ticksPerSec; 
	}


	inline float getDuration() { 
		return duration; 
	}


	inline const AnimNode& getRootNode() { 
		return rootNode; 
	}


	inline const std::map<std::string, BoneInfo>& getBoneInfo() {
		return boneInfo;
	}


private:
	float duration;
	int ticksPerSec;
	std::vector<Bone> bones;
	AnimNode rootNode;
	std::map<std::string, BoneInfo> boneInfo;


	void parseMissingBones(const aiAnimation* animation, Model& model) {
		auto& newBoneInfo = model.GetBoneInfoMap();
		int& boneCount = model.GetBoneCount();

		// going throught the channels, i.e bones taking part in an animation and the the corresponding keyframes
		for (int i = 0; i < animation->mNumChannels; i++) {
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (newBoneInfo.find(boneName) == newBoneInfo.end()) {
				newBoneInfo[boneName].id = boneCount;
				boneCount++;
			}
			bones.push_back( Bone(channel->mNodeName.data, newBoneInfo[channel->mNodeName.data].id, channel) );
		}

		boneInfo = newBoneInfo; // update the boneInfo map to the one containing the bones that were missing
	}


	void parseAnimNodeData(AnimNode& animNode, const aiNode* assimpNode) {
		animNode.setValues(assimpNode->mName.data, ConvertMatrixToGLMFormat(assimpNode->mTransformation), assimpNode->mNumChildren);  // set this node values

		// search for all children and set their values... bottom to top
		for (int i = 0; i < assimpNode->mNumChildren; i++) {
			AnimNode newAnimNode;
			parseAnimNodeData(newAnimNode, assimpNode->mChildren[i]);
			animNode.children.push_back(newAnimNode);
		}
	}
};
#endif