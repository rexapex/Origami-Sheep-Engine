#include "Camera.h"

namespace osm
{
	Camera::Camera() : mainTransform(new osm::Transform), camPos(mainTransform->translation)
	{
		transformInverse = new glm::mat4(glm::mat3(mainTransform->modelMatrix));
		*transformInverse = glm::inverse(*transformInverse);
		camDir = glm::vec3(0.0f, 0.0f, -1.0f);
		camUp = glm::vec3(0.0f, 1.0f, 0.0f);
		camLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
		camPosDelta = glm::vec3(0.0f, 0.0f, 0.0f);
		camPitch = camHeading = 0.0f;
		maxPitchRate = maxHeadingRate = 5.0f;
	//	camPos = mainTransform->translation;
	}

	/*Camera::Camera(Transform * transform) : camPos(transform->translation)
	{
		mainTransform = transform;
		transformInverse = glm::mat4(glm::mat3(transform->modelMatrix));
		//camPos = glm::vec3(0.0f, 0.0f, -1.0f);
		camDir = glm::vec3(0.0f, 0.0f, -1.0f);
		camUp = glm::vec3(0.0f, 1.0f, 0.0f);
		camLookAt = glm::vec3(0.0f, 0.0f, 0.0f);
		camPosDelta = glm::vec3(0.0f, 0.0f, 0.0f);
		camPitch = camHeading = 0.0f;
		maxPitchRate = maxHeadingRate = 5.0f;
	}*/

	Camera::~Camera()
	{
		if(mainTransform != nullptr)
			delete mainTransform;
		if(transformInverse != nullptr)
			delete transformInverse;
	}

	void Camera::changePitch(os_float_t pitchDeg)
	{
		if(pitchDeg < - maxPitchRate)
			pitchDeg = -maxPitchRate;
		else if(pitchDeg > maxPitchRate)
			pitchDeg = maxPitchRate;

		camPitch += pitchDeg;

		if(camPitch > 360.0f)
			camPitch -= 360.0f;
		else if(camPitch < -360.0f)
			camPitch += 360.0f;
	}

	void Camera::changeHeading(os_float_t headingDeg)
	{
		if(headingDeg < - maxHeadingRate)
			headingDeg = -maxHeadingRate;
		else if(headingDeg > maxHeadingRate)
			headingDeg = maxHeadingRate;

		if(camPitch > 90.0f && camPitch < 270.0f || (camPitch < -90.0f && camPitch > -270.0f))
			camHeading -= headingDeg;
		else
			camHeading += headingDeg;

		if(camHeading > 360.0f)
			camHeading -= 360.0f;
		else if(camHeading < -360.0f)
			camHeading += 360.0f;
	}
}
