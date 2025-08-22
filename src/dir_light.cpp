#include "dir_light.hpp"
#include "shader.hpp"

void DirLight::set_shader_data(const Shader& shader) const {
	shader.setVec3("dir_light.direction", this->direction);
	shader.setVec3("dir_light.ambient", this->ambient);
	shader.setVec3("dir_light.ambient", this->ambient);
	shader.setVec3("dir_light.specular", this->specular);
}
