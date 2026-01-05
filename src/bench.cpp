#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string readShaderSource(const std::string &f) {
	std::ifstream s(f);
	if(!s.is_open()) {
		throw std::runtime_error("Cannot open file: " + f);
	}
	std::stringstream ss;
	ss << s.rdbuf();
	return ss.str();
}

GLuint compileShader(GLenum t, const std::string source) {
	GLuint shader = glCreateShader(t);

	const char *source_cstr = source.c_str();
	glShaderSource(shader, 1, &source_cstr, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		char buf[512];
		glGetShaderInfoLog(shader, sizeof(buf), nullptr, buf);
		std::runtime_error("Shader compile error: " + std::string(buf));
		exit(-1);
	}

	return shader;
}

signed main(int argc, char *argv[]) {
	if(!glfwInit()) {
		std::cout << "Cannot init GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "Bechmark", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	std::cout << "Compiling Shaders..." << std::endl;

	GLuint vertex = compileShader(GL_VERTEX_SHADER, readShaderSource("shader/vertex.glsl"));
	GLuint fragment = compileShader(GL_FRAGMENT_SHADER, readShaderSource("shader/fragment.glsl"));
	GLuint oldProgram = glCreateProgram();
	glAttachShader(oldProgram, vertex);
	glAttachShader(oldProgram, fragment);
	glLinkProgram(oldProgram);

	GLuint compute = compileShader(GL_COMPUTE_SHADER, readShaderSource("shader/compute.glsl"));
	GLuint computeProgram = glCreateProgram();
	glAttachShader(computeProgram, compute);
	glLinkProgram(computeProgram);

	std::cout << "Compile DONE!" << std::endl;

	float vertices[] = {
		-1, -1, -1, 1, -1 ,0,
		-1, 1, 0, -1, 1, 0,
		1, -1, 0, 1, 1, 0
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);

	GLuint texOut;
	glGenTextures(1, &texOut);
	glBindTexture(GL_TEXTURE_2D, texOut);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 800, 600);
	glBindImageTexture(0, texOut, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	GLuint q[2];
	glGenQueries(2, q);
	
	while(!glfwWindowShouldClose(window)) {
		glBeginQuery(GL_TIME_ELAPSED, q[0]);
		glUseProgram(oldProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEndQuery(GL_TIME_ELAPSED);

		glBeginQuery(GL_TIME_ELAPSED, q[1]);
		glUseProgram(computeProgram);
		glDispatchCompute(800 / 16, 600 / 16, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glEndQuery(GL_TIME_ELAPSED);

		GLuint64 oldTime, computeTime;
		glGetQueryObjectui64v(q[0], GL_QUERY_RESULT, &oldTime);
		glGetQueryObjectui64v(q[1], GL_QUERY_RESULT, &computeTime);

		std::cout << "OLD: " << oldTime / (1000.0f * 1000.0f) << std::endl;
		std::cout << "COM: " << computeTime / (1000.f * 1000.f) << std::endl;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();

	return 0;
}
