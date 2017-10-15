/*
 * PhysicsEngine.cpp
 *
 *  Created on: 5 Oct 2015
 *      Author: james_000
 */

#include "PhysicsEngine.h"

namespace osp
{
	PhysicsEngine::PhysicsEngine()
	{
		// TODO Auto-generated constructor stub
	}

	PhysicsEngine::~PhysicsEngine()
	{
		// TODO Auto-generated destructor stub
	}


	void PhysicsEngine::stepSimulation(os_double_t deltaTimeSeconds)
	{
		for(auto rb : rigidbodies)
		{
			osm::translate(rb.parentTransform, os_float_t(deltaTimeSeconds) * rb.velocity);
		}

		if(collidesSphereSphere(sphereColliders[0], sphereColliders[1]))
			std::cerr << "COLLISION DETECTED!\n";

	/*	for(size_t i = 0; i < sphereColliders.size(); i++)
		{
			sphereColliders[i].lastParentPosition = sphereColliders[i].parentTransform->translation;
		}*/
	}


	os_bool_t PhysicsEngine::collidesSphereSphere(const SphereCollider & col1, const SphereCollider & col2)
	{
		if(col1.rigidbody == nullptr || col2.rigidbody == nullptr)	//Must have rigidbody for velocity based calculations
			return false;

		glm::vec3 velocityCol1 = col1.rigidbody->velocity;
		glm::vec3 velocityCol2 = col2.rigidbody->velocity;

		glm::vec3 velocityRelative = velocityCol2 - velocityCol1;

		glm::vec3 pos1 = col1.parentTransform->translation;
		glm::vec3 pos2 = col2.parentTransform->translation;

		//std::cerr << "Col 1 Velocity: " << velocityCol1.x << ", " << velocityCol1.y << ", " << velocityCol1.z << "\n";
		//std::cerr << "POS1: " << pos1.x << ", " << pos1.y << ", " << pos1.z << "\n";
		//std::cerr << "POS2: " << pos2.x << ", " << pos2.y << ", " << pos2.z << "\n";

		float sumRadii = col1.radius + col2.radius;
		float separationCentres  = glm::distance(pos1, pos2);		//Separation between sphere centres
		float separationSurfaces = separationCentres - sumRadii;	//Separation between sphere surfaces

		//std::cerr << "SEP: " << separationCentres << ", " << separationSurfaces << "\n";

		float velocityRelativeMag = glm::length(velocityRelative);

		if(velocityRelativeMag < separationSurfaces)	//Early escape test
			return false;

		glm::vec3 velocityNormalized = glm::normalize(velocityRelative);
		glm::vec3 vectorBetweenCentres = glm::vec3(pos2 - pos1);

		float d = glm::dot(velocityNormalized, vectorBetweenCentres);	//d = n.c.cos(theta) - therefore cos(theta) <= 0 means angle to large

		if(d <= 0)			//Escape test - Testing whether sphere1 is moving towards sphere 2
			return false;

		//f = square of shortest distance between velocity and centre
		float f = (separationCentres * separationCentres) - (d * d);
		float sumRadiiSquared = sumRadii * sumRadii;

		//If the shortest distance between sphere 1 & 2 is greatest than sum of their radii, then they cannot collide therefore return false
		if(f >= sumRadiiSquared)
			return false;

		//t = square of (velocity - distance needed to be travelled to touch sphere 2)
		float t = sumRadiiSquared - f;

		//If t < 0 then the triangle does not make sense, therefore spheres do not collide
		if(t < 0)
			return false;

		//The distance needed to move to touch sphere 2
		//std::cerr << "d: " << d << ", " << sqrt(t) << "\n";
		float distance = d - sqrt(t);

		if(velocityRelativeMag < distance)
			return false;

		//std::cerr << "DIST: " << distance << ", " << velocityRelativeMag << "\n";

		float multiplier = 1.0f - distance / velocityRelativeMag;

		//std::cerr << "Multiplier: " << multiplier << "\n";

		//velocityCol1 = glm::normalize(velocityCol1);

		osm::translate(col1.parentTransform, -multiplier * velocityCol1);
		osm::translate(col2.parentTransform, -multiplier * velocityCol2);

		return true;

		//std::cerr << "POS: " << pos1.x << ", " << pos1.y << ", " << pos1.z << " :: " << pos2.x << ", " << pos2.y << ", " << pos2.z << "\n";

		//std::cerr << "SEP: " << separation << "\n";
	}

	os_bool_t PhysicsEngine::collidesCuboidCuboid(const CuboidCollider & col1, const CuboidCollider & col2)
	{
		return false;
	}

	os_bool_t PhysicsEngine::collidesSphereCuboid(const SphereCollider & col1, const CuboidCollider & col2)
	{
		return false;
	}

	os_bool_t PhysicsEngine::collidesSimpleSphereSphere(const SphereCollider & col1, const SphereCollider & col2)
	{
		os_float_t separationCentres  = glm::distance(col1.colliderPosition, col2.colliderPosition);	//Separation between sphere centres
		
		if(separationCentres <= col1.radius + col2.radius)
			return true;
		else
			return false;
	}

	os_bool_t PhysicsEngine::collidesSimpleCuboidCuboid(const CuboidCollider & col1, const CuboidCollider & col2)
	{
		return false;
	}

	os_bool_t PhysicsEngine::collidesSimpleSphereCuboid(const SphereCollider & col1, const CuboidCollider & col2)
	{
		return false;
	}
} /* namespace osp */
