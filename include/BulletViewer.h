#pragma once
#ifndef BULLETVIEWER_H
#define BULLETVIEWER_H
//// Helper class; draws the world as seen by Bullet.
//// This is very handy to see it Bullet's world matches yours.
//// This example uses the old OpenGL API for simplicity, 
//// so you'll have to remplace GLFW_OPENGL_CORE_PROFILE by
//// GLFW_OPENGL_COMPAT_PROFILE in glfwWindowHint()
//// How to use this class :
//// Declare an instance of the class :
//// BulletDebugDrawer_DeprecatedOpenGL mydebugdrawer;
//// dynamicsWorld->setDebugDrawer(&mydebugdrawer);
//// Each frame, call it :
//// mydebugdrawer.SetMatrices(ViewMatrix, ProjectionMatrix);
//// dynamicsWorld->debugDrawWorld();
//
// this is purely for debugging purposes and taken from the opengl-tutorial
// link: https://github.com/opengl-tutorials/ogl/blob/master/misc05_picking/misc05_picking_BulletPhysics.cpp


class BulletDebugDrawer_DeprecatedOpenGL : public btIDebugDraw{
public:
	void SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix){
		glUseProgram(0); // Use Fixed-function pipeline (no shaders)
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&pViewMatrix[0][0]);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&pProjectionMatrix[0][0]);
	}
	virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color){
		glColor3f(color.x(), color.y(), color.z());
		glBegin(GL_LINES);
			glVertex3f(from.x(), from.y(), from.z());
			glVertex3f(to.x(), to.y(), to.z());
		glEnd();
	}
	virtual void drawContactPoint(const btVector3 &,const btVector3 &,btScalar,int,const btVector3 &){}
	virtual void reportErrorWarning(const char *){}
	virtual void draw3dText(const btVector3 &,const char *){}
	virtual void setDebugMode(int p){
		m = p;
	}
	int getDebugMode(void) const {return 3;}
	int m;
};

#endif