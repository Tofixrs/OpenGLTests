#pragma once
#include <string>
#include <glad/gl.h>
#include <expected>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>

class Shader {
public:
	GLuint id;
	static std::expected<Shader, std::string>
	create(const char* vertexPath, const char* fragmentPath);
	void use();
	void setBool(const char* name, bool v) const;
	void setFloat(const char* name, float v) const;
	void setInt(const char* name, int v) const;
	void setMat4(const char* name, float* v) const;
	void setMat4(const char* name, const glm::mat4&) const;
	void setMat3(const char* name, const glm::mat3&) const;
	void setMat3(const char* name, float* v) const;
	void setVec3(const char* name, float v1, float v2, float v3) const;
	void setVec3(const char* name, const glm::vec3& v) const;
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;
	~Shader() noexcept;

private:
	Shader(GLuint id);
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	static std::expected<void, std::string> checkCompileErrors(unsigned int shader, std::string type);
};
