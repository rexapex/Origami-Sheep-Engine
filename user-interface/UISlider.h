#pragma once

#include "UIComponent.h"
#include "../rendering/RenderingEngine.h"

namespace osi
{
	class UISlider;

	class SliderCallback
	{
	public:
		virtual void valueChanged(UISlider * sld, float val) const = 0;
	};

	class UISlider : public UIComponent
	{
	public:
		UISlider(std::string name, Origin orig, os_int_t x, os_int_t y, os_int_t width, os_int_t height, os_bool_t xExtended, os_bool_t yExtended, os_int_t texStd, os_int_t texHover, os_int_t texClick,
			int texBack, float minVal, float maxVal);
		virtual ~UISlider();

		//Deletes callbacks
		void freeComponent() {while(!sliderCallbacks.empty()) delete sliderCallbacks.back(), sliderCallbacks.pop_back();}

		void setStartVal(float val) {if(val > maxVal) val = maxVal; else if(val < minVal) val = minVal; handleX = (float)maxDistance * val/(maxVal-minVal);}

		bool shouldAttach(UIUpdateArguments & args, UIComponent ** comp);
		bool update(UIUpdateArguments & args, UICallback * cllbk);

		void calcComponentScreenCoords(os_int_t containerX, os_int_t containerY, os_int_t containerWidth, os_int_t containerHeight);

		void addSliderCallback(SliderCallback * callback) {sliderCallbacks.push_back(callback);}
	private:

		std::vector<SliderCallback *> sliderCallbacks;

		int maxDistance;
		bool moving;
		float minVal, maxVal;

		//Relative coords for the handle to move the slider
		float handleX, handleY, handleWidth, handleHeight;
	};
}
