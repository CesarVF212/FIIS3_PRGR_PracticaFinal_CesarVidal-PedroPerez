#pragma once
#include "common.h"
#include "vectorMath.h"
using namespace libPRGR;

typedef enum {
    sphere, AABB_t
} collTypes;

typedef enum {
    VERTEX_PARTICLE,    // Para un �nico v�rtice en 3D
    TRIANGLE_PARTICLE,  // Para un tri�ngulo (3 v�rtices) en 3D
    PIXEL_PARTICLE      // Para un p�xel en 2D
} ParticleType;

typedef struct particle {
    ParticleType type;  // Tipo de part�cula
    vector4f min;       // Coordenadas m�nimas (o posici�n del v�rtice/p�xel)
    vector4f max;       // Coordenadas m�ximas (o posici�n adicional para tri�ngulos)
    vector4f color;     // Color del p�xel (para objetos 2D, opcional)
} particle;

class Collider {
public:
    collTypes type = sphere;
    std::vector<particle> partList;
    std::vector<Collider*> sons; // opcional - para jerarqu�a

    Collider() {};
    virtual ~Collider() {
        for (auto& son : sons) {
            delete son;
        }
    }

    // M�todos para a�adir part�culas
    void addVertex(vector4f vertex) {
        particle p;
        p.type = VERTEX_PARTICLE;
        p.min = vertex;
        p.max = vertex; // Para v�rtices, min y max son iguales
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
        p.max = position; // Para p�xeles, min y max son iguales
        p.color = color;  // Almacena el color (para detectar transparencia)
        addParticle(p);
    }

    // M�todo para a�adir part�culas al colisionador
    virtual void addParticle(particle part) = 0;

    // Test de colisi�n entre dos objetos tipo collider
    virtual bool test(Collider* c2) = 0;

    // Actualizar el colisionador cuando las part�culas se mueven
    virtual void update(matrix4x4f mat) = 0;

    // Opcional - subdivisi�n para jerarqu�a de vol�menes
    virtual void subdivide() = 0;

    // Obtener el centro geom�trico
    virtual vector4f getCenter() const = 0;

    // Obtener tama�o
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

    // Implementaci�n de m�todos de la clase base
    void addParticle(particle part) override;
    bool test(Collider* c2) override;
    void update(matrix4x4f mat) override;
    void subdivide() override;

    // M�todos espec�ficos de Sphere
    vector4f getCenter() const override;
    vector4f getSize() const override;
    void computeBoundingSphere();
};

class AABB : public Collider {
public:
    vector4f min;            // Esquina m�nima de la caja
    vector4f max;            // Esquina m�xima de la caja
    vector4f minOrigin;      // Esquina m�nima original
    vector4f maxOrigin;      // Esquina m�xima original

    AABB();
    AABB(vector4f min, vector4f max);
    ~AABB() override;

    // Implementaci�n de m�todos de la clase base
    void addParticle(particle part) override;
    bool test(Collider* c2) override;
    void update(matrix4x4f mat) override;
    void subdivide() override;

    // M�todos espec�ficos de AABB
    vector4f getCenter() const override;
    vector4f getSize() const override;
    void computeBoundingBox();
};