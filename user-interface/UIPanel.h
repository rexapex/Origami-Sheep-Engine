#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include "UIComponent.h"
#include "UIScrollBar.h"

namespace osi
{
	class ComponentPressedCallback
	{
	public:
		virtual void componentPressed(UIComponent * comp) const = 0;
	};

	class UIPanel : public UIComponent	//Deletes pointers to all components upon destruction
	{
	public:
		UIPanel(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
			int listDirection, int horizontalGap, int verticalGap);
		virtual ~UIPanel();

		bool shouldAttach(UIUpdateArguments & args, UIComponent ** compCllbk);

		void calcComponentScreenCoords(os_int_t containerX, os_int_t containerY, os_int_t containerWidth, os_int_t containerHeight);

		//int getNumComponents() const;
		//int getNumTextChars() const;
		//int getCurrentTex() const;
		//const std::string & getText(size_t k) const;

		//Do not call this function! Call instead userInterface->addComponent(Parent, Component)
		void addComponent(UIComponent * comp);

		UIComponent * getComponent(size_t arrayEntry) const {return components[arrayEntry];}
		UIComponent * getComponent(const std::string & name) const {for(size_t i = 0; i < components.size(); i++) {if(components[i]->getName() == name) return components[i];}};

		//Destructs and removes all components from the list box
		void clear(std::vector<UIComponent *> & allComps);


		void addComponentPressedCallback(ComponentPressedCallback * callback) {componentPressedCallbacks.push_back(callback);}

		//DO NOT USE THIS METHOD - Use UserInterface::setScrollBarShown instead
		void setScrollBarShown(std::vector<UIComponent *> & components, os_bool_t showScrollBar, os_int_t texStd, os_int_t texHover, os_int_t texClick);
		os_bool_t isScrollBarShown() const {return scrollBar == nullptr ? false : true;}
	private:

		std::vector<ComponentPressedCallback *> componentPressedCallbacks;

		std::vector<UIComponent *> components;
		
		UIScrollBar * scrollBar;

		int listDirection;				//0 = horizontal, else = vertical
		int horizontalGap, verticalGap;	//The gap on either side
	};
}
