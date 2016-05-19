#include "GUI.h"

GUI::GUI()
{
}


GUI::~GUI()
{
}

bool GUI::CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

															  // Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)fontTexture;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);

	return true;
}

bool GUI::CreateDeviceObjects()
{
	// Backup GL state
	GLint last_texture, last_array_buffer, last_vertex_array;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	const GLchar *vertex_shader =
		"#version 330\n"
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader =
		"#version 330\n"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";

	guiProgram = glCreateProgram();
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertShader, 1, &vertex_shader, 0);
	glShaderSource(fragShader, 1, &fragment_shader, 0);
	glCompileShader(vertShader);
	glCompileShader(fragShader);
	glAttachShader(guiProgram, vertShader);
	glAttachShader(guiProgram, fragShader);
	glLinkProgram(guiProgram);

	attribLocationTexture = glGetUniformLocation(guiProgram, "Texture");
	attribLocationProjectionMatrix = glGetUniformLocation(guiProgram, "ProjMtx");
	attribLocationPosition = glGetAttribLocation(guiProgram, "Position");
	attribLocationUV = glGetAttribLocation(guiProgram, "UV");
	attribLocationColor = glGetAttribLocation(guiProgram, "Color");

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &elems);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(attribLocationPosition);
	glEnableVertexAttribArray(attribLocationUV);
	glEnableVertexAttribArray(attribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	glVertexAttribPointer(attribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(attribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(attribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

	CreateFontsTexture();

	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);

	return true;
}

bool GUI::Init(GLFWwindow* window, bool install_callbacks)
{
	this->window = window;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] =			GLFW_KEY_TAB;   // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] =		GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] =	GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] =		GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] =		GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] =		GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] =		GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] =			GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] =			GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Delete] =		GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] =		GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] =			GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] =		GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] =				GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] =				GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] =				GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] =				GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] =				GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] =				GLFW_KEY_Z;

	io.RenderDrawListsFn =	NULL;       // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	/*io.SetClipboardTextFn = SetClipboardText;
	io.GetClipboardTextFn = GetClipboardText;*/
#ifdef _WIN32
	io.ImeWindowHandle = glfwGetWin32Window(window);
#endif

	/*if (install_callbacks)
	{
		glfwSetMouseButtonCallback(window, ImGui_ImplGlfwGL3_MouseButtonCallback);
		glfwSetScrollCallback(window, ImGui_ImplGlfwGL3_ScrollCallback);
		glfwSetKeyCallback(window, ImGui_ImplGlfwGL3_KeyCallback);
		glfwSetCharCallback(window, ImGui_ImplGlfwGL3_CharCallback);
	}*/

	return true;
}

void GUI::Render(std::vector<tinyobj::shape_t>* shapes)
{
	PrepareNewFrame();

	ImVec4 clear_color = ImColor(114, 144, 154);
	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	bool* opened = false;
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	if (!ImGui::Begin("Example: Fixed Overlay", opened, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("Simple shape overlay");
	ImGui::Separator();
	ImGui::Text(("Shape name: " + (*shapes)[0].name).c_str());

	/*int s = (*shapes)[0].mesh.positions.size();
	std::stringstream size;
	size << "Number of vertices: " << s ;*/
	//ImGui::Text(size.str().c_str());
	ImGui::Text("Number of vertices: %i", (*shapes)[0].mesh.positions.size());
	ImGui::Text("Number of triangles: %i", ((*shapes)[0].mesh.positions.size()/3));

	ImGui::End();

	//ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
	//ImGui::ShowTestWindow();
	//ImGui::End();
	ImGui::Render();
	RenderDrawLists(ImGui::GetDrawData());
}

void GUI::LinkWindow(GLFWwindow * window)
{
	this->window = window;
}


void GUI::RenderDrawLists(ImDrawData* draw_data)
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
	GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
	GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
	GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Setup viewport, orthographic projection matrix
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f, 0.0f },
		{ -1.0f, 1.0f, 0.0f, 1.0f },
	};
	glUseProgram(guiProgram);
	glUniform1i(attribLocationTexture, 0);
	glUniformMatrix4fv(attribLocationProjectionMatrix, 1, GL_FALSE, &ortho_projection[0][0]);
	glBindVertexArray(vao);

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elems);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

		for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
		{
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	// Restore modified GL state
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFunc(last_blend_src, last_blend_dst);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

void GUI::PrepareNewFrame()
{
	if (!fontTexture)
		CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	glfwGetWindowSize(window, &w, &h);
	glfwGetFramebufferSize(window, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	// Setup time step
	double current_time = glfwGetTime();
	io.DeltaTime = time > 0.0 ? (float)(current_time - time) : (float)(1.0f / 60.0f);
	time = current_time;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
	if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
	{
		double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
	}
	else
	{
		io.MousePos = ImVec2(-1, -1);
	}

	for (int i = 0; i < 3; i++)
	{
		io.MouseDown[i] = mousePressed[i] || glfwGetMouseButton(window, i) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		mousePressed[i] = false;
	}

	io.MouseWheel = mouseWheel;
	mouseWheel = 0.0f;

	// Hide OS mouse cursor if ImGui is drawing it
	glfwSetInputMode(window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

	// Start the frame
	ImGui::NewFrame();
}
