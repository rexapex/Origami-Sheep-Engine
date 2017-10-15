#include "UIListBox.h"

namespace osi
{
	/*UIListBox::UIListBox(std::string name, Origin orig, int x, int y, int width, int height, bool xExtended, bool yExtended, int texStd, int texHover, int texClick,
		int listDirection, int horizontalGap, int verticalGap) :
		UIComponent(name, orig, x, y, width, height, xExtended, yExtended, texStd, texHover, texClick)
	{
		this->scrollBar = nullptr;
		this->listDirection = listDirection;
		this->horizontalGap = horizontalGap;
		this->verticalGap = verticalGap;
	}

	UIListBox::~UIListBox()	//Delete pointers to all components
	{
		while(!list.listComps.empty()) delete list.listComps.back().comp, list.listComps.pop_back();
	}




	bool UIListBox::update(float mousex, float mousey, float dx, float dy, int left, int right)
	{
		for(int i = 0; i < list.listComps.size(); i++)
		{
			UIComponent * comp = list.listComps[i].comp;

			if(mousex >= comp->screenX && mousex < comp->screenX + comp->screenWidth && mousey >= comp->screenY && mousey < comp->screenY + comp->screenHeight)
			{
				if(comp->update(mousex, mousey, dx, dy, left, right))
				{
					for(int j = 0; j < componentSelectedCallbacks.size(); j++)
					{
						componentSelectedCallbacks[j](list.listComps[i]);
					}
				}

				break;
			}
		}

		if(scrollBar != nullptr)
			scrollBar->update(mousex, mousey, dx, dy, left, right);

		return false;
	}

	std::vector<glm::vec4> UIListBox::getComponentScreenCoords(int containerX, int containerY, int containerWidth, int containerHeight)
	{
		std::vector<glm::vec4> lstScreenCoords;
		std::vector<glm::vec4> lstBase;
		lstBase = UIComponent::getComponentScreenCoords(containerX, containerY, containerWidth, containerHeight);

		lstScreenCoords.insert(lstScreenCoords.end(), lstBase.begin(), lstBase.end());

		//If horizontal panels components go off end of panel then include a scroll bar
		/*if(listDirection == 0 && components.size() > 0 && components.back()->screenX + components.back()->screenWidth > screenWidth + screenX)
		scrollBarShown = true;
		//If vertical panels components go off end of panel then include a scroll bar
		else if(listDirection != 0 && components.size() > 0 && components.back()->screenY < screenY)
		scrollBarShown = true;
		else
		scrollBarShown = false;*/

		/*if(scrollBar != nullptr)
		{
			if(listDirection == 0)
			{
				for(int i = 0; i < list.listComps.size(); i++)
				{
					list.listComps[i].comp->height = height - verticalGap*3 - scrollBar->height;
				}
			}
			else
			{
				for(int i = 0; i < list.listComps.size(); i++)
				{
					list.listComps[i].comp->width = width - horizontalGap*3 - scrollBar->width;
				}
			}
		}
		else
		{
			if(listDirection == 0)
			{
				for(int i = 0; i < list.listComps.size(); i++)
				{
					list.listComps[i].comp->height = height - verticalGap*2;
				}
			}
			else
			{
				for(int i = 0; i < list.listComps.size(); i++)
				{
					list.listComps[i].comp->width = width - horizontalGap*2;
				}
			}
		}



		int scrollOffset = 0;
		if(scrollBar != nullptr && list.listComps.size() > 0)	//Calculate the on screen offset of all components due to the scroll bar
			scrollOffset = scrollBar->maxDistance > 0 ? (int)(((float)scrollBar->y / (float)scrollBar->maxDistance) * (float)(screenHeight-list.listComps.back().comp->screenY-screenHeight+verticalGap)) : 0;



		if(scrollBar != nullptr)
		{
			//Calculate the scroll bars screen coords
			scrollBar->getComponentScreenCoords(screenX, screenY, screenWidth, screenHeight);


			//Calculate the size of the scroll bar
			int componentsSpan = 0;
			if(listDirection == 0 && list.listComps.size() > 0)
				componentsSpan = list.listComps.back().comp->screenX + list.listComps.back().comp->screenWidth;
			else if(listDirection != 0 && list.listComps.size() > 0)
				componentsSpan = list.listComps.front().comp->screenY + list.listComps.front().comp->screenHeight - list.listComps.back().comp->screenY;// < screenY

			float scrollBarPercentage = listDirection != 0 && (float)(screenHeight-screenY)/(float)componentsSpan < 1.0f ? (float)(screenHeight-screenY)/(float)componentsSpan : 1.0f;
			scrollBar->height = (int)(scrollBarPercentage * (float)(screenHeight-screenY)) - verticalGap*2;
			//std::cerr << "SCROLL BAR: " << componentsSpan << ", " << scrollBarPercentage << ", " << scrollBar->height << "\n";


			//Calculate the maximum distance the scroll bar can be moved
			scrollBar->maxDistance = listDirection == 0 ? screenWidth - scrollBar->width - horizontalGap*2 : screenHeight - scrollBar->height - verticalGap*2;

			if(scrollBar->listDirection == 0 && scrollBar->x > scrollBar->maxDistance)		scrollBar->x = scrollBar->maxDistance;
			else if(scrollBar->listDirection != 0 && scrollBar->y > scrollBar->maxDistance)	scrollBar->y = scrollBar->maxDistance;


			//Calculate the scroll bars screen coords
			std::vector<glm::vec4> lstScrollBar = scrollBar->getComponentScreenCoords(screenX, screenY, screenWidth, screenHeight);
			if(listDirection == 0)
				lstScrollBar[0].x -= horizontalGap;	//Correct scroll bar position by including horizontal/vertical as all scroll bars start at 0
			else
				lstScrollBar[0].y -= verticalGap;

			lstScreenCoords.insert(lstScreenCoords.end(), lstScrollBar.begin(), lstScrollBar.end());
		}



		for(int i = 0; i < list.listComps.size(); i++)
		{
			std::vector<glm::vec4> lstComp;
			lstComp = list.listComps[i].comp->getComponentScreenCoords(screenX, screenY, screenWidth, screenHeight);	//Relative to panel therefore pass through panel width & height

			for(int i = 0; i < lstComp.size(); i++)
			{
				lstComp[i].y += scrollOffset;
			}

			lstScreenCoords.insert(lstScreenCoords.end(), lstComp.begin(), lstComp.end());
		}


		return lstScreenCoords;
	}



	int UIListBox::getNumComponents() const
	{
		int numComponents = 1;	//Starts on one to ensure it includes itself

		for(int i = 0; i < list.listComps.size(); i++)
		{
			numComponents += list.listComps[i].comp->getNumComponents();
		}

		if(scrollBar != nullptr)
			numComponents++;

		return numComponents;
	}

	int UIListBox::getNumTextChars() const
	{
		int numTextChars = 0;

		for(int i = 0; i < list.listComps.size(); i++)
		{
			numTextChars += list.listComps[i].comp->getNumTextChars();
		}

		return numTextChars;
	}

	int UIListBox::getCurrentTex(size_t k) const
	{
		size_t subtractor = 1;

		if(k == 0)	return texStd;
		if(scrollBar != nullptr) {if(k == 1) return scrollBar->getCurrentTex(k); subtractor++;}

		k -= subtractor;

		//if(k < list.listComps.size())	return list.listComps[k].comp->texStd;

		for(size_t i = 0; i < list.listComps.size(); i++)
		{
			if(k < list.listComps[i].comp->getNumComponents())
				return list.listComps[i].comp->getCurrentTex(k);

			k -= list.listComps[i].comp->getNumComponents();
		}

		return 0;
	}

	const std::string & UIListBox::getText(size_t k) const
	{
		size_t subtractor = 1;

		if(k == 0)	return "";
		if(scrollBar != nullptr) {if(k == 1) return ""; subtractor++;}

		k -= subtractor;

		//if(k <= list.listComps.size())	return list.listComps[k-subtractor].comp->getText(0);

		for(size_t i = 0; i < list.listComps.size(); i++)
		{
			if(k < list.listComps[i].comp->getNumComponents())
				return list.listComps[i].comp->getText(k);

			k -= list.listComps[i].comp->getNumComponents();
		}

		return "";
	}



	void UIListBox::addComponent(UIComponent * comp, osrm::Entity & t)
	{
		int lastX	   = list.listComps.size() > 0 ? list.listComps.back().comp->x : 0;
		int lastY	   = list.listComps.size() > 0 ? list.listComps.back().comp->y : 0;
		int lastWidth  = list.listComps.size() > 0 ? list.listComps.back().comp->width : 0;
		int lastHeight = list.listComps.size() > 0 ? list.listComps.back().comp->height : 0;

		if(listDirection == 0)
		{
			comp->xExtended = comp->yExtended = false;
			comp->origin = LEFT_TOP;
			comp->x = lastX + lastWidth + horizontalGap;
			comp->y = y + verticalGap;
			comp->height = height - verticalGap*2;
		}
		else
		{
			comp->xExtended = comp->yExtended = false;
			comp->origin = LEFT_TOP;
			comp->x = x + horizontalGap;
			comp->y = lastY + lastHeight + verticalGap;
			comp->width = width - horizontalGap*2;
		}

		//std::cerr << "COMP: " << y << ", " << comp->y << "\n";

		UIListComponent<osrm::Entity> i {comp, t};

		list.listComps.push_back(i);
	}



	void UIListBox::setScrollBarShown(bool showScrollBar, int texStd, int texHover, int texClick, osr::RenderingEngine * renderingEngine)
	{
		if(scrollBar == nullptr && showScrollBar)
		{
			scrollBar = new UIScrollBar("scroll1", LEFT_TOP, width-15-horizontalGap, 0, 15, 30, false, false, texStd, texHover, texClick, renderingEngine);
			scrollBar->listDirection = listDirection;
		}
		else if(scrollBar != nullptr && !showScrollBar)
		{
			delete scrollBar;
		}
	}*/
}
