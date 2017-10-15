#pragma once

#include "../Globals.h"
#include "UIComponent.h"
#include <string>

namespace osi
{
	class UILabel : public UIComponent
	{
	public:
		UILabel(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
			const std::string & text, Origin textOrigin);
		virtual ~UILabel();
		
		//size_t getNumTextChars() const {return text.size();}
		//const std::string & getText(size_t k) const {return text;}
		const std::string & getText() const {return text;}
		const void setText(const std::string & t) {text = t;}
		Origin getTextOrigin() const override {return textOrigin;}
	private:
		std::string text;
		Origin textOrigin;
	};
}
