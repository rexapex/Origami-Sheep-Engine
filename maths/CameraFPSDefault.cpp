#include "CameraFPSDefault.h"


namespace osm
{
	CameraFPSDefault::CameraFPSDefault() : Camera::Camera()
	{
		
	}

	/*CameraFPSDefault::CameraFPSDefault(Transform * mainTransform) : Camera(mainTransform)
	{

	}*/

	CameraFPSDefault::~CameraFPSDefault()	//Base destructor called automatically
	{

	}

	void CameraFPSDefault::update(os_double_t deltaTimeSeconds, os_float_t deltaX, os_float_t deltaY, os_bool_t allowCamRotation, os_bool_t moveForward, os_bool_t moveBackward, os_bool_t moveRight, os_bool_t moveLeft, os_bool_t moveUp, os_bool_t moveDown)
	{
		if(allowCamRotation)
		{
			changeHeading(0.0015f * -0.675f * deltaX);
			changePitch  (0.001f * -0.505f * deltaY);
		}

		if(moveForward)
		{
			camPosDelta += 3.0f * (float)deltaTimeSeconds * camDir;
		}
		else if(moveBackward)
		{
			camPosDelta -= 3.0f * (float)deltaTimeSeconds * camDir;
		}
		if(moveRight)
		{
			camPosDelta += 3.0f * (float)deltaTimeSeconds * glm::cross(camDir, camUp);
		}
		else if(moveLeft)
		{
			camPosDelta -= 3.0f * (float)deltaTimeSeconds * glm::cross(camDir, camUp);
		}
		if(moveUp)
		{
			camPosDelta += 3.0f * (float)deltaTimeSeconds * camUp;
		}
		else if(moveDown)
		{
			camPosDelta -= 3.0f * (float)deltaTimeSeconds * camUp;
		}

		camDir = glm::normalize(camLookAt - camPos);

		glm::vec3 axis = glm::cross(camDir, camUp);

		glm::quat quatPitch = glm::angleAxis(camPitch, axis);

		glm::quat quatHeading = glm::angleAxis(camHeading, camUp);

		glm::quat temp = glm::cross(quatPitch, quatHeading);
		temp = glm::normalize(temp);

		camDir = glm::rotate(temp, camDir);

		camPos += camPosDelta;

		camLookAt = camPos + camDir;

		glm::mat4 lookAtMat = glm::lookAt(camPos, camLookAt, camUp);
		mainTransform->modelMatrix = glm::inverse(lookAtMat);
		*transformInverse = glm::mat4(glm::mat3(lookAtMat));

		camPitch *= 0.5f;
		camHeading *= 0.5f;
		camPosDelta = camPosDelta * 0.8f;
	}
}
