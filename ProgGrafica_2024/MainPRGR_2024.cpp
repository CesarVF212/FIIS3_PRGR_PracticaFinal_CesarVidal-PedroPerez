#define GLAD_BIN
#include "libprgr/Render.h"

using namespace libPRGR;
using namespace std;

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

int main(int argc, char** argv)
{
    // Iniciamos la clase Render.
    Render render;
    render.initGL(WINDOW_WIDTH, WINDOW_HEIGHT);

    // --- OBJETOS ---
    /*Object3D* esfera = new Object3D();
    esfera->loadFromFile("data/icosfera.fiis");
    esfera->position = { 0, 1.0f, 0, 0 };
    esfera->updateModelMatrix();*/

    Object3D* cubo = new Object3D();
    cubo->loadFromFile("data/cubo.fiis");
    cubo->position = { -2.5f, 1.0f, 0, 0 };
    cubo->updateModelMatrix();

    /*Object3D* floor = new Object3D();
    floor->loadFromFile("data/floor.fiis");
    floor->position = { 0, 0, 0, 0 };
    floor->scale = { 100.0f, 0.1f, 100.0f, 0 };
    floor->standartRotation = false;
    floor->updateModelMatrix();*/

    /*Object3D* sun = new Object3D();
    sun->loadFromFile("data/sun.fiis");
    sun->position = { 100.0f, 100.0f, 100.0f, 0 };
    sun->scale = { 10.0f, 0.1f, 10.0f, 0 };
    sun->standartRotation = false;
    sun->updateModelMatrix();*/

    // --- LUCES ---
    Light* goldenLight = new Light(Light::POINT, { 3, 3, 3, 1 }, { 1.0f, 0.85f, 0.0f, 1.0f }, 1.0f);
    render.putLight(goldenLight);

    // Primera luz orbital (eje Y por defecto)
    Light* orbitalLightY = new OrbitalLight(
        { 0, 0, 0, 1 },
        10.0f,
        0.2f,
        { 1.0f, 0.0f, 0.0f, 1 },  // rojo
        1.0f,
        { 0, 1, 0, 0 }
    );
    render.putLight(orbitalLightY);

    // Segunda luz orbital (eje X)
    Light* orbitalLightX = new OrbitalLight(
        { 0, 0, 0, 1 },
        10.0f,
        0.3f,
        { 0.0f, 0.0f, 1.0f, 1 },  // azul
        1.0f,
        { 1, 0, 0, 0 }
    );
    render.putLight(orbitalLightX);

    // Tercera luz orbital (eje diagonal)
    Light* orbitalLightDiagonal = new OrbitalLight(
        { 0, 5, 0, 1 },
        7.0f,
        0.15f,
        { 0.0f, 1.0f, 0.0f, 1 },  // verde
        1.0f,
        { 1, 1, 0, 0 }
    );
    render.putLight(orbitalLightDiagonal);

    // --- CÁMARA ---
    CameraFirstPerson* cameraFPS = new CameraFirstPerson(
        { 0, 1.8f, 5, 1 },
        { 0, 0, 0, 1 },
        { 0, 1, 0, 1 },
        90.0f,
        (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
        0.01f,
        100.0f
    );
    cameraFPS->setRenderer(&render);

    // Colocamos objetos y cámara
    //render.putObject(esfera);
    render.putObject(cubo);
    //render.putObject(floor);
    //render.putObject(sun);
    render.putCamera(cameraFPS);

    // Bucle principal
    render.mainLoop();

    // Liberación de memoria
    render.deinitGLFW();
    //delete esfera;
    delete cubo;
    //delete floor;
    //delete sun;
    delete goldenLight;
    delete orbitalLightY;
    delete orbitalLightX;
    delete orbitalLightDiagonal;

    return 0;
}
