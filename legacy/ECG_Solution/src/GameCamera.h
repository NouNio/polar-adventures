#pragma once
#pragma once
#include "Utils.h"
#include "INIReader.h"
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <streambuf>
#include <GLFW/glfw3.h>
#include <gl/GLU.h>
#include "glm/gtx/rotate_vector.hpp"
/*!
NOTE: I might have mixed up pitch and yaw
*/
class GameCamera
{
protected:
    glm::vec3 position;
    glm::vec3 xAxis; //pitch
    glm::vec3 yAxis;//yaw
    glm::vec3 zAxis;
    glm::mat4 perspective;
    float pitch;
    float prevX;
    float prevY;
    //-pi/2>yaw>pi/2
    float yaw;
  
    public:
        bool isPlayer;
        //angles given in degrees
        GameCamera(glm::vec3 initialPosition,
        float init_pitch, float init_yaw, float fovDegrees, float width, float height, float cam_near, float cam_far, bool player ) {
            isPlayer = player;
            position = initialPosition;
            pitch = glm::radians(init_pitch);
            yaw = glm::radians(init_yaw);
            prevX = width / 2;
            prevY = height / 2;
            //rotationAngles = glm::vec3(glm::radians(initialRotation.x), glm::radians(initialRotation.y), glm::radians(initialRotation.z));
            setAxes();

            perspective = calculatePerspective(fovDegrees, width, height, cam_near, cam_far);
        }
        glm::mat4 calculatePerspective(float fovDegrees, float width, float height, float cam_near, float cam_far) {
            float fov = glm::radians(fovDegrees);
            glm::mat4 perspective = glm::perspective<float>(fov, (height/(width)), cam_near, cam_far);
            perspective = glm::inverse(perspective);
            return perspective;
        }
        glm::vec3 getPosition() {
            return position;
        }
        /*!Movement.x movement -a/+d
        Movement.y movement +[space]/-[shift]
        movement.z movement -w/+s*/
        void moveCamera(glm::vec3 movement){
            position.y-=movement.y;
            if (isPlayer&&position.y > -1)position.y = -1;

          glm::vec3  x = glm::vec3(1, 0, 0);
           glm::vec3 y = glm::vec3(0, 1, 0);
           glm::vec3 z= glm::vec3(0, 0, 1);

         
           // zAxis = glm::rotate(zAxis, pitch, xAxis);

            x = glm::rotate(x, -yaw, y);
            z = glm::rotate(z, -yaw, y);


          

            //movement a/d
            position = position - (movement.x * x);

            //movement w/s
            position = position + (movement.z * z);

            setAxes();

        }
        
        glm::mat4 getCombined(){
            return  glm::inverse(GetViewMatrix()*perspective);
        }

        /*
        * view = calculate_lookAt_matrix(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        */

        glm::mat4 GetViewMatrix() {
            setAxes();
            glm::vec3 x = xAxis;
            glm::vec3 y = yAxis;
            glm::vec3 z = zAxis;

            glm::mat4 translation = glm::translate(glm::mat4(1.0), glm::vec3(-position.x, -position.y, -position.z));
            //combined rotationMatrix, x,y,z values because as normalized, it equals the sin/cos values, also the rotationvalues are on different axes
            glm::mat4 rotation = {
                glm::vec4(x.x, y.x, z.x, 0),
                glm::vec4(x.y, y.y, z.y, 0),
                glm::vec4(x.z, y.z, z.z, 0),
                glm::vec4(0,   0,   0,   1)
            };

            return (  translation*rotation);
        }
        glm::vec3 getViewVector() {
            return -glm::normalize(zAxis);
        }
        void updateRotation(float mouseX, float mouseY) {
            float dYaw = (mouseY-prevY) ;
            float dPitch = (mouseX -prevX);
            prevX = mouseX;
            prevY = mouseY;
       
            dYaw = glm::radians(dYaw);
            dPitch = glm::radians(dPitch);
            yaw += dYaw;
            float referenceValue = glm::half_pi<float>();
            if (yaw > glm::two_pi<float>()) { yaw -= glm::two_pi<float>(); }
            if (yaw < 0) { yaw += glm::two_pi<float>(); }


            if ((pitch + dPitch) > referenceValue) {
                pitch = glm::half_pi<float>();
            }
            else if ((pitch + dPitch) < -referenceValue){
                pitch = -glm::half_pi<float>();
            }
            else {
                    pitch += dPitch;
                }
            
      


                setAxes();
        }
        void setAxes() {
            xAxis = glm::vec3(1, 0, 0);
            yAxis = glm::vec3(0, 1, 0);
            zAxis = glm::vec3(0, 0, 1);

            yAxis = glm::rotate(yAxis, pitch, xAxis);
            zAxis = glm::rotate(zAxis, pitch, xAxis);

            xAxis = glm::rotate(xAxis, yaw, yAxis);
            zAxis = glm::rotate(zAxis, yaw, yAxis);
        }
        glm::mat4 getEnvMatrix() {
            glm::mat4 returnV= glm::mat4(glm::mat3(GetViewMatrix()));
            return inverse(returnV *perspective);
             
        }




    };

