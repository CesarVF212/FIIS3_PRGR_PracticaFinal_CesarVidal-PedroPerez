#include "libprgr/Shader.h"

Shader::Shader(string fileName) : fileName(fileName)
{
	if (fileName.ends_with(".vert")) {
		shaderType = GL_VERTEX_SHADER;
	}
	else if (fileName.ends_with(".frag")) {
		shaderType = GL_FRAGMENT_SHADER;
	}
	else {
		cout << "Error: Tipo de shader no soportado" << endl;
	}

	readSource();
	compileShader();
	checkErrors();
}

void Shader::readSource()
{
	ifstream f(fileName);
	if (f.is_open()) {
		source = string(istreambuf_iterator<char>(f), {});
	}
	else {
		cout << "ERROR: FICHERO NO ENCONTRADO " << __FILE__ << ":" << __LINE__ << " " << fileName << "\n";
	}
}

void Shader::compileShader()
{
	const char* shaderCode = source.c_str(); // Convertir el string a un puntero de char para pasarlo a OpenGL
	shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &shaderCode, NULL);
	glCompileShader(shaderId);
}

void Shader::checkErrors()
{
	GLint retCode;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &retCode);
	if (retCode != GL_TRUE)
	{
		GLsizei log_length = 0;
		GLchar infoLog[1024];
		glGetShaderInfoLog(shaderId, 1024, &log_length, infoLog);
		cout << "ERROR " << fileName << "\n" << infoLog << "\n\n";
	}
}

void Shader::clean()
{
	glDeleteShader(shaderId);
}