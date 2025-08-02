#include "./input_manager.hpp"
#include <exception>
#include <print>
#include <GLFW/glfw3.h>

InputManager::InputManager(GLFWwindow* window)
    : mouse_pos_callbacks {}
    , mouse_offset_callbacks {}
    , key_state {}
    , binds {}
    , clicked_state {} {
	glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);
};

void InputManager::input(const int& key, const int& scancode, const int& action) {
	auto elem = key_state.find(key);
	if (elem == key_state.end()) {
		key_state.emplace(key, action == GLFW_PRESS);
	} else {
		key_state[key] = action == GLFW_PRESS;
	}
}

void InputManager::mouse_input(GLFWwindow* window, const double& x_pos, const double& y_pos) {
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
		for (const auto& cb: mouse_pos_callbacks) {
			cb(x_pos, y_pos);
		}
	} else {
		float x_offset = x_pos - last_mouse_x;
		float y_offset = last_mouse_y - y_pos;

		last_mouse_x = x_pos;
		last_mouse_y = y_pos;

		for (const auto& cb: mouse_offset_callbacks) {
			cb(x_offset, y_offset);
		}
	}
};

void InputManager::bind(const std::string& action_name, const std::vector<int>& keys) {
	binds[action_name] = keys;
};

bool InputManager::held(const std::string& action) {
	try {
		auto& keys = binds.at(action);

		for (const auto& key: keys) {
			if (key_state[key]) return true;
		}
		return false;
	} catch (std::exception err) {
		return false;
	}
};

bool InputManager::clicked(const std::string& action) {
	bool held = this->held(action);
	auto& clicked = clicked_state[action];
	if (held && !clicked) {
		clicked = true;
		return true;
	} else if (held && clicked) {
		return false;
	} else if (!held && clicked) {
		clicked = false;
		return false;
	} else {
		return false;
	};
};

InputManager::InputManager(InputManager&& other) noexcept
    : mouse_pos_callbacks {std::move(other.mouse_pos_callbacks)}
    , mouse_offset_callbacks {std::move(other.mouse_offset_callbacks)}
    , key_state {std::move(other.key_state)}
    , binds {std::move(other.binds)}
    , clicked_state {std::move(other.clicked_state)}
    , last_mouse_x {other.last_mouse_x}
    , last_mouse_y {other.last_mouse_y} {};

void InputManager::reset_last_mouse_pos(GLFWwindow* window) {
	glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);
}

void InputManager::add_mouse_offset_callback(std::function<void(double, double)> cb) {
	mouse_offset_callbacks.push_back(std::move(cb));
};
void InputManager::add_mouse_pos_callback(std::function<void(double, double)> cb) {
	mouse_pos_callbacks.push_back(std::move(cb));
};
