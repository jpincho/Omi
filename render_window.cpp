#include "render_window.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static GLFWwindow *window_handle = nullptr;
static unsigned general_vao;

void GLFW_window_closed_callback(GLFWwindow *handle)
{
	if (window_handle == handle)
		window_handle = nullptr;
}


void GLFW_window_resized_callback(GLFWwindow *handle, int new_width, int new_height)
{
	glViewport(0, 0, new_width, new_height);
}

bool render_window_initialize(void)
{
	if (glfwInit() != GLFW_TRUE)
		return false;
	if (window_handle != nullptr)
		return true;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window_handle = glfwCreateWindow(1024, 768, "Omi", nullptr, nullptr);
	if (window_handle == nullptr)
		return false;
	glfwSetWindowPos(window_handle, 100, 100);

	glfwSetWindowCloseCallback(window_handle, GLFW_window_closed_callback);
	glfwSetWindowSizeCallback(window_handle, GLFW_window_resized_callback);
	glfwMakeContextCurrent(window_handle);
	glfwSwapInterval(1);

	gladLoadGL();
	//glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);
	glGenVertexArrays(1, &general_vao);
	glBindVertexArray(general_vao);
	glViewport(0, 0, 1024, 768);
	glEnable(GL_PROGRAM_POINT_SIZE);
	return true;
}

bool render_window_shutdown(void)
{
	glfwTerminate();
	window_handle = nullptr;
	return true;
}

bool render_window_is_alive(void)
{
	return (window_handle != nullptr);
}

void render_window_new_frame(void)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void render_window_end_frame(void)
{
	glfwSwapBuffers(window_handle);
	glfwPollEvents();
}

glm::uvec2 render_window_get_dimensions(void)
{
	int width, height;
	glfwGetWindowSize(window_handle, &width, &height);
	return glm::uvec2(width, height);
}
