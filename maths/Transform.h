/*
 * Transform.h
 *
 *  Created on: 17 Oct 2015
 *      Author: james_000
 */

#ifndef MATHS_TRANSFORM_H_
#define MATHS_TRANSFORM_H_

#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtx/quaternion.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"

namespace osm
{
	//Setting the variables without the use of translate/rotate/scale functions will mean children will not be updated
	struct Transform
	{
		//Transformation relative to the transform's parent
		glm::mat4 modelMatrix;
		glm::vec3 translation;
		glm::vec3 scale;
		glm::quat orientation;

		//Children of this transform
		std::vector<Transform *> children;	//TODO - Add in hierarchical transforms

		Transform()
		{
			modelMatrix = glm::mat4();
			translation = glm::vec3();
			scale = glm::vec3();
			orientation = glm::quat();
		}
	};

	//Calculates and sets the model matrix of the given child
	//Does not set the transfrom of t
	void calcModelMatrixChild(Transform * t, int child);
	
	//Calculates and sets the model matrices of all the children of the transform t
	//Does not set the transform of t
	void calcModelMatrixChildren(Transform * t);

	//Calculates and sets t's model matrix by multiplying the components
	void calcModelMatrix(Transform * t);

	//Translates the transform given and all its children & all their children & ... by the translation given
	//Function also translates model matrices by the given translation
	void translate(Transform * t, const glm::vec3 translation);

	//Rotates the transform given and all children by the rotation given
	//Functions also applies the given rotation to the model matrices
	void rotate(Transform * t, float angleRadians, glm::vec3 axis);

	//Scales the transform given and all children by the scale vector
	//Functions also applies the given scale to the model matrices
	void scale(Transform * t, glm::vec3 scaleVec);
};

#endif /* MATHS_TRANSFORM_H_ */
