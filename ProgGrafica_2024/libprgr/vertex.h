#pragma once
#include "vectorMath.h"

using namespace libPRGR;

typedef struct {
	vector4f vPos;
	vector4f vColor;
	vector4f vNormal;
	vector4f vTextureCoord;
}vertex_t; // Estructura de un vertice con posicion y color