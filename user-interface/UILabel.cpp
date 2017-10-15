#include "UILabel.h"

namespace osi
{
	UILabel::UILabel(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
		const std::string & text, Origin textOrig) :
		UIComponent(name, orig, x, y, width, height, xExtended, yExtended, texStd, texHover, texClick)
	{
		this->text = text;
		this->textOrigin = textOrig;
	}

	UILabel::~UILabel()
	{

	}
}
