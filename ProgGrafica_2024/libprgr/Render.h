#pragma once

#include <map>
#include <iostream>
#include <vector>

#include "EventManager.h"
#include "Object3D.h"
#include "Camera.h"
#include "Light.h"

// Declaración anticipada
class Camera;

using namespace std;
using namespace libPRGR;

class Render {
public:

    // --- VENTANA GL ---
    GLFWwindow* window; // Ventana de GLFW que utilizaremos para dibujar

    void initGL(int width, int height);
    void deinitGLFW();


    // --- CÁMARA ---
    Camera* camera;
    void putCamera(Camera* camera); // Establece la cámara a utilizar
    bool cameraCollision(Camera* camera);


    // --- LUCES ---
    vector<Light*> lights; // Lista de luces que utilizaremos para iluminar los objetos

    void putLight(Light* light); // Añade una luz a la lista de luces
    void putLight(vector<Light*> lights); // Añade múltiples luces a la lista


    // --- OBJETOS ---
    inline static map<int, Object3D*> objectList; // Lista de objetos a dibujar 
    map<int, bufferObject> bufferList; // Lista de buffers de objetos

    void setUpObject(Object3D* obj); // Configura un objeto para ser dibujado
    void putObject(Object3D* obj); // Agrega un objeto a la lista de objetos a dibujar
    void putObject(int ID, Object3D* obj);
    Object3D* getObject(int ID);
    void removeObject(Object3D* obj); // Elimina un objeto de la lista de objetos a dibujar


    // --- RENDERIZADO ---
    void drawGl(Object3D* obj); // Dibuja un objeto en la ventana


    // --- BUCLE PRINCIPAL ---
    void mainLoop(); // Loop principal de la aplicación

private:

    // Funciones auxiliares para el renderizado
    void setupProgramAndMatrices(Object3D* obj);
    void setupLights(Program* prg);
    void setupMaterial(Object3D* obj);
    void setupVertexAttributes(Object3D* obj);
    void renderObject(Object3D* obj);

};