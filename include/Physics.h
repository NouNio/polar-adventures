#pragma once
#ifndef PHYISCS_H
#define PHYSICS_H

#include <vector>
#include <iostream>

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <glm/glm.hpp>
#include <BulletViewer.h>


bool collisionCallback(btManifoldPoint& collisionPoint, const btCollisionObjectWrapper* obj0, int id0, int idx0, const btCollisionObjectWrapper* obj1, int id1, int idx1)
{
    //std::cout << "collision" << std::endl;
    return false;
}


class Physics
{
public:
    Physics(bool debug = false)
    {
        this->initBullet(debug);
    }


    void deleteAll()
    {
        for (unsigned int i = 0; i < this->rigidBodies.size(); i++)
        {
            this->world->removeCollisionObject(this->rigidBodies[i]);
            btMotionState* motionState = this->rigidBodies[i]->getMotionState();
            btCollisionShape* shape = this->rigidBodies[i]->getCollisionShape();

            delete this->rigidBodies[i];
            delete motionState;
            delete shape;
        }
        delete this->dispatcher;
        delete this->collisionConfig;
        delete this->solver;
        delete this->broadphase;
        delete this->world;
    }


    void stepSim(float deltaTime)
    {
        this->world->stepSimulation(deltaTime);
    }


    glm::vec3 getGravity()
    {
        return BulletVec3ToGlmVec3(this->world->getGravity());
    }


    void setGravity(glm::vec3 gravity)
    {
        world->setGravity(this->GlmVec3ToBulletVec3(gravity));
    }


    size_t getNumBodies()
    {
        return this->rigidBodies.size();
    }


    btDynamicsWorld* getWorld()
    {
        return this->world;
    }


    void activateColCallBack(btRigidBody* body)
    {
        body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);  // or-operator on the flags, to keep all flags that are already there and just add the new one
    }


    // the below two functions are taken from / inspired by [7] p. 570
    inline btVector3 GlmVec3ToBulletVec3(const glm::vec3& v)
    {
        return { v.x, v.y, v.z };
    }


    inline glm::vec3 BulletVec3ToGlmVec3(const btVector3& v)
    {
        return { v.x(), v.y(), v.z() };
    }


    void setDebugMatrices(glm::mat4 view, glm::mat4 projection)
    {
        this->debugDrawer.SetMatrices(view, projection);
    }


    void debugDraw()
    {
        this->world->debugDrawWorld();
    }


    btRigidBody* addBox(glm::vec3 position, float mass, glm::vec3 size)
    {
        btBoxShape* box = new btBoxShape(GlmVec3ToBulletVec3(size));  // collision shape  |  make this line generic for alle collision type, i.e. also change func params
        
        return this->setUpBody(position, mass, box);
    }


    btRigidBody* addSphere(glm::vec3 position, float mass, float radius)
    {
        btSphereShape* sphere = new btSphereShape(radius);

        return this->setUpBody(position, mass, sphere);
    }


    btRigidBody* addCapsule(glm::vec3 position, float mass, float radius, float height)
    {
        btCapsuleShape* capsule = new btCapsuleShape(radius, height);

        return this->setUpBody(position, mass, capsule);
    }


    btRigidBody* addMeshShape(Model* model, glm::vec3 position, float mass)
    {

        btTriangleMesh* triMesh = new btTriangleMesh();           // a trimesh defines a object in bullet via a collection of triangles

        for (unsigned int i = 0; i < model->getNumMeshes(); i++)  // that measn here we need to extract the triangles in vertice pairs of three from the loaded model data
        {
            Mesh currMesh = model->meshes[i];

            for (unsigned j{ 0 }; j < currMesh.getNumVertices(); j += 3)
            {
                btVector3 v1 = GlmVec3ToBulletVec3( currMesh.vertices[j].pos );
                btVector3 v2 = GlmVec3ToBulletVec3( currMesh.vertices[j+1].pos );
                btVector3 v3 = GlmVec3ToBulletVec3( currMesh.vertices[j+2].pos );

                triMesh->addTriangle(v1, v2, v3);
            }
        }

        btBvhTriangleMeshShape* mesh = new btBvhTriangleMeshShape(triMesh, true);  // second argument asks for usage of quantized bvh tree... why not

        return this->setUpBody(position, mass, mesh);
    }


    btRigidBody* addScaledMeshShape(btBvhTriangleMeshShape* mesh, glm::vec3 position, float mass, glm::vec3 scale)
    {
        btScaledBvhTriangleMeshShape* scaledMesh = new btScaledBvhTriangleMeshShape(mesh, GlmVec3ToBulletVec3(scale));

        return this->setUpBody(position, mass, scaledMesh);
    }


private:
    btDynamicsWorld* world;
    btDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btConstraintSolver* solver;
    btCollisionConfiguration* collisionConfig;
    std::vector<btRigidBody*> rigidBodies;           //storage of all current rigidBodies for easy access
    BulletDebugDrawer_DeprecatedOpenGL debugDrawer;  // debug viewer for bullet
    glm::vec3 gravity = glm::vec3(0, -9.81, 0);

    void initBullet(bool debug = false)
    {
        this->collisionConfig = new btDefaultCollisionConfiguration();
        this->dispatcher = new btCollisionDispatcher(collisionConfig);
        this->broadphase = new btDbvtBroadphase();
        this->solver = new btSequentialImpulseConstraintSolver();

        this->world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
        this->setGravity(gravity);
        gContactAddedCallback = collisionCallback;

        if (debug)
            this->world->setDebugDrawer(&this->debugDrawer);
    }


    // this calls a methods we repeatedly need for creating rigid bodies for our world;  Note colShape is 99% a specific shape like capsule, should still work though
    btRigidBody* setUpBody(glm::vec3 position, float mass, btCollisionShape* colShape)
    {
        btTransform t;    // quaternion, i.e. position and rotation
        t.setIdentity();  // set pos to (0,0,0) and rotation to (0,0,0,1)
        t.setOrigin(GlmVec3ToBulletVec3(position));

        btVector3 inertia(0, 0, 0);
        if (mass != 0.0)
            colShape->calculateLocalInertia(mass, inertia);

        btMotionState* motionState = new btDefaultMotionState(t);
        btRigidBody::btRigidBodyConstructionInfo info(
            mass,                    // mass:  static for 0 and dynamic else
            motionState,
            colShape,                     // collision shape
            inertia
        );

        btRigidBody* body = new btRigidBody(info);
        this->world->addRigidBody(body);
        this->rigidBodies.push_back(body);

        return body;
    }
};

#endif
