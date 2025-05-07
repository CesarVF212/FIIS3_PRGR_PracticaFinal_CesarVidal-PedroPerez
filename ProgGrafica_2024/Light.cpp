#include "libprgr/Light.h"

void Light::move(double timeStep)
{
	// Función delegada.
}

vector4f Light::normalizeColor(vector4f col) {
    if (col.x > 1.0f || col.y > 1.0f || col.z > 1.0f) {
        return {
            col.x / 255.0f,
            col.y / 255.0f,
            col.z / 255.0f,
            (col.w > 1.0f) ? col.w / 255.0f : col.w
        };
    }
    return col;
}

OrbitalLight::OrbitalLight(vector4f center, float radius, float speed,
    vector4f color, float intensity, vector4f rotationAxis) :
    Light(POINT, center, color, intensity),
    center(center),
    radius(radius),
    speed(speed),
    rotationAxis(normalize(rotationAxis))
{
    position = calculateOrbitPosition(0.0f);
}

vector4f OrbitalLight::calculateOrbitPosition(float angle) const 
{
    // Usamos tus operadores vectoriales existentes
    vector4f initialPoint = { radius, 0, 0, 1.0f }; // Punto inicial en el eje X

    // Rotación usando la fórmula de Rodrigues con tus operadores
    float cosTheta = cos(angle);
    float sinTheta = sin(angle);

    // Usamos tus operadores * (producto escalar) y ^ (producto cruz)
    vector4f term1 = initialPoint * cosTheta;
    vector4f term2 = (rotationAxis ^ initialPoint) * sinTheta;
    vector4f term3 = rotationAxis * (rotationAxis * initialPoint) * (1 - cosTheta);

    // Sumamos los términos y trasladamos al centro
    return center + (term1 + term2 + term3);

}

void OrbitalLight::move(double timeStep) 
{
    static float angle = 0.0f;
    angle += speed * timeStep;
    position = calculateOrbitPosition(angle);
}