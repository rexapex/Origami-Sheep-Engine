/*
 * Input.h
 *
 *  Created on: 27 Nov 2015
 *      Author: james_000
 */

#ifndef INPUT_INPUT_H_
#define INPUT_INPUT_H_

#include <string>
#include "../Globals.h"

namespace osi
{
	enum InputType {KEY = 0, MOUSE_MOVE = 1, MOUSE_CLICK = 2, MOUSE_SCROLL = 3};	//Types of input which are available in GLFW

	struct InputMethod
	{
		os_int_t inputID;			//The ID of the input, e.g. for the type KEY and key E key, inputID = GLFW_KEY_E
		os_uint_t requiredState;	//The state of the input required, e.g. if the type is KEY, the state required can be pressed GLFW_PRESS
		os_int_t inputType;			//The type of input, e.g. KEY, MOUSE_CLICK
	};

	class Input
	{
	public:
		Input();
		virtual ~Input();

		void setInputMethod1(os_int_t inputID, os_uint_t requiredInputState, os_int_t inputType);
		void setInputMethod2(os_int_t inputID, os_uint_t requiredInputState, os_int_t inputType, os_bool_t enabled);
		void setEnabledInput2(os_bool_t enabled)	{this->method2Enabled = enabled;}

		void evaluateInputMethod1(os_int_t currentInputState);	//Evaluates whether input method 1s requirements are met
		void evaluateInputMethod2(os_int_t currentInputState);	//Evaluates whether input method 2s requirements are met
		os_bool_t getInputEvaluation() const;	//Returns the value of the input, true if both methods match their required states, false otherwise

		InputMethod getInputMethod1() const	{return method1;}
		InputMethod getInputMethod2() const	{return method2;}

		void setName(const std::string & name)	{this->name = name;}
		std::string getName() const		{return this->name;}
	private:
		std::string name;					//The name of the input method so it can be searched for by name
		InputMethod method1, method2;		//Allows multi-inputs by having two InputMethod objects
		os_bool_t method1Eval, method2Eval;	//If the the state is true, eval. = true, if the state is not true, eval. = false
		os_bool_t method2Enabled;			//If true, input 1 must be triggered, then input 2, if false, only input 1 is usedd
	};
} /* namespace osi */
#endif /* INPUT_INPUT_H_ */
