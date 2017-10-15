/*
 * RenderingEngine.h
 *
 *  Created on: 5 Oct 2015
 *      Author: james_000
 */



#ifndef RENDERING_RENDERINGENGINE_H_
#define RENDERING_RENDERINGENGINE_H_

#define NOMINMAX

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <stddef.h>
#include <cstring>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <cassert>

#include "../animation/AnimationEngine.h"
#include "../maths/Transform.h"
#include "../user-interface/UserInterface.h"
#include "../maths/Camera.h"

typedef unsigned int ID;

namespace osr
{
	struct Material
	{
		glm::vec3 ambientColour;
		glm::vec3 diffuseColour;
		glm::vec3 specularColour;
		os_float_t specularShininess;
	};

	struct Uniform
	{
		os_int_t loc;
		os_uint_t func;
		os_uint_t modifiers;
		void * data;
	};

	struct Texture
	{
		os_uint_t texID;
		GLenum type;
	};

	struct RenderObject
	{
		std::vector<glm::mat4 *> transforms;		//Pointers to the transforms of the instances in world space
		std::vector<glm::mat4> camSpaceTransforms;	//Transforms of the instances in camera space, calculated each update
		size_t numInstances;
		os_uint_t instanceVBO;						//VBO containing transforms of all instances of render object
		os_uint_t VAO;
		std::vector<os_uint_t> VBOs;
		os_uint_t IBO;
		Material * materials;						//One material per render call
		Texture * textures;							//One texture per render call
		os_uint_t * numVerticesPerRenderCall;		//The number of vertices in specific render call, if array drawing -> size = 1
		os_uint_t * numTexturesPerRenderCall;		//The of textures in specific render call
		size_t sizeMaterials, sizeNumVerticesPerRenderCall, sizeTextures, sizeNumTexturesPerRenderCall;
		GLenum renderPrimitive;						//Default to GL_TRIANGLE
		std::vector<Uniform> uniforms;				//Must be used in the same order they are added rather than by name
	};

	struct ShaderProgram
	{
		os_uint_t program;

		ShaderProgram() {}
		ShaderProgram(os_uint_t prog) : program{prog} {}
	};

	struct ShaderGroup
	{
		os_uint_t UBO;				//Used to store uniforms used by all of the shader programs
		std::vector<ShaderProgram> shaderPrograms;		//Multiple shader programs for same object for merging results
		std::vector<RenderObject> renderObjects;
	};
	
	struct RenderPass	//Used to render batch of shader groups before other etc
	{
		os_uint_t FBO;
		os_int_t FBOWidth, FBOHeight;
		GLenum clear;
		os_bool_t depthTest, blend;
		std::vector<ShaderGroup> shaderGroups;
	};




	struct VertexAttrib	//Convenience struct used for vao setup
	{
		os_uint_t location;
		os_uint_t size;
		GLenum type;
		os_bool_t normalised;
		GLsizei stride;
		os_sizeiptr_t offset;
		os_uint_t VBO;
		os_uint_t divisor;
		os_uint_t attribID;		//Used by the setup -> linked to shader generator
	};




	struct DirectionLight
	{
		glm::vec4 dir;
		glm::vec4 ambientColour;
		glm::vec4 diffuseColour;
		glm::vec4 specularColour;

		os_float_t intensity;
		os_float_t padding4[3];
	};

	struct PointLight
	{
		glm::vec4 relativePos;
		glm::vec4 ambientColour;
		glm::vec4 diffuseColour;
		glm::vec4 specularColour;

		os_float_t intensity;

		//Attenuation
		os_float_t constant;
		os_float_t linear;
		os_float_t quadratic;
	};

	struct SpotLight
	{
		glm::vec4 relativePos;
		glm::vec4 dir;				//Direction of the light source

		glm::vec4 ambientColour;
		glm::vec4 diffuseColour;
		glm::vec4 specularColour;

		os_float_t cosInnerCutoff;	//Cosine of the inner cutoff angle
		os_float_t cosOuterCutoff;	//Cosine of the outer cutoff angle

		os_float_t intensity;

		os_float_t constant;
		os_float_t linear;
		os_float_t quadratic;

		os_float_t padding6[2];
	};
	//AreaLight
	//VolumetricLight

	struct Skybox	//TODO - Add in meshes
	{
		os_uint_t cubemapID;
	};



	
	


	enum class TexFilterMode
	{
		NEAREST, LINEAR,
		LINEAR_MIP_MAP_NEAREST,		//Uses nearest MIP Map texture and uses linear filtering on 4 samples from it
		LINEAR_MIP_MAP_LINEAR,		//Linear filters between MIP Maps and then uses linear filtering on the samples retrieved
		ANISOTROPIC_MIP_MAP_LINEAR
	};

	enum class AAMode	//Can take different modes of anti-aliasing
	{
		NONE,
		MSAA, SSAA, FSAA,	//Increase sample rate
		MLAA, FXAA, SMAA	//Blur edges/contrasts -> post processing AA
	};

	enum class Quality	//Used for texture/mesh/lighting quality
	{
		VERY_LOW, LOW, MEDIUM, HIGH, VERY_HIGH, ULTRA
	};




	class RenderingEngine	//Only one rendering engine will be created on the thread with the OpenGL context
	{
	public:
		osm::Transform * mainCamTransform;
		glm::mat4 * mainCamTransformInv;
		osm::Transform * getMainCamTransform() const {return mainCamTransform;}
		void setCamera(osm::Camera * cam) {mainCamTransform = cam->getTransform(); mainCamTransformInv = cam->getTransformInverse();}
	//	void setGUI(osi::GUI * gui) {this->gui = gui;}



		RenderingEngine(os_int_t framebufferWidth, os_int_t framebufferHeight);
		virtual ~RenderingEngine();

		void render();	//Renders the list of render-ables using the given shader programs
		void addRenderable(size_t renderPassIndex, const osr::ShaderGroup &);
		void addRenderPass(const osr::RenderPass & renderPass);

		void setSkybox(osr::ShaderGroup sg, os_uint_t tex) {this->skyboxShaderGroup = sg; this->skyboxTexID = tex;}
		void setUI(osr::ShaderGroup ui, os_uint_t tex) {this->uiShaderGroup = ui; this->uiTexID = tex;}
		void setText(osr::ShaderGroup text, os_uint_t tex) {this->textShaderGroup = text; this->textTexID = tex;}
		void setFramebuffer(os_uint_t fbo, os_uint_t pos, os_uint_t norm, os_uint_t col, os_uint_t dep, os_uint_t vbo, os_uint_t vao, os_uint_t prog) {gBufferFBO = fbo, gPos = pos, gNormal = norm, gColourSpec = col, gDepthRBO = dep, framebufferVBO = vbo, framebufferVAO = vao, framebufferProg = prog;}
		void setShadowMap(os_uint_t fbo, os_uint_t map, os_uint_t width, os_uint_t height, os_uint_t prog, os_uint_t pos0, os_uint_t pos1) {shadowMapFBO = fbo, shadowMap = map, shadowMapWidth = width, shadowMapHeight = height, shadowMapProg = prog, shadowMapPos0 = pos0, shadowMapPos1 = pos1;}

		//void addPointLights(std::vector<PointLight> newPointLights) {this->pointLights.insert(this->pointLights.end(), newPointLights.begin(), newPointLights.end());}
		void setLightPtrs(std::vector<PointLight> * pls, std::vector<DirectionLight> * dls, std::vector<SpotLight> * sls) {pointLights = pls; dirLights = dls; spotLights = sls;}
		void setLightUBO(os_uint_t lightUBO) {this->lightUBO = lightUBO;}

		void initDepthTesting();	//Initialise and enable depth testing
		void initFaceCulling();		//Initialise and enable face culling

		//float calcFrustumScale(const float fovDeg) const;	//Calculate the frustum scale using fov in degrees
		glm::mat4 genPerspectiveMatrix(const os_float_t fovRadians, const os_int_t width, const os_int_t height);	//Generates a 16-component array and returns as a pointer
		void setFOV(os_float_t angleDegrees);					//Sets the fov of the perspective projection matrix
		void windowPosCallbackImpl(os_int_t x, os_int_t y);
		void framebufferSizeCallbackImpl(os_int_t width, os_int_t height);
		os_uint_t getUBOIDMatrices() const {return uboIDMatrices;}




		os_uint_t genVBO(const void * data, const os_sizeiptr_t sizeBytes, const GLenum hint) const;		//Creates a vertex buffer object
		os_uint_t genIBO(const os_ushort_t * data, const os_sizeiptr_t sizeArray, const GLenum hint) const;		//Creates an index buffer object
		os_uint_t genIBOINT(const os_uint_t * data, const os_sizeiptr_t sizeArray, const GLenum hint) const;	//Creates an index buffer object with u-ints rather than u-shorts
		os_uint_t genVAO(const std::vector<VertexAttrib> & vertexAttribs, os_uint_t IBO) const;					//Creates a vertex array object
		os_uint_t genUBO(os_sizeiptr_t sizeBytes) const;									//Creates a uniform buffer object capable of storing the given size (in bytes)

		os_uint_t createShader(GLenum eShaderType, const std::string & strShaderCode) const;
		os_uint_t createShaderProgram(const std::vector<os_uint_t> & shaders) const;
		void deleteShader(const os_uint_t shader) const;

		os_uint_t createTexture(const os_ubyte_t * img, const os_int_t width, const os_int_t height, os_bool_t loadAlpha) const;
		os_uint_t createTextureAtlas(std::vector<os_ubyte_t *> imgs, std::vector<os_int_t> widths, std::vector<os_int_t> heights,
			std::vector<glm::vec4> & texCoords, os_bool_t useEqualDimensions = false, os_int_t internalFormat = GL_SRGB_ALPHA, GLenum format = GL_RGBA);
		os_uint_t createCubemapTexture(std::vector<os_ubyte_t *> imgs, std::vector<os_int_t> widths, std::vector<os_int_t> heights) const;




		void updateSkinnedAnimations(osan::AnimationEngine * animationEngine);	//Send bones transforms to skinned mesh shaders
		void updateUserInterface(osi::UserInterface * ui);						//Update user interface shaders on resize of framebuffer
		void fullUpdateUserInterface(osi::UserInterface * ui);
		void fullUpdateUserInterfaceText(osi::UserInterface * ui);

		void updateUBOLighting();

		void uniform(os_uint_t func, os_uint_t loc, void * data) const;




		void setDepthTesting(os_bool_t enabled);	//Enable or disable depth testing
		void setFaceCulling(os_bool_t enabled);	//Enable or disable face culling
		void setDepthClamping(os_bool_t enabled);//Enable or disable depth clamping

		void setUseVsync(os_bool_t enabled);
		void setUseMIPMapping(os_bool_t enabled);
		void setUseTripleBuffering(os_bool_t enabled);
		void setAntiAliasingMode(AAMode mode);
		void setTextureFilteringMode(TexFilterMode mode);
		void setAnisotropicLevel(GLfloat level);
		//void setBrightness(float);
		//void setTextureQuality(Quality);
		//void setMeshQuality(Quality);

		//void setLightUBO(os_uint_t ubo) {lightUBO = ubo;}

		glm::mat4 getPerspectiveProjectionMatrix() const {return perspectiveProjection;}
		glm::mat4 getOrthographicProjectionMatrix() const {return orthographicProjection;}



		void threadsafe_postShouldUpdateUI(os_int_t startIndex = -1, os_int_t endIndex = -1) {shouldUpdateUI = true, updateUIStartIndex = startIndex, updateUIEndIndex = endIndex;}
		os_bool_t getShouldUpdateUI() const {return shouldUpdateUI;}
		void threadsafe_postShouldUpdateUBOLighting() {shouldUpdateUBOLighting = true;}
		os_bool_t getShouldUpdateUBOLighting() const {return shouldUpdateUBOLighting;}
		


		os_int_t getFramebufferWidth()  const {return framebufferWidth;}
		os_int_t getFramebufferHeight() const {return framebufferHeight;}


		//void RenderingEngine::renderText(osi::FontManager * fontManager, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

		os_uint_t gBufferFBO;
	private:
		os_uint_t gPos, gNormal, gColourSpec, gDepthRBO, framebufferVAO, framebufferVBO, framebufferProg;	//The frame buffer to render to
		os_uint_t updateGBufferFBO(os_int_t width, os_int_t height);													//Update the frame buffer object
		os_uint_t updateGDepthRBO(os_int_t width, os_int_t height);														//Update the depth render buffer object

		os_uint_t shadowMapFBO, shadowMap, shadowMapWidth, shadowMapHeight, shadowMapProg, shadowMapPos0, shadowMapPos1;

		os_uint_t uboIDMatrices;


		glm::mat4 perspectiveProjection, orthographicProjection, lightProjection;
		os_int_t framebufferWidth, framebufferHeight, framebufferX, framebufferY;
		os_float_t fovRadians;
		

		os_uint_t matUBO;	//Uniform buffer object containing material data
		void genMaterialUBO(std::vector<Material> materials);	//Generates a UBO for passing materials to shaders


		//Separate from shadergroup list so always rendered last -> performance optimisation
		osr::ShaderGroup skyboxShaderGroup;
		os_uint_t skyboxTexID;

		osr::ShaderGroup uiShaderGroup;
		os_uint_t uiTexID;							//ID of the texture atlas for the user interface

		osr::ShaderGroup textShaderGroup;
		os_uint_t textTexID;

		//Separate from shadergroup list so always rendered first
		//osr::ShaderGroup uiShaderGroup;


		std::vector<osr::RenderPass> renderPasses;		//List of render passes containing lists of shader groups
		//std::vector<osr::ShaderGroup> shaderGroups;	//List of shader groups (list of render objects grouped by shader)

		//TODO - Separate lighting lists into priority based lists
		std::vector<osr::DirectionLight> * dirLights;	//Ptr to resource manager list of all dynamic lights in the scene
		std::vector<osr::PointLight> * pointLights;		//Ptr to resource manager list of all dynamic lights in the scene
		std::vector<osr::SpotLight> * spotLights;		//Ptr to resource manager list of all dynamic lights in the scene
		os_uint_t lightUBO;								//Uniform buffer object containing all dynamic light data



		os_bool_t useVsync;					//Whether vsync is enabled or disabled
		os_bool_t useTripleBuffering;		//Use triple buffering if true, use double buffering if false

		os_bool_t useMIPMapping;				//Whether MIP mapping is enabled or disabled
		TexFilterMode texFilterMode;	//Texture Filtering Mode to be used when creating textures
		GLfloat anisotropicLevel;		//The max number of samples to be taken when using anisotropic filtering

		AAMode antiAliasingMode;		//Whether anti aliasing is enabled or disabled

		//float brightness;		//The brightness of light in the world
		//Quality textureQuality;	//The highest quality a texture can be
		//Quality meshQuality;	//The highest quality a mesh can be*/

		os_bool_t shouldUpdateUI;			//Whether to update the user interface shaders on the next update
		os_bool_t shouldUpdateUBOLighting;	//Whether to update the lighting values on the next update
		os_int_t updateUIStartIndex, updateUIEndIndex;	//The range to be updated
	};
}

#endif /* RENDERING_RENDERINGENGINE_H_ */
