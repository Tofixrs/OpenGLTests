#include <glm/ext/vector_float3.hpp>
#include "shader.hpp"
struct PointLight {
	glm::vec3 pos;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;

	void set_shader_data(int index, const Shader& shader) const;
};
