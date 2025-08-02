#include "model.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>
#include "shader.hpp"
#include <GL/gl.h>
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#include <assimp/vector2.h>
#include <format>
#include <print>
#include <stb/image.h>

Model::Model(std::vector<Mesh> meshes): meshes(std::move(meshes)) {}

void Model::draw(const Shader& shader) {
	for (const auto& mesh: meshes) {
		mesh.draw(shader);
	}
}

std::expected<Model, std::string> Model::create(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene =
	    importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags * AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		return std::unexpected(std::format("Assimp Error: {}", importer.GetErrorString()));
	}

	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;
	auto dir = path.substr(0, path.find_last_of('/'));

	Model::process_node(scene->mRootNode, scene, meshes, dir, textures_loaded);

	return Model(std::move(meshes));
}
void Model::process_node(
    const aiNode* node,
    const aiScene* scene,
    std::vector<Mesh>& meshes,
    const std::string& dir,
    std::vector<Texture>& textures_loaded
) {
	for (uint32_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(Model::process_mesh(mesh, scene, dir, textures_loaded));
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		Model::process_node(node->mChildren[i], scene, meshes, dir, textures_loaded);
	}
}
Mesh Model::process_mesh(
    const aiMesh* mesh,
    const aiScene* scene,
    const std::string& dir,

    std::vector<Texture>& textures_loaded
) {
	std::vector<Vertex> verts;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		Vertex vert;
		vert.pos.x = mesh->mVertices[i].x;
		vert.pos.y = mesh->mVertices[i].y;
		vert.pos.z = mesh->mVertices[i].z;
		if (mesh->mTextureCoords[0]) {
			vert.tex_coords.x = mesh->mTextureCoords[0][i].x;
			vert.tex_coords.y = mesh->mTextureCoords[0][i].y;
		}

		vert.normal.x = mesh->mNormals[i].x;
		vert.normal.y = mesh->mNormals[i].y;
		vert.normal.z = mesh->mNormals[i].z;

		verts.push_back(std::move(vert));
	}
	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		const auto& face = mesh->mFaces[i];

		for (size_t j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// 1. diffuse maps
	std::vector<Texture> diffuse_maps = Model::load_material_textures(
	    material,
	    scene,
	    aiTextureType_DIFFUSE,
	    "texture_diffuse",
	    dir,
	    textures_loaded
	);
	textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
	// 2. specular maps
	std::vector<Texture> specular_maps = Model::load_material_textures(
	    material,
	    scene,
	    aiTextureType_SPECULAR,
	    "texture_specular",
	    dir,
	    textures_loaded
	);
	textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	// 3. normal maps
	std::vector<Texture> normal_maps = Model::load_material_textures(
	    material,
	    scene,
	    aiTextureType_HEIGHT,
	    "texture_normal",
	    dir,
	    textures_loaded
	);
	textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
	// 4. height maps
	std::vector<Texture> height_maps = Model::load_material_textures(
	    material,
	    scene,
	    aiTextureType_AMBIENT,
	    "texture_height",
	    dir,
	    textures_loaded
	);
	textures.insert(textures.end(), height_maps.begin(), height_maps.end());

	return Mesh(std::move(verts), std::move(indices), std::move(textures));
}

std::vector<Texture> Model::load_material_textures(
    const aiMaterial* mat,
    const aiScene* scene,
    aiTextureType type,
    const std::string& typeName,
    const std::string& dir,
    std::vector<Texture>& textures_loaded
) {
	std::vector<Texture> textures;
	for (size_t i = 0; i < mat->GetTextureCount(type); i++) {
		aiString path;
		mat->GetTexture(type, i, &path);

		const aiTexture* embedded_texture = scene->GetEmbeddedTexture(path.C_Str());
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), path.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (skip) continue;

		if (embedded_texture) {
			textures.push_back(std::move(gen_embedded_texture(embedded_texture, typeName, path.C_Str())));
		} else {
			textures.push_back(std::move(
			    texture_from_path(std::format("{}/{}", dir, path.C_Str()).c_str(), typeName.c_str())
			));
		}
	}
	return textures;
}

Texture Model::texture_from_path(const char* path, const char* type_name) {
	Texture m_texture;
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format;
	stbi_set_flip_vertically_on_load(true);

	int height;
	int width;
	int channels;
	uint8_t* data = stbi_load(path, &width, &height, &channels, 0);
	if (data) {
		if (channels == 4) {
			format = GL_RGBA;
		} else if (channels == 3) {
			format = GL_RGB;
		}
	} else {
		std::println("Failed to load texture from path: {}", path);
		return m_texture;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	m_texture.id = id;
	m_texture.type = std::move(type_name);
	m_texture.path = path;

	stbi_image_free(data);

	return m_texture;
}

Texture Model::gen_embedded_texture(
    const aiTexture* texture,
    const std::string type_name,
    std::string path
) {
	Texture m_texture;
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format;
	stbi_set_flip_vertically_on_load(false);

	int height;
	int width;
	int channels;
	uint8_t* data = stbi_load_from_memory(
	    (stbi_uc*) texture->pcData,
	    (int) texture->mWidth,
	    &width,
	    &height,
	    &channels,
	    0
	);
	if (data) {
		if (channels == 4) {
			format = GL_RGBA;
		} else if (channels == 3) {
			format = GL_RGB;
		}
	} else {
		std::println("Failed to load texture from memory: {}", path);
		return m_texture;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	m_texture.id = id;
	m_texture.type = std::move(type_name);
	m_texture.path = path;

	stbi_image_free(data);

	return m_texture;
}
