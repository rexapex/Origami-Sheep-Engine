#pragma once

#include "../rendering/RenderingEngine.h"
#include "UserInterface.h"
#include "UIComponent.h"
#include "UIScrollBar.h"
#include "../io-resource-management/ResourceManager.h"

namespace osi
{
	/*template <class T> struct UIListComponent
	{
		UIComponent * comp;
		T & t;
	};

	struct UIList
	{
		std::vector<UIListComponent<osrm::Entity>> listComps;
	};

	class UIListBox : public UIComponent
	{
	public:
		UIListBox(std::string name, Origin orig, int x, int y, int width, int height, bool xExtended, bool yExtended, int texStd, int texHover, int texClick,
			int listDirection, int horizontalGap, int verticalGap);
		virtual ~UIListBox();

		bool update(float mousex, float mousey, float dx, float dy, int left, int right);

		std::vector<glm::vec4> getComponentScreenCoords(int containerX, int containerY, int containerWidth, int containerHeight);

		int getNumComponents() const;
		int getNumTextChars() const;
		int getCurrentTex(size_t k) const;
		const std::string & getText(size_t k) const;

		void addComponent(UIComponent * comp, osrm::Entity & t);
		void clear() {while(!list.listComps.empty()) delete list.listComps.back().comp, list.listComps.pop_back();}	//Destructs and removes all components from the list box

		void addComponentSelectedCallback(void(* callback)(UIListComponent<osrm::Entity> &)) {componentSelectedCallbacks.push_back(callback);}
		void setScrollBarShown(bool showScrollBar, int texStd, int texHover, int texClick, osr::RenderingEngine * renderingEngine);
	private:

		std::vector<void(*)(UIListComponent<osrm::Entity> &)> componentSelectedCallbacks;

		UIList list;
		//std::vector<UIListComponent *> components;

		UIScrollBar * scrollBar;

		int listDirection;				//0 = horizontal, else = vertical
		int horizontalGap, verticalGap;	//The gap on either side
	};*/
}
