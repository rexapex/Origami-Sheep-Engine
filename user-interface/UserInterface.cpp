/*
 * UserInterface.cpp
 *
 *  Created on: 24 Apr 2016
 *      Author: james_000
 */

#include "UserInterface.h"
#include "UIPanel.h"

namespace osi
{
	UserInterface::UserInterface(int framebufferWidth, int framebufferHeight, InputManager * inputManager)
	{
		this->fbWidth = framebufferWidth;
		this->fbHeight = framebufferHeight;
		this->inputManager = inputManager;

		compToUpdate = nullptr;

		fontManager = new FontManager;
	}

	UserInterface::~UserInterface()
	{

	}




	void UserInterface::updateUI(UICallback * cllbk)
	{
		os_float_t mousex = static_cast<os_float_t>(inputManager->getMouseX());
		os_float_t mousey = static_cast<os_float_t>(fbHeight - (float)inputManager->getMouseY());
		os_float_t dx     = static_cast<os_float_t>(inputManager->getDeltaX());
		os_float_t dy     = static_cast<os_float_t>(inputManager->getDeltaY());
		os_bool_t left    = static_cast<os_bool_t>(inputManager->getInputValue("OSE::LEFT"));
		os_bool_t right   = static_cast<os_bool_t>(inputManager->getInputValue("OSE::RIGHT"));
		wchar_t last = inputManager->getLastCharTyped();

		UIUpdateArguments args {mousex, mousey, dx, dy, left, right, last};

		if(compToUpdate == nullptr)	//If no component is being updated...
		{
			for(size_t i = 0; i < rootComponents.size(); i++)	//Then test whether each component should be being updated
			{
				UIComponent & comp = *rootComponents[i];

				if(comp.shouldAttach(args, &compToUpdate))
					break;
			}
		}
		else
		{
			if(!compToUpdate->shouldDettach(args))		//If component to update should still be updated
			{
				if(compToUpdate->update(args, cllbk))	//Then continue to update component
				{
					//update returned true, therefore, POST_UPDATE_UI required
				//	(*cllbk)
				}
			}
			else
				compToUpdate = nullptr;
		}
	}
	

	void UserInterface::calcComponentsScreenCoords()	//Calculate the component screen coordinates of every component
	{
		for(auto c : rootComponents)
		{
			c->calcComponentScreenCoords(0, 0, fbWidth, fbHeight);	//Function is recursive, therefore, only need to call on root components
		}
	}




	void UserInterface::addComponent(UIComponent * comp)
	{
		//Prevents a dividing by zero error when calculating screen coordinates
		if(comp->xExtended && comp->transform.relWidth == 0)
			comp->transform.relWidth = 1;
		if(comp->yExtended && comp->transform.relHeight == 0)
			comp->transform.relHeight = 1;

		comp->transform.parentTransform = nullptr;

		components.push_back(comp);
		rootComponents.push_back(comp);	//No parent means that it is a root component
	}


	void UserInterface::addComponent(UIPanel * parentComp, UIComponent * comp)
	{
		if(parentComp == nullptr)		//If called with parent as nullptr, then call the convenience function and return from this one
			return addComponent(comp);

		parentComp->addComponent(comp);

		//Prevents a dividing by zero error when calculating screen coordinates
		if(comp->xExtended && comp->transform.relWidth == 0)
			comp->transform.relWidth = 1;
		if(comp->yExtended && comp->transform.relHeight == 0)
			comp->transform.relHeight = 1;

		comp->transform.parentTransform = nullptr;

		components.push_back(comp);
	}


	void UserInterface::setScrollBarShown(UIPanel * parentComp, os_bool_t showScrollBar, os_int_t texStd, os_int_t texHover, os_int_t texClick)
	{
		parentComp->setScrollBarShown(components, showScrollBar, texStd, texHover, texClick);
	}




	/*int UserInterface::calcNumComponents()
	{
		int numComponents = 0;

		for(int i = 0; i < components.size(); i++)
		{
			numComponents += components[i]->getNumComponents();
		}

		return numComponents;
	}*/


	size_t UserInterface::getNumTextChars() const
	{
		size_t numTextChars = 0;

		for(auto component : components)
		{
			numTextChars += component->getText().size();
		}

		return numTextChars;
	}
}
