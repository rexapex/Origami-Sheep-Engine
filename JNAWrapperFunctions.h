/*
 * JNAWrapperFunctions.h
 *
 *  Created on: 3 Apr 2016
 *      Author: james_000
 */

#ifndef JNAWRAPPERFUNCTIONS_H_
#define JNAWRAPPERFUNCTIONS_H_

//#include "io-resource-management/ResourceManager.h"
#include "rendering/RenderingEngine.h"
#include "multi-threading/TaskManager.h"
#include "GLFWFunctions.h"

extern struct JNAPtrs
{
	GLFWwindow * window;
	TaskManager * taskManager;
	osr::RenderingEngine * renderingEngine;
} jnaPtrs;
//ResourceManager * resourceManager;

extern "C" __declspec(dllexport) void __cdecl jnaSetMouseVisibility(int mode);			//Sets the mouse visibility
extern "C" __declspec(dllexport) void __cdecl jnaSetWindowSize(int width, int height);	//Sets the size of the glfw window
extern "C" __declspec(dllexport) void __cdecl jnaSetWindowPos(int x, int y);			//Sets the position of the glfw window
extern "C" __declspec(dllexport) void __cdecl jnaCreateRenderingEngine();
extern "C" __declspec(dllexport) void __cdecl jnaRenderScene();

#endif /* JNAWRAPPERFUNCTIONS_H_ */
