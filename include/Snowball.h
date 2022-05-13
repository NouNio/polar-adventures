#pragma once
#ifndef SNOWBALL_H
#define SNOWBALL_H

#include <Camera.h>
#include <Model.h>
#include <Physics.h>
#include <Constants.h>


extern Physics* pHandler;

class Snowball {
private:
	unsigned int s_id;
	Model* s_model;
	btRigidBody* s_body;
	glm::vec3 s_modelScale = glm::vec3(0.01f);
	btVector3 s_bodyScale;
	double s_shrinkFactor = 0.01;
	unsigned int cubeSide;

	void updateGravity() {
		// check which side of the cube the player is on, hard coded and set gravity and jump dir accordingly
		btVector3 currPos = this->s_body->getWorldTransform().getOrigin();
		
		// LEFT
		if (currPos.y() < cubeBounds.y_max && currPos.y() > cubeBounds.y_min
			&& currPos.z() < cubeBounds.z_max && currPos.z() > cubeBounds.z_min
			&& currPos.x() < cubeBounds.x_min) {
			
			s_body->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(9.81, 0.0, 0.0)));
			cubeSide = CUBE_LEFT;
		}
		// RIGHT
		else if (currPos.y() < cubeBounds.y_max && currPos.y() > cubeBounds.y_min
			&& currPos.z() < cubeBounds.z_max && currPos.z() > cubeBounds.z_min
			&& currPos.x() > cubeBounds.x_max) {
			s_body->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(-9.81, 0.0, 0.0)));
			cubeSide = CUBE_RIGHT;;
		}
		// FRONT
		else if (currPos.x() > cubeBounds.x_min && currPos.x() < cubeBounds.x_max
			&& currPos.y() < cubeBounds.y_max && currPos.y() > cubeBounds.y_min
			&& currPos.z() > cubeBounds.z_max) {
			s_body->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(0.0, 0.0, -9.81)));
			cubeSide = CUBE_FRONT;
		}
		// BACK
		else if (currPos.x() > cubeBounds.x_min && currPos.x() < cubeBounds.x_max
			&& currPos.y() < cubeBounds.y_max && currPos.y() > cubeBounds.y_min
			&& currPos.z() < cubeBounds.z_min) {
			s_body->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(0.0, 0.0, 9.81)));
			cubeSide = CUBE_BACK;
		}
		// TOP
		else if (currPos.y() > cubeBounds.y_max) {
			s_body->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(0.0, -9.81, 0.0)));
			cubeSide = CUBE_TOP;
		}
		// BOTTOM
		else if (currPos.y() < cubeBounds.y_min) {
			s_body->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(0.0, 9.81, 0.0)));
			cubeSide = CUBE_BOTTOM;
		}
	}
	
public:

	Physics* s_pHandler;
	Snowball(const unsigned int s_id, std::string const& path, glm::vec3 pos, float mass, float radius, glm::vec3 g, Physics* s_pHandler, Camera* s_camera) {
		this->s_id = s_id;
		this->s_model = new Model(path);
		this->s_pHandler = s_pHandler;
		this->s_body = s_pHandler->addSphere(pos, mass, radius);
		this->s_body->setGravity(s_pHandler->GlmVec3ToBulletVec3(g));
		this->updateGravity();
		this->s_pHandler->activateColCallBack(this->s_body);
		this->s_bodyScale = this->s_body->getCollisionShape()->getLocalScaling();
		this->s_body->setUserPointer(&this->s_id);
	}


	~Snowball() {
		this->s_pHandler->deleteBody(this->s_body);
	}


	void s_draw(Shader* s_shader, float s_angle, glm::vec3 s_rotationAxes) {
		
		// probably infer all these values from the physics model
		btVector3 currPos = this->s_body->getWorldTransform().getOrigin();
		this->s_model->draw(*s_shader, s_pHandler->BulletVec3ToGlmVec3(currPos), s_angle, s_rotationAxes, this->s_modelScale);
	}


	btRigidBody* getBody() {
		return this->s_body;
	}


	void setBody(btRigidBody* newBody) {
		this->s_body = newBody;
	}


	btVector3 getBodyScale() {
		return this->s_bodyScale;
	}


	unsigned int getID() {
		return this->s_id;
	}


	unsigned int* getIDptr() {
		return &this->s_id;
	}


	void shrink(float deltaTime) {
		updateGravity();
		s_bodyScale -= s_bodyScale * s_shrinkFactor * deltaTime;
		this->s_body->getCollisionShape()->setLocalScaling(s_bodyScale);
		this->s_pHandler->getWorld()->updateSingleAabb(this->s_body);

		this->s_modelScale -= glm::vec3(this->s_modelScale.x * s_shrinkFactor * deltaTime);
	}
};

#endif // !SNOWBALL_H
