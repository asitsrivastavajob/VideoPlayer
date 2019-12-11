
#include <cstdio>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include "shader.hpp"
#pragma warning(disable : 4996)
std::FILE *infile = nullptr;

#define PIX_FORMAT_YUV420P10LE 1

#if !PIX_FORMAT_YUV420P10LE
#define PIXEL_WIDTH  320
#define PIXEL_HEIGHT  180
#define BYTE_PER_PIXEL 1
#define SOURCE_DATA_TYPE GL_UNSIGNED_BYTE
#endif

#if PIX_FORMAT_YUV420P10LE
#define PIXEL_WIDTH  3840
#define PIXEL_HEIGHT  2160
#define BYTE_PER_PIXEL 2
#define SOURCE_DATA_TYPE GL_SHORT
#endif

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT  720

enum {
	pixel_w = PIXEL_WIDTH ,
	pixel_h = PIXEL_HEIGHT
};

uint8_t buf[pixel_w * BYTE_PER_PIXEL * pixel_h * 3 / 2];

uint8_t *plane[3];

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	//glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

																   // Open a window and create its OpenGL context
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "IDCC-VVCVideoPlayer", nullptr, nullptr);
	if (window == nullptr)
	{
		fprintf(stderr, "Failed to open GLFW window. \n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) 
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("videoCoordLoading.vs", "NewColorSpaceConversion.fs");
#if	!PIX_FORMAT_YUV420P10LE
	if ((infile = std::fopen("test_yuv420p_320x180.yuv", "rb")) == NULL)
#elif PIX_FORMAT_YUV420P10LE 
	if ((infile = std::fopen("../../3840x2160_yuv420_10b_le.yuv", "rb")) == NULL)
#endif
	{
		std::cerr << "cannot open this file\n" << std::endl;
	}
	
	plane[0] = buf;
	plane[1] = plane[0] + pixel_w  * BYTE_PER_PIXEL *   pixel_h;
	plane[2] = plane[1] + pixel_w * BYTE_PER_PIXEL *  pixel_h / 4;

	std::cout << "width height " << pixel_w << " " << pixel_h << std::endl;
	
	unsigned int VBO, VAO, EBO;

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // texture coords
		1.0f,  1.0f, 0.0f,     1.0f, 0.0f,		// top right
		1.0f, -1.0f, 0.0f,     1.0f, 1.0f,		// bottom right
		-1.0f, -1.0f, 0.0f,    0.0f, 1.0f,		// bottom left
		-1.0f,  1.0f, 0.0f,    0.0f, 0.0f		// top left 
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int textureY, textureU, textureV;

	while (!glfwWindowShouldClose(window))
	{

		if (std::fread(buf, 1, pixel_w * BYTE_PER_PIXEL * pixel_h * 3 / 2, infile) != pixel_w * BYTE_PER_PIXEL  * pixel_h * 3 / 2) {
			// Loop  
			std::cerr << "read byte not enough. " << std::endl;
			std::fseek(infile, 0, SEEK_SET);
			std::fread(buf, 1, pixel_w  * BYTE_PER_PIXEL * pixel_h * 3 / 2, infile);
		}

		glGenTextures(1, &textureY);
		glBindTexture(GL_TEXTURE_2D, textureY);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
		// set texture filtering parameters
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, pixel_w, pixel_h, 0, GL_RED, SOURCE_DATA_TYPE, plane[0]);
		glGenerateMipmap(GL_TEXTURE_2D);


		glGenTextures(1, &textureU);
		glBindTexture(GL_TEXTURE_2D, textureU);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
		// set texture filtering parameters
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, pixel_w / 2, pixel_h / 2, 0, GL_RED, SOURCE_DATA_TYPE, plane[1]);
		glGenerateMipmap(GL_TEXTURE_2D);


		glGenTextures(1, &textureV);
		glBindTexture(GL_TEXTURE_2D, textureV);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
		// set texture filtering parameters
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, pixel_w / 2, pixel_h / 2, 0, GL_RED, SOURCE_DATA_TYPE, plane[2]);
		glGenerateMipmap(GL_TEXTURE_2D);


		glUseProgram(programID);

		glUniform1i(glGetUniformLocation(programID, "textureY"), 0);
		glUniform1i(glGetUniformLocation(programID, "textureU"), 1);
		glUniform1i(glGetUniformLocation(programID, "textureV"), 2);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 0);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureY);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureU);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureV);

		// Use our shader
		glUseProgram(programID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

