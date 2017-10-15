#pragma once

#include "UserInterface.h"
#include "UIComponent.h"
#include <iostream>

namespace osi
{
	class UIButton;

	struct ButtonPressedCallback
	{
	public:
		virtual void buttonPressed(UIButton * btn) const = 0;
	};

	class UIButton : public UIComponent
	{
	public:
		UIButton(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
			const std::string & text, Origin textOrigin);
		virtual ~UIButton();

		bool shouldAttach(UIUpdateArguments & args, UIComponent ** comp);

		void addButtonPressedCallback(ButtonPressedCallback * cllbk) {buttonPressedCallbacks.push_back(cllbk);}

		//size_t getNumTextChars() const {return text.size();}
		//const std::string & getText(size_t k) const {return text;}
		const std::string & getText() const {return text;}
		Origin getTextOrigin() const override {return textOrigin;}
	private:
		std::string text;
		Origin textOrigin;

		std::vector<ButtonPressedCallback *> buttonPressedCallbacks;
	};
}
