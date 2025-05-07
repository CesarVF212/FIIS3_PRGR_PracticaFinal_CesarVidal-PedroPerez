#pragma once

#include "vertex.h"
#include "common.h"
#include "Program.h"
#include "Texture.h"
#include "Collider.h"

typedef struct {
	unsigned int idArray; // Identificador de array.
	unsigned int idVertexArray; // Identificador de vertices.
	unsigned int idIndexArray; // Identificador de indices de vertices.
}bufferObject;

using namespace libPRGR;

class Object3D
{

#pragma region --- ATRIBUTOS ---

private:

	static int idCounter; // Conteo de objetos.

public:

	int id = 0; // Identificador propio.

	// COLISIONES
	typedef enum {
		COLLIDER_SPHERE,  // Colisionador de tipo esfera
		COLLIDER_AABB     // Colisionador de tipo AABB (caja alineada a ejes)
	} ColliderType;

	ColliderType colliderType = COLLIDER_SPHERE;
	Collider* collider = nullptr;

	// MATERIAL

	typedef struct {

		float Ks, Kd;  // Reflexion especular y difusa
		float shine; // Brillo
		Texture* texture; // Textura de un Objeto.

	}material_t; // Estructura de material del objeto;

	material_t material;


	// POSICIÓN, ESCALA Y ROTACIÓN 

	vector4f position;
	vector4f scale;
	vector4f rotation;
	matrix4x4f modelMatrix;


	// VÉRTICES 

	vector<vertex_t> vertexList; // lista de vertices del objeto
	vector<int> idList; // lista de indices de vertices 

	Program* program; // programa que se utilizara para dibujar el objeto

	// ROTACION ESTÁNDAR.
	bool standartRotation = true;

#pragma endregion

#pragma region --- METODOS ---

	// Constructor por defecto
	Object3D();

	// Constructor con posicion
	Object3D(vector4f pos);

	// Constructor que carga el objeto desde un archivo
	Object3D(string file);

	// Crea un triangulo
	void createTriangle();

	// Actualiza la matriz de modelo
	void updateModelMatrix();

	// Mueve el objeto
	virtual void move(double timeStep);

	// Carga un objeto desde un archivo
	void loadFromFile(string file);

	// Lee los vertices de un archivo
	void leerVertices(std::ifstream& f);

	// Lee los colores de un archivo
	void leerColores(std::ifstream& f);

	// Lee las caras de un archivo
	void leerCaras(std::ifstream& f);

	// Lee las texturas de un archivo
	void leerTexturas(std::ifstream& f);

	// Lee las normales de un archivo
	void leerNormales(std::ifstream& f);

	// Métodos para configurar el tipo de colisionador
	void setColliderType(ColliderType type) { colliderType = type; }

	ColliderType getColliderType() const { return colliderType; }

	// Métodos para el colisionador
	void createCollider(ColliderType type = COLLIDER_SPHERE);

	void updateCollider();
#pragma endregion

};