#pragma once

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>

#include "animation\AnimationEngine.h"
#include "physics\PhysicsEngine.h"
#include "rendering\RenderingEngine.h"
#include "io-resource-management\ProjectIO.h"
#include "io-resource-management\ResourceManager.h"

#include <string>
#include <vector>
#include <iostream>

namespace script
{
	void init(osrm::ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osp::PhysicsEngine * physicsEngine,
		osan::AnimationEngine * animationEngine, osi::InputManager * inputManager, std::vector<osrm::Entity> & entities);
}