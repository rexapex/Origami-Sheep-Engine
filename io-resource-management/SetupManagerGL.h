#pragma once

#include "../Globals.h"
#include "SetupManager.h"

#include <vector>
#include <map>

namespace osrm
{
	class SetupManagerGL : public SetupManager
	{
	public:
		SetupManagerGL(osr::RenderingEngine * renderingEngine, osan::AnimationEngine * animationEngine);
		~SetupManagerGL();


		const OSError setupStaticMeshRenderers(std::vector<osrm::MeshFilter> loadedMeshes, std::vector<osr::RenderObject> & outRenderObjects);
		const OSError setupSkinnedMeshRenderers(std::vector<osrm::MeshFilter> loadedMeshes, std::vector<osan::SkinnedMesh *> & skinnedMeshes, std::vector<osr::RenderObject> & outRenderObjects);
		const OSError setupStaticPlaneRenderers(std::vector<osr::RenderObject> & outRenderObjects);
		osr::RenderObject setupSkybox(osr::Skybox skybox);
		virtual osr::RenderObject setupUI(const osrm::UIFilter & uiFilter, osi::UserInterface * userInterface, osr::RenderObject & textRenderObject);
	

		//Setup an arbitrary shader for any type of render object
		void setupShaders(std::vector<osr::RenderObject> * renderObjects, os_uint_t vertShader, os_uint_t fragShader,
			os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices, os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting,
			os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights, const std::vector<std::pair<os_uint_t, os_uint_t>> & locs);


		/*	The generated vertex attribute is returned in the attrib variable
			The value of offset is increased within the method and returned
			If using instance matrix, the matrix must be stored in its own VBO, inOutOffset is ignored
			If using bone offset, the offset must be stored in its own VBO, inOutOffset is ignored
		*/
		void genVertexAttrib(std::vector<osr::VertexAttrib> & outAttribs, const std::pair<os_uint_t, os_uint_t> & loc, GLsizei & inOutOffset, GLsizei VBO0Stride);


		void setupDeferredShadingShaders(osr::RenderingEngine * renderingEngine, os_int_t framebufferWidth, os_int_t framebufferHeight,
			os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting, os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights);

		void setupShadowMapShaders(osr::RenderingEngine * renderingEngine, os_int_t shadowMapWidth, os_int_t shadowMapHeight);

		///void setupStaticMeshShaders(std::vector<osr::RenderObject> * staticRenderObjects, os_uint_t vertShader, os_uint_t fragShader,
		///	os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices, os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting,
		///	os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights, const std::vector<std::pair<os_uint_t, os_uint_t>> & locs);

		///void setupSkinnedMeshShaders(std::vector<osr::RenderObject> * skinnedRenderObjects, std::vector<osan::SkinnedMesh *> & skinnedMeshes, os_uint_t vertShader, os_uint_t fragShader,
		///	os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices, os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting,
		///	os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights, const std::vector<std::pair<os_uint_t, os_uint_t>> & locs);

		void setupSkyboxShaders(osr::RenderObject skyboxRenderObject, os_uint_t skyboxCubemapID, os_uint_t vertShader, os_uint_t fragShader,
			os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices);

		void setupUIShaders(osr::RenderObject uiRenderObject, os_uint_t uiTexAtlasID, os_uint_t vertShader, os_uint_t fragShader);

		void setupTextShaders(osr::RenderObject textRenderObject, os_uint_t textTexAtlasID, os_uint_t vertShader, os_uint_t fragShader);

		//void setupShaders(std::vector<osr::RenderObject> * renderObjects, os_uint_t vertShader, os_uint_t fragShader);
	private:
		osr::RenderingEngine * renderingEngine;
		osan::AnimationEngine * animationEngine;
	};
}
