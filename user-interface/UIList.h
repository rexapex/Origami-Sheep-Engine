#pragma once

#include "UIComponent.h"
#include "UIPanel.h"

#include <vector>

namespace osi
{
	template <class T>
	struct UIListComponent
	{
		T linkedElement;
		UIComponent * comp;

		UIListComponent(T elem, UIComponent * comp) : linkedElement(elem), comp(comp) {}
	};

	template <class T>
	class UIList
	{
	public:
		UIList(UIPanel & pnl) : pnl(pnl) {}
		virtual ~UIList() {}

		void addComponent(UserInterface * ui, T linkedElement, UIComponent * comp)
		{
			UIListComponent<T> lstComp(linkedElement, comp);
			comps.push_back(lstComp);
			ui->addComponent(&pnl, comp);
		}

		//Destructs and removes all components from the list box
		void clear(UserInterface * ui) {pnl.clear(ui->getComponents());}//while(!comps.empty()) delete comps.back().comp, comps.pop_back(); ui->clear(pnl);}

		T * getLinkedElement(UIComponent * comp)
		{
			for(size_t i = 0; i < comps.size(); i++)
			{
				if(comp == comps[i].comp)
					return &comps[i].linkedElement;
			}

			return nullptr;
		}

	private:
		UIPanel & pnl;
		std::vector<UIListComponent<T>> comps;
	};
}
