#pragma once

#include <iostream>
#include <stdio.h>

#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "../rendering/RenderingEngine.h"
#include "../input/InputManager.h"
#include "../user-interface/UserInterface.h"

#include "WindowManager.h"

class WindowManagerGLFW : public WindowManager
{
public:
	WindowManagerGLFW();
	~WindowManagerGLFW();

	int		setMouseVisibility(int value);

	void	setWindowSize(int width, int height);
	void	setWindowPos(int x, int y);
	
	void	setTitle(const std::string & title);

	void	setNumSamples(int numSamples);

	void *  getWindow() const;
	void	createWindow(int windowMode);

	void	update();

	double  getTimeSeconds() const {return glfwGetTime();}

	void	setupCallbackClasses(osi::UserInterface * ui, osr::RenderingEngine * r, osi::InputManager * i);		//Sets the class to receive callbacks for changes to the window & input
private:
	int		initWindowingToolkit() const;

	GLFWwindow * window;

	osi::UserInterface * userInterface;
	//osi::GUI * gui;
	osr::RenderingEngine * renderingEngine;
	osi::InputManager * inputManager;

	void framebufferSizeCallbackImpl(GLFWwindow * window, int width, int height);
	static void framebufferSizeCallback(GLFWwindow * window, int width, int height);

	void windowPosCallbackImpl(GLFWwindow * window, int x, int y);
	static void windowPosCallback(GLFWwindow * window, int x, int y);

	void cursorPosCallbackImpl(GLFWwindow * window, double xPos, double yPos);
	static void cursorPosCallback(GLFWwindow * window, double xPos, double yPos);

	void mouseButtonCallbackImpl(GLFWwindow * window, int button, int action, int mods);
	static void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);

	void mouseScrollCallbackImpl(GLFWwindow * window, double xOffset, double yOffset);
	static void mouseScrollCallback(GLFWwindow * window, double xOffset, double yOffset);

	void keyCallbackImpl(GLFWwindow * window, int key, int scancode, int action, int mods);		//Implementation function which can access member variables
	static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);	//Static function which receives key callbacks from window

	void charCallbackImpl(GLFWwindow * window, unsigned int codePoint);
	static void charCallback(GLFWwindow * window, unsigned int codePoint);
};

