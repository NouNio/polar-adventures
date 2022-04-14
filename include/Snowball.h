#pragma once
#ifndef SNOWBALL_H
#define SNOWBALL_H

#include <Camera.h>
#include <Model.h>
#include <Physics.h>


class Snowball {
private:
	Model* model;
	btRigidBody* body;
	Physics* pHandler;
	glm::vec3 pos;
	float mass, radius;
	
public:
	Snowball(string const& path, glm::vec3 pos, float mass, float radius, glm::vec3 g, Physics* pHandler, Camera* camera) {
		this->model = new Model(path, camera);
		this->pHandler = pHandler;
		this->pos = pos;
		this->radius = radius;
		this->mass = mass;
		body = pHandler->addSphere(this->pos, this->mass, this->radius);
		body->setGravity(pHandler->GlmVec3ToBulletVec3(g));
	}

	void draw(Shader* shader, float angle, glm::vec3 rotationAxes, glm::vec3 scale) {
		
		// probably infer all these values from the physics model
		btVector3 currPos = this->body->getWorldTransform().getOrigin();
		this->model->draw(*shader, pHandler->BulletVec3ToGlmVec3(currPos), angle, rotationAxes, scale);
	}
};

#endif // !SNOWBALL_H
