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
	
	if (!glfwInit()){ std::cout << "ERROR: glfwInit failed\n"; return -1; }

	GLFWwindow* window = glfwCreateWindow(640, 480, "mesh", NULL, NULL);
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
	/*std::string inputfile = resourcePath + "teapot/teapot.obj";
	std::string mtlfile = resourcePath + "teapot/teapot.mtl";*/
	std::string inputfile = resourcePath + "sphere/sphere.obj";
	std::string mtlfile = resourcePath + "sphere/sphere.mtl";
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	if (!tinyobj::LoadObj(shapes, materials, err, inputfile.c_str(), mtlfile.c_str()))
		std::cerr << err << std::endl;
		
	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	//OBJECT LOADING
	//////////////////////////////////

	glm::mat4 view;
	view = glm::lookAt(	glm::vec3(0.0f, 0.0f, 1.05f),
						glm::vec3(0.0f, 0.0f, 0.0f),
						glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 projection = glm::perspective(35.0f, 1.f, 0.01f, 1000.0f);

	glm::mat4 model = glm::mat4();

	std::string shaderPath = "../../../source/shaders/";
	Program shaderProgram;
	Shader vshader(shaderPath + "vertexShader.glsl",	GL_VERTEX_SHADER);
	Shader fshader(shaderPath + "fragmentShader.glsl",	GL_FRAGMENT_SHADER);
	shaderProgram.attachShader(&vshader);
	shaderProgram.attachShader(&fshader);
	shaderProgram.linkProgram();

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, shapes[0].mesh.positions.size() * sizeof(float), &(shapes[0].mesh.positions[0]), GL_STATIC_DRAW);

	GLuint elems;
	glGenBuffers(1, &elems);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elems);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size() * sizeof(unsigned int), &(shapes[0].mesh.indices[0]), GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glm::mat4 mvp = projection * view;
	GLuint MatrixID = glGetUniformLocation(shaderProgram.getID(), "MVP");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(mvp));

	glClearColor(0.3, 0.3, 0.3, 1.0);


	while (!glfwWindowShouldClose(window)){

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shaderProgram.use();
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(mvp));
		//glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elems);
		
		glDrawElements(GL_TRIANGLES, shapes[0].mesh.indices.size(),GL_UNSIGNED_INT,(void*)0);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		gui.Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}