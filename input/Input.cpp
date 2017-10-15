/*
 * Input.cpp
 *
 *  Created on: 27 Nov 2015
 *      Author: james_000
 */

#include "Input.h"
#include "iostream"

namespace osi
{
	Input::Input()
	{
		method1Eval = method2Eval = method2Enabled = false;
	}

	Input::~Input()
	{
		// TODO Auto-generated destructor stub
	}




	void Input::setInputMethod1(os_int_t inputID, os_uint_t requiredInputState, os_int_t inputType)	//Set up the requirements for input method 1 to evaluated to true
	{
		method1.inputID = inputID;
		method1.requiredState = requiredInputState;
		method1.inputType = inputType;
	}

	void Input::setInputMethod2(os_int_t inputID, os_uint_t requiredInputState, os_int_t inputType, os_bool_t enabled)//Set up the requirements for input method 2 to evaluated to true
	{
		method2.inputID = inputID;
		method2.requiredState = requiredInputState;
		method2.inputType = inputType;
		method2Enabled = enabled;
	}




	void Input::evaluateInputMethod1(os_int_t currentInputState)	//Evaluates method1Val to true if inputState equals the required input state
	{
		//((inputState & method1.inputState) > 0)	Allows GLFW_PRESS and GLFW_REPEAT to be combined
		method1Eval = ((currentInputState & method1.requiredState) != 0) ? true : false;
		//std::cout << "Required State: " << method1.requiredState << ", State: " << currentInputState << std::endl;
		//std::cout << "Value: " << ((currentInputState ^ method1.requiredState) << 1) << std::endl;
	}

	void Input::evaluateInputMethod2(os_int_t currentInputState)	//Evaluates method2Val to true if inputState equals the required input state
	{
		method2Eval = (~(currentInputState ^ method1.requiredState) != 0) ? true : false;
	}




	os_bool_t Input::getInputEvaluation() const	//Returns whether the input is currently evaluated as true or false
	{
		return method2Enabled ? method1Eval && method2Eval : method1Eval;
	}
} /* namespace osi */







