#include "./camera.hpp"
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <print>

constexpr float sensitivity = 0.1f;
Camera::Camera(float fov)
    : pos(0, 0, 0)
    , front(0, 0, -1)
    , up(0, 1, 0)
    , yaw(-90)
    , pitch(0)
    , fov(fov) {}

void Camera::updateLook(double x_offset, double y_offset) {
	x_offset *= sensitivity;
	y_offset *= sensitivity;
	yaw += x_offset;
	pitch += y_offset;
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
	updateVectors();
}

void Camera::update(App& app, float deltaTime) {
	auto move = glm::vec3(0);
	float velocity = 10 * deltaTime;

	if (app.input_manager.held("forward")) {
		move += front;
	}
	if (app.input_manager.held("back")) {
		move -= front;
	}
	if (app.input_manager.held("left")) {
		move -= right;
	}
	if (app.input_manager.held("right")) {
		move += right;
	}
	if (app.input_manager.held("up")) {
		move += glm::vec3(0, 1, 0);
	}
	if (app.input_manager.held("down")) {
		move -= glm::vec3(0, 1, 0);
	}

	if (move.x == 0 && move.y == 0 && move.z == 0) return;
	move = glm::normalize(move);

	pos += move * velocity;
}

void Camera::updateVectors() {
	glm::vec3 front_n;
	front_n.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front_n.y = sin(glm::radians(pitch));
	front_n.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front_n);
	// also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0))
	); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}
