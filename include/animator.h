#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <Animation.h>
#include <Bone.h>

#define MAX_N_BONE_TRANFORMS 60

class Animator {
private:
	std::vector<glm::mat4> boneTransforms;
	Animation* anim;
	float currTime = 0.0;


public:
	Animator(Animation* initAnim) {
		anim = initAnim;
		initBoneTransforms();  // reserve vector space and fill with identity
	}


	void updateAnim(float dt) {
		if (anim) {
			updateAnimTime(dt);
			computeBoneTransforms(&anim->getRootNode(), glm::mat4(1.0f));
		}
	}


	void setAnim(Animation* newAnim) {
		this->anim = newAnim;
		currTime = 0.0f;
	}


	void computeBoneTransforms(const AnimNode* node, glm::mat4 parentTransform) {
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = retrieveBoneTransform(node);  // get the current time dependent transformation matrix of the bone related to that node
		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = anim->getBoneInfo();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
			boneTransforms[boneInfoMap[nodeName].id] = globalTransformation * boneInfoMap[nodeName].offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			computeBoneTransforms(&node->children[i], globalTransformation);
	}


	std::vector<glm::mat4>* getBoneTransforms() {
		return &boneTransforms;
	}


private:
	void initBoneTransforms() {
		boneTransforms.reserve(MAX_N_BONE_TRANFORMS);

		for (int i = 0; i < MAX_N_BONE_TRANFORMS; i++)
			boneTransforms.push_back(glm::mat4(1.0f));
	}


	void updateAnimTime(float dt) {
		currTime = fmod(currTime + anim->getTPS() * dt, anim->getDuration());
	}


	glm::mat4 retrieveBoneTransform(const AnimNode* node) {
		Bone* Bone = anim->findBone(node->name);

		if (Bone) {
			// get the current both transform after lerping wrt the time
			Bone->update(currTime);
			return Bone->getLocalTransform();
		}
		else
			// or use the nodes transformation otherwise
			return node->transformation;
	}
};
#endif