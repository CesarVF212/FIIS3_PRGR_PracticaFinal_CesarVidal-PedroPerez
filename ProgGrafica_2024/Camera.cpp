#include "libprgr/Camera.h"
#include "libprgr/EventManager.h"
#include "libprgr/Render.h"

using namespace std;
using namespace libPRGR;

matrix4x4f Camera::computeViewMatrix()
{
    matrix4x4f view = make_identity();
    vector4f f = normalize(this->lookAt - this->position);
    vector4f r = normalize(f ^ normalize(up));
    vector4f u = normalize(r ^ f);

    vector4f p = { -(r * this->position), -(u * this->position),(f * this->position), 1 };

    view.rows[0] = r;
    view.rows[1] = u;
    view.rows[2] = -1.0f * f;
    view.mat2D[0][3] = p.x;
    view.mat2D[1][3] = p.y;
    view.mat2D[2][3] = p.z;

    return view;
}

matrix4x4f Camera::computeProjectionMatrix()
{
    matrix4x4f proj = make_identity();

    double fovy = toRadians(this->fovy);

    proj.mat2D[0][0] = 1.0f / (this->aspectRatio * std::tan(this->fovy * 0.5));
    proj.mat2D[1][1] = 1.0f / std::tan(this->fovy * 0.5);
    proj.mat2D[2][2] = -(this->zNear + this->zFar) / (this->zFar - this->zNear);
    proj.mat2D[3][3] = 1;
    proj.mat2D[2][3] = 2.0f * (this->zNear * this->zFar) / (this->zFar - this->zNear);
    proj.mat2D[3][2] = -1;

    return proj;
}

void Camera::move(float timeStep)
{
    // Guardar posición antes de mover
    vector4f prevPosition = position;
    vector4f prevLookAt = lookAt;

    float speed = 0.1f;

    if (EventManager::keyState[GLFW_KEY_LEFT]) {
        position.x -= timeStep * speed;
        lookAt.x -= timeStep * speed;
    }

    if (EventManager::keyState[GLFW_KEY_RIGHT]) {
        position.x += timeStep * speed;
        lookAt.x += timeStep * speed;
    }

    if (EventManager::keyState[GLFW_KEY_UP]) {
        position.z += timeStep * speed;
        lookAt.z += timeStep * speed;
    }

    if (EventManager::keyState[GLFW_KEY_DOWN]) {
        position.z -= timeStep * speed;
        lookAt.z -= timeStep * speed;
    }

    // Actualizamos las colisiones.
    coll->update(make_identity()); // Usamos matriz identidad porque solo cambia la posición
    coll->center = position;

    // Verificar colisión (implementar esta función en Render).
    if (r && r->cameraCollision(this)) {
        cout << "DEBUG (Camera.cpp): Se he detectado una colisión, retrocediendo a la posición anterior." << endl;
        position = prevPosition;
        lookAt = prevLookAt;
        coll->update(make_translate(position.x, position.y, position.z));
    }
    else {
        cout << "DEBUG (Camera.cpp): Movimiento permitido. Moviendose a ( " << position.x << ", "
            << position.y << ", " << position.z << " )" << endl;
    }
}

void CameraFirstPerson::move(float timeStep)
{
    // Guardar posición antes de mover
    vector4f prevPosition = position;
    vector4f prevLookAt = lookAt;

    // Cogemos la posición del InputManager.
    double mouseX = EventManager::mouseState.posX;
    double mouseY = EventManager::mouseState.posY;

    // Calcular el desplazamiento del ratón
    float offsetX = (mouseX - lastX) * cameraMovementScalar;
    float offsetY = (mouseY - lastY) * cameraMovementScalar;

    lastX = mouseX;
    lastY = mouseY;

    yaw += offsetX;
    pitch -= offsetY;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Calcular nueva dirección
    vector4f direction;
    direction.x = cos(toRadians(yaw)) * cos(toRadians(pitch));
    direction.y = sin(toRadians(pitch));
    direction.z = sin(toRadians(yaw)) * cos(toRadians(pitch));
    direction.w = 0.0f;
    direction = normalize(direction);

    lookAt = this->position + direction;

    // Calcular vectores right y up
    vector4f forward = normalize(lookAt - this->position);
    vector4f right = normalize(forward ^ up);

    // Movimiento
    if (EventManager::keyState[GLFW_KEY_W]) {
        this->position = this->position + forward * (timeStep * speed);
        if (EventManager::keyState[GLFW_KEY_R]) {
            this->position = this->position + forward * (timeStep * speed * 3);
        }
    }
    if (EventManager::keyState[GLFW_KEY_S]) {
        this->position = this->position - forward * (timeStep * speed);
    }
    if (EventManager::keyState[GLFW_KEY_A]) {
        this->position = this->position - right * (timeStep * speed);
    }
    if (EventManager::keyState[GLFW_KEY_D]) {
        this->position = this->position + right * (timeStep * speed);
    }
    if (EventManager::keyState[GLFW_KEY_SPACE]) {
        this->position.y -= timeStep * speed;
    }
    if (EventManager::keyState[GLFW_KEY_LEFT_SHIFT]) {
        this->position.y += timeStep * speed;
    }

    // Actualizar colisionador
    coll->update(make_identity());
    coll->center = position;

    // Verificar colisión
    if (r && r->cameraCollision(this)) {
        cout << "DEBUG (Camera.cpp): Se he detectado una colisión, retrocediendo a la posición anterior.\n" << endl;
        position = prevPosition;
        lookAt = prevLookAt;
        coll->update(make_translate(position.x, position.y, position.z));
    }
    else {
        cout <<  "DEBUG (Camera.cpp): Movimiento permitido. Moviendose a ( " << position.x << ", "
            << position.y << ", " << position.z << ")\n" << endl;
    }
}