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
	glm::vec3 modelScale = glm::vec3(0.5f);
	btVector3 bodyScale;
	double shrinkFactor = 0.1;
	
public:
	Snowball(string const& path, glm::vec3 pos, float mass, float radius, glm::vec3 g, Physics* pHandler, Camera* camera) {
		this->model = new Model(path, camera);
		this->pHandler = pHandler;
		this->pos = pos;
		this->radius = radius;
		this->mass = mass;
		this->body = pHandler->addSphere(this->pos, this->mass, this->radius);
		this->body->setGravity(pHandler->GlmVec3ToBulletVec3(g));
		this->bodyScale = this->body->getCollisionShape()->getLocalScaling();
	}

	void draw(Shader* shader, float angle, glm::vec3 rotationAxes) {
		
		// probably infer all these values from the physics model
		btVector3 currPos = this->body->getWorldTransform().getOrigin();
		this->model->draw(*shader, pHandler->BulletVec3ToGlmVec3(currPos), angle, rotationAxes, this->modelScale);
	}

	void shrink(float deltaTime) {
		bodyScale -= bodyScale * shrinkFactor * deltaTime;
		this->body->getCollisionShape()->setLocalScaling(bodyScale);
		this->pHandler->getWorld()->updateSingleAabb(this->body);

		this->modelScale -= glm::vec3(this->modelScale.x * shrinkFactor * deltaTime);
	}
};

#endif // !SNOWBALL_H
