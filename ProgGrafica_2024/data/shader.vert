#version 330 core

attribute vec4 vPos;         // Posici�n del v�rtice (x,y,z,w)
attribute vec4 vColor;       // Color RGBA
attribute vec4 vNormal;      // Normal del v�rtice (x,y,z,w)
attribute vec4 vTextureCoord;  // Coordenadas de textura (4 componentes, aunque solo usaremos x,y)

// Matrices de transformaci�n:
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

// Variables de salida hacia el fragment shader:
out vec4 fColor;      
out vec4 fNormal;     
out vec4 fFragPos;    
out vec4 fTextureCoord;   

void main() {
    // Transformaci�n de posici�n:
    vec4 worldPos = uModel * vPos;
    
    // Posici�n final en clip space:
    gl_Position = uProjection * uView * worldPos;

    // Pasamos datos al fragment shader:
    fFragPos = worldPos;
    
    // Transformaci�n de normales:
    // Usamos la matriz normal (transpuesta de la inversa) para mantener ortogonalidad
    fNormal = transpose(inverse(uModel)) * vNormal;
    fNormal.w = 0.0; // Aseguramos que w sea 0 para que sea un vector y no un punto
    
    // Datos directos:
    fColor = vColor;
    fTextureCoord = vTextureCoord;
}