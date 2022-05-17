#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>



/* ------------------------------------------------------------------------------------ */
// CONVERSION FROM ASSIMP TO GLM
// total credit goes to learn-opengl: 
// https://github.com/JoeyDeVries/LearnOpenGL/blob/166aeced4b950daf8f7617a8e68568a9e500970f/includes/learnopengl/assimp_glm_helpers.h
/* ------------------------------------------------------------------------------------ */


glm::mat4 convertAssimp3x3ToGLM4x4Matrix(const aiMatrix3x3& assimpMat) {
    glm::mat4 rotMat(0.0);

    rotMat[0][0] = assimpMat.a1; rotMat[0][1] = assimpMat.a2; rotMat[0][2] = assimpMat.a3; rotMat[0][3] = 0.0f;
    rotMat[1][0] = assimpMat.b1; rotMat[1][1] = assimpMat.b2; rotMat[1][2] = assimpMat.b3; rotMat[1][3] = 0.0f;
    rotMat[2][0] = assimpMat.c1; rotMat[2][1] = assimpMat.c2; rotMat[2][2] = assimpMat.c3; rotMat[2][3] = 0.0f;
    rotMat[3][0] = 0.0f;         rotMat[3][1] = 0.0f;         rotMat[3][2] = 0.0f;         rotMat[3][3] = 1.0f;

    return glm::transpose(rotMat);  // this is important since GLM is column major and Assimp is row major  (oGL is also column major)

    //glm::mat4 to;
    ////the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    //to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    //to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    //to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    //to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    //return to;
}


static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from){
	glm::mat4 to{};
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

static inline glm::vec3 GetGLMVec(const aiVector3D& vec){
	return glm::vec3(vec.x, vec.y, vec.z);
}

static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation){
	return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}


void print_space(int space_count) {
    for (int i = 0; i < space_count; i++) {
        printf(" ");
    }
}


void print_assimp_matrix(const aiMatrix4x4& m, int space_count) {
    print_space(space_count); printf("%f %f %f %f\n", m.a1, m.a2, m.a3, m.a4);
    print_space(space_count); printf("%f %f %f %f\n", m.b1, m.b2, m.b3, m.b4);
    print_space(space_count); printf("%f %f %f %f\n", m.c1, m.c2, m.c3, m.c4);
    print_space(space_count); printf("%f %f %f %f\n", m.d1, m.d2, m.d3, m.d4);
}


#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))  // this is taken from ogldev


#endif // !UTILS_H

