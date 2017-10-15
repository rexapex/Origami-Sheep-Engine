/*
 * UserInterface.h
 *
 *  Created on: 24 Apr 2016
 *      Author: james_000
 */

#ifndef USER_INTERFACE_USERINTERFACE_H_
#define USER_INTERFACE_USERINTERFACE_H_

//#include <ft2build.h>
//#include FT_FREETYPE_H

#include <vector>
#include <algorithm>
#include "../glm/glm.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "../Globals.h"
#include <iostream>
#include "UIComponent.h"
#include "../input/InputManager.h"
#include "FontManager.h"

namespace osi
{
	class UIPanel;	//Required due to loop in header includes
	class UIScrollBar;

	class UserInterface
	{
	public:
		UserInterface(int framebufferWidth, int framebufferHeight, InputManager * inputManager);
		virtual ~UserInterface();

		void updateUI(UICallback * cllbk);
		void calcComponentsScreenCoords();
		//void calcComponentsScreenCoords(os_int_t containerX, os_int_t containerY, os_int_t containerWidth, os_int_t containerHeight);	//Probably not needed function

		//For use when clearing a standard UIPanel
	//	void clear(UIPanel * pnl) {while(!pnl->components.empty()) components.erase(std::remove(components.begin(), components.end(), components.back()), components.end()),  components.pop_back();}
		
		void addComponent(UIComponent * comp);	//Parent component set to be nullptr
		void addComponent(UIPanel * parentComp, UIComponent * comp);
		void setScrollBarShown(UIPanel * parentComp, os_bool_t showScrollBar, os_int_t texStd, os_int_t texHover, os_int_t texClick);

		//glm::vec2 getComponentScreenCoords(int arrayEntry);
		//glm::vec2 getComponentScreenSize(int arrayEntry);

		std::vector<UIComponent *> & getComponents() {return components;}
		UIComponent * getComponent(size_t arrayEntry) const {return arrayEntry < components.size() ? components[arrayEntry] : nullptr;}
		UIComponent * getComponent(const std::string & name) const {for(size_t i = 0; i < components.size(); i++) {if(components[i]->getName() == name) return components[i];} return nullptr;}


		size_t getNumComponents() const {return components.size();}
		size_t getNumTextChars() const;
		//int calcNumComponents();
		//int calcNumTextChars();

		int getFramebufferWidth()  const {return fbWidth;}
		int getFramebufferHeight() const {return fbHeight;}

		std::vector<glm::vec4> texCoords;

		void setFramebufferSize(int width, int height) {fbWidth = width; fbHeight = height;}

		void setInputManager(InputManager * inputManager) {this->inputManager = inputManager;}

		FontManager * getFontManager() const {return fontManager;}
	private:
		InputManager * inputManager;
		FontManager * fontManager;

		UIComponent * compToUpdate;		//The component to be fully updated - changes based on result of post update

		int fbWidth, fbHeight;
		std::vector<UIComponent *> components;
		std::vector<UIComponent *> rootComponents;
	};
}

#endif /* USER_INTERFACE_USERINTERFACE_H_ */
