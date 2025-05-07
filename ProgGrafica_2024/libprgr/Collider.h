#pragma once
#include "common.h"
#include <vector>
#include <limits>
#include "vectorMath.h"
using namespace libPRGR;

class Sphere;
class AABB;
class BoundingVolume;

// Tipos de partículas
enum ParticleType {
    VERTEX_PARTICLE,
    TRIANGLE_PARTICLE,
    PIXEL_PARTICLE
};

// Estructura para representar una partícula
struct particle {
    ParticleType type;
    vector4f min;
    vector4f max;
};

// Tipos de colisionadores
enum ColliderType {
    SPHERE_TYPE,
    AABB_TYPE
};

#pragma region --- COLLIDER BASE ---
// Clase base para todos los colisionadores
class Collider {
public:
    Collider() {}
    virtual ~Collider() {}

    virtual bool test(Collider* other) = 0;
    virtual void update(matrix4x4f mat) = 0;
    virtual vector4f getCenter() const = 0;
    virtual vector4f getSize() const = 0;
};
#pragma endregion

#pragma region --- COLLIDER ESFÉRICO ---
// Clase para colisionadores de tipo esfera
class Sphere : public Collider {
public:
    Sphere();
    Sphere(vector4f center, float radius);
    virtual ~Sphere();

    virtual bool test(Collider* other) override;
    virtual void update(matrix4x4f mat) override;
    virtual vector4f getCenter() const override;
    virtual vector4f getSize() const override;

    void computeBoundingSphere();

    vector4f center;      // Centro de la esfera transformado
    vector4f centerOrigin; // Centro original sin transformar
    float radius;         // Radio de la esfera transformado
    float radiusOrigin;  // Radio original sin transformar
};
#pragma endregion

#pragma region --- COLLIDER AABB (POR CAJAS) ---
// Clase para colisionadores de tipo caja alineada con los ejes
class AABB : public Collider {
public:
    AABB();
    AABB(vector4f min, vector4f max);
    virtual ~AABB();

    virtual bool test(Collider* other) override;
    virtual void update(matrix4x4f mat) override;
    virtual vector4f getCenter() const override;
    virtual vector4f getSize() const override;

    void computeBoundingBox();

    vector4f min;      // Esquina mínima de la caja transformada
    vector4f max;      // Esquina máxima de la caja transformada
    vector4f minOrigin; // Esquina mínima original sin transformar
    vector4f maxOrigin; // Esquina máxima original sin transformar
};
#pragma endregion

#pragma region --- COLLIDER BOUNDING VOLUME (JARAQUICO) ---
// Clase para la jerarquía de volúmenes envolventes
class BoundingVolume {
public:
    BoundingVolume();
    BoundingVolume(ColliderType type);
    ~BoundingVolume();

    // Manejo de partículas
    void addParticle(particle part);
    void clearParticles();

    // Detección de colisiones
    bool test(BoundingVolume* other);

    // Actualización y transformación
    void update(matrix4x4f mat);

    // Construcción de la jerarquía
    void buildHierarchy();
    void subdivide(BoundingVolume* node);

    // Getters
    vector4f getCenter() const;
    vector4f getSize() const;
    ColliderType getType() const { return type; }

    // Cálculo de volumenes envolventes.
    void computeBoundingSphere();
    void computeBoundingBox();

private:
    // Tipo de volumen envolvente (esfera o AABB)
    ColliderType type;

    // Colisionador concreto (esfera o caja)
    union {
        Sphere* sphere;
        AABB* aabb;
    } collider;

    // Lista de partículas contenidas en este volumen
    std::vector<particle> partList;

    // Nodos hijos (para la jerarquía)
    BoundingVolume* leftChild;
    BoundingVolume* rightChild;

    // Indica si este nodo es una hoja
    bool isLeaf;
};
#pragma endregion
