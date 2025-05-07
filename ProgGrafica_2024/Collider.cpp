#include "libprgr/Collider.h"

#pragma region --- COLLIDER ESFÉRICO ---
Sphere::Sphere() {
    center = { 0, 0, 0, 1 };
    centerOrigin = { 0, 0, 0, 1 };
    radius = 0;
    radiusOrigin = 0;
}

Sphere::Sphere(vector4f center, float radius)
    : center(center), radius(radius) {
    centerOrigin = center;
    radiusOrigin = radius;
}

Sphere::~Sphere() {
    // Nada que liberar aquí
}

bool Sphere::test(Collider* other) {
    if (Sphere* sph2 = dynamic_cast<Sphere*>(other)) {
        // Verificar si las esferas están colisionando
        float dist = distance(center, sph2->center);
        return dist <= (radius + sph2->radius);
    }
    else if (AABB* aabb = dynamic_cast<AABB*>(other)) {
        // Para colisiones esfera-AABB, delegamos al otro objeto
        return aabb->test(this);
    }

    return false;
}

void Sphere::update(matrix4x4f mat) {
    // Actualizar el centro aplicando la matriz
    center = mat * centerOrigin;

    // Para el radio, usar el mayor factor de escala de la matriz
    vector4f scale = {
        length(vector4f{mat.mat2D[0][0], mat.mat2D[0][1], mat.mat2D[0][2], 0}),
        length(vector4f{mat.mat2D[1][0], mat.mat2D[1][1], mat.mat2D[1][2], 0}),
        length(vector4f{mat.mat2D[2][0], mat.mat2D[2][1], mat.mat2D[2][2], 0}),
        0
    };

    float maxScale = max(max(scale.x, scale.y), scale.z);
    radius = radiusOrigin * maxScale;
}

vector4f Sphere::getCenter() const {
    return center;
}

vector4f Sphere::getSize() const {
    return { radius * 2, radius * 2, radius * 2, 0 };
}

void Sphere::computeBoundingSphere() {
    // Esta función ahora debe implementarse en BoundingVolume
    // Se mantiene vacía aquí para compatibilidad
}
#pragma endregion

#pragma region COLLIDER AABB (CAJAS) ---
AABB::AABB() {
    min = { 0, 0, 0, 1 };
    max = { 0, 0, 0, 1 };
    minOrigin = { 0, 0, 0, 1 };
    maxOrigin = { 0, 0, 0, 1 };
}

AABB::AABB(vector4f min, vector4f max)
    : min(min), max(max) {
    minOrigin = min;
    maxOrigin = max;
}

AABB::~AABB() {
    // Nada que liberar aquí
}

bool AABB::test(Collider* other) {
    if (AABB* aabb2 = dynamic_cast<AABB*>(other)) {
        // Comprobar si las cajas están colisionando
        return (min.x <= aabb2->max.x && max.x >= aabb2->min.x) &&
            (min.y <= aabb2->max.y && max.y >= aabb2->min.y) &&
            (min.z <= aabb2->max.z && max.z >= aabb2->min.z);
    }
    else if (Sphere* sph = dynamic_cast<Sphere*>(other)) {
        // Encontrar el punto más cercano de la caja al centro de la esfera
        vector4f closest;
        closest.x = std::max(min.x, std::min(sph->center.x, max.x));
        closest.y = std::max(min.y, std::min(sph->center.y, max.y));
        closest.z = std::max(min.z, std::min(sph->center.z, max.z));
        closest.w = 1;

        // Calcular distancia entre el centro de la esfera y el punto más cercano
        float dist = distance(closest, sph->center);

        // Hay colisión si la distancia es menor o igual al radio
        return dist <= sph->radius;
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
}

vector4f AABB::getCenter() const {
    return (min + max) * 0.5f;
}

vector4f AABB::getSize() const {
    return max - min;
}

void AABB::computeBoundingBox() {
    // Esta función ahora debe implementarse en BoundingVolume
    // Se mantiene vacía aquí para compatibilidad
}
#pragma endregion

#pragma region --- COLLIDER BOUNDING VOLUME (JARAQUICO) ---

BoundingVolume::BoundingVolume() :
    type(SPHERE_TYPE),
    leftChild(nullptr),
    rightChild(nullptr),
    isLeaf(true) {
    collider.sphere = new Sphere();
}

BoundingVolume::BoundingVolume(ColliderType type) :
    type(type),
    leftChild(nullptr),
    rightChild(nullptr),
    isLeaf(true) {
    if (type == SPHERE_TYPE) {
        collider.sphere = new Sphere();
    }
    else {
        collider.aabb = new AABB();
    }
}

BoundingVolume::~BoundingVolume() {
    // Liberar memoria del colisionador
    if (type == SPHERE_TYPE) {
        delete collider.sphere;
    }
    else {
        delete collider.aabb;
    }

    // Liberar los hijos recursivamente
    if (leftChild) {
        delete leftChild;
    }
    if (rightChild) {
        delete rightChild;
    }
}

void BoundingVolume::addParticle(particle part) {
    partList.push_back(part);

    // Recalcular el volumen envolvente
    if (type == SPHERE_TYPE) {
        computeBoundingSphere();
    }
    else {
        computeBoundingBox();
    }
}

void BoundingVolume::clearParticles() {
    partList.clear();
}

bool BoundingVolume::test(BoundingVolume* other) {
    // Comprobar colisión entre los colisionadores principales
    bool collision = false;

    if (type == SPHERE_TYPE && other->type == SPHERE_TYPE) {
        collision = collider.sphere->test(other->collider.sphere);
    }
    else if (type == SPHERE_TYPE && other->type == AABB_TYPE) {
        collision = collider.sphere->test(other->collider.aabb);
    }
    else if (type == AABB_TYPE && other->type == SPHERE_TYPE) {
        collision = collider.aabb->test(other->collider.sphere);
    }
    else if (type == AABB_TYPE && other->type == AABB_TYPE) {
        collision = collider.aabb->test(other->collider.aabb);
    }

    // Si no hay colisión, terminar
    if (!collision) {
        return false;
    }

    // Si hay colisión y ambos son hojas, hay colisión
    if (isLeaf && other->isLeaf) {
        return true;
    }

    // Si hay colisión y al menos uno tiene hijos, comprobar los hijos
    if (!isLeaf && !other->isLeaf) {
        // Ambos tienen hijos, comprobar todos contra todos
        bool anyCollision = false;
        anyCollision |= leftChild->test(other->leftChild);
        anyCollision |= leftChild->test(other->rightChild);
        anyCollision |= rightChild->test(other->leftChild);
        anyCollision |= rightChild->test(other->rightChild);
        return anyCollision;
    }
    else if (!isLeaf) {
        // Solo este tiene hijos
        return leftChild->test(other) || rightChild->test(other);
    }
    else {
        // Solo other tiene hijos
        return other->leftChild->test(this) || other->rightChild->test(this);
    }
}

void BoundingVolume::update(matrix4x4f mat) {
    // Actualizar el colisionador concreto
    if (type == SPHERE_TYPE) {
        collider.sphere->update(mat);
    }
    else {
        collider.aabb->update(mat);
    }

    // Actualizar los hijos recursivamente
    if (!isLeaf) {
        leftChild->update(mat);
        rightChild->update(mat);
    }
}

void BoundingVolume::buildHierarchy() {
    // Iniciar la subdivisión recursiva
    if (partList.size() > 1) {
        subdivide(this);
    }
}

void BoundingVolume::subdivide(BoundingVolume* node) {
    // Si no hay suficientes partículas, no tiene sentido subdividir
    if (node->partList.size() <= 1) {
        return;
    }

    // Determinar el eje de mayor extensión para división
    vector4f size = node->getSize();
    vector4f center = node->getCenter();

    int axisToSplit = 0; // 0:x, 1:y, 2:z
    if (size.y > size.x && size.y > size.z) {
        axisToSplit = 1;
    }
    else if (size.z > size.x && size.z > size.y) {
        axisToSplit = 2;
    }

    // Crear nodos hijos
    node->leftChild = new BoundingVolume(node->type);
    node->rightChild = new BoundingVolume(node->type);

    // Repartir partículas entre los hijos según el eje elegido
    for (auto& part : node->partList) {
        vector4f partCenter = (part.min + part.max) * 0.5f;

        if (axisToSplit == 0) { // X axis
            if (partCenter.x <= center.x) {
                node->leftChild->addParticle(part);
            }
            else {
                node->rightChild->addParticle(part);
            }
        }
        else if (axisToSplit == 1) { // Y axis
            if (partCenter.y <= center.y) {
                node->leftChild->addParticle(part);
            }
            else {
                node->rightChild->addParticle(part);
            }
        }
        else { // Z axis
            if (partCenter.z <= center.z) {
                node->leftChild->addParticle(part);
            }
            else {
                node->rightChild->addParticle(part);
            }
        }
    }

    // Verificar que ambos hijos tienen partículas
    bool leftHasParticles = !node->leftChild->partList.empty();
    bool rightHasParticles = !node->rightChild->partList.empty();

    if (leftHasParticles && rightHasParticles) {
        // Marcamos que este nodo ya no es una hoja
        node->isLeaf = false;

        // Subdividir recursivamente los hijos si tienen múltiples partículas
        if (node->leftChild->partList.size() > 1) {
            subdivide(node->leftChild);
        }

        if (node->rightChild->partList.size() > 1) {
            subdivide(node->rightChild);
        }
    }
    else {
        // Si la subdivisión no fue efectiva, limpiar
        delete node->leftChild;
        delete node->rightChild;
        node->leftChild = nullptr;
        node->rightChild = nullptr;
        node->isLeaf = true;
    }
}

vector4f BoundingVolume::getCenter() const {
    if (type == SPHERE_TYPE) {
        return collider.sphere->getCenter();
    }
    else {
        return collider.aabb->getCenter();
    }
}

vector4f BoundingVolume::getSize() const {
    if (type == SPHERE_TYPE) {
        return collider.sphere->getSize();
    }
    else {
        return collider.aabb->getSize();
    }
}

void BoundingVolume::computeBoundingSphere() {
    if (partList.empty()) {
        collider.sphere->center = { 0, 0, 0, 1 };
        collider.sphere->radius = 0;
        collider.sphere->centerOrigin = { 0, 0, 0, 1 };
        collider.sphere->radiusOrigin = 0;
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
        // Para triángulos, usar part.min y part.max
        else if (part.type == TRIANGLE_PARTICLE) {
            min.x = std::min(min.x, part.min.x);
            min.y = std::min(min.y, part.min.y);
            min.z = std::min(min.z, part.min.z);
            max.x = std::max(max.x, part.max.x);
            max.y = std::max(max.y, part.max.y);
            max.z = std::max(max.z, part.max.z);
        }
    }

    collider.sphere->centerOrigin = (min + max) * 0.5f;
    collider.sphere->centerOrigin.w = 1;
    collider.sphere->radiusOrigin = distance(collider.sphere->centerOrigin, max);
    collider.sphere->center = collider.sphere->centerOrigin;
    collider.sphere->radius = collider.sphere->radiusOrigin;
}

void BoundingVolume::computeBoundingBox() {
    if (partList.empty()) {
        collider.aabb->min = { 0, 0, 0, 1 };
        collider.aabb->max = { 0, 0, 0, 1 };
        collider.aabb->minOrigin = { 0, 0, 0, 1 };
        collider.aabb->maxOrigin = { 0, 0, 0, 1 };
        return;
    }

    collider.aabb->minOrigin = { numeric_limits<float>::max(),
                                numeric_limits<float>::max(),
                                numeric_limits<float>::max(), 1 };
    collider.aabb->maxOrigin = { -numeric_limits<float>::max(),
                                -numeric_limits<float>::max(),
                                -numeric_limits<float>::max(), 1 };

    for (const auto& part : partList) {
        if (part.type == VERTEX_PARTICLE || part.type == PIXEL_PARTICLE) {
            collider.aabb->minOrigin.x = std::min(collider.aabb->minOrigin.x, part.min.x);
            collider.aabb->minOrigin.y = std::min(collider.aabb->minOrigin.y, part.min.y);
            collider.aabb->minOrigin.z = std::min(collider.aabb->minOrigin.z, part.min.z);
            collider.aabb->maxOrigin.x = std::max(collider.aabb->maxOrigin.x, part.min.x);
            collider.aabb->maxOrigin.y = std::max(collider.aabb->maxOrigin.y, part.min.y);
            collider.aabb->maxOrigin.z = std::max(collider.aabb->maxOrigin.z, part.min.z);
        }
        else if (part.type == TRIANGLE_PARTICLE) {
            collider.aabb->minOrigin.x = std::min(collider.aabb->minOrigin.x, part.min.x);
            collider.aabb->minOrigin.y = std::min(collider.aabb->minOrigin.y, part.min.y);
            collider.aabb->minOrigin.z = std::min(collider.aabb->minOrigin.z, part.min.z);
            collider.aabb->maxOrigin.x = std::max(collider.aabb->maxOrigin.x, part.max.x);
            collider.aabb->maxOrigin.y = std::max(collider.aabb->maxOrigin.y, part.max.y);
            collider.aabb->maxOrigin.z = std::max(collider.aabb->maxOrigin.z, part.max.z);
        }
    }

    collider.aabb->min = collider.aabb->minOrigin;
    collider.aabb->max = collider.aabb->maxOrigin;
}
#pragma endregion