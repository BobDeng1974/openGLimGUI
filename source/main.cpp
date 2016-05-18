#pragma once
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "imgui\imgui.h"
#include "Program.h"

static void error_callback(int error, const char* description);
void init();

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

void versionPrint(){
	const GLubyte* renderer = glGetString(GL_RENDERER); 
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);
}

int main(int, char**)
{
	
	if (!glfwInit()){ std::cout << "ERROR: glfwInit failed\n"; return -1; }

	GLFWwindow* window = glfwCreateWindow(640, 480, "mesh", NULL, NULL);
	if (!window){ std::cout << "ERROR: window was not created\n"; return -1; }
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	versionPrint();

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);


	float vertices[] = {
		-0.5,  -0.5,	0,
		-0.5,	0.5,	0,
		 0.5,	0.5,	0,
       /////////////////////
	    -0.5,	-0.5,	0,
		 0.5,	0.5,	0,
		 0.5,   -0.5,	0
	};

	Program shaderProgram;
	Shader vshader("shaders//vertexShader.glsl",	GL_VERTEX_SHADER);
	Shader fshader("shaders//fragmentShader.glsl",	GL_FRAGMENT_SHADER);
	shaderProgram.attachShader(&vshader);
	shaderProgram.attachShader(&fshader);
	shaderProgram.linkProgram();

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), vertices, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	while (!glfwWindowShouldClose(window)){

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderProgram.use();
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 18);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}