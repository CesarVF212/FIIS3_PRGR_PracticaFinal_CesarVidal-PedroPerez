#pragma once
#include "common.h"
#include "vectorMath.h"
using namespace libPRGR;

typedef enum {
    sphere, AABB_t
} collTypes;

typedef enum {
    VERTEX_PARTICLE,    // Para un único vértice en 3D
    TRIANGLE_PARTICLE,  // Para un triángulo (3 vértices) en 3D
    PIXEL_PARTICLE      // Para un píxel en 2D
} ParticleType;

typedef struct particle {
    ParticleType type;  // Tipo de partícula
    vector4f min;       // Coordenadas mínimas (o posición del vértice/píxel)
    vector4f max;       // Coordenadas máximas (o posición adicional para triángulos)
    vector4f color;     // Color del píxel (para objetos 2D, opcional)
} particle;

class Collider {
public:
    collTypes type = sphere;
    std::vector<particle> partList;
    std::vector<Collider*> sons; // opcional - para jerarquía

    Collider() {};
    virtual ~Collider() {
        for (auto& son : sons) {
            delete son;
        }
    }

    // Métodos para añadir partículas
    void addVertex(vector4f vertex) {
        particle p;
        p.type = VERTEX_PARTICLE;
        p.min = vertex;
        p.max = vertex; // Para vértices, min y max son iguales
        addParticle(p);
    }

    void addTriangle(vector4f v1, vector4f v2, vector4f v3) {
        particle p;
        p.type = TRIANGLE_PARTICLE;
        p.min = { std::min({v1.x, v2.x, v3.x}),
                  std::min({v1.y, v2.y, v3.y}),
                  std::min({v1.z, v2.z, v3.z}), 1 };
        p.max = { std::max({v1.x, v2.x, v3.x}),
                  std::max({v1.y, v2.y, v3.y}),
                  std::max({v1.z, v2.z, v3.z}), 1 };
        addParticle(p);
    }

    void addPixel(vector4f position, vector4f color) {
        particle p;
        p.type = PIXEL_PARTICLE;
        p.min = position;
        p.max = position; // Para píxeles, min y max son iguales
        p.color = color;  // Almacena el color (para detectar transparencia)
        addParticle(p);
    }

    // Método para añadir partículas al colisionador
    virtual void addParticle(particle part) = 0;

    // Test de colisión entre dos objetos tipo collider
    virtual bool test(Collider* c2) = 0;

    // Actualizar el colisionador cuando las partículas se mueven
    virtual void update(matrix4x4f mat) = 0;

    // Opcional - subdivisión para jerarquía de volúmenes
    virtual void subdivide() = 0;

    // Obtener el centro geométrico
    virtual vector4f getCenter() const = 0;

    // Obtener tamaño
    virtual vector4f getSize() const = 0;
};

class Sphere : public Collider {
public:
    vector4f center;         // Centro actual de la esfera
    vector4f centerOrigin;   // Centro original de la esfera
    float radius;            // Radio actual de la esfera
    float radiusOrigin;      // Radio original de la esfera

    Sphere();
    Sphere(vector4f center, float radius);
    ~Sphere() override;

    // Implementación de métodos de la clase base
    void addParticle(particle part) override;
    bool test(Collider* c2) override;
    void update(matrix4x4f mat) override;
    void subdivide() override;

    // Métodos específicos de Sphere
    vector4f getCenter() const override;
    vector4f getSize() const override;
    void computeBoundingSphere();
};

class AABB : public Collider {
public:
    vector4f min;            // Esquina mínima de la caja
    vector4f max;            // Esquina máxima de la caja
    vector4f minOrigin;      // Esquina mínima original
    vector4f maxOrigin;      // Esquina máxima original

    AABB();
    AABB(vector4f min, vector4f max);
    ~AABB() override;

    // Implementación de métodos de la clase base
    void addParticle(particle part) override;
    bool test(Collider* c2) override;
    void update(matrix4x4f mat) override;
    void subdivide() override;

    // Métodos específicos de AABB
    vector4f getCenter() const override;
    vector4f getSize() const override;
    void computeBoundingBox();
};