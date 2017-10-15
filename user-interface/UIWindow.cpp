#include "UIWindow.h"


namespace osi
{
	UIWindow::UIWindow(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
		os_bool_t movable, os_bool_t resizable, os_bool_t minimisable, os_bool_t closable, os_int_t texTitleBar, os_int_t texMinimise, os_int_t texFullScreen, os_int_t texClose) :
		UIComponent(name, orig, x, y, width, height, xExtended, yExtended, texStd, texHover, texClick)
	{
		this->movable = movable;
		this->resizable = resizable;
		this->minimisable = minimisable;
		this->closable = closable;

		this->texTitleBar = texTitleBar;
		this->texMinimise = texMinimise;
		this->texFullScreen = texFullScreen;
		this->texClose = texClose;

		this->moving = false;
		this->absXTitleBar = absYTitleBar = absWTitleBar = absHTitleBar = 0;

		this->pnlTitleBar = new UIPanel("pnlTITLE_BAR"+name, Origin::CENTREX_TOP, 0, 0, 1, 30, true, false, texTitleBar, texTitleBar, texTitleBar, 0, 5, 5);
		this->pnlContent = new UIPanel("pnlWINDOW"+name, Origin::CENTREX_CENTREY, 0, 0, 1, 1, true, true, texStd, texStd, texStd, 0, 5, 5);
	}


	UIWindow::~UIWindow()
	{

	}




	bool UIWindow::shouldAttach(UIUpdateArguments & args, UIComponent ** comp)
	{
		if(args.mousex >= absXTitleBar && args.mousex < absXTitleBar + absWTitleBar && args.mousey >= absYTitleBar && args.mousey < absYTitleBar + absHTitleBar)
		{
			if(movable && args.left)
			{
				texCurrent = texClick;
				*comp = this;
				return true;
			}
			texCurrent = texHover;
		} else texCurrent = texStd;

		return false;
	}

	bool UIWindow::update(UIUpdateArguments & args, UICallback * cllbk)
	{
		transform.relX += (os_int_t)args.dx;
		transform.relY -= (os_int_t)args.dy;

		*cllbk = UICallback::POST_UPDATE_UI;

		return true;
	}




	void UIWindow::calcComponentScreenCoords(os_int_t containerX, os_int_t containerY, os_int_t containerWidth, os_int_t containerHeight)
	{
		///std::vector<glm::vec4> lstBase = UIComponent::calcComponentScreenCoords(containerX, containerY, containerWidth, containerHeight);

		//UIComponent::calcComponentScreenCoords(containerX, containerY, containerWidth, containerHeight);

		glm::ivec4 restraint {containerX, containerY, containerWidth, containerHeight};

		if(pnlRestraint != nullptr)
		{
			pnlRestraint->calcComponentScreenCoords(containerX, containerY, containerWidth, containerHeight);
			restraint = pnlRestraint->getComponentScreenCoords();
		}

		pnlTitleBar->calcComponentScreenCoords(restraint.x, restraint.y, restraint.z, restraint.w);
		pnlContent->calcComponentScreenCoords(restraint.x, restraint.y, restraint.z, restraint.w);

	//	componentScreenCoords = pnlContent->getComponentScreenCoords();

		/*glm::vec4 pnl(lstBase[0].x, lstBase[0].y + lstBase[0].w - 30, lstBase[0].z, 30);

		absXTitleBar = pnl.x;
		absYTitleBar = pnl.y;
		absWTitleBar = pnl.z;
		absHTitleBar = pnl.w;

		lstBase.push_back(pnl);

		std::vector<glm::vec4> lstContent = pnlContent->getComponentScreenCoords(lstBase[0].x, lstBase[0].y, lstBase[0].z, lstBase[0].w);
		lstBase.insert(lstBase.end(), lstContent.begin(), lstContent.end());

		componentScreenCoords = lstBase;

		return lstBase;*/
	}
}
