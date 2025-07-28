#pragma once

#include <glm/ext/vector_float3.hpp>
#include "app.hpp"
class Camera {
public:
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
	float yaw;
	float pitch;
	float lastMouseX;
	float lastMouseY;
	float fov;

	Camera(float fov);
	void updateLook(double x_offset, double y_offset);
	void update(App& app, float deltaTime);
	void updateVectors();
};
