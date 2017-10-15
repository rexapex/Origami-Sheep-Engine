#include "UIButton.h"

namespace osi
{
	UIButton::UIButton(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
		const std::string & text, Origin textOrig) :
		UIComponent(name, orig, x, y, width, height, xExtended, yExtended, texStd, texHover, texClick)
	{
		this->text = text;
		this->textOrigin = textOrig;
	}

	UIButton::~UIButton()
	{

	}

	bool UIButton::shouldAttach(UIUpdateArguments & args, UIComponent ** comp)
	{
		if(args.mousex >= screenX && args.mousex < screenX + screenWidth && args.mousey >= screenY && args.mousey < screenY + screenHeight)
		{
			if(args.left)
			{
				texCurrent = texClick;
				*comp = this;

				for(size_t i = 0; i < buttonPressedCallbacks.size(); i++)
				{
					buttonPressedCallbacks[i]->buttonPressed(this);
				}

				return true;
			}
			texCurrent = texHover;
		}
		else if(texCurrent != texStd)
		{
			texCurrent = texStd;
		}

		return false;
	}
}
