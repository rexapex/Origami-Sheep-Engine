#pragma once

#include "../Globals.h"
#include <vector>

namespace osi
{
	enum class Origin
	{
		LEFT_TOP = 0,		LEFT_CENTREY = 1,		LEFT_BOTTOM = 2,
		CENTREX_TOP = 3,	CENTREX_CENTREY = 4,	CENTREX_BOTTOM = 5,
		RIGHT_TOP = 6,		RIGHT_CENTREY = 7,		RIGHT_BOTTOM = 8
	};

	struct UITransform
	{
		Origin orig;
		os_int_t relX, relY, relWidth, relHeight;
		UITransform * parentTransform;
	};

	os_int_t getGlobalXUITransform(UITransform * t);
	os_int_t getGlobalYUITransform(UITransform * t);
	os_int_t getGlobalWidthUITransform(UITransform * t);
	os_int_t getGlobalHeightUITransform(UITransform * t);
}
