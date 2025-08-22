#include <glm/ext/vector_float3.hpp>
#include "shader.hpp"
struct DirLight {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void set_shader_data(const Shader& shader) const;
};
