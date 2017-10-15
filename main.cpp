/*
  main.cpp
 *
 *  Created on: 5 Oct 2015
 *      Author: james_000
 */

#define GLEW_STATIC

#include <iostream>
#include <stdio.h>

#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "rendering/RenderingEngine.h"
#include "physics/PhysicsEngine.h"
#include "multi-threading/TaskManager.h"
#include "JNAWrapperFunctions.h"
#include "windowing\WindowManager.h"
#include "windowing\WindowManagerGLFW.h"


int initGLEW();
//Functions which can be used from the java editor
//extern "C" __declspec(dllexport) int __cdecl getsetWindowMode(int mode);		//Sets/Gets the window mode


int main()
{
	std::cerr << "Hello World!" << std::endl;

	//Need c++11 for following line to compile
	//std::cout << "No. Cores = " << std::thread::hardware_concurrency();

	/*(initGLFW();
	std::cerr << "Initialised GLFW" << std::endl;

	glfwWindowHint(GLFW_SAMPLES, 4);
	getsetWindowMode(1);

	GLFWwindow * window = createGLFWWindow(getsetWindowMode(-1));
	glfwMakeContextCurrent(window);
	setMouseVisiblityGLFWWindow(window, 0);	//Grab the cursor
	jnaPtrs.window = window;
	if(!glfwGetCurrentContext())
	{
		std::cerr << "Failed to make context current" << std::endl;
	}
	//glfwShowWindow(window);*/
	WindowManager * windowManager = new WindowManagerGLFW;
	windowManager->createWindow(1);
	glfwMakeContextCurrent((GLFWwindow*)windowManager->getWindow());
	setMouseVisiblityGLFWWindow((GLFWwindow*)windowManager->getWindow(), 0);	//Grab the cursor
	if(!glfwGetCurrentContext())
	{
		std::cerr << "Failed to make context current" << std::endl;
	}
	std::cerr << "Initialised Window" << std::endl;

	initGLEW();
	std::cerr << "Initialised GLEW" << std::endl;
	if(GLEW_VERSION_3_2)
	{
		std::cerr << "OpenGL 3.2 Supported" << std::endl;
	}
	else
	{
		std::cerr << "OpenGL 3.2 Not Supported" << std::endl;

		if(GLEW_ARB_instanced_arrays)
		{
			std::cerr << "Instanced Arrays Available" << std::endl;
		}
		else
		{
			std::cerr << "Instanced Arrays Not Supported" << std::endl;
		}
	}

	TaskManager * taskManager = new TaskManager(windowManager);

	taskManager->setupThreads();
	jnaPtrs.taskManager = taskManager;
	taskManager->startThreads();

	delete taskManager;

	glfwDestroyWindow((GLFWwindow*)windowManager->getWindow());

//	delete windowManager;

	glfwTerminate();

	return 0;
}

//Load OpenGL functions using GLEW. Return of 0 = success, return of -1 = error.
int initGLEW()
{
	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
	  //GLEW Initialisation failed
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	  return -1;		//Return with error
	}
	fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	return 0;			//Return with success
}

/*extern "C" __declspec(dllexport) int __cdecl getsetWindowMode(int mode)	//Sets/Gets the window mode
{
	static int m = 0;	//Default window mode is 0 (fullscreen)
	if(mode != -1)		//If the mode passed is -1, the mode is not set...
		m = mode;
	return m;			//...And the previously set mode is returned
}*/
