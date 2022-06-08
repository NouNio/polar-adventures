#pragma once
#ifndef KINEMATICPLAYER_H
#define KINEMATICPLAYER_H

#include <Constants.h>
#include <FileManager.h>
#include <GravityModel.h>
#include <Model.h>
#include <Physics.h>
#include <Snowball.h>

#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletDynamics/Character/btKinematicCharacterController.h>
#include <glm/gtx/norm.hpp>
#include <irrklang/irrKlang.h>


/* ------------------------------------------------------------------------------------ */
// the player controller part of this file is inspired and following 4 different srcs
// 1 the official bullet character demo: https://github.com/kripken/bullet/blob/master/Demos/CharacterDemo/CharacterDemo.cpp
// 2 a well written character controller: https://github.com/lokesh-sharma/GameEngine/blob/master/src/Player.cpp
// 3 a general structure was inspired by [8]
// 4 it also uses ideas from here https://github.com/222464/EvolvedVirtualCreaturesRepo/blob/master/VirtualCreatures/Volumetric_SDL/Source/SceneObjects/Physics/DynamicCharacterController.h
/* ------------------------------------------------------------------------------------ */

extern std::map<unsigned int, Snowball*> snowballs;
extern std::vector<Snowball*> collectedSnowballs;
extern irrklang::ISoundEngine* soundEngine;
extern FileManager* fm;
extern CubeGravtiy cubeGravity;


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
	glm::vec3 playerOffset = glm::vec3(0, -1.5, 0);  // start value, where camera wolrdUp = (0,1,0) --> playerOffset = -(worldUp * 1.5)
	const glm::vec3 playerScale = glm::vec3(50.0f);
	float playerCamRot = 0.0f;
	glm::vec3 playerRotationAxes = glm::vec3(0.0f, 1.0f, 0.0f);
	unsigned int snowBallAmmo = 0;
	
	bool walking = false;

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
		ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);
		ghostObject->setUserPointer(&playerPtr);

		controller = new btKinematicCharacterController(ghostObject, capsule, stepHeight);
		controller->setGravity( pHandler->GlmVec3ToBulletVec3(pHandler->getGravity()) );
		controller->setMaxJumpHeight(maxJumpHeight);
		controller->setFallSpeed(fallSpeed);

		pHandler->getWorld()->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::AllFilter);
		pHandler->getWorld()->addAction(controller);
	}


	glm::vec3 getPlayerFront(glm::vec3 camFront) {
		glm::vec3 shootingPos = camFront;
		switch (this->cubeSide) {
		case CUBE_LEFT:
		case CUBE_RIGHT:
			shootingPos.x = 0.0f;
			break;
		case CUBE_FRONT:
		case CUBE_BACK:
			shootingPos.z = 0.0f;
			break;
		case CUBE_TOP:
		case CUBE_BOTTOM:
			shootingPos.y = 0.0f;
			break;
		}

		return shootingPos;
	}


	void updateCameraPos()
	{	
		btVector3 newPos = this->ghostObject->getWorldTransform().getOrigin();
		cameraOffset = 5.0f * this->camera->front + (-this->camera->worldUp); //glm::vec3(0, -1, 0);
		this->camera->setPosition(this->pHandler->BulletVec3ToGlmVec3(newPos) - cameraOffset);
	}


	void updateCameraViewPoint() {
		this->camera->setWorldUp(-glm::normalize(pHandler->BulletVec3ToGlmVec3(controller->getGravity())));
		this->camera->setCubeSide(this->cubeSide);
	}


	void updateGravity() {
		glm::vec3 newG = cubeGravity.getGradient(this->getPos());
		controller->setGravity(pHandler->GlmVec3ToBulletVec3(newG));

		updateCubeSide(newG);  // newG can be e.g. (0.0, -7, 2.81) then this should still be considered as being on the top side
		updateCameraPos();
		updateJumpDir();
	}


	void updateCubeSide(glm::vec3 newG) {
		glm::vec3 absG = glm::abs(newG);

		// search for the index of the max val
		int maxIdx = -1;
		float maxVal = 0;

		for (int i = 0; i < 3; i++) {
			if (absG[i] > maxVal) {
				maxIdx = i;
				maxVal = absG[i];
			}
		}

		// check LEFT or RIGHT
		if (maxIdx == 0) {
			if (newG[maxIdx] < 0)
				cubeSide = CUBE_RIGHT;
			if (newG[maxIdx] > 0)
				cubeSide = CUBE_LEFT;
		}
		// check TOP or BOTTOM
		else if (maxIdx == 1) {
			if (newG[maxIdx] < 0)
				cubeSide = CUBE_TOP;
			if (newG[maxIdx] > 0)
				cubeSide = CUBE_BOTTOM;
		}
		// cehck FRONT or BACK
		else if (maxIdx == 2) {
			if (newG[maxIdx] < 0)
				cubeSide = CUBE_FRONT;
			if (newG[maxIdx] > 0)
				cubeSide = CUBE_BACK;
		}
	}


	void updateJumpDir() {
		switch (cubeSide) {
		case CUBE_LEFT:
			jumpDir = btVector3(-jumpForce, 0.0f, 0.0f);
			break;
		case CUBE_RIGHT:
			jumpDir = btVector3(jumpForce, 0.0f, 0.0f);
			break;
		case CUBE_FRONT:
			jumpDir = btVector3(0.0f, 0.0f, jumpForce);
			break;
		case CUBE_BACK:
			jumpDir = btVector3(0.0f, 0.0f, -jumpForce);
			break;
		case CUBE_TOP:
			jumpDir = btVector3(0.0f, jumpForce, 0.0f);
			break;
		case CUBE_BOTTOM:
			jumpDir = btVector3(0.0f, -jumpForce, 0.0f);
			break;
		}
	}


	void updateMovement(Movement direction, float deltaTime) {
		if (direction == pUP)
			if (controller->canJump()) {
				if (sound)
					soundEngine->play2D(fm->getAudioPath("jump").c_str(), false);
				controller->jump(jumpDir);
			}


		float velocity = this->moveSpeed * deltaTime;
		glm::vec3 walkDir(0, 0, 0);
		glm::vec3 playerFrontDir = getPlayerFront(camera->front);
		switch (direction)
		{
		case pFORWARD:
			//walkDir += this->camera->front * velocity;
			walkDir += playerFrontDir * velocity;
			break;
		case pBACKWARD:
			walkDir -= playerFrontDir * velocity;
			break;
		case pLEFT:
			walkDir -= this->camera->right * velocity;
			break;
		case pRIGHT:
			walkDir += this->camera->right * velocity;
			break;
		case pNONE:
			break;
		}

		if (walkDir != glm::vec3(0, 0, 0)) {
			if (controller->onGround()) {
				controller->setWalkDirection(pHandler->GlmVec3ToBulletVec3(walkDir).normalized() / 5);
			}
			else
				controller->setWalkDirection(pHandler->GlmVec3ToBulletVec3(walkDir).normalized() / 10);
		}
		else
			controller->setWalkDirection(btVector3(0, 0, 0));
	}


	void updatePlayerOffset() {
		this->playerOffset = -(this->camera->worldUp * 1.5f);
	}


	glm::mat4 updatePlayerRotation(glm::mat4 model) {
		// rotate the player such that the model face up and front given the current cube side --> then rotate with camera yaw
		playerCamRot = -(this->camera->yaw);  // rotate  player with the camera left and right
		switch (cubeSide) {
		case CUBE_LEFT:
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 0, 1));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
			model = glm::rotate(model, glm::radians(playerCamRot + 90.0f), glm::vec3(0, 1, 0));  // rotate  player with the camera left and right
			break;
		case CUBE_RIGHT:
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0, 0, 1));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
			model = glm::rotate(model, glm::radians(playerCamRot - 90.0f), glm::vec3(0, 1, 0));  // rotate  player with the camera left and right
			break;
		case CUBE_FRONT:
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1, 0, 0));
			model = glm::rotate(model, glm::radians(playerCamRot + 90.0f), glm::vec3(0, 1, 0));  // rotate  player with the camera left and right
			break;
		case CUBE_BACK:
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
			model = glm::rotate(model, glm::radians(playerCamRot - 90.0f), glm::vec3(0, 1, 0));  // rotate  player with the camera left and right
			break;
		case CUBE_TOP:
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
			model = glm::rotate(model, glm::radians(playerCamRot - 90.0f), glm::vec3(0, 1, 0));  // rotate  player with the camera left and right
			break;
		case CUBE_BOTTOM:
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 0, 1));
			model = glm::rotate(model, glm::radians(playerCamRot), glm::vec3(0, 1, 0));  // rotate  player with the camera left and right
			break;
		}

		return model;
	}


	void updateSnowballAmmo() {
		this->snowBallAmmo = collectedSnowballs.size();
	}


public:
	unsigned int cubeSide = CUBE_TOP;
	Physics* pHandler;
	Model* player;
	Model* idleModel;
	btPairCachingGhostObject* ghostObject;  // according to bullet docs, good for player controller, makes use of AABB
	btKinematicCharacterController* controller;

	glm::vec3 getSnowBallShootingVec(glm::vec3 camFront) {
		glm::vec3 shootingPos = camFront;
		switch (this->cubeSide) {
		case CUBE_LEFT:
		case CUBE_RIGHT:
			shootingPos.x = 0.0f;
			break;
		case CUBE_FRONT:
		case CUBE_BACK:
			shootingPos.z = 0.0f;
			break;
		case CUBE_TOP:
		case CUBE_BOTTOM:
			shootingPos.y = 0.0f;
			break;
		}

		return shootingPos;
	}
	KinematicPlayer(Physics* pHandler, glm::vec3 position, Camera* camera, Model* model)
	{
		this->camera = camera;
		this->pHandler = pHandler;
		this->pos = position;
		this->player = model;
		this->idleModel = idleModel;
		this->activatePlayer();
	}


	~KinematicPlayer()
	{
		// the ghost object is delete in the physics handlers deleteAll(), which also deletes the corresponding collision shape
		// the new btGhostPairCallback() gets delete in the p Handler as well, with delete broadPhase in deleteAll()
		delete this->controller;
	}


	bool onGround() {
		return this->controller->onGround();
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

		updateMovement(direction, deltaTime);

		updateCameraPos();

		updatePlayerOffset();

		updateGravity();

		updateCameraViewPoint(); 

		updateSnowballAmmo();
	}


	void draw(Shader* shader, float rescale)
	{
		shader->setMat4("projection", glm::perspective(glm::radians(this->camera->zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f));
		shader->setMat4("view", this->camera->GetViewMatrix());
		
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, getPos() + playerOffset);
		
		model = updatePlayerRotation(model);

		model = glm::scale(model, playerScale * rescale);
		shader->setMat4("model", model);

		player->draw(*shader);
	}


	glm::vec3 getPos()
	{
		return pHandler->BulletVec3ToGlmVec3(ghostObject->getWorldTransform().getOrigin());
	}


	bool isWalking() const {
		return this->walking;
	}


	unsigned int getSnowBallAmmo()  {
		return this->snowBallAmmo;
	}


	void setWalking(bool newWalking) {
		this->walking = newWalking;
	}


	void shootSnowball() {
		if (this->snowBallAmmo > 0) {

			Snowball* snowball = collectedSnowballs[collectedSnowballs.size()-1];							// retrieve the last collected snow ball
			collectedSnowballs.pop_back();																	// and remove it from the list of collected snowballs

			glm::vec3 shootingPos = getPos() + 3.0f * getPlayerFront(camera->front); // getPos() + 3*camera->front

			btRigidBody* newSphere = pHandler->addSphere(shootingPos, 1.0, 1.0);	// create a new sphere object (since apparently this is more desirable, then translating an existing object
			snowballs.insert(std::pair<unsigned int, Snowball*>(snowball->getID(), snowball));				// insert it into the list of snowballs in the world

			
			
			// TODO the below 5 lines can be refactored to Snowball.h
			snowball->setBody(newSphere);																	// set the snowballs rigidBody to this new sphere
			pHandler->activateColCallBack(newSphere);														// activate collision callback for the sphere
			newSphere->setUserPointer(snowball->getIDptr());												// update the userPtr for collision call back, as this is a new sphere
			newSphere->getCollisionShape()->setLocalScaling(snowball->getBodyScale());						// also update the amount the snow ball --
			pHandler->getWorld()->updateSingleAabb(newSphere);												// --> has already shrunk
			
			newSphere->setGravity(controller->getGravity());												// set gravity to match with the player one
			newSphere->setLinearVelocity( pHandler->GlmVec3ToBulletVec3(this->camera->front * 10.0f) );		// and then set some shooting velocity

			this->snowBallAmmo--;																			// reduce ammo by one
		}
	}
};

#endif !KINEMATICPLAYER_H
