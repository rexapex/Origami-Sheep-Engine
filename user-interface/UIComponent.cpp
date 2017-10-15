#include "UIComponent.h"


namespace osi
{
	UIComponent::UIComponent(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick)
	{
		this->name = name;
		this->transform = {orig, x, y, width, height, nullptr};
		this->xExtended = xExtended;
		this->yExtended = yExtended;
		this->texCurrent = this->texStd = texStd;
		this->texHover = texHover;
		this->texClick = texClick;
	}

	UIComponent::~UIComponent()
	{

	}



	void UIComponent::calcComponentScreenCoords(os_int_t containerX, os_int_t containerY, os_int_t containerWidth, os_int_t containerHeight)
	{
		///glm::vec4 screenCoords;

		//Screen width/height are widths/heights of component in screen co-ordinates
		screenWidth = static_cast<float>(transform.relWidth);
		screenHeight = static_cast<float>(transform.relHeight);

		if(xExtended)
			screenWidth  = static_cast<float>(containerWidth/transform.relWidth   - transform.relX);		//Division by zero not checked for as button.width cannot equal zero
		if(yExtended)
			screenHeight = static_cast<float>(containerHeight/transform.relHeight - transform.relY);

		//Origin of UI element is based on which screen origin is used
		switch(transform.orig)	//TODO - Test all origins to ensure correct results
		{
		case Origin::LEFT_TOP:
			screenX = static_cast<float>(containerX + transform.relX);
			screenY = static_cast<float>(containerY + containerHeight - transform.relY) - screenHeight;
			break;
		case Origin::LEFT_CENTREY:
			screenX = static_cast<float>(containerX + transform.relX);
			screenY = static_cast<float>(containerY + containerHeight/2 + transform.relY) - screenHeight/2;
			break;
		case Origin::LEFT_BOTTOM:
			screenX = static_cast<float>(containerX + transform.relX);
			screenY = static_cast<float>(containerY + transform.relY);
			break;
		case Origin::CENTREX_TOP:
			screenX = static_cast<float>(containerX + containerWidth/2 + transform.relX) - screenWidth/2;
			screenY = static_cast<float>(containerY + containerHeight - transform.relY) - screenHeight;
			break;
		case Origin::CENTREX_CENTREY:
			screenX = static_cast<float>(containerX + containerWidth/2 + transform.relX) - screenWidth/2;
			screenY = static_cast<float>(containerY + containerHeight/2 + transform.relY) - screenHeight/2;
			break;
		case Origin::CENTREX_BOTTOM:
			screenX = static_cast<float>(containerX + containerWidth/2 + transform.relX) - screenWidth/2;
			screenY = static_cast<float>(containerY + transform.relY);
			break;
		case Origin::RIGHT_TOP:
			screenX = static_cast<float>(containerX + containerWidth - transform.relX) - screenWidth;
			screenY = static_cast<float>(containerY + containerHeight - transform.relY) - screenHeight;
			break;
		case Origin::RIGHT_CENTREY:
			screenX = static_cast<float>(containerX + containerWidth - transform.relX) - screenWidth;
			screenY = static_cast<float>(containerY + containerHeight/2 + transform.relY) - screenHeight/2;
			break;
		case Origin::RIGHT_BOTTOM:
			screenX = static_cast<float>(containerX + containerWidth - transform.relX) - screenWidth;
			screenY = static_cast<float>(containerY + transform.relY);
			break;
		default:
			screenX = static_cast<float>(transform.relX);
			screenY = static_cast<float>(transform.relY);
			break;
		}

		componentScreenCoords = glm::vec4(screenX, screenY, screenWidth, screenHeight);

		///std::vector<glm::vec4> lstScreenCoords;
		///lstScreenCoords.push_back(screenCoords);

		///componentScreenCoords = lstScreenCoords;

		///return lstScreenCoords;
	}



	bool UIComponent::shouldDettach(UIUpdateArguments & args)
	{
		if(args.left)
			return false;

		texCurrent = texStd;
		return true;
	}
}
