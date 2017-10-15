#include "UISlider.h"

namespace osi
{
	UISlider::UISlider(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
		int texBack, float minVal, float maxVal) :
		UIComponent(name, orig, x, y, width, height, xExtended, yExtended, texStd, texHover, texClick)
	{
		this->moving = false;
		this->minVal = minVal;
		this->maxVal = maxVal;
		handleX = handleY = 0;
		handleWidth = width / 7.0f;
		handleHeight = height;
		maxDistance = static_cast<int>(width - handleWidth);
	}

	UISlider::~UISlider()
	{
		freeComponent();
	}




	bool UISlider::shouldAttach(UIUpdateArguments & args, UIComponent ** comp)
	{
		if(args.mousex >= screenX && args.mousex < screenX + screenWidth && args.mousey >= screenY && args.mousey < screenY + screenHeight)
		{
			if(args.left)
			{
				texCurrent = texClick;
				*comp = this;
				return true;
			}
			texCurrent = texHover;
		} else texCurrent = texStd;

		return false;
	}

	bool UISlider::update(UIUpdateArguments & args, UICallback * cllbk)
	{
		handleX = handleX + args.dx < maxDistance && handleX + args.dx > 0 ? handleX + args.dx : (handleX + args.dx >= maxDistance ? maxDistance : 0);

		float val = (handleX/static_cast<float>(maxDistance)) * (maxVal - minVal) + minVal;

		for(size_t i = 0; i < sliderCallbacks.size(); i++)
		{
			sliderCallbacks[i]->valueChanged(this, val);
		}

		*cllbk = UICallback::POST_UPDATE_UI;

		return true;
	}




	void UISlider::calcComponentScreenCoords(os_int_t containerX, os_int_t containerY, os_int_t containerWidth, os_int_t containerHeight)
	{
		///glm::vec4 screenCoords;

		//Screen width/height are widths/heights of component in screen co-ordinates
		screenWidth = transform.relWidth;
		screenHeight = transform.relHeight;

		if(xExtended)
			screenWidth  = containerWidth/transform.relWidth   - transform.relX;		//Division by zero not checked for as button.width cannot equal zero
		if(yExtended)
			screenHeight = containerHeight/transform.relHeight - transform.relY;

		handleWidth = static_cast<int>(screenWidth / 7.0f);
		handleHeight = static_cast<int>(screenHeight);
		maxDistance = static_cast<int>(screenWidth - handleWidth);

		switch(transform.orig)
		{
		case Origin::LEFT_TOP:
			screenX = containerX + transform.relX;
			screenY = containerY + containerHeight - transform.relY - screenHeight;
			break;
		case Origin::LEFT_CENTREY:
			screenX = containerX + transform.relX;
			screenY = containerY + containerHeight/2 + transform.relY - screenHeight/2;
			break;
		case Origin::LEFT_BOTTOM:
			screenX = containerX + transform.relX;
			screenY = containerY + transform.relY;
			break;
		case Origin::CENTREX_TOP:
			screenX = containerX + containerWidth/2 + transform.relX - screenWidth/2;
			screenY = containerY + containerHeight - transform.relY - screenHeight;
			break;
		case Origin::CENTREX_CENTREY:
			screenX = containerX + containerWidth/2 + transform.relX - screenWidth/2;
			screenY = containerY + containerHeight/2 + transform.relY - screenHeight/2;
			break;
		case Origin::CENTREX_BOTTOM:
			screenX = containerX + containerWidth/2 + transform.relX - screenWidth/2;
			screenY = containerY + transform.relY;
			break;
		case Origin::RIGHT_TOP:
			screenX = containerX + containerWidth - transform.relX - screenWidth;
			screenY = containerY + containerHeight - transform.relY - screenHeight;
			break;
		case Origin::RIGHT_CENTREY:
			screenX = containerX + containerWidth - transform.relX;
			screenY = containerY + containerHeight/2 + transform.relY - screenHeight/2;
			break;
		case Origin::RIGHT_BOTTOM:
			screenX = containerX + containerWidth - transform.relX - screenWidth;
			screenY = containerY + transform.relY;
			break;
		default:
			screenX = transform.relX;
			screenY = transform.relY;
			break;
		}

		float handleScreenX = screenX + handleX;
		float handleScreenY = screenY + handleY;

		componentScreenCoords = glm::vec4(handleScreenX, handleScreenY, handleWidth, handleHeight);
		///std::vector<glm::vec4> lstScreenCoords;
		///lstScreenCoords.push_back(screenCoords);

		///componentScreenCoords = lstScreenCoords;

		///return lstScreenCoords;
	}
}
