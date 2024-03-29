#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#pragma once
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <Frustum.h>
#include <Constants.h>

// this code is inspired by learnopengl.com as this is my main resource for opengl information
// other resources my have been influential as well, this file howver should describes my way of implementing the theoretical concepts


enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP
};

// Default camera values
const float YAW = -90.0f;           // angle that describes how much the camera looks left or right
const float PITCH = 0.0f;           // angle that describes how much the camera looks up or down
const float MAX_PITCH = 89.0f;
const float MAX_ZOOM = 45.0f;
const float MIN_ZOOM = 1.0f;
const float SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera
{
public:
    // camera Attributes
    glm::vec3 pos, front, up, right;
    // world attribute
    glm::vec3 worldUp;
    //projection Matrix
    glm::mat4 projection;
    // euler Angles
    float yaw, pitch;
    // camera options
    float moveSpeed, mouseSensitivity, zoom, near, far, aspect;
    std::unique_ptr<Frustum> frustum;
    bool VFCEnabled;


    Camera(float fov, float aspect, float near, float far,glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), frustum(std::make_unique<Frustum>(Frustum(glm::radians(zoom), near, far, aspect))), projection(glm::perspective(glm::radians(zoom), aspect, near, far))

    {
        this->aspect = aspect;
        this->near = near;
        this->far = far;
        this->pos = pos;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        projection = glm::perspective(glm::radians(zoom), aspect, near, far);
        updateCameraVectors();
    }

   
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->pos,                   // position of the camera
                           this->pos + this->front,     // target position, where should the camera look to, i.e. a vector pointing from pos to targetPos
                           this->up);                   //
    }
    glm::mat4 GetProjection() {
        return projection;
    }

    void setCubeSide(unsigned int newCubeSide){
        
   
      //*
       unsigned int prev = this->cubeSide;
     this->cubeSide = newCubeSide;
       if (cubeSide != prev) {
   
           /*
           switch (this->cubeSide) {
           case CUBE_LEFT:
               this->front = glm::normalize(glm::vec3(-22, 0, 0) - this->pos);

               break;
           case CUBE_RIGHT:
               this->front = glm::normalize(glm::vec3(22, 0, 0) - this->pos);
               break;
           case CUBE_FRONT:
               this->front = glm::normalize(glm::vec3(0, 0, -22) - this->pos);
               break;
           case CUBE_BACK:
               this->front = glm::normalize(glm::vec3(0, 0, 22) - this->pos);
               break;
           case CUBE_TOP:
               this->front = glm::normalize(glm::vec3(0, 22, 0) - this->pos);
               break;
           case CUBE_BOTTOM:
               this->front = glm::normalize(glm::vec3(0, -22, 0) - this->pos);

               break;
           }
       //*/
           pitch = 0;
         
           unsigned int s1 = CUBE_TOP;
           unsigned int s2 = CUBE_LEFT;
           bool c = (cubeSide == s1 && prev == s2) || (prev == s1 && cubeSide == s2);
           if (c) {
                   processMouseMovement((180.0)/mouseSensitivity, 0.0f, true);
           }
             s1 = CUBE_BOTTOM;
             s2 = CUBE_RIGHT;
            c = (cubeSide == s1 && prev == s2) || (prev == s1 && cubeSide == s2);
           if (c) {
               processMouseMovement((180.0) / mouseSensitivity, 0.0f, true);
           }




           std::vector<unsigned int> pathsides = { CUBE_LEFT, CUBE_FRONT, CUBE_RIGHT, CUBE_BACK };
           if (std::find(pathsides.begin(), pathsides.end(), prev) != pathsides.end() && std::find(pathsides.begin(), pathsides.end(), cubeSide) != pathsides.end()) {
               float v = 90;
               c = (prev == CUBE_FRONT && cubeSide == CUBE_LEFT) || (prev == CUBE_LEFT && cubeSide == CUBE_BACK) || (prev == CUBE_RIGHT && cubeSide == CUBE_FRONT) || (prev == CUBE_BACK && cubeSide == CUBE_RIGHT)
                   ;
               if (CUBE_RIGHT == prev || CUBE_RIGHT == cubeSide)v *= -1;
               if (c)v *= -1;
               processMouseMovement((v) / mouseSensitivity, 0.0f, true);
               updateCameraVectors();
           }



           processMouseMovement(0.0f, 0.0f, true);
           processKeyboard(FORWARD, 0.5f);
          
       }
 
      
    }


    void setPosition(glm::vec3 newPos)
    {
        this->pos = newPos;
    }


    void setWorldUp(glm::vec3 newWorldUp) {
        this->worldUp = newWorldUp;
    }


    bool getVFCEnabled() {
        return VFCEnabled;
    }


    void setVFCEnabled(bool value) {
        this->VFCEnabled = value;
    }


    // processes input received from any keyboard-like input system. 
    // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems) (I think this a very nice idea from Joey de Vries)
    void processKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = this->moveSpeed * deltaTime;
        if (direction == FORWARD) 
        {
            this->pos += this->front * velocity;
        }
        if (direction == BACKWARD)
        {
            this->pos -= this->front * velocity;
        }
        if (direction == LEFT)
        {
            this->pos -= this->right * velocity;
        }
        if (direction == RIGHT)
        {
            this->pos += this->right * velocity;
        }
        if (direction == UP)
        {
            this->pos += this->up * velocity;
        }
        frustum->reset(pitch, yaw, pos, up, front);
    }


    // processes input received as the offset in x and y w.r.t. the last measured mouse pos.
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= this->mouseSensitivity;
        yoffset *= this->mouseSensitivity;

        this->yaw += xoffset;   // update euler angles w.r.t new mouse pos
        this->pitch += yoffset;

        if (constrainPitch)  // if pitch (up / down angle) would leave bounds, the view would be inverted
        {
            if (this->pitch > MAX_PITCH)
                this->pitch = MAX_PITCH;
            if (this->pitch < -MAX_PITCH)
                this->pitch = -MAX_PITCH;
        }

        updateCameraVectors();
    }


private:
    unsigned int cubeSide = CUBE_TOP;

    // update the camera vectors, i.e. FRONT, UP, RIGHT. Note that order matters here, as the UP values uses the new RIGHT value & RIGHT uses the new FRONT.
    void updateCameraVectors()
    {
        updateFrontVector();
        updateRightVector();
        updateUpVector();
        frustum->reset(pitch, yaw, pos, up, front);
    }


    void updateFrontVector()  // update the cameras front vector using eulers formula --> different order of eulers values for different cube sides
    {
        switch (this->cubeSide) {
            case CUBE_LEFT:
                this->front = glm::normalize( -glm::vec3(sin(glm::radians(this->pitch)),
                                              (cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))),
                                              sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))));
                break;
            case CUBE_RIGHT:
                this->front = glm::normalize( glm::vec3(sin(glm::radians(this->pitch)),
                                              -(cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))),
                                              sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))));
                break;
            case CUBE_FRONT:
                this->front = glm::normalize(glm::vec3(cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                                             -sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                                             sin(glm::radians(this->pitch))));
                break;
            case CUBE_BACK:
                this->front = glm::normalize(glm::vec3(cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                                             sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                                             -sin(glm::radians(this->pitch))));
                break;
            case CUBE_TOP:
                this->front = glm::normalize( glm::vec3(cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                                              sin(glm::radians(this->pitch)),
                                              sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))));
                break;
            case CUBE_BOTTOM:
                this->front = glm::normalize(glm::vec3(cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),
                                             -sin(glm::radians(this->pitch)),
                                             -sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))));

                break;  
        }
    }

        
    void updateRightVector()
    {
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));  // normalize, cause the length gets closer to 0 the more you look up or down, which would result in slower movement
    }


    void updateUpVector()
    {
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};

#endif

