#include "UserInterfaceNew.h"


namespace osui
{
	UserInterface::UserInterface(os_int_t framebufferWidth, os_int_t framebufferHeight, osi::InputManager * inputManager)
	{
		this->fbWidth = framebufferWidth;
		this->fbHeight = framebufferHeight;
		this->inputManager = inputManager;
	}

	UserInterface::~UserInterface()
	{

	}

	os_void_t UserInterface::update()
	{
		//Does nothing at the moment
	}

	//Methods for adding components to the UI
	os_uint_t UserInterface::addButton()
	{
		return 0;
	}
}

