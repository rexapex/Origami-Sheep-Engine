#include "UITextField.h"


namespace osi
{
	UITextField::UITextField(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick) :
		UIComponent(name, orig, x, y, width, height, xExtended, yExtended, texStd, texHover, texClick)
	{
		this->numChars = this->firstChar = 0;
	}

	UITextField::~UITextField()
	{

	}

	bool UITextField::shouldAttach(UIUpdateArguments & args, UIComponent ** comp)
	{
		if(args.mousex >= screenX && args.mousex < screenX + screenWidth && args.mousey >= screenY && args.mousey < screenY + screenHeight)
		{
			if(args.left)
			{
				texCurrent = texClick;
				*comp = this;
				return true;
			}
			texCurrent = texHover;
		} else texCurrent = texStd;

		return false;
	}

	bool UITextField::update(UIUpdateArguments & args, UICallback * cllbk)
	{
		(*cllbk) = UICallback::POST_UPDATE_UI;

		if(args.last != 0)
		{
			if(args.last == '\b')
				text.pop_back();
			else
				text += args.last;

			for(size_t i = 0; i < stringCallbacks.size(); i++)
			{
				stringCallbacks[i]->valueChanged(this, text);
			}
		}

		return true;
	}

	bool UITextField::shouldDettach(UIUpdateArguments & args)
	{
		//If the left mouse button is pressed when outside of the text field then dettach the text field
		if(args.left && !(args.mousex >= screenX && args.mousex < screenX + screenWidth && args.mousey >= screenY && args.mousey < screenY + screenHeight))
		{
			texCurrent = texStd;
			return true;
		}
		return false;
	}
}
