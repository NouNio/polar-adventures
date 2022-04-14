#pragma once
#ifndef KINEMATICPLAYER_H
#define KINEMATICPLAYER_H

#include <Model.h>
#include <Physics.h>
#include <Snowball.h>
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


enum worldSide {
	cFRONT,
	cBACK,
	cLEFT,
	cRIGHT,
	cTOP,
	cBOTTOM,
};


class KinematicPlayer
{
private:
	Camera* camera;
	btTransform transform;
	glm::vec3 pos;
	glm::vec3 cameraOffset;
	float moveSpeed = 100.0f;
	btScalar jumpForce = 20.0;
	btVector3 jumpDir = btVector3(0.0f, jumpForce, 0.0f);
	float fallSpeed = 40.0;
	float maxJumpHeight = 1.5f;
	const float radius = 0.5f, height = 2.0f, mass = 1.0f;
	const float stepHeight = 0.35;
	const glm::vec3 playerOffset = glm::vec3(0, -1.5, 0);
	const glm::vec3 playerScale = glm::vec3(0.2f);
	float playerRotAngle = 0.0f;
	glm::vec3 playerRotationAxes = glm::vec3(0.0f, 1.0f, 0.0f);
	unsigned int snowBallAmmu = 4;

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
		controller->setFallSpeed(fallSpeed);

		pHandler->getWorld()->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
		pHandler->getWorld()->addAction(controller);
	}


	void updateCameraPos()
	{	
		btVector3 newPos = this->ghostObject->getWorldTransform().getOrigin();
		cameraOffset = 5.0f * this->camera->front + glm::vec3(0, -1, 0);
		this->camera->setPosition(this->pHandler->BulletVec3ToGlmVec3(newPos) - cameraOffset);
	}


	void updateGravity() {
		// check which side of the cube the player is on, hard coded and set gravity and jump dir accordingly
		glm::vec3 currPos = this->getPos();
		// check if on LEFT
		if (currPos.y < 56 && currPos.y > 26
			&& currPos.z < 38 && currPos.z > 8
			&& currPos.x < -54) {
			std::cout << currPos.y << endl;
			controller->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(9.81, 0.0, 0.0)));
			jumpDir = btVector3(-jumpForce, 0.0f, 0.0f);
		}
		// check if on RIGHT
		else if (currPos.y < 56 && currPos.y > 26
			&& currPos.z < 38 && currPos.z > 8
			&& currPos.x > -24) {
			std::cout << currPos.y << endl;
			controller->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(-9.81, 0.0, 0.0)));
			jumpDir = btVector3(jumpForce, 0.0f, 0.0f);
		}
		// check if on FRONT
		else if (currPos.x > -54 && currPos.x < -24
			&& currPos.y < 56 && currPos.y > 26
			&& currPos.z > 34) {
			std::cout << currPos.y << endl;
			controller->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(0.0, 0.0, -9.81)));
			jumpDir = btVector3(0.0f, 0.0f, jumpForce);
		}
		// check if on BACK
		else if (currPos.x > -54 && currPos.x < -24
			&& currPos.y < 56 && currPos.y > 26
			&& currPos.z < 4) {
			std::cout << currPos.y << endl;
			controller->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(0.0, 0.0, 9.81)));
			jumpDir = btVector3(0.0f, 0.0f, -jumpForce);
		}
		// check if on TOP
		else if (currPos.y > 52) {
			controller->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(0.0, -9.81, 0.0)));
			jumpDir = btVector3(0.0f, jumpForce, 0.0f);
		}
		// check if on BOTTOM
		else if (currPos.y < 26) {
			controller->setGravity(pHandler->GlmVec3ToBulletVec3(glm::vec3(0.0, 9.81, 0.0)));
			jumpDir = btVector3(0.0f, -jumpForce, 0.0f);
		}
	}


public:
	Physics* pHandler;
	Model* player;
	btPairCachingGhostObject* ghostObject;  // according to bullet docs, good for player controller, makes use of AABB
	btKinematicCharacterController* controller;


	KinematicPlayer(Physics* pHandler, glm::vec3 position, Camera* camera, Model* model)
	{
		this->camera = camera;
		this->pHandler = pHandler;
		this->pos = position;
		this->player = model;
		this->activatePlayer();
	}


	~KinematicPlayer()
	{
		// the ghost object is delete in the physics handlers deleteAll(), which also deletes the corresponding collision shape
		// the new btGhostPairCallback() gets delete in the p Handler as well, with delete broadPhase in deleteAll()
		delete this->controller;
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

	
		float velocity = this->moveSpeed * deltaTime;
		glm::vec3 walkDir(0,0,0);
		switch(direction)
		{
			case pFORWARD:
				//controller->applyImpulse( this->pHandler->GlmVec3ToBulletVec3(this->camera->front).normalize() * velocity);
				walkDir += this->camera->front * velocity;
				break;
			case pBACKWARD:
				//controller->applyImpulse(this->pHandler->GlmVec3ToBulletVec3(-this->camera->front).normalize() * velocity);
				walkDir -= this->camera->front * velocity;
				break;
			case pLEFT:
				//controller->applyImpulse(this->pHandler->GlmVec3ToBulletVec3(-this->camera->right).normalize() * velocity);
				walkDir -= this->camera->right * velocity;
				break;
			case pRIGHT:
				//controller->applyImpulse(this->pHandler->GlmVec3ToBulletVec3(this->camera->right).normalize() * velocity);
				walkDir += this->camera->right * velocity;
				break;
			case pNONE:
				break;
		}

		if (walkDir != glm::vec3(0, 0, 0)) {
			if(controller->onGround())
				controller->setWalkDirection(pHandler->GlmVec3ToBulletVec3(walkDir).normalized() / 5);
			else
				controller->setWalkDirection(pHandler->GlmVec3ToBulletVec3(walkDir).normalized() / 10);
		}
		else
			controller->setWalkDirection(btVector3(0, 0, 0));

		updateCameraPos();

		updateGravity();
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


	unsigned int getSnowBallAmmu() {
		return this->snowBallAmmu;
	}


	void shootSnowball() {
		if (this->snowBallAmmu > 0) {

			btRigidBody* newSnowball = pHandler->addSphere(getPos() + glm::vec3(0.0, 3.0, 0.0), 1.0, 1.0);
			newSnowball->setLinearVelocity( pHandler->GlmVec3ToBulletVec3(this->camera->front * 20.0f) );

			this->snowBallAmmu--;
		}
	}
};

#endif // !KINEMATICPLAYER_H
