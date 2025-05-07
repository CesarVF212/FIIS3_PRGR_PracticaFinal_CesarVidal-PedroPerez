#pragma once
#include "common.h"

class EventManager {

public:

	// Incializador del manejador.
	static void init(GLFWwindow* window);


	// --- RATÓN ---

	typedef struct {
		double posX, posY; // Coordenadas X e Y de la posición del mouse
		std::map<int, bool> buttons; // Mapa que almacena el estado de los botones del mouse(presionado o no)
	} mouseState_t;

	static inline mouseState_t mouseState = { 0, 0 };

	// Gestor de los botones del ratón.
	static void mouseButtonManager(GLFWwindow* window, int button, int action, int mods);

	// Gestor del movimiento del ratón.
	static void mousePosManager(GLFWwindow* window, double x, double y);

	
	// --- TECLADO ---

	inline static map <int, bool> keyState;

	// Gestor del input del teclado.
	static void keyEventManager(GLFWwindow* window, int key, int scancode, int action, int mods);

};