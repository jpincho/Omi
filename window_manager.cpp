#include "window_manager.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static GLFWwindow* window_handle = nullptr;

void GLFW_window_closed_callback(GLFWwindow* handle)
{
	if (window_handle == handle)
		window_handle = nullptr;
}

bool wm_initialize(void)
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

	glfwMakeContextCurrent(window_handle);
	glfwSwapInterval(1);

	gladLoadGL();
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);
	return true;
}

bool wm_shutdown(void)
{
	glfwTerminate();
	window_handle = nullptr;
	return true;
}

bool wm_is_alive(void)
{
	return (window_handle != nullptr);
}

void wm_new_frame(void)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void wm_end_frame(void)
{
	glfwSwapBuffers(window_handle);
	glfwPollEvents();
}

