#pragma once

#include "../rendering/RenderingEngine.h"
#include "UIComponent.h"
#include "UIPanel.h"

namespace osi
{
	class UIWindow : public UIComponent
	{
	public:
		UIWindow(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
			os_bool_t movable, os_bool_t resizable, os_bool_t minimisable, os_bool_t closable, os_int_t texTitleBar, os_int_t texMinimise, os_int_t texFullScreen, os_int_t texClose);
		~UIWindow();

		bool shouldAttach(UIUpdateArguments & args, UIComponent ** comp);
		bool update(UIUpdateArguments & args, UICallback * cllbk);

		void calcComponentScreenCoords(os_int_t containerX, os_int_t containerY, os_int_t containerWidth, os_int_t containerHeight);

		size_t getNumComponents() {return 2;}	//pnlContent includes itself
		//os_int_t getCurrentTex() {if(k == 0) return texStd; else return texTitleBar;}

		void setRestraintPanel(UIPanel * pnlRestraint) {this->pnlRestraint = pnlRestraint;}

		void addComponent(UIComponent * comp) {pnlContent->addComponent(comp);}
		UIComponent * getComponent(size_t arrayEntry) const {return pnlContent->getComponent(arrayEntry);}
		UIComponent * getComponent(const std::string & name) const {return pnlContent->getComponent(name);};

	private:
		UIPanel * pnlContent, * pnlTitleBar, * pnlRestraint;

		os_bool_t movable, resizable, minimisable, closable;
		os_bool_t moving;

		os_int_t absXTitleBar, absYTitleBar, absWTitleBar, absHTitleBar;

		os_int_t texTitleBar, texMinimise, texClose, texFullScreen;
	};
}
