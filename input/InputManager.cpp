/*
 * InputManager.cpp
 *
 *  Created on: 27 Nov 2015
 *      Author: james_000
 */

#include "InputManager.h"
#include <iostream>
#include "Input.h"

namespace osi
{
	InputManager::InputManager(GLFWwindow * window)
	{
		this->window = window;
		lastMouseX = lastMouseY = deltaX = deltaY = 0;
		lastCharTyped = 0;
	}

	InputManager::~InputManager()
	{
		// TODO Auto-generated destructor stub
	}


	void InputManager::updateInput()
	{
		double x, y;
		glfwGetCursorPos(this->window, &x, &y);

		deltaX = x - lastMouseX;
		deltaY = y - lastMouseY;

		lastMouseX = x;
		lastMouseY = y;
	}



	//Adds the input specified, if inputType and requiredInputState do not match -> undefined behaviour
	//requiredInputState is the state of the input for the input to be evaluated to true
	void InputManager::addInput(std::string name, int inputID, int requiredInputState, int inputType)
	{
		Input input;
		input.setName(name);
		input.setInputMethod1(inputID, requiredInputState, inputType);
		inputs.push_back(input);
	}

	//Works the same as addInput but makes a multi-input, an input where one input must be true and then the next in order for
	//the whole input to be evaluated to true
	void InputManager::addMultiInput(std::string name, int inputID, int requiredInputState,
			int inputType, int input2ID, int requiredInput2State, int input2Type)
	{
		Input input;
		input.setName(name);
		input.setInputMethod1(inputID, requiredInputState, inputType);
		input.setInputMethod2(input2ID, requiredInput2State, input2Type, true);
		inputs.push_back(input);
	}

	//Modifies existing input with the specified name, for adding a new input use addInput or addMultiInput
	void InputManager::setInput(std::string name, int inputID, int requiredInputState, int inputType)
	{
		for(unsigned i = 0; i < inputs.size(); i++)
		{
			if(inputs[i].getName() == name)
			{
				inputs[i].setInputMethod1(inputID, requiredInputState, inputType);
			}
		}
	}

	//Returns the state of the input with the specified name, the input can either be evaluated to true or false
	//If the name given does not match an input, the method returns false
	os_bool_t InputManager::getInputValue(std::string name)
	{
		for(unsigned i = 0; i < inputs.size(); i++)
		{
			//std::cout << "Input Name: " << inputs[i].getName() << ", Searched Name: " << name << std::endl;
			if(inputs[i].getName() == name)
			{
				return inputs[i].getInputEvaluation();
			}
		}

		return false;
	}





	void InputManager::cursorPosCallbackImpl(double xPos, double yPos)
	{
		std::cerr << "X Pos: " << xPos << ", Last X Pos: " << lastMouseX << std::endl;

		deltaX = xPos - lastMouseX;
		deltaY = yPos - lastMouseY;
		/*for(unsigned i = 0; i < inputs.size(); i++)
		{
			if(inputs[i].getInputMethod1().inputType == MOUSE_MOVE)		//Set method 1 if direction and type matches
			{
				if(inputs[i].getInputMethod1().inputID == 0)	//Input ID of 0 means a change in x pos is required
					inputs[i].evaluateInputMethod1(deltaX);
				else
					inputs[i].evaluateInputMethod1(deltaY);		//Input ID other than 1 means a change in y is required
			}
			else if(inputs[i].getInputMethod2().inputType == MOUSE_MOVE)	//Set method 2 if direction and type matches
			{
				if(inputs[i].getInputMethod2().inputID == 0)	//Input ID of 0 means a change in x pos is required
					inputs[i].evaluateInputMethod2(deltaX);
				else
					inputs[i].evaluateInputMethod2(deltaY);		//Input ID other than 1 means a change in y is required
			}
		}*/
		lastMouseX = xPos;
		lastMouseY = yPos;
	}



	void InputManager::mouseButtonCallbackImpl(int button, int action, int mods)
	{
		for(unsigned i = 0; i < inputs.size(); i++)
		{
			if(inputs[i].getInputMethod1().inputType == MOUSE_CLICK && inputs[i].getInputMethod1().inputID == button)	//Set method 1 if the key matches
				inputs[i].evaluateInputMethod1(action);
			else if(inputs[i].getInputMethod2().inputType == MOUSE_CLICK && inputs[i].getInputMethod2().inputID == button)	//Set method 2 if the key matches
				inputs[i].evaluateInputMethod2(action);
		}
	}



	void InputManager::mouseScrollCallbackImpl(double xOffset, double yOffset)
	{
		//TODO - Add Code
	}



	void InputManager::keyCallbackImpl(int key, int scancode, int action, int mods)//Key callback implementation method
	{
		for(unsigned i = 0; i < inputs.size(); i++)
		{
			if(inputs[i].getInputMethod1().inputType == KEY && inputs[i].getInputMethod1().inputID == key)	//Set method 1 if the key matches
				inputs[i].evaluateInputMethod1(action);
			else if(inputs[i].getInputMethod2().inputType == KEY && inputs[i].getInputMethod2().inputID == key)	//Set method 2 if the key matches
				inputs[i].evaluateInputMethod2(action);
		}
	}



	void InputManager::charCallbackImpl(unsigned int codePoint)
	{
		//std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
		//std::string u8str = converter.to_bytes(codePoint);
		lastCharTyped = wchar_t(codePoint);//u8str.at(0);
	}
} /* namespace osi */


