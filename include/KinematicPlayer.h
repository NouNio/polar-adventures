#pragma once
#ifndef KINEMATICPLAYER_H
#define KINEMATICPLAYER_H

#include <Model.h>
#include <Physics.h>
#include <Constants.h>

#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <glm/gtx/norm.hpp>


/* ------------------------------------------------------------------------------------ */
// the player controller part of this file is inspired and following 4 different srcs
// 1 the official bullet character demo: https://github.com/kripken/bullet/blob/master/Demos/CharacterDemo/CharacterDemo.cpp
// 2 an well wirtten character controller: https://github.com/lokesh-sharma/GameEngine/blob/master/src/Player.cpp
// 3 a general structure was inspired by [8]
// 4 it also uses ideas from here https://github.com/222464/EvolvedVirtualCreaturesRepo/blob/master/VirtualCreatures/Volumetric_SDL/Source/SceneObjects/Physics/DynamicCharacterController.h
/* ------------------------------------------------------------------------------------ */


enum Movement {
	pFORWARD,
	pBACKWARD,
	pLEFT,
	pRIGHT,
	pUP,
	pNONE,
};


class KinematicPlayer
{
private:
	Camera* camera;
	glm::vec3 pos;
	btTransform transform;
	btKinematicCharacterController* controller;
	glm::vec3 cameraOffset;
	float velocity = 10.0f;
	const btVector3 jumpDir = btVector3(0.0f, 6.0f, 0.0f);
	float maxJumpHeight = 1.5f;
	const float radius = 0.5f, height = 2.0f, mass = 1.0f;
	const float stepHeight = 0.35;
	const glm::vec3 playerOffset = glm::vec3(0, -1.5, 0);
	const glm::vec3 playerScale = glm::vec3(0.2f);
	float playerRotAngle = 0.0f;
	glm::vec3 playerRotationAxes = glm::vec3(0.0f, 1.0f, 0.0f);

	void activatePlayer()
	{
		transform.setIdentity();
		transform.setOrigin(pHandler->GlmVec3ToBulletVec3(pos));
		transform.setRotation( btQuaternion(btVector3(1, 0, 0), 80.0f) );

		ghostObject = new btPairCachingGhostObject();
		ghostObject->setWorldTransform(transform);
		pHandler->getWorld()->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());  // set the callback for the ghost object
		btConvexShape* capsule = new btCapsuleShape(this->radius, this->height);
		ghostObject->setCollisionShape(capsule);
		ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

		controller = new btKinematicCharacterController(ghostObject, capsule, stepHeight);
		controller->setGravity( pHandler->GlmVec3ToBulletVec3(pHandler->getGravity()) );
		controller->setMaxJumpHeight(maxJumpHeight);
		controller->setFallSpeed(1.0);

		pHandler->getWorld()->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
		pHandler->getWorld()->addAction(controller);
	}


	void updateCameraPos()
	{	
		btVector3 newPos = this->ghostObject->getWorldTransform().getOrigin();
		cameraOffset = 5.0f * this->camera->front + glm::vec3(0, -1, 0);
		this->camera->setPosition(this->pHandler->BulletVec3ToGlmVec3(newPos) - cameraOffset);
	}


public:
	Physics* pHandler;
	Model* player;
	btRigidBody* body;
	btPairCachingGhostObject* ghostObject;  // according to bullet docs, good for player controller, makes use of AABB


	KinematicPlayer(Physics* pHandler, glm::vec3 position, Camera* camera, Model* model)
	{
		this->camera = camera;
		this->pHandler = pHandler;
		this->pos = position;
		this->player = model;
		this->activatePlayer();
	}


	void update(Movement direction, float deltaTime) 
	{
		btTransform t = controller->getGhostObject()->getWorldTransform();
		// position
		btVector3 pos = t.getOrigin();
		
		// rotation
		btQuaternion quat = t.getRotation();
		float angle = quat.getAngle();
		btVector3 axis = quat.getAxis();

		glm::vec3 dir(0, 0, 0);

		if (direction == pUP)
			if (controller->canJump())
				controller->jump(jumpDir);
		
		if (true)  //controller->onGround
		{
			switch(direction)
			{
				case pFORWARD:
					controller->applyImpulse( this->pHandler->GlmVec3ToBulletVec3(this->camera->front).normalize() * velocity);
					break;
				case pBACKWARD:
					controller->applyImpulse(this->pHandler->GlmVec3ToBulletVec3(-this->camera->front).normalize() * velocity);
					break;
				case pLEFT:
					controller->applyImpulse(this->pHandler->GlmVec3ToBulletVec3(-this->camera->right).normalize() * velocity);
					break;
				case pRIGHT:
					controller->applyImpulse(this->pHandler->GlmVec3ToBulletVec3(this->camera->right).normalize() * velocity);
					break;
				case pNONE:
					break;
			}
		}

		updateCameraPos();
	}


	void drawPlayer(Shader* shader)
	{
		playerRotAngle = -(this->camera->yaw);  // rotate  player with the camera

		glm::mat4 projection = glm::perspective(glm::radians(this->camera->zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader->setMat4("projection", projection);

		glm::mat4 view = this->camera->GetViewMatrix();
		shader->setMat4("view", view);

		shader->use();
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, getPos() + playerOffset);
		model = glm::rotate(model, glm::radians(playerRotAngle + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, playerScale);
		shader->setMat4("model", model);

		player->draw(*shader);
	}


	glm::vec3 getPos()
	{
		return pHandler->BulletVec3ToGlmVec3(ghostObject->getWorldTransform().getOrigin());
	}
};

#endif // !KINEMATICPLAYER_H
