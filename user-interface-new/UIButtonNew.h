#pragma once

#include "../Globals.h"
#include "UserInterfaceNew.h"

class UIButton //: public UIComponent
{
public:
	UIButton();
	~UIButton();

	virtual os_bool_t shouldAttach();	//Returns true if component should be updated
	virtual os_bool_t shouldDettach();	//Returns true if component should no longer continue being updated
	virtual os_bool_t update();			//Updates the component and returns true if UI co-ords in shader should be updated
};

