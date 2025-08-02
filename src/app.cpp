#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "app.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <print>
#include "input_manager.hpp"
#include "shader.hpp"
#include "errorReporting.hpp"
#include <stb/image.h>
#include "camera.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <model.hpp>

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
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto app = (App*) glfwGetWindowUserPointer(window);
		app->input_manager.input(key, scancode, action);
	});
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
		auto app = (App*) glfwGetWindowUserPointer(window);
		app->input_manager.mouse_input(window, x, y);
	});

	return App(window, std::move(input_manager));
};

void App::run() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	auto& io = ImGui::GetIO();

	// Setup Polatform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(
	    window,
	    true
	); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	auto res = Shader::create("./shaders/vert.glsl", "./shaders/frag.glsl");
	if (!res.has_value()) {
		//cant print the std::string* directly lol
		std::println("{}", res.error().c_str());
		return;
	}
	auto shader = std::move(*res);

	auto res_no_shade = Shader::create("./shaders/no_shade_v.glsl", "./shaders/no_shade_f.glsl");
	if (!res_no_shade.has_value()) {
		//cant print the std::string* directly lol
		std::println("{}", res_no_shade.error().c_str());
		return;
	}
	auto shader_no_shade = std::move(*res_no_shade);

	auto lightPos = glm::vec3(0.f, 10.f, 0.f);

	auto lightColor = glm::vec3(1.f, 1.f, 1.f);
	shader.use();
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);
	shader.setFloat("ambientStrength", 0.3);
	shader.setVec3("lightColor", lightColor);
	shader.setVec3("lightPos", lightPos);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	double lastLoopTime = glfwGetTime();

	auto cam = Camera(90.f);
	input_manager.add_mouse_offset_callback([&cam](double x_offset, double y_offset) {
		cam.updateLook(x_offset, y_offset);
	});

	auto model_res = Model::create("./models/teapot.glb");
	if (!model_res) {
		std::println("{}", model_res.error().c_str());
		return;
	}
	auto model = std::move(*model_res);

	while (!glfwWindowShouldClose(window)) {
		double currentFrame = glfwGetTime();
		double deltaTime = currentFrame - lastLoopTime;
		lastLoopTime = currentFrame;
		ImGuiIO& io = ImGui::GetIO();
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
			io.ConfigFlags |= ImGuiConfigFlags_NoKeyboard;
		} else {
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			io.ConfigFlags &= ~ImGuiConfigFlags_NoKeyboard;
		}
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Debug");
		ImGui::Text("Debug Window");
		ImGui::Text("FPS: %f", 1 / deltaTime);
		ImGui::ColorEdit3("Light color", glm::value_ptr(lightColor));
		ImGui::End();

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
		shader.use();

		glm::mat4 view;
		view = glm::lookAt(cam.pos, cam.pos + cam.front, cam.up);
		int screen_width, screen_height;

		glfwGetWindowSize(window, &screen_width, &screen_height);
		auto projection =
		    glm::perspective(cam.fov, ((float) screen_width / (float) screen_height), 0.1f, 100.f);
		glm::mat4 model_matrix = glm::mat4(1.0f);
		model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
		model_matrix = glm::rotate(
		    model_matrix,
		    (float) glm::radians(glfwGetTime() * 100.f),
		    glm::vec3(0.0f, 1.0f, 0.0f)
		);
		shader.setMat4("projection", glm::value_ptr(projection));
		shader.setMat4("view", glm::value_ptr(view));
		shader.setMat4("model", glm::value_ptr(model_matrix));
		shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model_matrix))));
		shader.setVec3("lightColor", lightColor);
		shader.setVec3("viewPos", cam.pos);
		model.draw(shader);

		shader_no_shade.use();
		shader_no_shade.setMat4("projection", glm::value_ptr(projection));
		shader_no_shade.setMat4("view", glm::value_ptr(view));
		shader_no_shade.setMat4("model", model_matrix);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	shader.~Shader();

	glfwTerminate();
}

App::App(GLFWwindow* window, InputManager input_manager)
    : window(window)
    , input_manager(std::move(input_manager)) {};

App::App(App&& other) noexcept
    : window(other.window)
    , input_manager(std::move(other.input_manager)) {
	glfwSetWindowUserPointer(window, this);
};

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
