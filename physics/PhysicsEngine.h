/*
 * PhysicsEngine.h
 *
 *  Created on: 5 Oct 2015
 *      Author: james_000
 */

#ifndef PHYSICS_PHYSICSENGINE_H_
#define PHYSICS_PHYSICSENGINE_H_

#include "../Globals.h"
#include <vector>
#include <iostream>
#include "../maths/Transform.h"

namespace osp
{
	struct RigidBody	//Stores info used in rigid body physics
	{
		osm::Transform * parentTransform;	//Pointer to the transform of the parent, e.g. entity, that the collider belongs to
		os_float_t mass;			//The mass of the object, kilogrammes
		os_float_t drag;			//The force of air resistance affecting movement
		os_float_t angularDrag;		//The force of air resistance affecting rotation
		glm::vec3 velocity;			//The velocity of the rigidbody
		glm::vec3 centreOfMass;		//Relative to the origin of the entity
		//os_bool_t useGravity;	//Whether to apply the force of gravity to the object
	};

	/*struct SoftBody		//Stores info used in soft body physics
	{

	};*/

	struct SphereCollider
	{
		RigidBody * rigidbody;				//Pointer to the rigidbody belongind to the parent of sphere collider
		osm::Transform * parentTransform;	//Pointer to the transform of the parent, e.g. entity, that the collider belongs to
		glm::vec3 lastParentPosition;		//The position of the parent on the last update, e.g. entity, for this frame only
		glm::vec3 colliderPosition;			//The position of the collider relative to the entity
		os_float_t radius;					//The radius of the collider

		SphereCollider()
		{
			rigidbody = nullptr;
			parentTransform = nullptr;
			lastParentPosition = glm::vec3(0.0f);
			colliderPosition = glm::vec3(0.0f);
			radius = 1.0f;
		}
	};

	struct CuboidCollider
	{
		osm::Transform * parentTransform;	//Pointer to the transform of the parent, e.g. entity, that the collider belongs to
		glm::vec4 colliderPosition;			//The position of the collider relative to the entity
		float posXDisp, posYDisp, posZDisp;	//Displacement of vertices from the centre in the positive direction
		float negXDisp, negYDisp, negZDisp;	//Displacement of vertices from the centre in the negative direction
	};

	class PhysicsEngine
	{
	public:
		PhysicsEngine();
		virtual ~PhysicsEngine();

		void stepSimulation(os_double_t deltaTimeSeconds);

		void addRigidbody(const RigidBody & rb) {rigidbodies.push_back(rb);}
		void addSphereCollider(const SphereCollider & col) {sphereColliders.push_back(col);}
	private:
		std::vector<RigidBody> rigidbodies;
		std::vector<SphereCollider> sphereColliders;
		std::vector<CuboidCollider> cuboidColliders;

		//Simple variations do not use rigidbodies, therefore, only test for collision this frame with no regard for velocity
		os_bool_t collidesSimpleSphereSphere(const SphereCollider & col1, const SphereCollider & col2);
		os_bool_t collidesSimpleCuboidCuboid(const CuboidCollider & col1, const CuboidCollider & col2);
		os_bool_t collidesSimpleSphereCuboid(const SphereCollider & col1, const CuboidCollider & col2);

		os_bool_t collidesSphereSphere(const SphereCollider & col1, const SphereCollider & col2);
		os_bool_t collidesCuboidCuboid(const CuboidCollider & col1, const CuboidCollider & col2);
		os_bool_t collidesSphereCuboid(const SphereCollider & col1, const CuboidCollider & col2);
	};
} /* namespace osp */

#endif /* PHYSICS_PHYSICSENGINE_H_ */
