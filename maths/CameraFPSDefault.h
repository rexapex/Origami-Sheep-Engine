#pragma once

#include <iostream>

#include "Camera.h"
#include "../Globals.h"
#include "Transform.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"

namespace osm
{
	class CameraFPSDefault : public Camera
	{
	public:
		CameraFPSDefault();
		//CameraFPSDefault(Transform * mainTransform);
		virtual ~CameraFPSDefault();

		void update(os_double_t deltaTimeSeconds, os_float_t deltaX, os_float_t deltaY, os_bool_t allowCamRotation, os_bool_t moveForward, os_bool_t moveBackward, os_bool_t moveRight, os_bool_t moveLeft, os_bool_t moveUp, os_bool_t moveDown);
	};
}
