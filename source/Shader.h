#pragma once
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>

class Shader
{
public:

	Shader(std::string path, int shaderType);
	~Shader();
	
	void loadSourceFromFile(std::string path, int shaderType);
	bool compileShader();

	int getShaderID();
	int getShaderType();
	std::string getShaderSource();

	
private:

	std::string	source;
	
	int	shaderType;
	int	shaderID = 0;

	bool sourceLoaded = false;
	bool compiled = false;

};

