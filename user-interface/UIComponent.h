#pragma once

#include "../glm/glm.hpp"
#include "UITransform.h"
#include "../Globals.h"
#include <vector>
#include <iostream>

namespace osi
{
	enum class UICallback
	{
		NONE, POST_UPDATE_UI
	};

	struct UIUpdateArguments
	{
		os_float_t mousex, mousey, dx, dy;
		os_bool_t left, right;
		wchar_t last;
	};

	class UIComponent
	{
	public:
		UIComponent(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick);
		virtual ~UIComponent();

		virtual void freeComponent() {}

		virtual void clear(std::vector<UIComponent *> & allComps) {}

		//Returns true if component is selected
		virtual bool shouldAttach(UIUpdateArguments & args, UIComponent ** comp) {return false;}
		//Returns true if the ui needs to be updated
		virtual bool update(UIUpdateArguments & args, UICallback * cllbk) {return false;}
		//Returns true if the component is deselected
		virtual bool shouldDettach(UIUpdateArguments & args);

		virtual void calcComponentScreenCoords(os_int_t containerX, os_int_t containerY, os_int_t containerWidth, os_int_t containerHeight);
		const glm::vec4 & getComponentScreenCoords() const {return componentScreenCoords;}

		///virtual int getNumComponents() const {return 1;}
		virtual os_int_t getCurrentTex() const {return texCurrent;}
		///virtual size_t getNumTextChars() const {return 0;}
		virtual const std::string & getText() const {return os_emptystring;}
		virtual Origin getTextOrigin() const {return Origin::CENTREX_CENTREY;}

		UITransform transform;
		float screenX, screenY, screenWidth, screenHeight;
		bool xExtended, yExtended;						//If true, width and height values specify divisons of extended
		int texCurrent, texStd, texHover, texClick;		//NOT OpenGL texture IDs - Used to determine correct texture coords from texture atlas

		UIComponent * parent;	//This UIComponent is positioned realtive to parent UIComponent

		const std::string & getName() const {return name;}
	private:
		std::string name;
		UIComponent * panelPopup;
	protected:
		//std::vector<glm::vec4> componentScreenCoords;
		glm::vec4 componentScreenCoords;
	};
}
