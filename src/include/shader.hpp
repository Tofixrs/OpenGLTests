#pragma once
#include <string>
#include <glad/gl.h>
#include <expected>
#include <glm/ext/matrix_transform.hpp>

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
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;
	~Shader() noexcept;

private:
	Shader(GLuint id);
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	static std::expected<void, std::string> checkCompileErrors(unsigned int shader, std::string type);
};
