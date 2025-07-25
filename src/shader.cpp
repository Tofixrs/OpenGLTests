#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <expected>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <print>

Shader::Shader(GLuint id): id(id) {}
Shader::~Shader() noexcept { glDeleteProgram(id); }

Shader::Shader(Shader&& other) noexcept: id(std::exchange(other.id, 0)) {}

Shader& Shader::operator=(Shader&& other) noexcept {
	if (this != &other) {
		id = std::exchange(other.id, 0);
	}
	return *this;
}
std::expected<Shader, std::string>
Shader::create(const char* vertexPath, const char* fragmentPath) {
	std::ifstream vertFile(vertexPath);
	std::ifstream fragFile(fragmentPath);

	if (!vertFile.good() || !fragFile.good()) {
		return std::unexpected("Shader files not found");
	}

	std::stringstream vShaderStream, fShaderStream;

	vShaderStream << vertFile.rdbuf();
	fShaderStream << fragFile.rdbuf();

	vertFile.close();
	fragFile.close();

	std::string fragmentCode, vertexCode;
	vertexCode = vShaderStream.str();
	fragmentCode = fShaderStream.str();
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	auto frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fShaderCode, nullptr);
	glCompileShader(frag);

	auto fragCompileRes = checkCompileErrors(frag, "Fragment");
	if (!fragCompileRes) {
		return std::unexpected(std::format("{}: {}", fragmentPath, fragCompileRes.error()));
	}

	auto vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vShaderCode, nullptr);
	glCompileShader(vert);
	auto vertCompileRes = checkCompileErrors(vert, "Vertex");
	if (!vertCompileRes) {
		return std::unexpected(std::format("{}: {}", vertexPath, vertCompileRes.error()));
	}

	auto id = glCreateProgram();
	glAttachShader(id, frag);
	glAttachShader(id, vert);
	glLinkProgram(id);
	auto progCompileRes = checkCompileErrors(id, "Program");
	if (!progCompileRes) {
		return std::unexpected(progCompileRes.error());
	}

	glDeleteShader(vert);
	glDeleteShader(frag);

	return Shader(id);
}

void Shader::setBool(const char* name, bool v) const {
	auto location = glGetUniformLocation(id, name);
	glUniform1i(location, v);
}
void Shader::setInt(const char* name, int v) const {
	auto location = glGetUniformLocation(id, name);
	glUniform1i(location, v);
}

void Shader::setFloat(const char* name, float v) const {
	auto location = glGetUniformLocation(id, name);
	glUniform1f(location, v);
}

void Shader::setMat4(const char* name, float* v) const {
	auto location = glGetUniformLocation(id, name);

	glUniformMatrix4fv(location, 1, GL_FALSE, v);
}

void Shader::use() { glUseProgram(id); }

std::expected<void, std::string> Shader::checkCompileErrors(GLuint id, std::string type) {
	char buff[1024];
	int sucess;
	if (type == "Program") {
		glGetProgramiv(id, GL_COMPILE_STATUS, &sucess);
		if (sucess) return {};

		glGetProgramInfoLog(id, sizeof(buff), NULL, buff);
		return std::unexpected(buff);
	} else {
		glGetShaderiv(id, GL_COMPILE_STATUS, &sucess);
		if (sucess) return {};

		glGetShaderInfoLog(id, sizeof(buff), nullptr, buff);

		return std::unexpected(buff);
	}

	return {};
};
