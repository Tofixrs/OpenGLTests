#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <string>
#include <vector>
#include <glad/gl.h>
#include "shader.hpp"
struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

struct Texture {
	GLuint id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> indicies, std::vector<Texture> textures);
	void draw(const Shader& shader) const;

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

private:
	GLuint VAO, VBO, EBO;
};
