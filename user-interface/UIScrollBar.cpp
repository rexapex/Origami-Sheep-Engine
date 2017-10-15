#include "UIScrollBar.h"


namespace osi
{
	UIScrollBar::UIScrollBar(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick) :
		UIComponent(name, orig, x, y, width, height, xExtended, yExtended, texStd, texHover, texClick)
	{
		this->moving = false;
		this->scrollOffset = 0;
	}

	UIScrollBar::~UIScrollBar()
	{

	}



	bool UIScrollBar::shouldAttach(UIUpdateArguments & args, UIComponent ** comp)
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

	bool UIScrollBar::update(UIUpdateArguments & args, UICallback * cllbk)
	{
		if(listDirection == 0)
		{
			transform.relX = transform.relX + args.dx;
			if (transform.relX > maxDistance)
				transform.relX = maxDistance;
			else if(transform.relX < 0)
				transform.relX = 0;
		}
		else
		{
			transform.relY = transform.relY + args.dy;
			if (transform.relY > maxDistance)
				transform.relY = maxDistance;
			else if(transform.relY < 0)
				transform.relY = 0;
			//transform.relY = transform.relY + args.dy < maxDistance && transform.relY + args.dy > 0 ? transform.relY + args.dy : (transform.relY + args.dy >= maxDistance ? maxDistance : 0);
		}

		*cllbk = UICallback::POST_UPDATE_UI;
		
		return true;
	}
};
