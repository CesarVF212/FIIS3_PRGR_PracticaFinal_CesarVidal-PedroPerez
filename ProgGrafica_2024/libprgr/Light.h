#pragma once

#include "common.h"
#include "vectorMath.h"

using namespace libPRGR;

#pragma region --- LIGHT ---

class Light {
public:

    // Enumerado de los tipos de luz.
    typedef enum {
        DIRECTIONAL = 0,
        POINT = 1
    } lightTypes_e;

    // Propiedades de la luz.
    vector4f position;
    vector4f color;
    float i;
    lightTypes_e type;
    vector4f direction ;

    // Constructor de la clase
    Light(lightTypes_e type, vector4f position, vector4f color, float I, vector4f direction = { 0, 0, 0, 0 }) :
        type(type), position(position), color(color), i(I), direction(direction) {

        if (i < 0.0f)
        {
            this->i = 0;
        }
        this->color = normalizeColor(color);
    };

    // Funci�n move delegada (la luz est�ndar no se mueve)
    virtual void move(double timeStep);

    // En el caso de recibir colores fuera del rango v�lido.
    vector4f normalizeColor(vector4f col);
};

#pragma endregion

#pragma region --- ORBITAL LIGHT ---

class OrbitalLight : public Light {
public:

    // Variables para el movimiento orbital
    vector4f center;
    float radius;
    float speed;
    vector4f rotationAxis;

    // Constructor con eje de rotaci�n personalizado
    OrbitalLight(vector4f center, float radius, float speed,
        vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f },
        float intensity = 1.0f,
        vector4f rotationAxis = { 0.0f, 1.0f, 0.0f, 0.0f });

    // Funci�n move que actualiza la posici�n orbital
    void move(double timeStep) override;

private:

    // Funci�n auxiliar para calcular la posici�n en la �rbita
    vector4f calculateOrbitPosition(float angle) const;
};

#pragma endregion