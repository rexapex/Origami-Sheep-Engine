#pragma once

#include "../Globals.h"
#include "../input/InputManager.h"

#include <string>

namespace osui
{
	struct UIGlobalTransform
	{
		os_float_t x, y, w, h;
	};

	struct UIRenderable
	{
		UIGlobalTransform & transform;
		os_int_t & tex;					//Tex Index into TexAtlas - Not OpenGL Tex ID
	};

	struct UITextRenderable
	{
		UIGlobalTransform & transform;
		std::string text;
	};

	class UIComponent
	{
	public:
		//Only one component updated every UserInterface::update call
		virtual os_bool_t shouldAttach() = 0;	//Returns true if component should be updated
		virtual os_bool_t shouldDettach() = 0;	//Returns true if component should no longer continue being updated
		virtual os_bool_t update() = 0;			//Updates the component and returns true if UI co-ords in shader should be updated
	private:
		os_float_t x, y, w, h;			//Relative x, y, width, height
		UIGlobalTransform gloTransform;	//Global x, y, width, height stored in structure
	};


	class UserInterface
	{
	public:
		UserInterface(os_int_t framebufferWidth, os_int_t framebufferHeight, osi::InputManager * inputManager);
		~UserInterface();

		os_void_t update();

		//Methods for adding components to the UI
		os_uint_t addButton();
		/*os_uint_t addPanel();
		os_uint_t addWindow();
		os_uint_t addListBox();
		os_uint_t addLabel();
		os_uint_t addSlider();
		os_uint_t addTextField();
		os_uint_t addPanelPopUp();
		os_uint_t addDropDown();
		os_uint_t addNumericUpDown();
		os_uint_t addScrollBar();*/
	private:
		osi::InputManager * inputManager;
//		FontManager * fontManager;

		UIComponent * compToUpdate;		//The component to be fully updated - changes based on result of post update
		
		os_int_t fbWidth, fbHeight;
	};
}
