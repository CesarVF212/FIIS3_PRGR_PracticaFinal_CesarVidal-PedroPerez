#include "libprgr/Object3D.h"
#include "libprgr/EventManager.h"


int Object3D::idCounter = 0;

Object3D::Object3D() // Constructor por defecto, crea un objeto en la posicion (0,0,0) con escala (1,1,1) y rotacion (0,0,0). no tiene vertices
{
	id = idCounter++;
	this->position = { 0.0, 0.0, 0.0, 1.0 };
	this->scale = { 1.0, 1.0, 1.0, 1.0 };
	this->rotation = { 0.0, 0.0, 0.0, 1.0 };

	this->modelMatrix = make_identity();

	updateModelMatrix();
}


Object3D::Object3D(string file) {
	id = idCounter++;
	this->position = { 0,0,0,1 };
	this->scale = { 1,1,1,1 };
	loadFromFile(file);
}


Object3D::Object3D(vector4f pos) { // Constructor con posicion, crea un objeto en la posicion pos con escala (1,1,1) y rotacion (0,0,0). no tiene vertices
	id = idCounter++;
	this->position = pos;
	this->scale = { 1.0, 1.0, 1.0, 1.0 };
	this->rotation = { 0.0, 0.0, 0.0, 1.0 };

	this->modelMatrix = make_identity();

	updateModelMatrix();
}

// Crea un triangulo con vertices en (0,0.5,0), (-0.5,-0.5,0) y (0.5,-0.5,0), es decir, centrado en el origen
void Object3D::createTriangle() 
{
	id = idCounter++;

	this->position = { 0.0, 0.0, 0.0, 1.0 };
	this->scale = { 1.0, 1.0, 1.0, 1.0 };
	this->rotation = { 0.0, 0.0, 0.0, 1.0 };

	this->vertexList.push_back({ {  0.0,  0.5, 0.0, 1.0 },{1,0,0,1} }); // pushback es una funcion que agrega un elemento al final del vector
	this->vertexList.push_back({ { -0.5, -0.5, 0.0, 1.0 },{0,1,0,1} });
	this->vertexList.push_back({ {  0.5, -0.5, 0.0, 1.0 },{0,0,1,1} });

	this->idList = { 0, 1, 2 }; // lista de indices de vertices, orden en que se dibujan

	modelMatrix = make_identity();
}

void Object3D::updateModelMatrix()
{
	matrix4x4f scaleMatrix = make_scale(scale.x, scale.y, scale.z); // No se pedia, pero no cuesta nada
	matrix4x4f rotationMatrix = make_rotate(rotation.x, rotation.y, rotation.z);  // matriz de rotacion
	matrix4x4f translationMatrix = make_translate(position.x, position.y, position.z); // matriz de traslacion
	modelMatrix = translationMatrix * rotationMatrix * scaleMatrix; // matriz de modelo que se consigue multiplicando las 3 matrices anteriores
}

void Object3D::move(double timeStep) 
{
	// En el caso de estar activado, le metemos una rotación por defecto.
	/*if (this->standartRotation)
	{
		this->rotation.y += 15.0f * timeStep;
	}*/

	updateModelMatrix();
}

void Object3D::loadFromFile(string file)
{
	ifstream f(file);
	if (f.is_open())
	{
		
		leerVertices(f);
		leerColores(f);
		leerNormales(f);
		leerTexturas(f);
		leerCaras(f);
		// Crear el colisionador (usará COLLIDER_SPHERE por defecto)
		createCollider();
		// createCollider(COLLIDER_AABB);  // Fuerza el tipo AABB

		// Actualizar el colisionador con la matriz modelo inicial
		updateCollider();
		program = new Program();
		program->addShader("data/shader.frag");
		program->addShader("data/shader.vert");
		program->linkProgram();
		program->checkErrors();
	}
	else {
		cout << "Error al abrir el archivo" << endl;
	}
}

void Object3D::leerNormales(std::ifstream& f)
{
	//repetir
	string linea = "";
	do {
		//	leer linea
		std::getline(f, linea);

		//si no es comentario
		if ((linea[0] != '/' && linea[1] != '/') && (linea != "end"))
		{
			// // separar linea en identificador y posiciones
			std::stringstream l(linea);
			string identificador;
			string normal;
			l >> identificador;
			l >> normal;
			std::vector<float> n = splitString<float>(normal, ',');
			int vertexId = splitString<int>(identificador, ':')[0];
			this->vertexList[vertexId - 1].vNormal = { n[0],n[1],n[2],n[3] };
		}
		//hasta "end"
	} while (linea != "end");
}

void Object3D::createCollider(ColliderType type) {
	if (vertexList.empty()) return;

	// Calcular los límites del objeto (mínimos y máximos)
	vector4f min = { numeric_limits<float>::max(),
					 numeric_limits<float>::max(),
					 numeric_limits<float>::max(), 1 };
	vector4f max = { -numeric_limits<float>::max(),
					 -numeric_limits<float>::max(),
					 -numeric_limits<float>::max(), 1 };

	for (const auto& vertex : vertexList) {
		min.x = std::min(min.x, vertex.vPos.x);
		min.y = std::min(min.y, vertex.vPos.y);
		min.z = std::min(min.z, vertex.vPos.z);
		max.x = std::max(max.x, vertex.vPos.x);
		max.y = std::max(max.y, vertex.vPos.y);
		max.z = std::max(max.z, vertex.vPos.z);
	}

	// Eliminar el colisionador existente si lo hay
	if (collider) {
		delete collider;
		collider = nullptr;
	}

	// Crear el colisionador según el tipo especificado
	switch (type) {
	case COLLIDER_SPHERE: {
		vector4f center = (min + max) * 0.5f;
		center.w = 1;
		float radius = distance(center, max) * 1.0;
		collider = new Sphere(center, radius);
		break;
	}
	case COLLIDER_AABB: {
		collider = new AABB(min, max);
		break;
	}
					  // Podemos añadir más  (ej: COLLIDER_CAPSULE, COLLIDER_MESH, etc.), como quieras
	default:
		collider = new Sphere((min + max) * 0.5f, distance(min, max) * 0.5f);
		break;
	}

	// Añadir todos los vértices como partículas al colisionador
	for (const auto& vertex : vertexList) {
		collider->addVertex(vertex.vPos);
	}

	// Opcional: Subdividir el colisionador si hay muchos vértices
	if (vertexList.size() > 10) {
		collider->subdivide();
	}
}

void Object3D::updateCollider() {
	if (!collider) {
		createCollider(colliderType);  // Crear el colisionador con el tipo actual
	}
	collider->update(modelMatrix);     // Actualizar con la matriz modelo
}


void Object3D::leerVertices(std::ifstream& f)
{
	// Leer linea hasta encontrar un end
	string linea;
	do {
		std::getline(f, linea);
		// Averiguar si no es comentario
		if ((linea[0] != '/' && linea[1] != '/') && (linea != "end"))
		{
			// Seprara línea en identificador y posiciones
			std::stringstream l(linea);
			string identificador;
			string posiciones;

			l >> identificador;
			l >> posiciones;

			std::vector<float> pos = splitString<float>(posiciones, ','); // Divide la palabra posiciones

			// Asignar posiciones a nuevo vértice
			vertex_t v;
			v.vPos.x = pos[0];
			v.vPos.y = pos[1];
			v.vPos.z = pos[2];
			v.vPos.w = 1; // Lo dejamos a 1 porque es una posicion
			this->vertexList.push_back(v);


		}
	} while (linea != "end");


}


void Object3D::leerColores(std::ifstream& f)
{
	// Leer linea hasta encontrar un end
	string linea;
	do {
		std::getline(f, linea);
		// Averiguar si no es comentario
		if ((linea[0] != '/' && linea[1] != '/') && (linea != "end"))
		{
			// Seprara línea en identificador y posiciones
			std::stringstream l(linea);
			string identificador;
			string colores;

			l >> identificador;
			l >> colores;

			std::vector<float> color = splitString<float>(colores, ','); // Divide la palabra posiciones

			int vertexId = splitString<int>(identificador, ';')[0];

			this->vertexList[vertexId - 1].vColor = { color[0], color[1], color[2], color[3] };

		}
	} while (linea != "end");


}



void Object3D::leerCaras(std::ifstream& f)
{
	// Leer linea hasta encontrar un end
	string linea;
	do {
		std::getline(f, linea);
		// Averiguar si no es comentario
		if ((linea[0] != '/' && linea[1] != '/') && (linea != "end"))
		{
			// Seprara línea en identificador y posiciones
			std::stringstream l(linea);
			string identificador;
			string vertexIds;

			l >> identificador;
			l >> vertexIds;

			std::vector<int> vIds = splitString<int>(vertexIds, ','); // Divide la palabra posiciones
			this->idList.push_back(vIds[0] - 1);
			this->idList.push_back(vIds[1] - 1);
			this->idList.push_back(vIds[2] - 1);

		}
	} while (linea != "end");


}
void Object3D::leerTexturas(std::ifstream& f)
{
	//repetir
	string linea = "";
	do { //primer bucle: leer coordenadas de textura hasta "end"
		//	leer linea
		std::getline(f, linea);

		//si no es comentario
		if ((linea[0] != '/' && linea[1] != '/') && (linea != "end"))
		{
			// // separar linea en identificador y posiciones
			std::stringstream l(linea);
			string identificador;
			string textureCoord;
			l >> identificador;
			l >> textureCoord;
			std::vector<float> tc = splitString<float>(textureCoord, ',');
			int vertexId = splitString<int>(identificador, ':')[0];
			this->vertexList[vertexId - 1].vTextureCoord = { tc[0],tc[1],-1,-1 };
		}
		//hasta "end"
	} while (linea != "end");

	do {//leer fichero de textura
		//	leer linea
		std::getline(f, linea);
		//si no es comentario
		if ((linea[0] != '/' && linea[1] != '/') && (linea != "end"))
		{
			//fichero de textura en variable linea
			this->material.texture = new Texture(linea);
		}
		//hasta "end"
	} while (linea != "end");
}

