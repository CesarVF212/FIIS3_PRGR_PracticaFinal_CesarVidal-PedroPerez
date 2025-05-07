#pragma once

#include "vectorMath.h"
#include "EventManager.h"
#include "Collider.h" // Añadir include para Collider

// Declaración anticipada para evitar dependencia circular
class Render;

#define SPEED_PEDRO 0.1f
#define SPEED_CESAR 0.5f

using namespace libPRGR;

#pragma region --- CAMERA ---

class Camera
{
public:
    Render* r;
    // -- POSICIÓN, DIRECCION Y APUNTADO.
    vector4f position;
    vector4f up;
    vector4f lookAt;
    Sphere* coll; // Colisionador de la cámara

    Camera(vector4f pos, vector4f lookAt, vector4f up, float fovy, float aspectRatio,
        float zNear, float zfar) {

        this->up = normalize(up);
        this->position = pos;
        this->lookAt = lookAt;

        this->fovy = fovy;
        this->aspectRatio = aspectRatio;
        this->zNear = zNear;
        this->zFar = zfar;

        // Crear colisionador para la cámara (0.25*0.25*0.25 unidades)
        this->coll = new Sphere(pos, 0.125f); // Radio = 0.125 (mitad de 0.25)
    }

    ~Camera() {
        if (coll) delete coll;
    }

    float fovy;
    float aspectRatio;
    float zNear; // Distancia mínima a la que un objeto es visible.
    float zFar;  // Distancia máxima a la que un objeto es visible.

    matrix4x4f computeViewMatrix();
    matrix4x4f computeProjectionMatrix();
    virtual void move(float timeStep);
    void setRenderer(Render* render) { this->r = render; }
};

#pragma endregion

#pragma region --- FIRST PERSON CAMERA ---

#define DFT_FPSCAMERA_SPEED_PEDRO 0.2f
#define DFT_FPSCAMERA_SPEED_CESAR 0.5f
#define DFT_FPSCAMERA_MOVEMENT_SCALAR 0.2f
#define DFT_MOUSE_SENSITIVITY 0.1f

class CameraFirstPerson : public Camera
{
private:
    int lastX; // Última posición X del ratón
    int lastY; // Última posición Y del ratón
    float yaw;
    float pitch;
    float mouseSensitivity;
    float speed;
    float cameraMovementScalar;

public:
    CameraFirstPerson(vector4f pos, vector4f lookAt, vector4f up, float fovy, float aspectRatio, float zNear, float zFar)
        : Camera(pos, lookAt, up, fovy, aspectRatio, zNear, zFar)
    {
        this->lastX = 0;
        this->lastY = 0;
        this->yaw = -90.0f;
        this->pitch = 0.0f;
        this->speed = DFT_FPSCAMERA_SPEED_CESAR;
        this->cameraMovementScalar = DFT_FPSCAMERA_MOVEMENT_SCALAR;
    }

    void move(float timeStep) override;
    void setRenderer(Render* render) { this->r = render; }
};

#pragma endregion