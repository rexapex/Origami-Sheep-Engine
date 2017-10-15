/*
 * GLFWFunctions.h
 *
 *  Created on: 16 Oct 2015
 *      Author: james_000
 */

#define GLEW_STATIC

#include <iostream>
#include <stdio.h>

#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#ifndef GLFWFUNCTIONS_H_
#define GLFWFUNCTIONS_H_

int initGLFW();
GLFWwindow * createGLFWWindow(int windowMode);
void setMouseVisiblityGLFWWindow(GLFWwindow * window, int value);
void resizeGLFWWindow(GLFWwindow * window, int width, int height);
void setPosGLFWWindow(GLFWwindow * window, int x, int y);

#endif /* GLFWFUNCTIONS_H_ */
