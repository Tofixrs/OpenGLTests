#include <cstddef>
#include <cstdint>
#include <ctime>
#include <string>
#include <vector>
#include <format>
#include <mesh.hpp>
#include "shader.hpp"

Mesh::Mesh(
    std::vector<Vertex> verts,
    std::vector<unsigned int> indicies,
    std::vector<Texture> textures
)
    : vertices(std::move(verts))
    , indices(std::move(indicies))
    , textures(std::move(textures)) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, pos));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
	    GL_ELEMENT_ARRAY_BUFFER,
	    indices.size() * sizeof(unsigned int),
	    indices.data(),
	    GL_STATIC_DRAW
	);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
	    2,
	    2,
	    GL_FLOAT,
	    GL_FALSE,
	    sizeof(Vertex),
	    (void*) offsetof(Vertex, tex_coords)
	);

	glBindVertexArray(0);
}
void Mesh::draw(const Shader& shader) const {
	uint32_t diff_num = 0;
	uint32_t spec_num = 0;
	std::string name;
	int number;
	for (size_t i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		name = textures[i].type;

		if (name == "texture_diffuse") {
			number = diff_num++;
		} else if (name == "texture_specular") {
			number = spec_num++;
		}

		std::string uniform_name = std::format("{}[{}]", name, number);
		shader.setInt(uniform_name.c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	shader.setInt("diff_txt_number", diff_num);
	shader.setInt("specular_txt_number", spec_num);
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
