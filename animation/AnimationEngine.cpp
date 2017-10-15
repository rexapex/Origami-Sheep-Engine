/*
 * AnimationEngine.cpp
 *
 *  Created on: 20 Mar 2016
 *      Author: james_000
 */

#include "AnimationEngine.h"

namespace osan
{
	void addBoneData(VertexBoneData * vbd, GLuint boneID, GLfloat weight)
	{
		//std::cerr << "ID 0 = " << vbd->IDs[0] << std::endl;
		for(os_uint_t i = 0; i < 4; i++)
		{
			if(vbd->weights[i] == 0.0f)
			{
				//std::cout << "weights[i] = 0" << std::endl;
				vbd->IDs[i] = boneID;
				vbd->weights[i] = weight;
				return;
			}
		}
		//std::cerr << "Reached Bone Bone Influence Limit" << std::endl;
	}



	AnimationEngine::AnimationEngine()
	{
		// TODO Auto-generated constructor stub
	}

	AnimationEngine::~AnimationEngine()
	{
		// TODO Auto-generated destructor stub
	}

	void AnimationEngine::updateSkeletalAnimations(GLfloat timeSeconds)
	{
		for(size_t a = 0; a < animationControllers.size(); a++)
		{
			osan::AnimationController & animController = animationControllers[a];
			osan::SkinnedMesh & skinnedMesh = animController.skinnedMesh;

			glm::mat4 identity = glm::mat4();

			os_int_t currentAnim = skinnedMesh.nodes[0]->currentAnimation;

			GLfloat animationTime;
			if(currentAnim >= 0 && currentAnim < skinnedMesh.nodes[0]->nodeAnims.size())
			{
				GLfloat timeTicks = (timeSeconds - animController.animStartTimeSeconds) * skinnedMesh.nodes[0]->nodeAnims[currentAnim].ticksPerSecond;
				animationTime = fmod(timeTicks, skinnedMesh.nodes[0]->nodeAnims[currentAnim].duration);	//Calculate how long into the animation the current time is
			}
			else
			{
				animationTime = 0;
			}


			std::vector<glm::mat4> transformStack;	//Stack of parent transformations

			for(size_t i = 0; i < skinnedMesh.nodes.size(); i++)	//Calculate all parent transforms first based on positions into vector
			{
				SkeletalNode * node = skinnedMesh.nodes[i];

				glm::mat4 parentMatrix;

				if(node->parent < 0)
					parentMatrix = identity;
				else
					parentMatrix = transformStack[node->parent];

				transformStack.push_back(updateBoneHierarchy(animationTime, timeSeconds, currentAnim, animController, skinnedMesh.nodes[i], parentMatrix));	//Update the bones
			}
		}
	}

	glm::mat4 AnimationEngine::updateBoneHierarchy(os_float_t animationTime, os_float_t timeSeconds, os_int_t currentAnim, osan::AnimationController & animController,
		SkeletalNode * node, glm::mat4 parentTransform)
	{
		if(node == nullptr)
		{
			//std::cerr << "Node is nullptr" << std::endl;
			return glm::mat4();
		}

		glm::mat4 nodeTransform = node->transform;

		//std::cerr << "Current Anim: " << node->currentAnimation << "\n";
		//std::cerr << "Num Anims: " << node->nodeAnims.size() << "\n";
		NodeAnimation * nodeAnim = nullptr;
		if(node->currentAnimation >= 0 && node->currentAnimation < node->nodeAnims.size())
		{
			//std::cerr << "Current Anim: " << node->currentAnimation << "\n";
			nodeAnim = &node->nodeAnims[node->currentAnimation];

			if(node->currentAnimation != currentAnim)	//Different animations therefore may have different timings
			{
				currentAnim = node->currentAnimation;
				GLfloat timeTicks = (timeSeconds - animController.animStartTimeSeconds) * nodeAnim->ticksPerSecond;
				animationTime = fmod(timeTicks, nodeAnim->duration);
			}
		}
		//std::cerr << "Anim Time: " << animationTime << "\n";


		
		//Calculate matrices for scale, orientation and translation
		if(nodeAnim != nullptr && nodeAnim->posKeys != nullptr)		//Pos keys as nullptr signifies a blank node animation
		{
			//std::cerr << "Anim Time: " << animationTime << "\n";
			glm::mat4 translationMatrix = glm::translate(glm::mat4(), interpolateTranslation(animationTime, nodeAnim));
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(), interpolateScale(animationTime, nodeAnim));
			glm::mat4 rotationMatrix = glm::toMat4(interpolateOrientation(animationTime, nodeAnim));

			//Calculate the nodes transform matrix relative to the parent
			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}

		glm::mat4 globalTransform = parentTransform * nodeTransform;

		if(node->boneIndex >= 0 && node->boneIndex < animController.skinnedMesh.bones.size())	//Set the final matrix on the bone equal to the nodes world transform times its offset matrix
		{
			//std::cerr << "Bone Index: " << node->boneIndex << "\n";

			osan::Bone & thisBoneData = animController.skinnedMesh.bones[node->boneIndex];
			thisBoneData.finalMatrix = globalTransform * thisBoneData.offsetMatrix;
		}


		return globalTransform;
	}




	glm::vec3 AnimationEngine::interpolateTranslation(os_float_t animationTime, NodeAnimation * boneAnim)
	{
		if(boneAnim->numPosKeys == 1)
			return boneAnim->posKeys[0].value;

		os_uint_t index = getTranslationIndex(animationTime, boneAnim);
		os_uint_t nextIndex = index + 1;

		if(nextIndex >= boneAnim->numPosKeys)
			nextIndex = 0;

		os_float_t deltaTime = boneAnim->posKeys[nextIndex].time - boneAnim->posKeys[index].time;
		os_float_t factor = (animationTime - boneAnim->posKeys[index].time) / deltaTime;

		return glm::mix(boneAnim->posKeys[index].value, boneAnim->posKeys[nextIndex].value, factor);
	}

	glm::vec3 AnimationEngine::interpolateScale(os_float_t animationTime, NodeAnimation * boneAnim)
	{
		if(boneAnim->numScaleKeys == 1)
			return boneAnim->scaleKeys[0].value;

		os_uint_t index = getScaleIndex(animationTime, boneAnim);
		os_uint_t nextIndex = index + 1;

		if(nextIndex >= boneAnim->numScaleKeys)
			nextIndex = 0;

		os_float_t deltaTime = boneAnim->scaleKeys[nextIndex].time - boneAnim->scaleKeys[index].time;
		os_float_t factor = (animationTime - boneAnim->scaleKeys[index].time) / deltaTime;

		return glm::mix(boneAnim->scaleKeys[index].value, boneAnim->scaleKeys[nextIndex].value, factor);
	}

	glm::quat AnimationEngine::interpolateOrientation(os_float_t animationTime, NodeAnimation * boneAnim)
	{
		if(boneAnim->numOrientKeys == 1)
			return boneAnim->orientKeys[0].orientation;

		os_uint_t index = getOrientationIndex(animationTime, boneAnim);
		os_uint_t nextIndex = index + 1;

		if(nextIndex >= boneAnim->numOrientKeys)
			nextIndex = 0;

		os_float_t deltaTime = boneAnim->orientKeys[nextIndex].time - boneAnim->orientKeys[index].time;
		os_float_t factor = (animationTime - boneAnim->orientKeys[index].time) / deltaTime;

		return glm::slerp(boneAnim->orientKeys[index].orientation, boneAnim->orientKeys[nextIndex].orientation, factor);
	}


	os_uint_t AnimationEngine::getTranslationIndex(os_float_t animationTime, NodeAnimation * boneAnim)
	{
		for(os_uint_t i = 0; i < boneAnim->numPosKeys - 1; i++)
		{
			if(animationTime < boneAnim->posKeys[i + 1].time)		//Find the tranlsation keyframe for a given time through the animation
			{
				return i;
			}
		}

		return 0;
	}

	os_uint_t AnimationEngine::getScaleIndex(os_float_t animationTime, NodeAnimation * boneAnim)
	{
		for(os_uint_t i = 0; i < boneAnim->numScaleKeys - 1; i++)
		{
			if(animationTime < boneAnim->scaleKeys[i + 1].time)		//Find the scale keyframe for a given time through the animation
			{
				return i;
			}
		}

		return 0;
	}

	os_uint_t AnimationEngine::getOrientationIndex(os_float_t animationTime, NodeAnimation * boneAnim)
	{
		for(os_uint_t i = 0; i < boneAnim->numOrientKeys - 1; i++)
		{
			if(animationTime < boneAnim->orientKeys[i + 1].time)	//Find the orientation keyframe for a given time through the animation
			{
				return i;
			}
		}

		return 0;
	}



	void AnimationEngine::setAnimation(os_int_t animControllerIndex, os_int_t animation, os_float_t timeSeconds, std::bitset<32> bitmask)
	{
		if(animControllerIndex < animationControllers.size())
		{
			AnimationController & animController = animationControllers[animControllerIndex];

			animController.animStartTimeSeconds = timeSeconds;
			animController.currentAnimation = animation;
			
			for(size_t i = 0; i < animController.skinnedMesh.nodes.size() && i < 32; i++)	//Max num bits in mask is 32 therefore don't loop over 32
			{
				if(bitmask[i] == 1)
				{
					animController.skinnedMesh.nodes[i]->currentAnimation = animation;
				}
			}
		}
	}

} /* namespace osan */








