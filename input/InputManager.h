/*
 * InputManager.h
 *
 *  Created on: 27 Nov 2015
 *      Author: james_000
 */

#ifndef INPUT_INPUTMANAGER_H_
#define INPUT_INPUTMANAGER_H_

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "../Globals.h"
#include <vector>
#include <string>
//#include <locale>
//#include <codecvt>
//#include <cassert>
#include "Input.h"

namespace osi
{
	class InputManager
	{
	public:
		InputManager(GLFWwindow * window);
		virtual ~InputManager();

		//Updates all input methods which aren't implemented using callbacks
		void updateInput();

		//Adds the input specified, if inputType and requiredInputState do not match -> undefined behaviour
		//requiredInputState is the state of the input for the input to be evaluated to true
		void addInput(std::string name, int inputID, int requiredInputState, int inputType);
		//Works the same as addInput but makes a multi-input, an input where one input must be true and then the next in order for
		//the whole input to be evaluated to true
		void addMultiInput(std::string name, int inputID, int requiredInputState, int inputType, int input2ID, int requiredInput2State, int input2Type);
		//Can either be a prebuilt single input or prebuilt multi input
		void addInput(Input input) {inputs.push_back(input);}


		//Modifies existing input with the specified name, for adding a new input use addInput or addMultiInput
		void setInput(std::string name, int inputID, int requiredInputState, int inputType);

		//Returns the state of the input with the specified name, the input can either be evaluated to true or false
		os_bool_t getInputValue(std::string name);


		//Callback functions called from the window manager class
		void cursorPosCallbackImpl(double xPos, double yPos);
		void mouseButtonCallbackImpl(int button, int action, int mods);
		void mouseScrollCallbackImpl(double xOffset, double yOffset);
		void keyCallbackImpl(int key, int scancode, int action, int mods);
		void charCallbackImpl(unsigned int codePoint);


		double getMouseX() const {return lastMouseX;}
		double getMouseY() const {return lastMouseY;}
		double getDeltaX() const {return deltaX;}
		double getDeltaY() const {return deltaY;}


		//Function resets last char typed after func call
		//Function will only return lat char typed once
		wchar_t getLastCharTyped() {wchar_t returnChar = lastCharTyped; lastCharTyped = 0; return returnChar;}
	private:
		GLFWwindow * window;

		wchar_t lastCharTyped;

		double lastMouseX, lastMouseY, deltaX, deltaY;

		std::vector<Input> inputs;		//List of all inputs
	};
} /* namespace osi */

#endif /* INPUT_INPUTMANAGER_H_ */
