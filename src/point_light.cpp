#include "point_light.hpp"
#include <format>
#include "shader.hpp"

void PointLight::set_shader_data(int index, const Shader& shader) const {
	shader.setVec3(std::format("point_lights[{}].pos", index).c_str(), this->pos);
	shader.setVec3(std::format("point_lights[{}].ambient", index).c_str(), this->ambient);
	shader.setVec3(std::format("point_lights[{}].diffuse", index).c_str(), this->diffuse);
	shader.setVec3(std::format("point_lights[{}].specular", index).c_str(), this->specular);

	shader.setFloat(std::format("point_lights[{}].constant", index).c_str(), this->constant);
	shader.setFloat(std::format("point_lights[{}].linear", index).c_str(), this->linear);
	shader.setFloat(std::format("point_lights[{}].quadratic", index).c_str(), this->quadratic);
}
