#pragma once
#include "common.h"

// Clase Shader: Encargada de manejar los shaders (vertex y fragment shaders)
class Shader
{
public:
    unsigned int shaderId; // ID del shader generado por OpenGL
    string fileName;       // Nombre del archivo del shader
    GLenum shaderType;     // Tipo de shader (GL_VERTEX_SHADER o GL_FRAGMENT_SHADER)
    string source;         // Código fuente del shader

    // Constructor: Inicializa el shader con el nombre del archivo
    Shader(string fileName);

    // Lee el código fuente del shader desde un archivo
    void readSource();

    // Compila el shader en OpenGL
    void compileShader();

    // Limpia y elimina el shader de OpenGL
    void clean();

    // Verifica si hubo errores durante la compilación del shader
    void checkErrors();
};