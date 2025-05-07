#pragma once
#include <string>
#include "common.h"

using namespace std;

class Texture {
public:
	
    // Identificador de la textura.
    unsigned int textureID = -1;

    // Ancho y alto de la textura.
    int w = 0; 
    int h = 0; 

    // Información de un pixel. Su color.
    typedef struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    } pixel_t;

    vector<pixel_t> pixels; 

    // Archivo de la textura.
	string fileName = ""; 

	
    // Constructor.
    Texture(string fileName) : fileName(fileName) { 
        loadFile(fileName);
        updateGPU();
    };

    // Carga una textura desde un archivo.
    void loadFile(string fileName);

    // Sube la textura a gráfica.
    void updateGPU();

    // Bindea la textura a gráfica.
    void bind(int textureUnit);
};