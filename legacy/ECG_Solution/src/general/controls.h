#pragma once
#include <vector>
#include <memory>



static bool fullscreen = false;
static bool _wireframe = false;
static bool _culling = false;
static bool E_W = false;
static bool E_A = false;
static bool E_S = false;
static bool E_D = false;
static bool E_LS = false;
static bool E_RS = false;
static bool E_SP = false;
static bool E_DW = false;
static bool E_UP = false;
static bool E_LF = false;
static bool E_RT = false;
static bool debugCamera = false;

glm::vec3 positionChange;
float u_d = 0;
float l_r = 0;
void updateMovement(float changeVar)
{
	if (E_W)
		positionChange.z += changeVar;
	if (E_A)
		positionChange.x += changeVar;
	if (E_S)
		positionChange.z -= changeVar;
	if (E_D)
		positionChange.x -= changeVar;
	if (E_LS)
		positionChange.y -= changeVar;
	if (E_RS)
		positionChange.y += changeVar;
	if (E_SP)
		positionChange.y += changeVar;
	if (E_DW)
		u_d -= changeVar;
	if (E_UP)
		u_d += changeVar;
	if (E_LF)
		l_r -= changeVar;
	if (E_RT)
		l_r += changeVar;
}
static void toggleFullscreen() {
	fullscreen = !fullscreen;
	if (fullscreen && monitor) {
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// F1 - Wireframe
	// F2 - Culling
	// Esc - Exit
	{
		if (GLFW_KEY_W == key) {
			if (action == GLFW_PRESS)
				E_W = true;
			if (action == GLFW_RELEASE)
				E_W = false;
		}
		if (GLFW_KEY_A == key) {
			if (action == GLFW_PRESS)
				E_A = true;
			if (action == GLFW_RELEASE)
				E_A = false;
		}
		if (GLFW_KEY_S == key) {
			if (action == GLFW_PRESS)
				E_S = true;
			if (action == GLFW_RELEASE)
				E_S = false;
		}
		if (GLFW_KEY_D == key) {
			if (action == GLFW_PRESS)
				E_D = true;
			if (action == GLFW_RELEASE)
				E_D = false;
		}
		if (GLFW_KEY_LEFT_SHIFT == key) {
			if (action == GLFW_PRESS)
				E_LS = true;
			if (action == GLFW_RELEASE)
				E_LS = false;
		}
		if (GLFW_KEY_RIGHT_SHIFT == key) {
			if (action == GLFW_PRESS)
				E_RS = true;
			if (action == GLFW_RELEASE)
				E_RS = false;
		}
		if (GLFW_KEY_SPACE == key) {
			if (action == GLFW_PRESS)
				E_SP = true;
			if (action == GLFW_RELEASE)
				E_SP = false;
		}


		//steering of cuboid-mirror
		if (GLFW_KEY_DOWN == key) {
			if (action == GLFW_PRESS)
				E_DW = true;
			if (action == GLFW_RELEASE)
				E_DW = false;
		}

		if (GLFW_KEY_UP == key) {
			if (action == GLFW_PRESS)
				E_UP = true;
			if (action == GLFW_RELEASE)
				E_UP = false;
		}
		if (key == GLFW_KEY_LEFT) {
			if (action == GLFW_PRESS)
				E_LF = true;
			if (action == GLFW_RELEASE)
				E_LF = false;
		}

		if (GLFW_KEY_RIGHT == key) {
			if (action == GLFW_PRESS)
				E_RT = true;
			if (action == GLFW_RELEASE)
				E_RT = false;
		}
	}

	if (action != GLFW_RELEASE)return;//if key isn't released return

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;

	case GLFW_KEY_F1:
		_wireframe = !_wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, _wireframe ? GL_LINE : GL_FILL);
		break;
	case GLFW_KEY_F2:
		_culling = !_culling;
		if (_culling) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		break;
	case GLFW_KEY_F:
		toggleFullscreen();
		break;
	case GLFW_KEY_F11:
		toggleFullscreen();
		break;
	case GLFW_KEY_TAB:
		debugCamera = !debugCamera;
		break;


	}

}