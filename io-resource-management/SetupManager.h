#pragma once

#include "../Globals.h"
#include "../rendering/RenderingEngine.h"
#include "../animation/AnimationEngine.h"
#include "ResourceManager.h"

#include <utility>
#include <string>
#include <vector>
#include <bitset>
#include <sstream>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#include <fstream>
#include <cassert>

namespace osrm
{
	class SetupManager
	{
	public:
		SetupManager();
		~SetupManager();



		//pos, normals, texCoords & colour specify the number of floats per vector for each attrib
		//value of 0 means the attrib will not be included
		const OSError buildVertShader(std::stringstream & outTxt, std::vector<std::pair<os_uint_t, os_uint_t>> & locs, os_bool_t ortho,
			os_uint_t pos, os_uint_t normals, os_uint_t texCoords, os_uint_t colour,
			os_bool_t instanced, os_bool_t shadows, os_bool_t skinned, os_int_t maxNumBones, os_bool_t normalMapping);
		const OSError buildFragShader(std::stringstream & outTxt, os_bool_t ortho, os_uint_t pos, os_uint_t normals, os_uint_t texCoords,
			os_uint_t colour, os_bool_t deferred, os_bool_t shadows, os_bool_t highDynamicRange, os_bool_t normalMapping);



		virtual const OSError setupStaticMeshRenderers(std::vector<osrm::MeshFilter> loadedMeshes, std::vector<osr::RenderObject> & outRenderObjects) = 0;
		virtual const OSError setupSkinnedMeshRenderers(std::vector<osrm::MeshFilter> loadedMeshes, std::vector<osan::SkinnedMesh *> & skinnedMeshes, std::vector<osr::RenderObject> & outRenderObjects) = 0;
		virtual const OSError setupStaticPlaneRenderers(std::vector<osr::RenderObject> & outRenderObjects) = 0;
		virtual osr::RenderObject setupSkybox(osr::Skybox skybox) = 0;
		virtual osr::RenderObject setupUI(const osrm::UIFilter & uiFilter, osi::UserInterface * userInterface, osr::RenderObject & textRenderObject) = 0;


		virtual void setupShaders(std::vector<osr::RenderObject> * renderObjects, os_uint_t vertShader, os_uint_t fragShader,
			os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices, os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting,
			os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights, const std::vector<std::pair<os_uint_t, os_uint_t>> & locs) = 0;


		virtual void setupDeferredShadingShaders(osr::RenderingEngine * renderingEngine, os_int_t framebufferWidth, os_int_t framebufferHeight,
			os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting, os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights) = 0;

		virtual void setupShadowMapShaders(osr::RenderingEngine * renderingEngine, os_int_t shadowMapWidth, os_int_t shadowMapHeight) = 0;

		///virtual void setupStaticMeshShaders(std::vector<osr::RenderObject> * staticRenderObjects, os_uint_t vertShader, os_uint_t fragShader,
		///	os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices, os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting,
		///	os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights, const std::vector<std::pair<os_uint_t, os_uint_t>> & locs) = 0;

		///virtual void setupSkinnedMeshShaders(std::vector<osr::RenderObject> * skinnedRenderObjects, std::vector<osan::SkinnedMesh *> & skinnedMeshes, os_uint_t vertShader, os_uint_t fragShader,
		///	os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices, os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting,
		///	os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights, const std::vector<std::pair<os_uint_t, os_uint_t>> & locs) = 0;

		virtual void setupSkyboxShaders(osr::RenderObject skyboxRenderObject, os_uint_t skyboxCubemapID, os_uint_t vertShader, os_uint_t fragShader,
			os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices) = 0;

		virtual void setupUIShaders(osr::RenderObject uiRenderObject, os_uint_t uiTexAtlasID, os_uint_t vertShader, os_uint_t fragShader) = 0;

		virtual void setupTextShaders(osr::RenderObject textRenderObject, os_uint_t textTexAtlasID, os_uint_t vertShader, os_uint_t fragShader) = 0;

		const std::string loadTextFile(const std::string & strShaderFile) const;

	private:

		//Map: int = type of var e.g. pos/normal/colour represented as a number, index = glsl layout location
		const OSError buildVertShaderGLSL(std::stringstream & source, std::vector<std::pair<os_uint_t, os_uint_t>> & locs,
			os_int_t version, const std::vector<std::string> & defines,
			const std::vector<std::pair<os_uint_t, os_uint_t>> & inVars, const std::vector<std::pair<os_uint_t, os_uint_t>> & outVars,
			const std::vector<std::string> & uniformTypes, const std::vector<std::string> & uniformNames,
			const std::vector<os_uint_t> & defaultFuncs, const std::vector<std::string> & customFuncs,
			os_bool_t instanced, os_bool_t shadows, os_bool_t skinned, os_bool_t normalMapping);

		const OSError buildFragShaderGLSL(std::stringstream & source, os_int_t version, const std::vector<std::string> & defines,
			const std::vector<std::pair<os_uint_t, os_uint_t>> & inVars, const std::vector<std::pair<os_uint_t, os_uint_t>> & outVars,
			const std::vector<std::string> & uniformTypes, const std::vector<std::string> & uniformNames,
			const std::vector<os_uint_t> & defaultFuncs, const std::vector<std::string> & customFuncs,
			os_bool_t deferred, os_bool_t materials, os_bool_t highDynamicRange, os_bool_t normalMapping);
	};
}
