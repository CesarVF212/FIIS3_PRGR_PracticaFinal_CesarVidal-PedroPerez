# FIIS_PRGR_PracticaFinal_CesarVidal-PedroPerez. Sistema de Colisiones 3D.

## Descripción del Proyecto
Este proyecto implementa un sistema de detección de colisiones utilizando volúmenes envolventes para objetos 3D en un motor gráfico. El sistema permite detectar colisiones entre diferentes objetos en la escena y aplicar comportamientos en consecuencia.

## Clases Principales

### Clase Collider
Clase base que representa un colisionador genérico:
```cpp
class Collider {
public:
    // Método obligatorio para detectar colisiones
    virtual bool checkCollision(Collider* other) = 0;
    
    // Método opcional para implementar jerarquía de volúmenes envolventes
    virtual void subdivide() = 0; // Opcional
};
```

### Clase Sphere
Implementación específica para colisiones basadas en esferas:
- **Características**:
  - Representada por un centro y un radio
  - El centro se calcula como la media de los máximos y mínimos en XYZ de todas las partículas
  - El radio se calcula para contener todas las partículas
- **Actualización**: Se aplican transformaciones (traslación, rotación y escalado) tanto al centro como al radio

### Clase Object3D
Modificaciones para soportar colisiones:
- Nuevo atributo: `Collider* coll`
- **Representación de partículas**:
  - Objetos 3D: Pueden usar vértices individuales o agrupaciones de 3 vértices (triángulos)
  - Objetos 2D: Cada píxel no transparente de la textura representa una partícula
- Nuevo método `updateCollider()` para actualizar la matriz modelo y el colisionador

### Clase Render
Modificaciones:
- Cambio de lista de objetos a mapa estático indexado por ID:
  ```cpp
  inline static std::map<int, Object3D*> objectList;
  ```
- Método `putObject` modificado para ser estático y recibir un ID:
  ```cpp
  static void putObject(int ID, Object3D* obj);
  ```
- Nuevo método estático para búsqueda de objetos por ID:
  ```cpp
  static Object3D* getObject(int ID);
  ```
- Actualización de colisionadores antes de mover objetos en `mainLoop()`

### Clase Camera
Modificaciones:
- Nuevo atributo: `Collider* coll` (tamaño 0.25×0.25×0.25 unidades)
- Actualización del método `step()` para:
  - Actualizar el colisionador con la nueva posición
  - Detectar colisiones con objetos de la escena
  - Retroceder en caso de colisión

## Implementación Básica (5 puntos)
- Carga de un cubo 3D en la posición (0,0,0)
- Punto de luz en la posición (3,3,3)
- Cámara en la posición (0,0,3)
- Detección de colisiones entre cámara y cubo
- Uso de una esfera envolvente para cada objeto
- Las esferas se crean usando partículas de tamaño 0.01×0.01×0.01 en la posición de cada vértice

## Implementación Avanzada (+5 puntos)
- Jerarquía de volúmenes envolventes
- Implementación del escenario "naves2D":
  - Clases Enemy, Bullet y SpaceShip
  - Partículas a nivel de píxel no transparente
- Subdivisión de esferas en 4 subesferas hasta llegar a un píxel por hoja

## Evaluación
La práctica se evaluará según los siguientes criterios:

### Aprobado (5 puntos)
- Sistema de colisiones básico (sin jerarquía)
- Detección de colisiones entre cubo y cámara
- Renderizado con shaders vistos en clase

### Extras (hasta +5 puntos)
1. **Detección de colisiones óptima** (hasta 2 puntos)
   - Jerarquías de volúmenes envolventes
   - Adaptación para mallas 3D o texturas 2D
   - Uso de cajas alineadas con los ejes donde sea necesario

2. **Efectos físicos y modos de juego** (hasta 2 puntos)
   - Implementación de aplicación gráfica compleja
   - Se evaluará la complejidad de la aplicación

3. **Assets propios y herramientas** (hasta 2 puntos)
   - Uso de plugins para exportar modelos
   - Desarrollo de herramientas de importación/exportación

4. **Efectos gráficos avanzados** (hasta 2 puntos extra)
   - Implementación de shaders GLSL no vistos en clase
   - Ejemplos: Toon/Cel shading, Fur shader, generación procedural

## Entrega
- Entregar código a través de Blackboard antes de la fecha límite
- Penalización del 50% por entregas tardías
- Formato del archivo: `PRG6_NOMBREAPELLIDO1_NOMBREAPELLIDO2.zip`
- La práctica debe defenderse ante el profesor
