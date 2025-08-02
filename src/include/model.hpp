#include "assimp/scene.h"
#include "mesh.hpp"
#include "shader.hpp"
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <string>
#include <vector>
class Model {
public:
	void draw(const Shader& shader);
	static std::expected<Model, std::string> create(const std::string& path);

private:
	static void process_node(
	    const aiNode* node,
	    const aiScene* scene,
	    std::vector<Mesh>& meshes,
	    const std::string& dir,
	    std::vector<Texture>& textures_loaded
	);
	static Mesh process_mesh(
	    const aiMesh* mesh,
	    const aiScene* scene,
	    const std::string& dir,
	    std::vector<Texture>& textures_loaded
	);
	static std::vector<Texture> load_material_textures(
	    const aiMaterial* mat,
	    const aiScene* scene,
	    aiTextureType type,
	    const std::string& type_name,
	    const std::string& dir,
	    std::vector<Texture>& textures_loaded
	);
	static Texture
	gen_embedded_texture(const aiTexture* texture, const std::string type_name, std::string path);
	static Texture texture_from_path(const char* path, const char* type_name);
	Model(std::vector<Mesh> meshes);

private:
	std::vector<Mesh> meshes;
};
