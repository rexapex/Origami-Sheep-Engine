#pragma once

#include "../rendering/RenderingEngine.h"
#include "UIComponent.h"

#include <string>

namespace osi
{
	class UITextField;

	class StringCallback
	{
	public:
		virtual void valueChanged(UITextField * txt, std::string & str) const = 0;
	};

	class UITextField : public UIComponent
	{
	public:
		UITextField(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick);
		virtual ~UITextField();

		bool shouldAttach(UIUpdateArguments & args, UIComponent ** comp);
		bool update(UIUpdateArguments & args, UICallback * cllbk);
		bool shouldDettach(UIUpdateArguments & args);

		//size_t getNumTextChars() const {return text.size();}
		//const std::string & getText(size_t k) const {return text;}
		const std::string & getText() const {return text;}
		const os_uint_t getCaretPos() const {return caretPos;}
		const void setCaretPos(const os_uint_t pos) {caretPos = pos;}
		void setText(const std::string & text) {this->text = text;}
		Origin getTextOrigin() const override {return Origin::LEFT_CENTREY;}

		void addStringCallback(StringCallback * callback) {stringCallbacks.push_back(callback);}
	private:
		os_uint_t caretPos;

		std::vector<StringCallback *> stringCallbacks;

		bool typing;
		size_t firstChar, numChars;

		std::string text;
	};
}
