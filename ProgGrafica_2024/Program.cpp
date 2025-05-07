#include "libprgr/Program.h"

Program::Program() 
{
	idProgram = glCreateProgram();
	if (idProgram == -1) {
		cout << "ERROR: " << __FILE__ << ":" << __LINE__ << " (" << __func__ << ") Error al crear el programa" << endl;
	}
}

void Program::addShader(string fileName) 
{
	Shader* shader = new Shader(fileName);
	glAttachShader(idProgram, shader->shaderId);
	shaderList.push_back(shader);
	shader->clean();
}

void Program::loadDescription(string fileName) 
{
	ifstream file(fileName);
	if(!file.is_open()) {
		cout << "ERROR: " << __FILE__ << ":" << __LINE__ << " (" << __func__ << ") Error al abrir el archivo de " << fileName << endl;
		return;
	}
	string line;
	while (getline(file, line)) {
		size_t equalsPos = line.find('='); // Posicion del igual para separar nombre de variable, y valor
		if (equalsPos != string::npos) { // Si no se encuentra el igual, no se hace nada, npos es un valor que indica que no se encontro
			string key = line.substr(0, equalsPos); // Nombre de la variable, cogemos desde el principio hasta el igual
			string varValue = line.substr(equalsPos + 1); // Valor de la variable, cogemos desde el igual hasta el final
			if (key == "attributeVertPos") {
				attributeVertPos = varValue; // Guardamos el valor de la variable en attributeVertPos
			}
			else if (key == "attributeVertColor") {
				attributeVertColor = varValue; 
			}
			else if (key == "uniformMVP") {
				uniformMVP = varValue;
			}

		}
	}
}

GLint Program::getAttributeVertPosLocation() const 
{
	return glGetAttribLocation(idProgram, attributeVertPos.c_str());
}

GLint Program::getUniformMVPLocation() const 
{
	return glGetUniformLocation(idProgram, uniformMVP.c_str());
}

GLint Program::getAttributeVertColorLocation() const 
{
	GLint loc = glGetAttribLocation(idProgram, attributeVertColor.c_str()); //DEBUG
	if (loc == -1) {
		std::cerr << "ERROR: " << __FILE__ << ":" << __LINE__ << " (" << __func__ << ") No se pudo encontrar el color del v" << std::endl;

	}
	return loc;
}

GLint Program::getAttributeVertNormLocation() const 
{
	return glGetAttribLocation(idProgram, "vNorm");
}

GLint Program::getAttributeVertTextureCoordLocation() const 
{
	return glGetAttribLocation(idProgram, "vTextureCoord");
}

GLint Program::getUniformMLocation() const 
{
	return glGetUniformLocation(idProgram, "M");
}




void Program::linkProgram() 
{
	glLinkProgram(idProgram);
	checkErrors();
	readVarList();
}

void Program::checkErrors() 
{
	GLint retCode;
	glGetProgramiv(idProgram, GL_LINK_STATUS, &retCode);
	if (retCode != GL_TRUE) {
		GLsizei log_length = 0;
		GLchar infoLog[1024];
		glGetProgramInfoLog(idProgram, 1024, &log_length, infoLog);
		cout << "ERROR: " << __FILE__ << ":" << __LINE__ << " (" << __func__ << ") " << "\n" << infoLog << "\n\n";

	}

}

void Program::clean() {
	for (auto shader : shaderList) {
		glDetachShader(idProgram, shader->shaderId);
		shader->clean();
	}
	glDeleteProgram(idProgram);
}

void Program::readVarList() 
{
	int numAttributes = 0;
	int numUniforms = 0;
	glGetProgramiv(idProgram, GL_ACTIVE_ATTRIBUTES, &numAttributes);
	for (int i = 0; i < numAttributes; i++)
	{
		char varName[100];
		int bufSize = 100, length = 0, size = 0;
		GLenum type = -1;
		glGetActiveAttrib(idProgram, (GLuint)i, bufSize, &length, &size, &type, varName);
		varList[std::string(varName)] = glGetAttribLocation(this->idProgram, varName);

	}
	glGetProgramiv(this->idProgram, GL_ACTIVE_UNIFORMS, &numUniforms);
	for (int i = 0; i < numUniforms; i++)
	{
		string varName; varName.resize(100);
		int bufSize = 100, length = 0, size = 0;
		GLenum type = -1;
		glGetActiveUniform(idProgram, (GLuint)i, bufSize, &length, &size, &type, varName.data());
		varName = std::string(varName.c_str()); 
		if (varName[varName.length() - 1] == ']') {
			std::string arrName = varName.substr(0, varName.find('['));
			for (int i = 0; i < size; i++) 
			{
				std::string arrNameIdx = arrName + "[" + std::to_string(i) + "]";
				varList[arrNameIdx] = glGetUniformLocation(idProgram, arrNameIdx.c_str());
			}
		}
		else
			varList[varName] = glGetUniformLocation(idProgram, varName.c_str());
	}
}

void Program::use() 
{
	glUseProgram(idProgram);
}

void Program::setUniformData(dataType_e tipo, void* dato, string nombre) {
	if (varList.find(nombre) == varList.end())
	{
		cout << "ERROR: Variable no encontrada " << __FILE__ << ":" << __LINE__ << " (" << __func__ << ") " << nombre << "\n";

	}
	else
	{
		switch (tipo)
		{
		case matrix4:
		{
			glUniformMatrix4fv(varList[nombre], 1, GL_TRUE, (float*)dato);
		} break;
		case integer:
		{
			glUniform1i(varList[nombre], ((int*)dato)[0]);
		} break;
		case floatpoint:
		{
			glUniform1f(varList[nombre], ((float*)dato)[0]);
		} break;
		case vector4:
		{
			//glUniform4f(varList[nombre], ((float*)dato)[0], ((float*)dato)[1], ((float*)dato)[2], ((float*)dato)[3]); VALIDA TAMBIEN
			glUniform4fv(varList[nombre], 1, (float*)dato);
		} break;

		default:
			cout << "ERROR: Tipo de dato no soportado " << __FILE__ << ":" << __LINE__ << " (" << __func__ << ") " << nombre << "\n";

			break;
		}
	}
}

void Program::setAttributeData(string nombre, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer) {
	if (varList.find(nombre) == varList.end())
	{
		cout << "ERROR: Variable no encontrada " << __FILE__ << ":" << __LINE__ << " (" << __func__ << ") " << nombre << "\n";
	}
	else
	{
		unsigned int index = varList[nombre];
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	}
}

void Program::setLightUniform(const Light& light, int index) { // funcion para establecer los datos de la luz en el shader
	string base = "uLights[" + std::to_string(index) + "]"; // base de la variable, ejemplo: uLights[0]
	glUniform1i(varList[base + ".type"], light.type); // tipo de luz
	glUniform4fv(varList[base + ".position"], 1, (light.position.data)); // posicion de la luz
	glUniform4fv(varList[base + ".color"], 1, (light.color.data)); // color de la luz
	glUniform1f(varList[base + ".intensity"], light.i); // intensidad de la luz
	glUniform4fv(varList[base + ".direction"], 1, (light.direction.data));	// direccion de la luz
}
