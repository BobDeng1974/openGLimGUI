#pragma once
#include <GL\glew.h>
#include <GLFW/glfw3.h>
#include "imgui\imgui.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>


class ImGUI
{
public:
	ImGUI();
	~ImGUI();

	void Render();
	void LinkWindow(GLFWwindow* window);

private:

	void				RenderDrawLists(ImDrawData* draw_data);
	static const char*	GetClipboardText();
	static void			SetClipboardText(const char* text);
	void				MouseButtonCallback(GLFWwindow*, int button, int action);
	void				ScrollCallback(GLFWwindow*, double yoffset);
	void				KeyCallback(GLFWwindow*, int key, int, int action, int mods);
	void				CharCallback(GLFWwindow*, unsigned int c);
	void				CreateFontsTexture();
	bool				CreateDeviceObjects();
	void				InvalidateDeviceObjects();
	bool				Init(GLFWwindow* window, bool install_callbacks);
	void				Shutdown();
	void				PrepareNewFrame();

	GLFWwindow*	window = NULL;
	double		time = 0.0f;
	bool		mousePressed[3] = { false, false, false };
	float		mouseWheel = 0.0f;
	GLuint		fontTexture = 0;
	
	int			guiProgram = 0;
	int			vertShader = 0;
	int			fragShader = 0;
	
	int			attribLocationTexture = 0;
	int			attribLocationProjectionMatrix = 0;
	int			attribLocationPosition = 0;
	int			attribLocationUV = 0;
	int			attribLocationColor = 0;
	
	unsigned int vbo = 0;
	unsigned int vao = 0;
	unsigned int elems = 0;

};

