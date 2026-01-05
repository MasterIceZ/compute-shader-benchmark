#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

signed main(int argc, char *arg[]) {
	if(!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OPENGL TESTING", NULL, NULL);
	if(!window) {
		std::cout << "Cannot create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to init GLAD" << std::endl;
		return -1;
	}

	std::cout << "Using GPU: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OPENGL Version: " << glGetString(GL_VERSION) << std::endl;

	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}
