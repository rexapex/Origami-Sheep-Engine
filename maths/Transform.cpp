/*
 * Transform.cpp
 *
 *  Created on: 13 Apr 2016
 *      Author: james_000
 */

#include "Transform.h"

namespace osm
{
	//Calculates and sets the global model matrix of the given child
	//Does not set the transfrom of t
	void calcModelMatrixChild(Transform * t, int child)
	{
		if(child < t->children.size())
		{
			t->children[child]->modelMatrix = t->modelMatrix * (glm::translate(glm::mat4(), t->translation)
				* glm::toMat4(t->orientation)
				* glm::scale(glm::mat4(), t->scale));
		}
	}

	//Calculates and sets the global model matrices of all the children of the transform t
	//Does not set the transform of t
	void calcModelMatrixChildren(Transform * t)
	{
		for(size_t i = 0; i < t->children.size(); i++)
		{
			t->children[i]->modelMatrix = t->modelMatrix * (glm::translate(glm::mat4(), t->translation)
				* glm::toMat4(t->orientation)
				* glm::scale(glm::mat4(), t->scale));
		}
	}

	//Calculates and sets ts model matrix by multiplying the components
	void calcModelMatrix(Transform * t)
	{
		t->modelMatrix = glm::translate(glm::mat4(), t->translation) * glm::toMat4(t->orientation) * glm::scale(glm::mat4(), t->scale);
	}

	//Translates the transform given and all its children & all their children & ... by the translation given
	//Function also translates model matrices by the given translation
	void translate(Transform * t, const glm::vec3 translation)
	{
		t->translation.x += translation.x;
		t->translation.y += translation.y;
		t->translation.z += translation.z;

		//Translates by the translation given and not the overall translation
		t->modelMatrix = glm::translate(t->modelMatrix, translation);

		for(size_t i = 0; i < t->children.size(); i++)
		{
			translate(t->children[i], translation);
		}
	}

	//Rotates the transform given and all children by the rotation given
	//Functions also applies the given rotation to the model matrices
	void rotate(Transform * t, float angleRadians, glm::vec3 axis)
	{
		t->orientation = glm::rotate(t->orientation, angleRadians, axis);	//Rotate the orientation quaternion by the angle

		t->modelMatrix = glm::rotate(t->modelMatrix, angleRadians, axis);	//Rotate the model matrix by the angle

		for(unsigned int i = 0; i < t->children.size(); i++)
		{
			rotate(t->children[i], angleRadians, axis);
		}
	}

	//Multiplies the components of the scale of the transform given and all children by the scale vectors components
	//Functions also applies the given scale to the model matrices
	void scale(Transform * t, glm::vec3 scaleVec)
	{
		t->scale.x *= scaleVec.x;
		t->scale.y *= scaleVec.y;
		t->scale.z *= scaleVec.z;

		t->modelMatrix = glm::scale(t->modelMatrix, scaleVec);

		for(unsigned int i = 0; i < t->children.size(); i++)
		{
			scale(t->children[i], scaleVec);
		}
	}
};
