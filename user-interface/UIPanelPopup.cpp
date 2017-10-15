#include "UIPanelPopup.h"

namespace osi
{
	UIPanelPopup::UIPanelPopup(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
		int listDirection, int horizontalGap, int verticalGap) :
		UIPanel(name, orig, x, y, width, height, xExtended, yExtended, texStd, texHover, texClick,
			listDirection, horizontalGap, verticalGap)
	{

	}

	UIPanelPopup::~UIPanelPopup()	//Base destructor called automatically
	{

	}

	//Returns true if the component is deselected
	bool UIPanelPopup::shouldDettach(UIUpdateArguments & args)
	{
		return (args.mousex < screenX || args.mousex > screenX + screenWidth
			|| args.mousey > screenY || args.mousey < screenY + screenHeight) && (args.left || args.right) ? true : false;
	}
}
