#pragma once
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include "imgui\imgui.h"
#include "Program.h"
#include "GUI.h"
#include "tinyobjloader\tiny_obj_loader.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static void error_callback(int error, const char* description);

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

	int width = 900;
	int height = 900;
	
	if (!glfwInit()){ std::cout << "ERROR: glfwInit failed\n"; return -1; }

	GLFWwindow* window = glfwCreateWindow(width, height, "mesh", NULL, NULL);
	if (!window){ std::cout << "ERROR: window was not created\n"; return -1; }
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	versionPrint();

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	GUI gui;
	gui.Init(window, false);

	float vertices[] = {
		-0.5,  -0.5,	0,
		-0.5,	0.5,	0,
		 0.5,	0.5,	0,
       /////////////////////
	    -0.5,	-0.5,	0,
		 0.5,	0.5,	0,
		 0.5,   -0.5,	0
	};
	
	//////////////////////////////////
	//OBJECT LOADING
	std::string resourcePath = "../../../resources/";
	//std::string inputfile = resourcePath + "teapot/teapot.obj";
	//std::string mtlfile = resourcePath + "teapot/teapot.mtl";
	//std::string inputfile = resourcePath + "sphere/sphere.obj";
	//std::string mtlfile = resourcePath + "sphere/sphere.mtl";
	//std::string inputfile = resourcePath + "dragon/dragon.obj";
	std::string inputfile = resourcePath + "monkey/monkey.obj";
	//std::string mtlfile = resourcePath + "monkey/monkey.mtl";
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(shapes, materials, err, inputfile.c_str()))
		std::cerr << err << std::endl;
		
	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	//OBJECT LOADING
	//////////////////////////////////

	glm::vec3 cpos = glm::vec3(0.0f, 0.0f, 10.0f);

	glm::mat4 view;
	view = glm::lookAt(	cpos,
						glm::vec3(0.0f, 0.0f, 0.0f),
						glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 projection = glm::perspective(45.0f, (float)height/(float)width, 0.01f, 1000.0f);

	glm::mat4 model = glm::mat4();

	std::string shaderPath = "../../../source/shaders/";
	Program shaderProgram;
	Shader vshader(shaderPath + "vertexShader.glsl",	GL_VERTEX_SHADER);
	Shader fshader(shaderPath + "fragmentShader.glsl",	GL_FRAGMENT_SHADER);
	shaderProgram.attachShader(&vshader);
	shaderProgram.attachShader(&fshader);
	shaderProgram.linkProgram();

	GLuint elems;
	glGenBuffers(1, &elems);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elems);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(unsigned int), &(shapes[0].mesh.indices[0]), GL_STATIC_DRAW);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), &(shapes[0].mesh.positions[0]), GL_STATIC_DRAW);

	GLuint nbo;
	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.normals.size() * sizeof(float), &(shapes[0].mesh.normals[0]), GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint ModelMatrixHandle = glGetUniformLocation(shaderProgram.getID(), "ModelMatrix");
	GLuint ViewMatrixHandle = glGetUniformLocation(shaderProgram.getID(), "ViewMatrix");
	GLuint ProjectionMatrixHandle = glGetUniformLocation(shaderProgram.getID(), "ProjectionMatrix");
	
	glUniformMatrix4fv(ModelMatrixHandle, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(ViewMatrixHandle, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(ProjectionMatrixHandle, 1, GL_FALSE, glm::value_ptr(projection));

	glClearColor(0.3, 0.3, 0.3, 1.0);

	while (!glfwWindowShouldClose(window)){

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shaderProgram.use();
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elems);

		model = glm::rotate(model, 0.001f, glm::vec3(0, 1, 0));
		
		glUniformMatrix4fv(ModelMatrixHandle, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(ViewMatrixHandle, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(ProjectionMatrixHandle, 1, GL_FALSE, glm::value_ptr(projection));
		
		glDrawElements(GL_TRIANGLES, shapes[0].mesh.indices.size(),GL_UNSIGNED_INT,(void*)0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		gui.Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}