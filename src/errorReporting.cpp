#include <glad/gl.h>
#include <print>
#include "errorReporting.hpp"

void enableReportGlErrors() {
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	glDebugMessageCallback(0, nullptr);
}
const char* type_to_string(GLenum type) {
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: return "Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behaviour";
	case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
	case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
	case GL_DEBUG_TYPE_MARKER: return "Marker";
	case GL_DEBUG_TYPE_PUSH_GROUP: return "Push Group";
	case GL_DEBUG_TYPE_POP_GROUP: return "Pop Group";
	case GL_DEBUG_TYPE_OTHER: return "Other";
	};
	return "unkown";
}

const char* source_to_string(GLenum source) {
	switch (source) {
	case GL_DEBUG_SOURCE_API: return "API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
	case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
	case GL_DEBUG_SOURCE_APPLICATION: return "Source: Application";
	case GL_DEBUG_SOURCE_OTHER: return "Source: Other";
	};
	return "unkown";
}

const char* severity_to_string(GLenum severity) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: return "high";
	case GL_DEBUG_SEVERITY_MEDIUM: return "medium";
	case GL_DEBUG_SEVERITY_LOW: return "low";
	case GL_DEBUG_SEVERITY_NOTIFICATION: return "notification";
	};
	return "unkown";
}

void GLAPIENTRY glDebugOutput(
    GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei length,
    const char* message,
    const void* userParam
) {
	// ignore non-significant error/warning codes
	// if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 131222) return;
	// if (type == GL_DEBUG_TYPE_PERFORMANCE) return;

	auto sourceStr = source_to_string(source);
	auto typeStr = type_to_string(type);
	auto severityStr = severity_to_string(severity);
	std::println("--------------------");
	std::println("------GL_DEBUG------");
	std::println("Source: {}", source);
	std::println("Type: {}", type);
	std::println("Severity: {}", severity);
	std::println("Message: {}", message);
}
