#pragma once
#ifndef GRAVITYMODEL
#define GRAVITYMODEL

#include <glm/glm.hpp>

struct CubeGravtiy {
    glm::vec3 d;           // half the width of the planet
    glm::vec3 zeroVec = glm::vec3(0.0f);
    float G = 9.81f;

    glm::vec3 e_x = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 e_y = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 e_z = glm::vec3(0.0f, 0.0f, 1.0f);
    float epsilon = 0.1f;
    glm::vec3 gradient = glm::vec3(0.0f);
    glm::vec3 scaled_gradient = glm::vec3(0.0f);

    CubeGravtiy(glm::vec3 halfWay) {
        d = halfWay;
    }

    void approxGradient(glm::vec3 objectPos) {
        gradient = glm::vec3(
            f(objectPos + epsilon * e_x) - f(objectPos - epsilon * e_x),
            f(objectPos + epsilon * e_y) - f(objectPos - epsilon * e_y),
            f(objectPos + epsilon * e_z) - f(objectPos - epsilon * e_z)
        );

        scaleGradient();
    }

    glm::vec3 getGradient(glm::vec3 objectPos) {
        approxGradient(objectPos);
        return scaled_gradient;
    }

    void scaleGradient() {
        glm::vec3 absGradient = glm::abs(gradient);
        float gradientSum = absGradient.x + absGradient.y + absGradient.z;   // compute total gradien amount
        glm::vec3 percGradient = gradient / gradientSum;                     // scale each gradient component to %
        scaled_gradient = -1.0f * G * percGradient;                          // times -1 to reverse gradient direction
    }

    float f(glm::vec3 pos) {
        glm::vec3 absPos = glm::abs(pos);
        glm::vec3 distance = absPos - d;
        glm::vec3 maxPos = glm::max(distance, zeroVec);
        return glm::length(maxPos);

        //return glm::length( glm::max( (glm::abs(objectPos) - d), zeroVec) );
    }
};

#endif // !GRAVITYMODEL
