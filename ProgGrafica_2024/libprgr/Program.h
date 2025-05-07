#pragma once
#include "common.h"
#include "Shader.h"
#include "Light.h"

class Program {
public:

	typedef enum
	{
		integer,
		floatpoint,
		vector4,
		matrix4,
		light
	} dataType_e;

	// Atributos
	unsigned int idProgram = -1; // Identificador de programa
	vector<Shader*> shaderList; // Lista de shaders que componen el programa
	map<string, unsigned int > varList; // Lista de variables del programa

	string attributeVertPos; // Nombre del atributo de posicion
	string attributeVertColor; // Nombre del atributo de color
	string uniformMVP; // Nombre del uniform de matriz MVP

	// Métodos
	Program(); // Constructor
	void addShader(string fileName); // Agrega un shader al programa
	void loadDescription(string fileName); // Carga la descripcion del programa, es decir los nombres de las variables y atributos

	// Métodos para obtener la localización de los atributos y uniforms, const porque no modifican el estado del objeto
	GLint getAttributeVertColorLocation() const; 
	GLint getAttributeVertNormLocation() const; 
	GLint getAttributeVertTextureCoordLocation() const; 
	GLint getUniformMLocation() const;
	GLint getUniformMVPLocation() const; 
	GLint getAttributeVertPosLocation() const;

	void linkProgram(); // Linkea el programa
	void checkErrors(); // Revisa si hubo errores al linkear el programa
	void clean(); // Limpia el programa
	void readVarList(); // Lee las variables del programa
	void use(); // Usa el programa

	void setUniformData(dataType_e tipo, void* dato, string nombre); // Establece datos de GPU de tipo Uniform, los de tipo uniform son variables que se envian al shader y no cambian durante el ciclo de vida del shader
	void setAttributeData(string nombre, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer); // Establecen datos de GPU de tipo Attribute, los de tipo attribute son variables que se envian al shader y cambian durante el ciclo de vida del shader
	void setLightUniform(const Light& light, int index); // Establece los datos de la luz en el shader
};

