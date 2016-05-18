#include "Program.h"
#include "shader.h"


Program::Program()
{
	programID = glCreateProgram();
}

Program::~Program()
{
}

void Program::attachShader(Shader* shdr)
{
	glAttachShader(programID, shdr->getShaderID());
	hasShader = true;
}

void Program::use()
{
	if (hasShader)
		glUseProgram(programID);
}

void Program::linkProgram()
{
	if (hasShader)
		glLinkProgram(programID);
}