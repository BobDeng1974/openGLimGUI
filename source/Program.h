#pragma once
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

class Program
{
public:
	Program();
	~Program();

	void attachShader(Shader* shdr);
	void use();
	void linkProgram();
	int getID() {
		return programID;
	}

private:

	int programID = 0;
	bool hasShader = false;

};


