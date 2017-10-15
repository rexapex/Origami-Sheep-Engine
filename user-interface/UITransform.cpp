#include "UITransform.h"

namespace osi
{
	os_int_t getGlobalXUITransform(UITransform * t)
	{
		if(t->parentTransform != nullptr)
			return t->relX + getGlobalXUITransform(t->parentTransform);
		else
			return t->relX;
	}

	os_int_t getGlobalYUITransform(UITransform * t)
	{
		if(t->parentTransform != nullptr)
			return t->relY + getGlobalYUITransform(t->parentTransform);
		else
			return t->relY;
	}

	os_int_t getGlobalWidthUITransform(UITransform * t)
	{
		if(t->parentTransform != nullptr)
			return t->relWidth + getGlobalWidthUITransform(t->parentTransform);
		else
			return t->relWidth;
	}

	os_int_t getGlobalHeightUITransform(UITransform * t)
	{
		if(t->parentTransform != nullptr)
			return t->relHeight + getGlobalHeightUITransform(t->parentTransform);
		else
			return t->relHeight;
	}
}