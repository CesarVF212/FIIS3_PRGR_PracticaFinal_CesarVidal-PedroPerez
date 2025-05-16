#include "libprgr/Collider.h"

// Sphere Implementation
Sphere::Sphere() {
    type = sphere;
    center = { 0, 0, 0, 1 };
    centerOrigin = { 0, 0, 0, 1 };
    radius = 0;
    radiusOrigin = 0;
}

Sphere::Sphere(vector4f center, float radius)
    : center(center), radius(radius) {
    type = sphere;
    centerOrigin = center;
    radiusOrigin = radius;
}

Sphere::~Sphere() {
    // Base class destructor will handle sons deletion
}

void Sphere::addParticle(particle part) {
    partList.push_back(part);

    // Recalcular la esfera envolvente
    computeBoundingSphere();
}

bool Sphere::test(Collider* c2) {
    if (c2->type == sphere) {
        Sphere* sph2 = static_cast<Sphere*>(c2);

        // Verificar si las esferas están colisionando
        float dist = distance(center, sph2->center);
        bool result = dist <= (radius + sph2->radius);

        // Si tienen hijos y las esferas colisionan, verificar los hijos
        if (result) {
            if (!sons.empty() && !c2->sons.empty()) {
                // Ambos tienen subdivisiones, hay que comprobar hijos con hijos
                bool anyCollision = false;
                for (auto& son1 : sons) {
                    for (auto& son2 : c2->sons) {
                        anyCollision |= son1->test(son2);
                    }
                }
                return anyCollision;
            }
            else if (!sons.empty()) {
                // Solo yo tengo subdivisiones
                bool anyCollision = false;
                for (auto& son : sons) {
                    anyCollision |= son->test(c2);
                }
                return anyCollision;
            }
            else if (!c2->sons.empty()) {
                // Solo c2 tiene subdivisiones
                bool anyCollision = false;
                for (auto& son : c2->sons) {
                    anyCollision |= this->test(son);
                }
                return anyCollision;
            }
        }

        return result;
    }
    else if (c2->type == AABB_t) {
        // Para colisiones esfera-AABB, podría implementarse aquí
        // O simplemente delegar al otro objeto si implementa la lógica
        return c2->test(this);
    }

    return false;
}

void Sphere::update(matrix4x4f mat) {
    // Actualizar el centro aplicando la matriz
    center = mat * centerOrigin;

    // Para el radio, vamos a usar el mayor factor de escala
    // de la matriz para escalarlo uniformemente
    vector4f scale = {
        length(vector4f{mat.mat2D[0][0], mat.mat2D[0][1], mat.mat2D[0][2], 0}),
        length(vector4f{mat.mat2D[1][0], mat.mat2D[1][1], mat.mat2D[1][2], 0}),
        length(vector4f{mat.mat2D[2][0], mat.mat2D[2][1], mat.mat2D[2][2], 0}),
        0
    };

    float maxScale = max(max(scale.x, scale.y), scale.z);
    radius = radiusOrigin * maxScale;

    // Actualizar recursivamente los hijos si existen
    for (auto& son : sons) {
        son->update(mat);
    }
}

void Sphere::subdivide() {
    // Si no hay suficientes partículas, no tiene sentido subdividir
    if (partList.size() <= 1) {
        return;
    }

    // Crear dos nuevos nodos hijos
    Sphere* leftSon = new Sphere();
    Sphere* rightSon = new Sphere();

    // Determinar el eje de mayor extensión para división
    vector4f size = getSize();
    vector4f center = getCenter();

    int axisToSplit = 0; // 0:x, 1:y, 2:z
    if (size.y > size.x && size.y > size.z) {
        axisToSplit = 1;
    }
    else if (size.z > size.x && size.z > size.y) {
        axisToSplit = 2;
    }

    // Repartir partículas entre los hijos según el eje elegido
    for (auto& part : partList) {
        vector4f partCenter = (part.min + part.max) * 0.5f;

        if (axisToSplit == 0) { // X axis
            if (partCenter.x <= center.x) {
                leftSon->addParticle(part);
            }
            else {
                rightSon->addParticle(part);
            }
        }
        else if (axisToSplit == 1) { // Y axis
            if (partCenter.y <= center.y) {
                leftSon->addParticle(part);
            }
            else {
                rightSon->addParticle(part);
            }
        }
        else { // Z axis
            if (partCenter.z <= center.z) {
                leftSon->addParticle(part);
            }
            else {
                rightSon->addParticle(part);
            }
        }
    }

    // Verificar que ambos hijos tienen partículas
    bool leftHasParticles = !leftSon->partList.empty();
    bool rightHasParticles = !rightSon->partList.empty();

    // Añadir o eliminar los hijos según corresponda
    if (leftHasParticles && rightHasParticles) {
        sons.push_back(leftSon);
        sons.push_back(rightSon);

        // Subdividir recursivamente los hijos si tienen múltiples partículas
        if (leftSon->partList.size() > 1) {
            leftSon->subdivide();
        }

        if (rightSon->partList.size() > 1) {
            rightSon->subdivide();
        }
    }
    else {
        // Si la subdivisión no fue efectiva, limpiar
        delete leftSon;
        delete rightSon;
    }
}

vector4f Sphere::getCenter() const {
    return center;
}

vector4f Sphere::getSize() const {
    return { radius * 2, radius * 2, radius * 2, 0 };
}

void Sphere::computeBoundingSphere() {
    if (partList.empty()) {
        center = { 0, 0, 0, 1 };
        radius = 0;
        return;
    }

    vector4f min = { numeric_limits<float>::max(),
                     numeric_limits<float>::max(),
                     numeric_limits<float>::max(), 1 };
    vector4f max = { -numeric_limits<float>::max(),
                     -numeric_limits<float>::max(),
                     -numeric_limits<float>::max(), 1 };

    for (const auto& part : partList) {
        // Para vértices y píxeles, usar solo part.min
        if (part.type == VERTEX_PARTICLE || part.type == PIXEL_PARTICLE) {
            min.x = std::min(min.x, part.min.x);
            min.y = std::min(min.y, part.min.y);
            min.z = std::min(min.z, part.min.z);
            max.x = std::max(max.x, part.min.x);
            max.y = std::max(max.y, part.min.y);
            max.z = std::max(max.z, part.min.z);
        }
        // Para triángulos, usar part.min y part.max (ya calculados al añadir el triángulo)
        else if (part.type == TRIANGLE_PARTICLE) {
            min.x = std::min(min.x, part.min.x);
            min.y = std::min(min.y, part.min.y);
            min.z = std::min(min.z, part.min.z);
            max.x = std::max(max.x, part.max.x);
            max.y = std::max(max.y, part.max.y);
            max.z = std::max(max.z, part.max.z);
        }
    }

    centerOrigin = (min + max) * 0.5f;
    centerOrigin.w = 1;
    radiusOrigin = distance(centerOrigin, max);
    center = centerOrigin;
    radius = radiusOrigin;
}

// AABB Implementation
AABB::AABB() {
    type = AABB_t;
    min = { 0, 0, 0, 1 };
    max = { 0, 0, 0, 1 };
    minOrigin = { 0, 0, 0, 1 };
    maxOrigin = { 0, 0, 0, 1 };
}

AABB::AABB(vector4f min, vector4f max)
    : min(min), max(max) {
    type = AABB_t;
    minOrigin = min;
    maxOrigin = max;
}

AABB::~AABB() {
    // Base class destructor will handle sons deletion
}

void AABB::addParticle(particle part) {
    partList.push_back(part);

    // Recalcular la caja envolvente
    computeBoundingBox();
}

bool AABB::test(Collider* c2) {
    if (c2->type == AABB_t) {
        AABB* aabb2 = static_cast<AABB*>(c2);

        // Comprobar si las cajas están colisionando
        bool result = (min.x <= aabb2->max.x && max.x >= aabb2->min.x) &&
            (min.y <= aabb2->max.y && max.y >= aabb2->min.y) &&
            (min.z <= aabb2->max.z && max.z >= aabb2->min.z);

        // Si tienen hijos y las cajas colisionan, verificar los hijos
        if (result) {
            if (!sons.empty() && !c2->sons.empty()) {
                // Ambos tienen subdivisiones, hay que comprobar hijos con hijos
                bool anyCollision = false;
                for (auto& son1 : sons) {
                    for (auto& son2 : c2->sons) {
                        anyCollision |= son1->test(son2);
                    }
                }
                return anyCollision;
            }
            else if (!sons.empty()) {
                // Solo yo tengo subdivisiones
                bool anyCollision = false;
                for (auto& son : sons) {
                    anyCollision |= son->test(c2);
                }
                return anyCollision;
            }
            else if (!c2->sons.empty()) {
                // Solo c2 tiene subdivisiones
                bool anyCollision = false;
                for (auto& son : c2->sons) {
                    anyCollision |= this->test(son);
                }
                return anyCollision;
            }
        }

        return result;
    }
    else if (c2->type == sphere) {
        Sphere* sph = static_cast<Sphere*>(c2);

        // Encontrar el punto más cercano de la caja al centro de la esfera
        vector4f closest;
        closest.x = std::max(min.x, std::min(sph->center.x, max.x));
        closest.y = std::max(min.y, std::min(sph->center.y, max.y));
        closest.z = std::max(min.z, std::min(sph->center.z, max.z));
        closest.w = 1;

        // Calcular distancia al cuadrado entre el centro de la esfera y el punto más cercano
        float dist = distance(closest, sph->center);

        // Hay colisión si la distancia es menor o igual al radio
        bool result = dist <= sph->radius;

        // Verificar hijos si hay colisión en el nivel actual
        if (result) {
            if (!sons.empty() && !c2->sons.empty()) {
                bool anyCollision = false;
                for (auto& son1 : sons) {
                    for (auto& son2 : c2->sons) {
                        anyCollision |= son1->test(son2);
                    }
                }
                return anyCollision;
            }
            else if (!sons.empty()) {
                bool anyCollision = false;
                for (auto& son : sons) {
                    anyCollision |= son->test(c2);
                }
                return anyCollision;
            }
            else if (!c2->sons.empty()) {
                bool anyCollision = false;
                for (auto& son : c2->sons) {
                    anyCollision |= this->test(son);
                }
                return anyCollision;
            }
        }

        return result;
    }

    return false;
}

void AABB::update(matrix4x4f mat) {
    // Para transformar una AABB correctamente con una matriz arbitraria,
    // hay que transformar los 8 vértices de la caja y luego recalcular la AABB

    // Obtener los 8 vértices de la caja original
    vector4f corners[8] = {
        {minOrigin.x, minOrigin.y, minOrigin.z, 1},
        {maxOrigin.x, minOrigin.y, minOrigin.z, 1},
        {minOrigin.x, maxOrigin.y, minOrigin.z, 1},
        {maxOrigin.x, maxOrigin.y, minOrigin.z, 1},
        {minOrigin.x, minOrigin.y, maxOrigin.z, 1},
        {maxOrigin.x, minOrigin.y, maxOrigin.z, 1},
        {minOrigin.x, maxOrigin.y, maxOrigin.z, 1},
        {maxOrigin.x, maxOrigin.y, maxOrigin.z, 1}
    };

    // Transformar los vértices
    for (int i = 0; i < 8; i++) {
        corners[i] = mat * corners[i];
    }

    // Reiniciar los límites
    min = { numeric_limits<float>::max(), numeric_limits<float>::max(), numeric_limits<float>::max(), 1 };
    max = { -numeric_limits<float>::max(), -numeric_limits<float>::max(), -numeric_limits<float>::max(), 1 };

    // Recalcular la AABB a partir de los vértices transformados
    for (int i = 0; i < 8; i++) {
        min.x = std::min(min.x, corners[i].x);
        min.y = std::min(min.y, corners[i].y);
        min.z = std::min(min.z, corners[i].z);

        max.x = std::max(max.x, corners[i].x);
        max.y = std::max(max.y, corners[i].y);
        max.z = std::max(max.z, corners[i].z);
    }

    // Actualizar recursivamente los hijos si existen
    for (auto& son : sons) {
        son->update(mat);
    }
}

void AABB::subdivide() {
    // Si no hay suficientes partículas, no tiene sentido subdividir
    if (partList.size() <= 1) {
        return;
    }

    // Crear dos nuevos nodos hijos
    AABB* leftSon = new AABB();
    AABB* rightSon = new AABB();

    // Determinar el eje de mayor extensión para división
    vector4f size = getSize();
    vector4f center = getCenter();

    int axisToSplit = 0; // 0:x, 1:y, 2:z
    if (size.y > size.x && size.y > size.z) {
        axisToSplit = 1;
    }
    else if (size.z > size.x && size.z > size.y) {
        axisToSplit = 2;
    }

    // Repartir partículas entre los hijos según el eje elegido
    for (auto& part : partList) {
        vector4f partCenter = (part.min + part.max) * 0.5f;

        if (axisToSplit == 0) { // X axis
            if (partCenter.x <= center.x) {
                leftSon->addParticle(part);
            }
            else {
                rightSon->addParticle(part);
            }
        }
        else if (axisToSplit == 1) { // Y axis
            if (partCenter.y <= center.y) {
                leftSon->addParticle(part);
            }
            else {
                rightSon->addParticle(part);
            }
        }
        else { // Z axis
            if (partCenter.z <= center.z) {
                leftSon->addParticle(part);
            }
            else {
                rightSon->addParticle(part);
            }
        }
    }

    // Verificar que ambos hijos tienen partículas
    bool leftHasParticles = !leftSon->partList.empty();
    bool rightHasParticles = !rightSon->partList.empty();

    // Añadir o eliminar los hijos según corresponda
    if (leftHasParticles && rightHasParticles) {
        sons.push_back(leftSon);
        sons.push_back(rightSon);

        // Subdividir recursivamente los hijos si tienen múltiples partículas
        if (leftSon->partList.size() > 1) {
            leftSon->subdivide();
        }

        if (rightSon->partList.size() > 1) {
            rightSon->subdivide();
        }
    }
    else {
        // Si la subdivisión no fue efectiva, limpiar
        delete leftSon;
        delete rightSon;
    }
}

vector4f AABB::getCenter() const {
    return (min + max) * 0.5f;
}

vector4f AABB::getSize() const {
    return max - min;
}

void AABB::computeBoundingBox() {
    if (partList.empty()) {
        min = { 0, 0, 0, 1 };
        max = { 0, 0, 0, 1 };
        return;
    }

    minOrigin = { numeric_limits<float>::max(),
                  numeric_limits<float>::max(),
                  numeric_limits<float>::max(), 1 };
    maxOrigin = { -numeric_limits<float>::max(),
                  -numeric_limits<float>::max(),
                  -numeric_limits<float>::max(), 1 };

    for (const auto& part : partList) {
        if (part.type == VERTEX_PARTICLE || part.type == PIXEL_PARTICLE) {
            minOrigin.x = std::min(minOrigin.x, part.min.x);
            minOrigin.y = std::min(minOrigin.y, part.min.y);
            minOrigin.z = std::min(minOrigin.z, part.min.z);
            maxOrigin.x = std::max(maxOrigin.x, part.min.x);
            maxOrigin.y = std::max(maxOrigin.y, part.min.y);
            maxOrigin.z = std::max(maxOrigin.z, part.min.z);
        }
        else if (part.type == TRIANGLE_PARTICLE) {
            minOrigin.x = std::min(minOrigin.x, part.min.x);
            minOrigin.y = std::min(minOrigin.y, part.min.y);
            minOrigin.z = std::min(minOrigin.z, part.min.z);
            maxOrigin.x = std::max(maxOrigin.x, part.max.x);
            maxOrigin.y = std::max(maxOrigin.y, part.max.y);
            maxOrigin.z = std::max(maxOrigin.z, part.max.z);
        }
    }

    min = minOrigin;
    max = maxOrigin;
}