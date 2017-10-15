#pragma once

#include "UIPanel.h"

namespace osi
{
	class UIPanelPopup : public UIPanel
	{
	public:
		UIPanelPopup(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
			int listDirection, int horizontalGap, int verticalGap);
		~UIPanelPopup();

		//Returns true if the component is deselected
		virtual bool shouldDettach(UIUpdateArguments & args);
	};
}
