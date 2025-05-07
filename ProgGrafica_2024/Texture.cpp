#include "libprgr/Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <GLFW/stb_image.h>
#include <iostream>

// Carga de textura a gráfica.
void Texture::loadFile(string fileName) 
{
    // Las texturas tienen cuatro canales de información (r, g, b, a).
    int channels = 4;

    // Carga esta información.
    unsigned char* data = stbi_load(fileName.c_str(), &w, &h, &channels, 4);

    if (data == nullptr || w == 0 || h == 0) 
    {
        cout << "ERROR: " << __FILE__ << ":" << __LINE__ << " (" << __func__ << ") Fallo al cargar la textura o textura no encontrada en: " << fileName << endl;
        pixels.clear();
        return;
    }

    else
    {
        pixels.resize(w * h);

        // Copia la textura y borra donde ya no se necesita.
        memcpy(pixels.data(), data, pixels.size() * sizeof(pixel_t));
        stbi_image_free(data);
    }
}

// Sube la textura a GPU.   
void Texture::updateGPU()
{
    // En el caso de ya no tener un id se le da uno.
    if (textureID == -1) 
		glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID); // Vincula la textura 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Establece el filtro de minificación, esto es lo que se usa cuando la textura es más pequeña que el área de textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Establece el filtro de magnificación, esto es lo que se usa cuando la textura es más grande que el área de textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Establece el modo de envoltura en el eje S (horizontal)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Establece el modo de envoltura en el eje T (vertical)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()); // Carga la textura en la GPU
}

// Bindea la textura a gráfica.
void Texture::bind(int textureUnit)
{
    if (textureID == -1 || pixels.empty()) 
    {
        cout << "ERROR: " << __FILE__ << ":" << __LINE__ << " (" << __func__ << ") Vinculando textura no válida" << endl;
        return;
    }

	glActiveTexture(GL_TEXTURE0 + textureUnit); // Activa la unidad de textura
	glBindTexture(GL_TEXTURE_2D, textureID); // Vincula la textura
}