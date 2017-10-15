/*
 * JNAWrapperFunctions.cpp
 *
 *  Created on: 3 Apr 2016
 *      Author: james_000
 */

#include "JNAWrapperFunctions.h"

JNAPtrs jnaPtrs;

extern "C" __declspec(dllexport) void __cdecl jnaSetMouseVisibility(int mode)	//Sets the mouse visibility
{
	setMouseVisiblityGLFWWindow(jnaPtrs.window, mode);
}


extern "C" __declspec(dllexport) void __cdecl jnaSetWindowSize(int width, int height)	//Sets the size of the glfw window
{
	resizeGLFWWindow(jnaPtrs.window, width, height);
}


extern "C" __declspec(dllexport) void __cdecl jnaSetWindowPos(int x, int y)			//Sets the position of the glfw window
{
	setPosGLFWWindow(jnaPtrs.window, x, y);
}

extern "C" __declspec(dllexport) void __cdecl jnaCreateRenderingEngine()
{
	jnaPtrs.renderingEngine = new osr::RenderingEngine(0, 0);
	jnaPtrs.renderingEngine->initDepthTesting();
	jnaPtrs.renderingEngine->setFaceCulling(false);
}

extern "C" __declspec(dllexport) void __cdecl jnaRenderScene()
{
	jnaPtrs.renderingEngine->render();
}
