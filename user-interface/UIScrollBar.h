#pragma once

#include "../rendering/RenderingEngine.h"
#include "UIComponent.h"

#include <iostream>

namespace osi
{
	class UIScrollBar : public UIComponent
	{
	public:
		UIScrollBar(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick);
		virtual ~UIScrollBar();


		bool shouldAttach(UIUpdateArguments & args, UIComponent ** comp);
		bool update(UIUpdateArguments & args, UICallback * cllbk);



		int scrollOffset;	//The amount to offset all components by
		int listDirection;	//0 = horizontal, else = vertical
		int maxDistance;	//The maximum distance the scroll bar can travel from the relative origin

	private:
		bool moving;		//The scroll bar has been pressed & movements of the mouse move the scroll bar
	};
};
