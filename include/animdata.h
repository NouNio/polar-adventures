#pragma once

#ifndef ANIMDATA_H
#define ANIMDATA_H

#include<glm/glm.hpp>

struct BoneInfo {
	int id;				// idx in finalBoneMatrices
	glm::mat4 offset;	// this transfrom vertex from model to bone space
};
#endif