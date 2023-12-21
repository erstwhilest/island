#pragma once

// stl includes
#include <iostream>
#include <cmath>
#include <Windows.h>
#include <cstdlib>
#include <ctime>
#include <random>

// render includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H 

// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// project files
#include "shader.h"
#include "camera.h"
#include "modelData.h"
#include "textmanager.h"

void framebuffer_size_wrapper(GLFWwindow* window, int width, int height);
void key_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_wrapper(GLFWwindow* window, int button, int action, int mods);
void mouse_wrapper(GLFWwindow* window, double xposIn, double yposIn);
void scroll_wrapper(GLFWwindow* window, double xoffset, double yoffset);

class Application
{
public:
// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera{glm::vec3(0.0f, 0.0f, -5.0f)};

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseFocus = false;
bool render = true;
bool renderNormals = true;


ImGuiIO* ioptr{};

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLFWwindow* window{};

double fpsLimit{1/300.f};
double lastTime{};
double currentTime{};
double deltaTime2{};
unsigned int fps{};

TextManager textmanager{};
ModelData model;

void draw()
{
	// render
		// ------
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		if (render)
		{
			// pass projection matrix to shader (note that in this case it could change every frame)
			model.projectionTransform = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 1000.0f);
			model.viewTransform = camera.GetViewMatrix();
			model.modelTransform = glm::mat4(1.0f);

			model.render();

		}
		// text rendering
		textmanager.RenderText("FPS:"+std::to_string(fps), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

public:

Application()
	: model{glm::vec3( 0.0f,  0.0f,  0.0f)}
{
	// random initialize
	std::srand(std::time(nullptr));

	// glfw initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// glfw window creation
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

	// check if window is created
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(1);
	}

	// glfw callback configuration
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_wrapper);
	glfwSetCursorPosCallback(window, mouse_wrapper);
	glfwSetScrollCallback(window, scroll_wrapper);
	glfwSetKeyCallback(window, key_wrapper);
	glfwSetMouseButtonCallback(window, mouse_button_wrapper);

	// glfw mouse capture
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	// imgui configuration
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ioptr=&io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(1);
	}

	glEnable(GL_DEPTH_TEST);

	// glad transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// glad face culling
	// glEnable(GL_CULL_FACE);

	// glad anti aliasing, samples set by glfw above
	glEnable(GL_MULTISAMPLE);

	// glfw vsync off
	glfwSwapInterval(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	model.init();
	
	textmanager.init();

	// unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
	{
		camera.MovementSpeed+=300.0f;
	}
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
	{
		camera.MovementSpeed-=300.0f;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ioptr->AddMouseButtonEvent(button, action);

	if (!ioptr->WantCaptureMouse)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			mouseFocus=action;
			firstMouse=true;
		}
		if (action)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (mouseFocus)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


void run()
{
	lastTime=glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;

		if (deltaTime > fpsLimit)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGui::Begin("Settings");
			ImGui::Checkbox("Render", &render);
			ImGui::End();
			ImGui::Render();
			processInput(window);
			draw();
			lastFrame=currentFrame;
			deltaTime2+=deltaTime;
		}

		if (deltaTime2 > 1.0f / 5)
		{
			fps=static_cast<int>(1/deltaTime);
			deltaTime2=0;
		}

		glfwPollEvents();
	}
}

void cleanUp()
{
	glfwTerminate();
}
};

static Application* callback{};

void framebuffer_size_wrapper(GLFWwindow* window, int width, int height)
{
	if (callback)
	{
		callback->framebuffer_size_callback(window, width, height);
	}
}

void key_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (callback)
	{
		callback->key_callback(window,  key,  scancode,  action,  mods);
	}
}

void mouse_button_wrapper(GLFWwindow* window, int button, int action, int mods)
{
	if (callback)
	{
		callback->mouse_button_callback(window,  button,  action,  mods);
	}
}

void mouse_wrapper(GLFWwindow* window, double xposIn, double yposIn)
{
	if (callback)
	{
		callback->mouse_callback(window,  xposIn,  yposIn);
	}
}

void scroll_wrapper(GLFWwindow* window, double xoffset, double yoffset)
{
	if (callback)
	{
		callback->scroll_callback(window,  xoffset,  yoffset);
	}
}