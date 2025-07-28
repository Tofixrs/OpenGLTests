#pragma once

#include <expected>
#include <GLFW/glfw3.h>
#include <string>
#include "./input_manager.hpp"

class App {
public:
	static std::expected<App, std::string> create();
	InputManager input_manager;
	void run();

private:
	static std::expected<GLFWwindow*, std::string> init_gl();
	void glfw_error_callback(int error_code, const char* desc);
	GLFWwindow* window;

	App(GLFWwindow* window, InputManager input_manager);
};
