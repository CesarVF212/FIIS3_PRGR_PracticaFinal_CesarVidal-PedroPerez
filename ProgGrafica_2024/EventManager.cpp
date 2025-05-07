#include "libprgr/EventManager.h"

void EventManager::init(GLFWwindow* window)
{
	glfwSetKeyCallback(window, keyEventManager);
	glfwSetCursorPosCallback(window, mousePosManager);
	glfwSetMouseButtonCallback(window, mouseButtonManager);
}

// --- RATÓN ---

void EventManager::mouseButtonManager(GLFWwindow* window, int button, int action, int mods)
{
	switch (action) {
	case GLFW_PRESS: {
		mouseState.buttons[button] = true;
	} break;

	case GLFW_RELEASE: {
		mouseState.buttons[button] = false;
	} break;
	}
}


void EventManager::mousePosManager(GLFWwindow* window, double x, double y)
{
	mouseState.posX = x;
	mouseState.posY = y;
}


// --- TECLADO ---

void EventManager::keyEventManager(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	switch (action) {

		case GLFW_PRESS:
			keyState[key] = true;
			break;

		case GLFW_RELEASE:
			keyState[key] = false;
			break;
	
	}
}