#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "app.hpp"
#include <cstdint>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <print>
#include "shader.hpp"
#include "errorReporting.hpp"
#include <stb/image.h>
#include "camera.hpp"

std::expected<App, std::string> App::create() {
	auto gl_res = App::init_gl();
	if (!gl_res) return std::unexpected(gl_res.error());

	auto window = *gl_res;
	auto input_manager = InputManager(window);
	input_manager.bind("forward", {GLFW_KEY_W});
	input_manager.bind("back", {GLFW_KEY_S});
	input_manager.bind("left", {GLFW_KEY_A});
	input_manager.bind("right", {GLFW_KEY_D});
	input_manager.bind("up", {GLFW_KEY_SPACE});
	input_manager.bind("down", {GLFW_KEY_LEFT_SHIFT});
	input_manager.bind("toggle_mouse", {GLFW_KEY_ESCAPE});

	return App(window, std::move(input_manager));
};

void App::run() {
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto app = (App*) glfwGetWindowUserPointer(window);
		app->input_manager.input(key, scancode, action);
	});
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
		auto app = (App*) glfwGetWindowUserPointer(window);
		app->input_manager.mouse_input(window, x, y);
	});
	auto res = Shader::create("./shaders/vert.glsl", "./shaders/frag.glsl");
	if (!res.has_value()) {
		//cant print the std::string* directly lol
		std::println("{}", res.error().c_str());
		return;
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

	auto cam = Camera(90.f);
	input_manager.add_mouse_offset_callback([&cam](double x_offset, double y_offset) {
		cam.updateLook(x_offset, y_offset);
	});

	while (!glfwWindowShouldClose(window)) {
		double currentFrame = glfwGetTime();
		double deltaTime = currentFrame - lastLoopTime;
		lastLoopTime = currentFrame;
		cam.update(*this, deltaTime);

		if (input_manager.clicked("toggle_mouse")) {
			auto mode = glfwGetInputMode(window, GLFW_CURSOR);
			switch (mode) {
			case GLFW_CURSOR_DISABLED: glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); break;
			default:
			case GLFW_CURSOR_NORMAL:
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				input_manager.reset_last_mouse_pos(window);
				break;
			}
		}

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
		glm::mat4 view;
		view = glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);
		int screen_width, screen_height;

		glfwGetWindowSize(window, &screen_width, &screen_height);
		auto projection =
		    glm::perspective(cam.fov, ((float) screen_width / (float) screen_height), 0.1f, 100.f);
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

App::App(GLFWwindow* window, InputManager input_manager)
    : window(window)
    , input_manager(std::move(input_manager)) {};

std::expected<GLFWwindow*, std::string> App::init_gl() {
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int req_width, int req_height) {
		glViewport(0, 0, req_width, req_height);
	});

	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0) {
		return std::unexpected("Failed to init glad");
	}
	enableReportGlErrors();
	glEnable(GL_DEPTH_TEST);

	return window;
}
