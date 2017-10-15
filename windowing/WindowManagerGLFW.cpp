#include "WindowManagerGLFW.h"



WindowManagerGLFW::WindowManagerGLFW()
{
	if(initWindowingToolkit() == -1)
		fprintf(stderr, "Error: %s\n", "Failed to initialise GLFW");
}

WindowManagerGLFW::~WindowManagerGLFW()
{

}

void WindowManagerGLFW::setupCallbackClasses(osi::UserInterface * ui, osr::RenderingEngine * r, osi::InputManager * i)		//Sets the class to receive callbacks for changes to the window & input
{
	this->userInterface = ui;
//	this->gui = gui;
	this->renderingEngine = r;
	this->inputManager = i;
}




static void errorCallback(int error, const char * description)	//Prints error message description to stderr
{
	fprintf(stderr, "Error: %s\n", description);
}

int WindowManagerGLFW::initWindowingToolkit() const
{
	glfwSetErrorCallback(errorCallback);

	if(!glfwInit())		/**Initialise GLFW*/
		return -1;		/**Return -1 if failed to initialise*/

	return 0;
}

void WindowManagerGLFW::createWindow(int windowMode)
{
	GLFWwindow * window;

	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	switch(windowMode)
	{
		case 0:	//Fullscreen window
		{
			window = glfwCreateWindow(mode->width, mode->height, "Origami Sheep Engine", glfwGetPrimaryMonitor(), NULL);
			break;
		}
		case 1:	//Windowed mode
		{
			window = glfwCreateWindow(mode->width, mode->height, "Origami Sheep Engine", NULL, NULL);
			break;
		}
		case 2:	//Borderless windowed mode
		{
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			window = glfwCreateWindow(mode->width, mode->height, "Origami Sheep Engine", glfwGetPrimaryMonitor(), NULL);
			break;
		}
		default://Default to fullscreen window
		{
			window = glfwCreateWindow(mode->width, mode->height, "Origami Sheep Engine", glfwGetPrimaryMonitor(), NULL);
			break;
		}
	}


	if(!window)
	{
		fprintf(stderr, "Error: %s\n", "Failed to create GLFW window");
		this->window = nullptr;		//Return NULL if failed to create window
	}

	glfwSetWindowUserPointer(window, this);		//Set the window pointer to be this InputManager for later use
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);	//Set callbacks
//	glfwSetWindowPosCallback(window, windowPosCallback);
//	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	//glfwSetScrollCallback(window, mouseScrollCallback);
//	glfwSetCharCallback(window, charCallback);

	std::cerr << "Created GLFW Window" << std::endl;

	this->window = window;
}

void * WindowManagerGLFW::getWindow() const
{
	return window;
}



void WindowManagerGLFW::update()
{
	//std::cerr << "HOW!?!?!?\n";
	//double xpos, ypos;
	//glfwGetCursorPos(window, &xpos, &ypos);		//Callback function not called frequently enough to update camera
	//inputManager->cursorPosCallbackImpl(xpos, ypos);
}




void WindowManagerGLFW::setTitle(const std::string & title)
{
	glfwSetWindowTitle(window, title.c_str());
}




int WindowManagerGLFW::setMouseVisibility(int value)
{
	if(value == 0)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else if(value == 1)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	else if(value == 2)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return 0;
}



void WindowManagerGLFW::setWindowSize(int width, int height)
{
	glfwSetWindowSize(window, width, height);
}

void WindowManagerGLFW::setWindowPos(int x, int y)
{
	glfwSetWindowPos(window, x, y);
}



void WindowManagerGLFW::setNumSamples(int numSamples)
{
	
}




void WindowManagerGLFW::framebufferSizeCallbackImpl(GLFWwindow * window, int width, int height)
{
	//Must update user interface frame buffer size before updating rendering engine
	userInterface->setFramebufferSize(width, height);				//Update the user interface to store the new dimensions
	renderingEngine->framebufferSizeCallbackImpl(width, height);	//Update the rendering engines framebuffer
//	gui->framebufferResized(width, height);
	renderingEngine->threadsafe_postShouldUpdateUI();
}

void WindowManagerGLFW::framebufferSizeCallback(GLFWwindow * window, int width, int height)
{
	WindowManagerGLFW * windowManager = reinterpret_cast<WindowManagerGLFW *>(glfwGetWindowUserPointer(window));
	windowManager->framebufferSizeCallbackImpl(window, width, height);
}



void WindowManagerGLFW::windowPosCallbackImpl(GLFWwindow * window, int x, int y)
{
	renderingEngine->windowPosCallbackImpl(x, y);
}

void WindowManagerGLFW::windowPosCallback(GLFWwindow * window, int x, int y)
{
	WindowManagerGLFW * windowManager = reinterpret_cast<WindowManagerGLFW *>(glfwGetWindowUserPointer(window));	//Get the window user pointer
	windowManager->windowPosCallbackImpl(window, x, y);
}



void WindowManagerGLFW::cursorPosCallbackImpl(GLFWwindow * window, double xPos, double yPos)
{
	//gui->injectMousePos(xPos, yPos);
}

void WindowManagerGLFW::cursorPosCallback(GLFWwindow * window, double xPos, double yPos)
{
	WindowManagerGLFW * windowManager = reinterpret_cast<WindowManagerGLFW *>(glfwGetWindowUserPointer(window));	//Get the window user pointer
	windowManager->cursorPosCallbackImpl(window, xPos, yPos);		//Forward the callback to the member implementation method
}



void WindowManagerGLFW::mouseButtonCallbackImpl(GLFWwindow * window, int button, int action, int mods)
{
	inputManager->mouseButtonCallbackImpl(button, action, mods);
	//if(action == GLFW_PRESS)
	//	gui->injectMouseDown(button);
	//else
	//	gui->injectMouseUp(button);
}

void WindowManagerGLFW::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
	WindowManagerGLFW * windowManager = reinterpret_cast<WindowManagerGLFW *>(glfwGetWindowUserPointer(window));	//Get the window user pointer
	windowManager->mouseButtonCallbackImpl(window, button, action, mods);		//Forward the callback to the member implementation method
}



void WindowManagerGLFW::mouseScrollCallbackImpl(GLFWwindow * window, double xOffset, double yOffset)
{
	inputManager->mouseScrollCallbackImpl(xOffset, yOffset);
	//gui->injectMouseWheelChange(yOffset);
}

void WindowManagerGLFW::mouseScrollCallback(GLFWwindow * window, double xOffset, double yOffset)
{
	WindowManagerGLFW * inputManager = reinterpret_cast<WindowManagerGLFW *>(glfwGetWindowUserPointer(window));	//Get the window user pointer
	inputManager->mouseScrollCallbackImpl(window, xOffset, yOffset);		//Forward the callback to the member implementation method
}



void WindowManagerGLFW::keyCallbackImpl(GLFWwindow * window, int key, int scancode, int action, int mods)//Key callback implementation method
{
	inputManager->keyCallbackImpl(key, scancode, action, mods);
	//if(action == GLFW_PRESS || action == GLFW_REPEAT)
	//	gui->injectKeyDown(key);
	//else
	//	gui->injectKeyUp(key);
}

void WindowManagerGLFW::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)	//Receives input from the window
{
	WindowManagerGLFW * windowManager = reinterpret_cast<WindowManagerGLFW *>(glfwGetWindowUserPointer(window));	//Get the window user pointer
	windowManager->keyCallbackImpl(window, key, scancode, action, mods);		//Forward the callback to the member implementation method
}



void WindowManagerGLFW::charCallbackImpl(GLFWwindow * window, unsigned int codePoint)
{
	inputManager->charCallbackImpl(codePoint);
	//gui->injectChar(codePoint);
}

void WindowManagerGLFW::charCallback(GLFWwindow * window, unsigned int codePoint)
{
	WindowManagerGLFW * windowManager = reinterpret_cast<WindowManagerGLFW *>(glfwGetWindowUserPointer(window));	//Get the window user pointer
	windowManager->charCallbackImpl(window, codePoint);								//Forward the callback to the member implementation method
}
