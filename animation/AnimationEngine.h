/*
 * AnimationEngine.h
 *
 *  Created on: 20 Mar 2016
 *      Author: james_000
 */

#ifndef ANIMATION_ANIMATIONENGINE_H_
#define ANIMATION_ANIMATIONENGINE_H_

#include "../glm/detail/type_vec.hpp"
#include "../glm/fwd.hpp"

#define GLEW_STATIC

#include "../Globals.h"

#include <bitset>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>

#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtx/quaternion.hpp"
#include "../glm/detail/type_mat.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"

#include "../io-resource-management/SkinnedMesh.h"

#define NUM_BONES_PER_VERTEX 4	//Temporary TODO - Dynamic number of bones per vertex 

namespace osan
{
	//NOTE - If change to use pointers, change node animation copy constructor as sizeof() may be wrong
	struct KeyFrame		//Keyframe for position/scale
	{
		os_float_t time;
		glm::vec3 value;//Can be either scale or position
	};

	struct KeyFrameOrient
	{
		os_float_t time;
		glm::quat orientation;
	};

	struct NodeAnimation				//The animation of a particular node
	{
		std::string name;
		KeyFrame * posKeys;				//Positions at keyframes
		KeyFrame * scaleKeys;			//Scales at keyframes
		KeyFrameOrient * orientKeys;	//Orientations at keyframes
		os_uint_t numPosKeys, numScaleKeys, numOrientKeys;

		os_float_t ticksPerSecond;
		os_float_t duration;

		NodeAnimation()
		{
			posKeys = nullptr;
			scaleKeys = nullptr;
			orientKeys = nullptr;
			numPosKeys = 0;
			numScaleKeys = 0;
			numOrientKeys = 0;
		}

		NodeAnimation(const NodeAnimation & source)	//Deep Copy Constructor - Allocates Memory for Keyframe Pointer Arrays
		{
			name = source.name;
			numPosKeys = source.numPosKeys;
			numScaleKeys = source.numScaleKeys;
			numOrientKeys = source.numOrientKeys;
			ticksPerSecond = source.ticksPerSecond;
			duration = source.duration;
			
			if(source.numPosKeys > 0)
				posKeys = new KeyFrame[source.numPosKeys];
			else
				posKeys = nullptr;
			if(source.numScaleKeys > 0)
				scaleKeys = new KeyFrame[source.numScaleKeys];
			else
				scaleKeys = nullptr;
			if(source.numOrientKeys > 0)
				orientKeys = new KeyFrameOrient[source.numOrientKeys];
			else
				orientKeys = nullptr;
	
			if(source.posKeys != nullptr && posKeys != nullptr)
				memcpy(posKeys, source.posKeys, source.numPosKeys * sizeof(KeyFrame));
			if(source.scaleKeys != nullptr && scaleKeys != nullptr)
				memcpy(scaleKeys, source.scaleKeys, source.numScaleKeys * sizeof(KeyFrame));
			if(source.orientKeys != nullptr && orientKeys != nullptr)
				memcpy(orientKeys, source.orientKeys, source.numOrientKeys * sizeof(KeyFrameOrient));
		}

		~NodeAnimation()
		{
			if(posKeys != nullptr)
				delete[] posKeys;
			if(scaleKeys != nullptr)
				delete[] scaleKeys;
			if(orientKeys != nullptr)
				delete[] orientKeys;
			posKeys = nullptr;
			scaleKeys = nullptr;
			orientKeys = nullptr;
		}
	};

	struct SkeletalNode			//Can be a bone, or a node for transforming other bones. Transform nodes will have bone info index as -1
	{
		std::string name;
		glm::mat4 transform;

		os_int_t boneIndex;			//The index to the related bone info in the bone info array, -1 if transform node

		os_int_t parent;

		std::vector<NodeAnimation> nodeAnims;
		os_int_t currentAnimation;

		SkeletalNode() : currentAnimation(-1) {}

		SkeletalNode(const SkeletalNode & source)	//Deep Copy Constructor
		{
			//std::cerr << "Creating Copy of Node" << std::endl;

			name = source.name;
			transform = source.transform;
			boneIndex = source.boneIndex;
			parent = source.parent;
			nodeAnims = source.nodeAnims;
			currentAnimation = source.currentAnimation;
		}

		~SkeletalNode(){}
	};

	struct Bone					//Bone structure for each bone in aiScene
	{
		glm::mat4 offsetMatrix, finalMatrix;
	};

	//One VertexBoneData Per Vertex
	struct VertexBoneData		//VertexBoneData structure for each vertex of the aiScene
	{
		os_uint_t  IDs[NUM_BONES_PER_VERTEX];
		os_float_t weights[NUM_BONES_PER_VERTEX];
	};

	struct SkinnedMesh
	{
		VertexBoneData * vertexBoneData;
		std::vector<Bone> bones;
		std::vector<SkeletalNode *> nodes;
		os_uint_t numAnimations, numVertexBoneData;
		os_uint_t shaderProg;
		size_t totalBoneOffset;			//The offset into bone array for first bone of this skinned mesh

		SkinnedMesh()
		{
			vertexBoneData = nullptr;
			numVertexBoneData = 0;
			numAnimations = 0;
			totalBoneOffset = 0;
		}

		SkinnedMesh(const SkinnedMesh & source)	//Deep Copy Constructor
		{
			//std::cerr << "Creating a Copy of Skinned Mesh" << std::endl;

			bones = source.bones;
			numAnimations = source.numAnimations;
			numVertexBoneData = source.numVertexBoneData;
			shaderProg = source.shaderProg;
			totalBoneOffset = 0;

			for(int i = 0; i < source.nodes.size(); i++)
			{
				nodes.push_back(new SkeletalNode(*source.nodes[i]));
			}

			if(source.vertexBoneData != nullptr)
				vertexBoneData = new VertexBoneData(*source.vertexBoneData);
			else
				vertexBoneData = nullptr;
		}

		~SkinnedMesh()
		{
			//if(vertexBoneData != nullptr)
			//	delete vertexBoneData;
			
			while(!nodes.empty())
			{
				if(nodes.back() != nullptr)
					delete nodes.back();
				nodes.pop_back();
			}
		}
	};

	void addBoneData(VertexBoneData * vbd, GLuint boneID, GLfloat weight);

	struct Connector	//One way connector between two animations of a single animation controller
	{
		os_int_t animationFrom;		//The animation to switch from
		os_int_t animationTo;		//The animation to switch to
		std::string condition;	//TODO - Temporary - Name of an input object contained within input manager

		//TODO - Make size variable to support higher numbers of bones
		//uint32_t bitmask;	//Specifying which bones should switch animation
		std::bitset<32> bitmask;

		Connector(SkinnedMesh & skinnedMesh, os_int_t animationFrom, os_int_t animationTo, std::string condition, std::bitset<32> mask)
		{
			this->animationFrom = animationFrom;
			this->animationTo = animationTo;
			this->condition = condition;

			this->bitmask = mask;
			for(int i = 0; i < skinnedMesh.nodes.size() && i < 32; i++)	//32 max num nodes in bitmask
			{
				SkeletalNode * node = skinnedMesh.nodes[i];

				//1 = Contains, 0 = Not Contains
				if(node->parent != -1)
				{
					if(bitmask[i] == 0 && bitmask[node->parent] == 1)	//If bitmask contains parent index but does not contain i
					{
						bitmask[i] = 1;									//Add i the list of nodes to switch animation
						//node->currentAnimation = animation;				//Switch the animaton of i
					}
					else if(bitmask[i] == 1 && bitmask[node->parent] == 1)	//If bitmask contains parent and i then it signifies the parent will change animation but not it's children
					{
						bitmask[i] = 0;										//Remove i so that children do not change animation
					}
				}
			}
		}
	};

	struct AnimationController
	{
		SkinnedMesh & skinnedMesh;

		//Used to apply an offset into the bone transform array for each instance 
		os_int_t skinnedMeshIndex;	//The index of the animation controller for the particular skinned mesh

		//The time the animation was started
		os_float_t animStartTimeSeconds;

		os_int_t currentAnimation;
		std::vector<Connector> connectors;	//Array of connectors

		AnimationController(SkinnedMesh & sMesh, os_int_t skinnedIndex) : skinnedMesh(*new osan::SkinnedMesh(sMesh))
		{
			//std::cerr << "Creating Animation Controller\n";
			skinnedMeshIndex = skinnedIndex;
			currentAnimation = -1;
		}

		void addConnector(os_int_t animationFrom, os_int_t animationTo, std::string condition, std::bitset<32> mask)
		{
			Connector c(skinnedMesh, animationFrom, animationTo, condition, mask);	//Animation indices offset by 1 as 0 used to represent from any/to no animation
			connectors.push_back(c);
		}
	};







	class AnimationEngine
	{
	public:
		AnimationEngine();
		virtual ~AnimationEngine();

		void updateSkeletalAnimations(GLfloat timeSeconds);

		void addAnimationController(AnimationController animController) {animationControllers.push_back(animController);}
		std::vector<osan::AnimationController> & getAnimationControllers() {return animationControllers;}
		AnimationController & getAnimationController(size_t a) {return animationControllers[a];}

		void setAnimation(os_int_t animControllerIndex, os_int_t animation, os_float_t timeSeconds, std::bitset<32> bitmask);

		//void addSkeletalAnimation(SkeletalAnimation anim) {this->skeletalAnimations.push_back(anim);}
		//std::vector<SkeletalAnimation> * getSkeletalAnimations() {return &skeletalAnimations;}
		//SkeletalAnimation * getSkeletalAnimation(size_t a) {return &(skeletalAnimations[a]);}
	private:
		std::vector<AnimationController> animationControllers;

		glm::vec3 interpolateTranslation(os_float_t animationTime, NodeAnimation * boneAnim);
		glm::vec3 interpolateScale(os_float_t animationTime, 		NodeAnimation * boneAnim);
		glm::quat interpolateOrientation(os_float_t animationTime, NodeAnimation * boneAnim);

		os_uint_t getTranslationIndex(os_float_t animationTime, NodeAnimation * boneAnim);
		os_uint_t getScaleIndex(os_float_t animationTime, NodeAnimation * boneAnim);
		os_uint_t getOrientationIndex(os_float_t animationTime, NodeAnimation * boneAnim);


		glm::mat4 updateBoneHierarchy(os_float_t animationTime, os_float_t timeSeconds, os_int_t currentAnim, osan::AnimationController & animController, SkeletalNode * node, glm::mat4 parentTransform);
	};
} /* namespace osan */

#endif /* ANIMATION_ANIMATIONENGINE_H_ */
