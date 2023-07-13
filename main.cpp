#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

std::vector<char> ReadFile(std::filesystem::path const& path) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	assert(!file.fail());
	if (file.fail()) {
		throw std::runtime_error("Cannot read " + path.string());
	}
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);
	if (file.read(buffer.data(), size)) {
		return buffer;
	}
	throw std::runtime_error("Cannot read " + path.string());
}

void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void* userParam) {
	std::string sourceStr;
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		sourceStr = "API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		sourceStr = "WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		sourceStr = "SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		sourceStr = "OTHER";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		sourceStr = "APPLICATION";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		sourceStr = "THIRD_PARTY";
		break;
	default:
		sourceStr = "Unknown";
	}
	std::string typeStr;
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		typeStr = "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		typeStr = "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		typeStr = "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		typeStr = "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		typeStr = "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_MARKER:
		typeStr = "MARKER";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		typeStr = "PUSH_GROUP";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		typeStr = "POP_GROUP";
		break;
	case GL_DEBUG_TYPE_OTHER:
		typeStr = "OTHER";
		break;
	default:
		typeStr = "Unknown";
		break;
	}
	std::string severityStr;
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		severityStr = "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		severityStr = "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		severityStr = "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		severityStr = "NOTIFICATION";
		break;
	}
	std::cout << "Debug callback:" << std::endl;
	std::cout << "\t source: " << sourceStr << std::endl;
	std::cout << "\t type: " << typeStr << std::endl;
	std::cout << "\t id: 0x" << std::hex << id << std::dec << std::endl;
	std::cout << "\t severity: " << severityStr << std::endl;
	std::cout << "\t message: " << message << std::endl;
}

uint32_t CreateShader(std::filesystem::path const& path, GLenum stage) {
	uint32_t shader = glCreateShader(stage);
	assert(shader != 0);
	std::vector<char> spirv = ReadFile(path);
	glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), (int)spirv.size());
	glSpecializeShader(shader, "main", 0, nullptr, nullptr);
	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		int len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		std::vector<char> infoLog(len);
		glGetShaderInfoLog(shader, len, &len, infoLog.data());
		throw std::runtime_error("Cannot compile shader " + path.string() + ":\n" + std::string(infoLog.data()));
	}
	return shader;
}

uint32_t CreateProgram(uint32_t vs, uint32_t ps) {
	uint32_t program = glCreateProgram();
	program = glCreateProgram();
	assert(program != 0);
	glAttachShader(program, vs);
	glAttachShader(program, ps);
	glLinkProgram(program);
	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		int len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		if (len != 0) {
			std::vector<char> infoLog(len);
			glGetProgramInfoLog(program, len, &len, infoLog.data());
			std::cerr << infoLog.data() << std::endl;
		} else {
			std::cerr << "Info log is empty" << std::endl; // please fix
		}
		throw std::runtime_error("Cannot link program");
	}
	return program;
}

void run() {
	int err = glfwInit();
	assert(err == GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "test", nullptr, nullptr);
	assert(window != nullptr);
	glfwMakeContextCurrent(window);
	err = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	assert(err != 0);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(DebugCallback, nullptr);
	std::cout << "Picked " << glGetString(GL_RENDERER) << std::endl;

	// example 1
	{
		uint32_t vs = CreateShader("shaders/vs.spirv", GL_VERTEX_SHADER);
		uint32_t ps = CreateShader("shaders/ps.spirv", GL_FRAGMENT_SHADER);
		for (int i = 0; i < 2; i++) {
			uint32_t program = CreateProgram(vs, ps); // failed on the second iteration
			glDeleteProgram(program);
		}
		glDeleteShader(ps);
		glDeleteShader(vs);
	}

	// example 2
	{
		uint32_t vs = CreateShader("shaders/vs2.spirv", GL_VERTEX_SHADER);
		uint32_t ps = CreateShader("shaders/ps2.spirv", GL_FRAGMENT_SHADER);
		uint32_t program = CreateProgram(vs, ps); // failed
		glDeleteProgram(program);
		glDeleteShader(ps);
		glDeleteShader(vs);
	}

	glfwTerminate();
}

int main() {
	try {
		run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}