/*
 * ProjectIO.h
 *
 *  Created on: 9 Oct 2015
 *      Author: james_000
 */

#ifndef IO_RESOURCE_MANAGEMENT_PROJECTIO_H_
#define IO_RESOURCE_MANAGEMENT_PROJECTIO_H_

#define GLEW_STATIC
#define NOMINMAX

#include "../rendering/RenderingEngine.h"
#include "../animation/AnimationEngine.h"
#include "../physics/PhysicsEngine.h"
#include "../multi-threading/TaskManager.h"
#include "../input/InputManager.h"
#include "../input/Input.h"
#include "ResourceManager.h"
#include "SetupManager.h"
#include "SetupManagerGL.h"

#include "../user-interface/UIListBox.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../maths/Transform.h"

namespace osrm
{
	void loadScene(ResourceManager * resourceManager, const std::string & path);	//TODO - Temporary fnc for testing, to be replaced
	void parseSceneHeaderFile(ResourceManager * resourceManager, std::string path);
	void parseSceneBodyFile(ResourceManager * resourceManager, std::string path);
	void parseInputFile(ResourceManager * resourceManager, std::string path);
	void parseUIFile(ResourceManager * resourceManager, std::string path);
	void buildSceneHeader(ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osi::InputManager * inputManager, osi::FontManager * fontManager);
	void buildSceneBody(ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osan::AnimationEngine * animationEngine, osi::UserInterface * ui, bool genFBO = true);
	void buildShaders(SetupManager * setupManager, ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osan::AnimationEngine * animationEngine,
		std::vector<osr::RenderObject> * staticRenderObjects, std::vector<osr::RenderObject> * skinnedRenderObjects, std::vector<osan::SkinnedMesh *> & skinnedMeshes,
		osr::RenderObject skyboxRenderObject, GLuint skyboxCubemapID, osr::RenderObject uiRenderObject, GLuint uiTexAtlasID,
		osr::RenderObject textRenderObject, GLuint textTexAtlasID, bool genFBO);

	void saveScene(const std::string & path, ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osan::AnimationEngine * animationEngine, osp::PhysicsEngine * physicsEngine);
	void saveSceneHeader(const std::string & path, ResourceManager * resourceManager);
	void saveSceneBody(const std::string & path, ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osan::AnimationEngine * animationEngine, osp::PhysicsEngine * physicsEngine);
}


#endif /* IO_RESOURCE_MANAGEMENT_PROJECTIO_H_ */
