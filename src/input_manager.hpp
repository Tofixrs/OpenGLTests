#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
class InputManager {

public:
	void bind(const std::string& action_name, const std::vector<int>& keys);
	bool held(const std::string& action);
	bool clicked(const std::string& action);
	void input(const int& key, const int& scancode, const int& action);
	void mouse_input(GLFWwindow* window, const double& x_pos, const double& y_pos);
	void add_mouse_offset_callback(std::function<void(double, double)>);
	void add_mouse_pos_callback(std::function<void(double, double)>);
	void reset_last_mouse_pos(GLFWwindow*);
	InputManager(GLFWwindow*);
	InputManager(InputManager&& other) noexcept;

private:
	std::vector<std::function<void(double, double)>> mouse_offset_callbacks;
	std::vector<std::function<void(double, double)>> mouse_pos_callbacks;
	std::unordered_map<int, bool> key_state;
	std::unordered_map<std::string, std::vector<int>> binds;
	std::unordered_map<std::string, bool> clicked_state;
	double last_mouse_x;
	double last_mouse_y;
};
