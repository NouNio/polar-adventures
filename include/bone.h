#pragma once

#ifndef BONE_H
#define BONE_H

#include <vector>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>

#include <Utils.h>

#define KEY_POS 0
#define KEY_ROT 1 
#define KEY_SCL 2

struct KeyPosition {
	glm::vec3 position;
	float timeStamp;

	KeyPosition(glm::vec3 pos, float time) {
		position = pos;
		timeStamp = time;
	}
};

struct KeyRotation {
	glm::quat orientation;
	float timeStamp;

	KeyRotation(glm::quat ori, float time) {
		orientation = ori;
		timeStamp = time;
	}
};

struct KeyScale {
	glm::vec3 scale;
	float timeStamp;

	KeyScale(glm::vec3 scl, float time) {
		scale = scl;
		timeStamp = time;
	}
};


class Bone {
public:
	Bone(const std::string& name, int ID, const aiNodeAnim* channel) : name(name), ID(ID), localTransform(1.0f) {
		initKeyPositions(channel);
		initKeyRotations(channel);
		initKeyScalings(channel);
	}


	void update(float animTime) {
		localTransform = lerpPos(animTime) * lerpRot(animTime) * lerpScl(animTime);  // mat4 * mat4 * mat4
	}
	
	
	glm::mat4 getLocalTransform() const { 
		return localTransform; 
	}
	
	
	std::string getBoneName() const { 
		return name; 
	}
	
	
	int getBoneID() const { 
		return ID; 
	}


	int getPositionIndex(float animTime) {
		// check at which keyframe pos we are now, the animation time t will lie between two keyframes ki and kj with idx i and idx i+1  --> return the lower
		for (int idx = 0; idx < nKeyPosis - 1; ++idx) {
			if (animTime < keyPosis[idx + 1].timeStamp)
				return idx;
		}
		assert(0);
	}


	int getRotationIndex(float animTime) {
		// same as for the kex positions
		for (int idx = 0; idx < nKeyRots - 1; ++idx) {
			if (animTime < keyRots[idx + 1].timeStamp)
				return idx;
		}
		assert(0);
	}


	int getScaleIndex(float animTime) {
		// same as for the key positions
		for (int idx = 0; idx < nKeyScls - 1; ++idx) {
			if (animTime < keyScls[idx + 1].timeStamp)
				return idx;
		}
		assert(0);
	}


private:
	std::vector<KeyPosition> keyPosis;
	std::vector<KeyRotation> keyRots;
	std::vector<KeyScale> keyScls;
	int nKeyPosis, nKeyRots, nKeyScls;
	glm::mat4 localTransform;
	std::string name;
	int ID;


	void initKeyPositions(const aiNodeAnim* channel) {
		nKeyPosis = channel->mNumPositionKeys;
		
		for (int posIdx = 0; posIdx < nKeyPosis; ++posIdx) {
			KeyPosition data(GetGLMVec(channel->mPositionKeys[posIdx].mValue),  // from channel we get an aiVector3 --> convert to glm
				             channel->mPositionKeys[posIdx].mTime);             // retrieve the timestamp
			keyPosis.push_back(data);
		}
	}


	void initKeyRotations(const aiNodeAnim* channel) {
		nKeyRots = channel->mNumRotationKeys;
		
		for (int rotationIndex = 0; rotationIndex < nKeyRots; ++rotationIndex) {
			KeyRotation data(GetGLMQuat(channel->mRotationKeys[rotationIndex].mValue),
				             channel->mRotationKeys[rotationIndex].mTime);
			keyRots.push_back(data);
		}
	}


	void initKeyScalings(const aiNodeAnim* channel) {
		nKeyScls = channel->mNumScalingKeys;
		
		for (int keyIndex = 0; keyIndex < nKeyScls; ++keyIndex) {
			KeyScale data(GetGLMVec(channel->mScalingKeys[keyIndex].mValue),
				          channel->mScalingKeys[keyIndex].mTime);
			keyScls.push_back(data);
		}
	}


	float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
		return (animationTime - lastTimeStamp) / (nextTimeStamp - lastTimeStamp);  // middle way length / frame Difference
	}


	glm::mat4 lerpPos(float animTime) {
		int currPosIdx = getPositionIndex(animTime);  // get the index of the current animation position based on the current time
		
		float scaleFactor = getScaleFactor(keyPosis[currPosIdx].timeStamp, keyPosis[currPosIdx+1].timeStamp, animTime);  // check how much time we advanced to next kex pos in percent

		return glm::translate(glm::mat4(1.0f), glm::mix(keyPosis[currPosIdx].position, keyPosis[currPosIdx + 1].position, scaleFactor));  // lerp the two postions
	}


	glm::mat4 lerpScl(float animTime) {
		int currScaleIdx = getScaleIndex(animTime);  // proces similar to lerping the positions

		float scaleFactor = getScaleFactor(keyScls[currScaleIdx].timeStamp, keyScls[currScaleIdx+1].timeStamp, animTime);
		
		return glm::scale(glm::mat4(1.0f), glm::mix(keyScls[currScaleIdx].scale, keyScls[currScaleIdx + 1].scale, scaleFactor));
	}


	glm::mat4 lerpRot(float animTime) {
		int currRotIdx = getRotationIndex(animTime);  // process similar to lerping positions / scalings, but using glms inbuild quaternion lerper
		
		float scaleFactor = getScaleFactor(keyRots[currRotIdx].timeStamp, keyRots[currRotIdx+1].timeStamp, animTime);
		
		return glm::toMat4(glm::normalize(glm::slerp(keyRots[currRotIdx].orientation, keyRots[currRotIdx + 1].orientation, scaleFactor)));
	}
};
#endif