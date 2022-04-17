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
    float moveSpeed, mouseSensitivity, zoom;
    std::unique_ptr<Frustum> frustum;

    Camera(float fov, float aspect, float near, float far,glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), frustum(std::make_unique<Frustum>(Frustum(fov, near, far, aspect))),projection(glm::perspective(fov, aspect, near, far))

    {
        this->pos = pos;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }


    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        this->pos = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

   
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->pos,                   // position of the camera
                           this->pos + this->front,     // target position, where should the camera look to, i.e. a vector pointing from pos to targetPos
                           this->up);                   //
    }


    void setCubeSide(unsigned int newCubeSide){
        this->cubeSide = newCubeSide;
    }


    void setPosition(glm::vec3 newPos)
    {
        this->pos = newPos;
    }


    void setWorldUp(glm::vec3 newWorldUp) {
        this->worldUp = newWorldUp;
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

        // TODO: add update of player left right rotation, with the yaw value

        updateCameraVectors();
    }


    // process mouse input received from the vertical wheel-axis and clip any exceeding values
    void processMouseScroll(float yoffset)
    {
        this->zoom -= (float)yoffset;
        if (this->zoom < MIN_ZOOM)
            this->zoom = MIN_ZOOM;
        if (this->zoom > MAX_ZOOM)
            this->zoom = MAX_ZOOM;
    }


    /*bool isInFrustum(Mesh h) {
        frustum.get()->isInside(h.bound.getPoints());
        return false;

    }*/

private:
    unsigned int cubeSide = CUBE_TOP;

    // update the camera vectors, i.e. FRONT, UP, RIGHT. Note that order matters here, as the UP values uses the new RIGHT value & RIGHT uses the new FRONT.
    void updateCameraVectors()
    {
        updateFrontVector();
        updateRightVector();
        updateUpVector();
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
                                             sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))));
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

