#include "UIPanel.h"

namespace osi
{
	UIPanel::UIPanel(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
		int listDirection, int horizontalGap, int verticalGap) :
		UIComponent(name, orig, x, y, width, height, xExtended, yExtended, texStd, texHover, texClick)
	{
		this->scrollBar = nullptr;
		this->listDirection = listDirection;
		this->horizontalGap = horizontalGap;
		this->verticalGap = verticalGap;
	}

	UIPanel::~UIPanel()
	{
		//while(!components.empty()) delete components.back(), components.pop_back();
	}




	bool UIPanel::shouldAttach(UIUpdateArguments & args, UIComponent ** compCllbk)
	{
		if(args.mousex >= screenX && args.mousex < screenX + screenWidth && args.mousey >= screenY && args.mousey < screenY + screenHeight)
		{
			texCurrent = args.left ? texClick : texHover;

			for(size_t i = 0; i < components.size(); i++)
			{
				UIComponent * subComp = components[i];
				
				if(subComp->shouldAttach(args, compCllbk))
				{
					for(size_t j = 0; j < componentPressedCallbacks.size(); j++)
					{
						componentPressedCallbacks[j]->componentPressed(subComp);
					}

					break;
				}
			}

			if(scrollBar != nullptr)
				scrollBar->shouldAttach(args, compCllbk);

			return true;
		}

		return false;
	}

	void UIPanel::calcComponentScreenCoords(os_int_t containerX, os_int_t containerY, os_int_t containerWidth, os_int_t containerHeight)
	{
		///std::vector<glm::vec4> lstScreenCoords;
		///std::vector<glm::vec4> lstBase;
		UIComponent::calcComponentScreenCoords(containerX, containerY, containerWidth, containerHeight);

		///lstScreenCoords.insert(lstScreenCoords.end(), lstBase.begin(), lstBase.end());

		//If horizontal panels components go off end of panel then include a scroll bar
		/*if(listDirection == 0 && components.size() > 0 && components.back()->screenX + components.back()->screenWidth > screenWidth + screenX)
			scrollBarShown = true;
		//If vertical panels components go off end of panel then include a scroll bar
		else if(listDirection != 0 && components.size() > 0 && components.back()->screenY < screenY)
			scrollBarShown = true;
		else
			scrollBarShown = false;*/


		//Make sure components are within the bounds of the panel and the vertical/horizontal gaps
		if(scrollBar != nullptr)
		{
			if(listDirection == 0)
			{
				for(int i = 0; i < components.size(); i++)
				{
					components[i]->transform.relHeight = transform.relHeight - verticalGap*3 - scrollBar->transform.relHeight;
				}
			}
			else
			{
				for(int i = 0; i < components.size(); i++)
				{
					components[i]->transform.relWidth = transform.relWidth - horizontalGap*3 - scrollBar->transform.relWidth;
				}
			}
		}
		else
		{
			if(listDirection == 0)
			{
				for(int i = 0; i < components.size(); i++)
				{
					components[i]->transform.relHeight = transform.relHeight - verticalGap*2;
				}
			}
			else
			{
				for(int i = 0; i < components.size(); i++)
				{
					components[i]->transform.relWidth = transform.relWidth - horizontalGap*2;
				}
			}
		}


		

		if(scrollBar != nullptr && components.size() > 0)	//Calculate the on screen offset of all components due to the scroll bar
		{
			if(listDirection != 0)
				scrollBar->scrollOffset = scrollBar->maxDistance > 0 ? (int)(((float)scrollBar->transform.relY / (float)scrollBar->maxDistance) * (screenHeight-components.back()->screenY-screenHeight+(float)verticalGap)) : 0;
		}
		



		if(scrollBar != nullptr)
		{
			//Calculate the scroll bars screen coords
			scrollBar->calcComponentScreenCoords(screenX, screenY, screenWidth, screenHeight);


			//Calculate the size of the scroll bar
			float componentsSpan = 0.0f;
			if(listDirection == 0 && components.size() > 0)
				componentsSpan = components.back()->screenX + components.back()->screenWidth - components.front()->screenX;
			else if(listDirection != 0 && components.size() > 0)
				componentsSpan = components.front()->screenY + components.front()->screenHeight - components.back()->screenY;// < screenY

			float scrollBarPercentage = listDirection != 0 && (screenHeight-screenY)/componentsSpan < 1.0f ? (screenHeight-screenY)/componentsSpan : 1.0f;
			scrollBar->transform.relHeight = (int)(scrollBarPercentage * (screenHeight-screenY)) - verticalGap*2;
			//std::cerr << "SCROLL BAR: " << componentsSpan << ", " << scrollBarPercentage << ", " << scrollBar->height << "\n";


			//Calculate the maximum distance the scroll bar can be moved
			scrollBar->maxDistance = listDirection == 0 ? screenWidth - scrollBar->transform.relWidth - horizontalGap*2 : screenHeight - scrollBar->transform.relHeight - verticalGap*2;

			if(scrollBar->listDirection == 0 && scrollBar->transform.relX > scrollBar->maxDistance)			scrollBar->transform.relX = scrollBar->maxDistance;
			else if(scrollBar->listDirection != 0 && scrollBar->transform.relY > scrollBar->maxDistance)	scrollBar->transform.relY = scrollBar->maxDistance;


			//Calculate the scroll bars screen coords
			if(listDirection == 1)
				scrollBar->calcComponentScreenCoords(screenX, screenY - verticalGap, screenWidth, screenHeight);
			else
				scrollBar->calcComponentScreenCoords(screenX - horizontalGap, screenY, screenWidth, screenHeight);
			/*std::vector<glm::vec4> lstScrollBar = scrollBar->calcComponentScreenCoords(screenX, screenY, screenWidth, screenHeight);
			if(listDirection == 0)
				lstScrollBar[0].x -= horizontalGap;	//Correct scroll bar position by including horizontal/vertical as all scroll bars start at 0
			else
				lstScrollBar[0].y -= verticalGap;

			lstScreenCoords.insert(lstScreenCoords.end(), lstScrollBar.begin(), lstScrollBar.end());*/
		}



		for(size_t i = 0; i < components.size(); i++)
		{
			///std::vector<glm::vec4> lstComp;
			///lstComp = components[i]->calcComponentScreenCoords((int)screenX, (int)(screenY), (int)screenWidth, (int)screenHeight);	//Relative to panel therefore pass through panel width & height

			if(scrollBar == nullptr)
				components[i]->calcComponentScreenCoords((os_int_t)screenX, (os_int_t)(screenY), (os_int_t)screenWidth, (os_int_t)screenHeight);	//Relative to panel therefore pass through panel width & height
			else
			{
				if(listDirection == 1)
					components[i]->calcComponentScreenCoords((os_int_t)screenX, (os_int_t)(screenY+scrollBar->scrollOffset), (os_int_t)screenWidth-scrollBar->screenWidth-horizontalGap, (os_int_t)screenHeight);		//Relative to panel therefore pass through panel width & height
				else
					components[i]->calcComponentScreenCoords((os_int_t)screenX, (os_int_t)(screenY), (os_int_t)screenWidth, (os_int_t)screenHeight-scrollBar->screenHeight-verticalGap);	//Relative to panel therefore pass through panel width & height
			}

			/*if(scrollBar != nullptr)
			{
				size_t numCycles = lstComp.size();
				for(size_t j = 0; j < numCycles; j++)
				{
					lstComp[j].y += (float)scrollBar->scrollOffset;

					/*if(lstComp[j].y < screenY)
					{
						lstComp.pop_back();
						numCycles--;
					}
					else if(lstComp[j].y > screenY + screenHeight)
					{
						lstComp.erase(lstComp.begin());
						numCycles--;
					}
				}
			}*/

			///lstScreenCoords.insert(lstScreenCoords.end(), lstComp.begin(), lstComp.end());
		}

		///componentScreenCoords = lstScreenCoords;

		///return lstScreenCoords;
	}





	void UIPanel::clear(std::vector<UIComponent *> & allComps)
	{
		while(!components.empty())
		{
			allComps.erase(std::remove(allComps.begin(), allComps.end(), components.back()), allComps.end());
			components.back()->clear(allComps);
			delete components.back();
			components.pop_back();
		}
	}






	/*int UIPanel::getNumComponents() const
	{
		int numComponents = 1;	//Starts on one to ensure it includes itself

		for(int i = 0; i < components.size(); i++)
		{
			numComponents += components[i]->getNumComponents();
		}

		if(scrollBar != nullptr)
			numComponents++;

		return numComponents;
	}*/

	/*int UIPanel::getNumTextChars() const
	{
		int numTextChars = 0;

		for(int i = 0; i < components.size(); i++)
		{
			numTextChars += components[i]->getNumTextChars();
		}

		return numTextChars;
	}*/

	/*int UIPanel::getCurrentTex() const
	{
		size_t subtractor = 1;

		if(k == 0)	return texStd;
		if(scrollBar != nullptr) {if(k == 1) return scrollBar->getCurrentTex(k); subtractor++;}

		k -= subtractor;

		//if(k < components.size())	return components[k]->texStd;

		for(size_t i = 0; i < components.size(); i++)
		{
			if(k < components[i]->getNumComponents())
				return components[i]->getCurrentTex(k);

			k -= components[i]->getNumComponents();
		}

		return 0;
	}*/

	/*const std::string & UIPanel::getText(size_t k) const
	{
		size_t subtractor = 1;

		if(k == 0)	return os_emptystring;
		if(scrollBar != nullptr) {if(k == 1) return os_emptystring; subtractor++;}

		k -= subtractor;

		//if(k <= components.size())	return components[k-subtractor]->getText(0);

		for(size_t i = 0; i < components.size(); i++)
		{
			if(k < components[i]->getNumComponents())
				return components[i]->getText(k);

			k -= components[i]->getNumComponents();
		}

		return os_emptystring;
	}*/



	void UIPanel::addComponent(UIComponent * comp)
	{
		int lastX	   = components.size() > 0 ? components.back()->transform.relX : 0;
		int lastY	   = components.size() > 0 ? components.back()->transform.relY : 0;
		int lastWidth  = components.size() > 0 ? components.back()->transform.relWidth : 0;
		int lastHeight = components.size() > 0 ? components.back()->transform.relHeight : 0;

		if(listDirection == 0)
		{
			comp->xExtended = comp->yExtended = false;
			comp->transform.orig = Origin::LEFT_TOP;
			comp->transform.relX = lastX + lastWidth + horizontalGap;
			comp->transform.relY += verticalGap;
			comp->transform.relHeight = transform.relHeight - verticalGap*2;
		}
		else
		{
			comp->xExtended = comp->yExtended = false;
			comp->transform.orig = Origin::LEFT_TOP;
			comp->transform.relX += horizontalGap;
			comp->transform.relY = lastY + lastHeight + verticalGap;
			comp->transform.relWidth = transform.relWidth - horizontalGap*2;
		}

		//std::cerr << "COMP: " << y << ", " << comp->y << "\n";

		components.push_back(comp);
	}



	//DO NOT USE THIS METHOD - Use UserInterface::setScrollBarShown instead
	void UIPanel::setScrollBarShown(std::vector<UIComponent *> & components, os_bool_t showScrollBar, os_int_t texStd, os_int_t texHover, os_int_t texClick)
	{
		if(scrollBar == nullptr && showScrollBar)
		{
			scrollBar = new UIScrollBar("", Origin::LEFT_TOP, transform.relWidth-15-horizontalGap, 0, 15, 30, false, false, texStd, texHover, texClick);
			scrollBar->listDirection = listDirection;
			components.push_back(scrollBar);
		}
		else if(scrollBar != nullptr && !showScrollBar)
		{
			components.erase(std::remove(components.begin(), components.end(), scrollBar), components.end());	//Ensure scroll bar is removed from the user interfaces components list
			delete scrollBar;
		}
	}
}
