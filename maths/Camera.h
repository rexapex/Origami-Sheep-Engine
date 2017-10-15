#pragma once

#include <iostream>

#include "../Globals.h"
#include "Transform.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

namespace osm
{
	class Camera
	{
	public:
		Camera();
		//Camera(Transform * mainTransform);
		virtual ~Camera();

		virtual void update(os_double_t deltaTimeSeconds, os_float_t deltaX, os_float_t deltaY, os_bool_t allowCamRotation, os_bool_t moveForward, os_bool_t moveBackward, os_bool_t moveRight, os_bool_t moveLeft, os_bool_t moveUp, os_bool_t moveDown) = 0;

		Transform * getTransform() {return mainTransform;}
		glm::mat4 * getTransformInverse() {return transformInverse;}

	protected:
		Transform * mainTransform;
		glm::mat4 * transformInverse;

		glm::vec3 & camPos;
		glm::vec3 camPosDelta, camLookAt, camDir, camUp;
		os_float_t camPitch, camHeading, maxPitchRate, maxHeadingRate;

		void changePitch(os_float_t pitchDeg);
		void changeHeading(os_float_t headingDeg);
	};
}
