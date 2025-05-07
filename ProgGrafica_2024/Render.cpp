#include "libprgr/Render.h"

void Render::initGL(int width, int height)
{
	if (glfwInit() != GLFW_TRUE)
	{
		cout << "ERROR: " << __FILE__ << ":" << __LINE__ << " (" << __func__ << ") al inicializar GLFW" << endl;
		return;
	}

	// Caso de usar el codigo en MAC.
#ifdef __APPLE__ 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	window = glfwCreateWindow(width, height, "Window", nullptr, nullptr); // Crear ventana con parametros de ancho y alto y nombre
	glfwMakeContextCurrent(window); // Hacer que la ventana creada sea la ventana actual
	gladLoadGL(glfwGetProcAddress); //usarla después de haber iniciado GLFW
	EventManager::init(window); // Inicializar el EventManager con la ventana creada
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hacer que las teclas se queden presionadas
	glEnable(GL_DEPTH_TEST); // Habilitar el uso de profundidad
	glDepthFunc(GL_LESS); // Habilitar el uso de profundidad
}

void Render::deinitGLFW()
{
	glfwTerminate();
}

void Render::setUpObject(Object3D* obj)
{
	bufferObject bo;

	// Generar un buffer de datos
	glGenVertexArrays(1, &bo.idArray); // Generar un buffer de arrays y guardarlo en idArray
	glGenBuffers(1, &bo.idVertexArray); // Generar un buffer de vertices y guardarlo en idVertexArray
	glGenBuffers(1, &bo.idIndexArray); // Generar un buffer de indices y guardarlo en idIndexArray

	// Por cada buffer
	glBindVertexArray(bo.idArray); // Activar buffer de arrays con idArray 

	// Activar buffer
	glBindBuffer(GL_ARRAY_BUFFER, bo.idVertexArray); // Activar buffer de vertices

	// Subir por cada buffer sus datos
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * obj->vertexList.size(), obj->vertexList.data(), GL_STATIC_DRAW);

	// Activar buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.idIndexArray);

	// Subir por cada buffer sus datos
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * obj->idList.size(), obj->idList.data(), GL_STATIC_DRAW);
	bufferList[obj->id] = bo;
}

void Render::putObject(Object3D* obj) {
	putObject(obj->id, obj); 
}

void Render::putObject(int ID, Object3D* obj) {
	if (objectList.find(ID) != objectList.end()) {
		removeObject(objectList[ID]);
	}
	objectList[ID] = obj;
	setUpObject(obj);
}

Object3D* Render::getObject(int ID) {
	auto it = objectList.find(ID);
	return (it != objectList.end()) ? it->second : nullptr;
}


void Render::drawGl(Object3D* obj)
{
	if (!obj || !obj->program) return;

	// Configuración básica del programa y matrices
	setupProgramAndMatrices(obj);

	// Configuración de luces
	setupLights(obj->program);

	// Configuración de materiales y texturas
	setupMaterial(obj);

	// Configuración de atributos de vértices usando setAttributeData
	setupVertexAttributes(obj);

	// Dibujado del objeto
	renderObject(obj);
}

void Render::setupProgramAndMatrices(Object3D* obj)
{
	// Preparamos el programa y lo usamos
	Program* prg = obj->program;
	prg->use();

	// Matriz de modelo
	matrix4x4f M = obj->modelMatrix;
	prg->setUniformData(Program::matrix4, M.mat1, "uModel");

	// Matrices de vista y proyección si hay cámara
	if (camera) {
		prg->setUniformData(Program::matrix4, camera->computeViewMatrix().mat1, "uView");
		prg->setUniformData(Program::matrix4, camera->computeProjectionMatrix().mat1, "uProjection");

		// Posición de la cámara
		vector4f camPos = { camera->position.x, camera->position.y, camera->position.z, 1 };
		prg->setUniformData(Program::vector4, &camPos, "uViewPos");
	}
}

void Render::setupLights(Program* prg)
{
	// Usamos setUniformData en lugar de glUniform* directo
	int numLights = static_cast<int>(lights.size());
	prg->setUniformData(Program::integer, &numLights, "uNumLights");

	for (size_t i = 0; i < lights.size(); ++i)
	{
		const auto& light = lights[i];
		std::string base = "uLights[" + std::to_string(i) + "]";

		// Tipo de luz
		prg->setUniformData(Program::integer, &(light->type), (base + ".type").c_str());

		// Posición de la luz
		prg->setUniformData(Program::vector4, &(light->position), (base + ".position").c_str());

		// Color de la luz
		prg->setUniformData(Program::vector4, &(light->color), (base + ".color").c_str());

		// Intensidad de la luz
		prg->setUniformData(Program::floatpoint, &(light->i), (base + ".intensity").c_str());

		// Dirección de la luz
		prg->setUniformData(Program::vector4, &(light->direction), (base + ".direction").c_str());
	}
}

void Render::setupMaterial(Object3D* obj)
{
	Program* prg = obj->program;

	// Parámetros de material básicos usando setUniformData
	float kd = 0.8f;
	prg->setUniformData(Program::floatpoint, &kd, "uKd");

	float ks = 0.5f;
	prg->setUniformData(Program::floatpoint, &ks, "uKs");

	int shininess = 32;
	prg->setUniformData(Program::integer, &shininess, "uShininess");

	// Textura si existe
	if (obj->material.texture)
	{
		obj->material.texture->bind(0);
		int textureUnit = 0;
		prg->setUniformData(Program::integer, &textureUnit, "uTexture");
	}
}

void Render::setupVertexAttributes(Object3D* obj)
{
	auto& bo = bufferList[obj->id];
	Program* prg = obj->program;

	glBindVertexArray(bo.idArray);
	glBindBuffer(GL_ARRAY_BUFFER, bo.idVertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.idIndexArray);

	// Usando setAttributeData para cada atributo
	prg->setAttributeData("vPos", 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
		(void*)offsetof(vertex_t, vPos));

	prg->setAttributeData("vColor", 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
		(void*)offsetof(vertex_t, vColor));

	prg->setAttributeData("vNormal", 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
		(void*)offsetof(vertex_t, vNormal));

	prg->setAttributeData("vTextureCoord", 4, GL_FLOAT, GL_FALSE, sizeof(vertex_t),
		(void*)offsetof(vertex_t, vTextureCoord));
}

void Render::renderObject(Object3D* obj)
{
	glDrawElements(GL_TRIANGLES, obj->idList.size(), GL_UNSIGNED_INT, nullptr);
}

void Render::mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera->move(0.008);

		for (auto light : lights) {
			light->move(0.001);
		}

		// Updated object loop
		for (auto& [id, obj] : objectList) {
			obj->updateCollider(); // Added collider update
			obj->move(0.001);
			obj->updateModelMatrix();
			drawGl(obj);
		}

		glfwSwapBuffers(window);
	}
}

void Render::putCamera(Camera* cam)
{
	this->camera = cam;
}

bool Render::cameraCollision(Camera* camera)
{
	if (!camera || !camera->coll)
	{
		cout << "DEBUG (" << __FILE__ << "): El colisionador de la cámara es NULO.\n" << endl;
		return false;
	}

	// Obtén el colisionador real (Sphere o AABB) desde el BoundingVolume
	cout << "DEBUG: Comprobando colisiones para cámara en posición ("
		<< camera->position.x << ", " << camera->position.y << ", "
		<< camera->position.z << ")" << endl;

	// Añade esta información si está disponible
	if (camera->coll->getType() == SPHERE_TYPE) {
		cout << " con radio: " << camera->coll->getSize().x / 2 << endl;
	}

	for (auto& [id, obj] : objectList)
	{
		if (obj->collider)
		{
			cout << "DEBUG: Probando contra objeto ID: " << id
				<< " en posición (" << obj->position.x << ", "
				<< obj->position.y << ", " << obj->position.z << ")";

			// Mostrar información específica del tipo de volumen
			if (obj->collider->getType() == SPHERE_TYPE) {
				cout << " con radio: " << obj->collider->getSize().x / 2;
			}

			cout << endl;
			bool collision = obj->collider->test(camera->coll);
			if (collision)
			{
				cout << "DEBUG: COLISIÓN DETECTADA con objeto ID: " << id << endl;
				return true;
			}
		}
	}
	cout << "DEBUG: No se han detectado colisiones" << endl;
	return false;
}

void Render::putLight(Light* light)
{
	this->lights.push_back(light);
}

void Render::putLight(vector<Light*> newLights)
{
	this->lights.insert(this->lights.end(), newLights.begin(), newLights.end());
}


void Render::removeObject(Object3D* obj) {
	auto bufferIter = bufferList.find(obj->id);
	if (bufferIter != bufferList.end()) {
		bufferObject bo = bufferIter->second;
		glDeleteBuffers(1, &bo.idVertexArray);
		glDeleteBuffers(1, &bo.idIndexArray);
		glDeleteVertexArrays(1, &bo.idArray);
		bufferList.erase(bufferIter);
	}

	auto objIter = objectList.find(obj->id);
	if (objIter != objectList.end()) {
		objectList.erase(objIter);
	}
}
