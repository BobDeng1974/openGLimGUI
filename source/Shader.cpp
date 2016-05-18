#include "shader.h"

Shader::Shader(std::string path, int shaderType)
{
	loadSourceFromFile(path, shaderType);
	bool success = compileShader();
}


Shader::~Shader()
{
}

void Shader::loadSourceFromFile(std::string path, int shaderType)
{

	std::ifstream in(path);
	std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	source = s;
	this->shaderType = shaderType;
}

bool Shader::compileShader()
{
	bool success = true;

	shaderID = glCreateShader(shaderType);

	const char *c_str = source.c_str();
	glShaderSource(shaderID, 1, &c_str, 0);
	glCompileShader(shaderID);

	GLint isCompiled;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE){
		GLint len = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
		GLchar* cstr = new GLchar[len];
		glGetShaderInfoLog(shaderID, len, &len, cstr);
		std::string log(cstr);
		std::cout << "Shader ID: " << shaderID << " -> failed\n" "======================\n" << log << std::endl;
		delete[] cstr;
		success = false;
	}
	else{
		compiled = true;
		std::cout << "Shader ID: " << shaderID << " -> compiled\n";
	}

	return success;
}

int Shader::getShaderID()
{
	return shaderID;
}

int Shader::getShaderType()
{
	return shaderType;
}

std::string Shader::getShaderSource()
{
	return source;
}