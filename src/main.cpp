#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include "shader.hpp"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <expected>
#include <print>
#include "errorReporting.hpp"
#include <filesystem>
#include <stb/image.h>

std::expected<GLFWwindow*, std::string> initGL();
constexpr auto fov = glm::radians(90.f);
constexpr auto near = 0.1f;
constexpr auto far = 100.f;

static auto screen_width = 400;
static auto screen_height = 500;

int main() {
	auto result = initGL();
	if (!result.has_value()) {
		std::println("{}", result.error().c_str());
		return -1;
	}
	enableReportGlErrors();

	auto window = *result;

	auto res = Shader::create("./shaders/vert.glsl", "./shaders/frag.glsl");
	if (!res.has_value()) {
		//cant print the std::string* directly lol
		std::println("{}", res.error().c_str());
		return -1;
	}
	auto shader = std::move(*res);

	// clang-format off
    float vertices[] = {
      // Front face
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-right
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // Top-left

      // Back face
      -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // Bottom-left
       0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-right
       0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-right
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-left

      // Left face
      -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // Top-right
      -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-left
      -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // Bottom-left
      -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-right

      // Right face
       0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-left
       0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-left
       0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-right
       0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-right

      // Top face
      -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left
       0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right
       0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-right
      -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-left

      // Bottom face
      -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, // Top-left
       0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // Top-right
       0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // Bottom-right
      -0.5f, -0.5f, -0.5f,  0.0f, 0.0f  // Bottom-left
  };

  GLuint indicies[] = {
      // Front face
      0, 1, 2,  // First triangle
      2, 3, 0,  // Second triangle

      // Back face
      4, 5, 6,
      6, 7, 4,

      // Left face
      8, 9, 10,
      10, 11, 8,

      // Right face
      12, 13, 14,
      14, 15, 12,

      // Top face
      16, 17, 18,
      18, 19, 16,

      // Bottom face
      20, 21, 22,
      22, 23, 20
  };
	// clang-format on

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);

	//tex cord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	auto image_data = stbi_load("./textures/sans.png", &width, &height, &channels, 0);
	GLenum format = GL_RGB;
	if (channels == 4) {
		format = GL_RGBA;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image_data);

	GLuint awesomeTexture;
	glGenTextures(1, &awesomeTexture);
	glBindTexture(GL_TEXTURE_2D, awesomeTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image_data = stbi_load("./textures/brick.jpg", &width, &height, &channels, 0);
	format = GL_RGB;
	if (channels == 4) {
		format = GL_RGBA;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image_data);

	glBindVertexArray(0);

	shader.use();
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	double lastLoopTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		double currentFrame = glfwGetTime();
		double deltaTime = currentFrame - lastLoopTime;
		lastLoopTime = currentFrame;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, awesomeTexture);
		shader.use();

		auto model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(
		    model,
		    (float) (glfwGetTime() * glm::radians(100.f)),
		    glm::vec3(0.5f, 1.0f, 0.0f)
		);
		auto view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		auto projection =
		    glm::perspective(fov, ((float) screen_width / (float) screen_height), near, far);
		shader.setMat4("projection", glm::value_ptr(projection));
		shader.setMat4("model", glm::value_ptr(model));
		shader.setMat4("view", glm::value_ptr(view));
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	shader.~Shader();

	glfwTerminate();
}
void handleInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
	const char* keyName = glfwGetKeyName(key, scancode);
	if (keyName != nullptr) {
		std::println("[KEY_EVENT]: {} {}", keyName, action ? "PRESS" : "RELEASE");
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	std::println("[MOUSE_EVENT]: {} {}", xpos, ypos);
}

std::expected<GLFWwindow*, std::string> initGL() {
	glfwSetErrorCallback([](int errorCode, const char* desc) {
		std::println("[GLFW_ERROR]: {} {}", errorCode, desc);
	});
	if (!glfwInit()) {
		return std::unexpected("Failed to init glfw");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(400, 500, "OpenGL", nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		return std::unexpected("Failed to open window");
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int req_width, int req_height) {
		screen_width = req_width;
		screen_height = req_height;
		glViewport(0, 0, screen_width, screen_height);
	});
	glfwSetKeyCallback(window, handleInput);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		return std::unexpected("Failed to init glad");
	}
	enableReportGlErrors();
	glEnable(GL_DEPTH_TEST);

	return window;
}
