#include "UIButtonNew.h"



UIButton::UIButton()
{

}


UIButton::~UIButton()
{

}

os_bool_t UIButton::shouldAttach()	//Returns true if component should be updated
{
	return false;
}

os_bool_t UIButton::shouldDettach()	//Returns true if component should no longer continue being updated
{
	return false;
}

os_bool_t UIButton::update()		//Updates the component and returns true if UI co-ords in shader should be updated
{
	return false;
}
