/*
 * GLFWFunctions.cpp
 *
 *  Created on: 16 Oct 2015
 *      Author: james_000
 */

#include "GLFWFunctions.h"


static void error_callback(int error, const char * description)	//Prints error message description to stderr
{
	fprintf(stderr, "Error: %s\n", description);
}


/**Initialise GLFW*/
int initGLFW()
{
	glfwSetErrorCallback(error_callback);

	if(!glfwInit())		/**Initialise GLFW*/
		return -1;		/**Return -1 if failed to initialise*/

	return 0;
}

/**Create a GLFW window and return pointer to window*/
GLFWwindow * createGLFWWindow(int windowMode)
{
	GLFWwindow * window;

	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	switch(windowMode)
	{
		case 0:	//Fullscreen window
		{
			window = glfwCreateWindow(mode->width, mode->height, "Origami Sheep Game", glfwGetPrimaryMonitor(), NULL);
			break;
		}
		case 1:	//Windowed mode
		{
			window = glfwCreateWindow(mode->width, mode->height, "Origami Sheep Game", NULL, NULL);
			//glfwMaximizeWindow(window);
			break;
		}
		case 2:	//Borderless windowed mode
		{
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			window = glfwCreateWindow(mode->width, mode->height, "Origami Sheep Game", glfwGetPrimaryMonitor(), NULL);
			break;
		}
		default://Default to fullscreen window
		{
			window = glfwCreateWindow(mode->width, mode->height, "Origami Sheep Game", glfwGetPrimaryMonitor(), NULL);
			break;
		}
	}


	if(!window)
	{
		fprintf(stderr, "Error: %s\n", "Failed to create GLFW window");
		return NULL;		//Return NULL if failed to create window
	}

	std::cerr << "Created GLFW Window" << std::endl;

	return window;
}


//0 = normal, 1 = hidden, 2 = grabbed, any other value = nothing
void setMouseVisiblityGLFWWindow(GLFWwindow * window, int value)
{
	if(value == 0)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else if(value == 1)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	else if(value == 2)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}


void resizeGLFWWindow(GLFWwindow * window, int width, int height)
{
	glfwSetWindowSize(window, width, height);
}



void setPosGLFWWindow(GLFWwindow * window, int x, int y)
{
	glfwSetWindowPos(window, x, y);
}



