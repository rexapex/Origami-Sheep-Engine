/*
 * ResourceManager.h
 *
 *  Created on: 9 Oct 2015
 *      Author: james_000
 */

#ifndef IO_RESOURCE_MANAGEMENT_RESOURCEMANAGER_H_
#define IO_RESOURCE_MANAGEMENT_RESOURCEMANAGER_H_

#include <stdio.h>
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <algorithm>

#include "Mesh.h"
#include "SkinnedMesh.h"
#include "../animation/AnimationEngine.h"
#include "../rendering/RenderingEngine.h"
#include "../maths/Transform.h"

namespace osrm	//System used when editing and loading to prevent resources being loaded multiple times
{
	/*struct Shader			//Shaders are loaded from scenes header file
	{
		std::string path;	//Path of the shader
		GLenum type;		//Type of the shader
	};*/

	/*struct ShaderProgram	//Shader programs are loaded from scenes header file
	{
		std::string name;
		std::vector<Shader> shaders;
	};*/

	struct MeshRenderer
	{
		std::string name;
		GLuint shaderProgram;
		osm::Transform * eTransform;	//Pointer to the transform of the entity which is waiting for the mesh filter to load
	};

	struct TextureFilter	//Texture filters are loaded from scenes header file
	{
		std::string path;
		unsigned char * img;
		int width, height;
		std::vector<GLuint *> addressOfTexturesWaiting;	//Only required for texture filter because materials define their textures

		TextureFilter() : img{nullptr} {}

		TextureFilter(const std::string & texPath) : TextureFilter() {path = texPath;}

		//Deep Copy Constructor
		/*TextureFilter(const TextureFilter & filter)
		{
			path = filter.path;
			width = filter.width;
			height = filter.height;
			addressOfTexturesWaiting = filter.addressOfTexturesWaiting;

			if(filter.img != nullptr)
			{
				img = new unsigned char[width * height];
				memcpy(img, filter.img, width * height);
			}
			else img = nullptr;
		}

		~TextureFilter()	//Frees img using SOIL_free_image_data
		{
			if(img != nullptr)
				SOIL_free_image_data(img);
		}*/
	};

	struct CubemapFilter	//TODO - Implement registering and loading of cubemaps for skyboxes
	{
		std::vector<std::string> paths;
		std::vector<unsigned char *> imgs;
		std::vector<int> widths;
		std::vector<int> heights;
		std::vector<GLuint *> addressOfTexturesWaiting;	//Only required for texture filter because materials define their textures

		/*~CubemapFilter()	//Frees imgs using SOIL_free_image_data
		{
			for(size_t i = 0; i < imgs.size(); i++)
			{
				SOIL_free_image_data(imgs[i]);
			}
		}*/
	};

	struct AnimationConnectorFilter
	{
		std::string name, condition;
		os_int_t animationTo, animationFrom;
		os_int_t mask;
	};

	struct AnimationControllerFilter
	{
		std::string name;
		os_int_t meshIndex, skinnedMeshInstanceIndex, startAnimation;
		std::vector<AnimationConnectorFilter> connectors;
	};

	struct MeshFilter		//Mesh filters are loaded from scenes header file
	{
		std::string path;
		Mesh * mesh;
		std::vector<MeshRenderer> meshRenderers;
		std::vector<MeshRenderer> skinnedMeshRenderers;
		std::vector<AnimationControllerFilter> skinnedAnimControllers;
	};

	struct PlaneFilter
	{
		
	};

	struct UIComponentFilter
	{
		osi::UIComponent * comp;
		osi::UIPanel * parent;
	};

	struct UIFilter	//User interface filter, contains imgs needed for tex atlas and list of root components
	{
		std::vector<UIComponentFilter> comps;
		std::vector<TextureFilter> textures;
		GLuint texAtlasID;
		std::vector<glm::vec4> texCoords;

		UIFilter() : texAtlasID(0) {}
	};

	struct VertShaderFilter
	{
		os_uint_t pos, normals, texCoords, colours, maxNumBones;	//The number of values per vector for pos, normals, texCoords, colours
		os_bool_t instanced, shadows, skinned, normalMaping;
	};

	struct FragShaderFilter
	{
		os_uint_t pos, normals, texCoords, colours, maxNumBones;	//The number of values per vector for pos, normals, texCoords, colours
		os_bool_t deferred, shadows, highDynamicRange, normalMaping;
	};

	struct ShaderProgramFilter
	{
		std::vector<VertShaderFilter *> vertShaders;
		std::vector<FragShaderFilter *> fragShaders;
	};




	struct Entity	//Used for simplicity in editing
	{
		ID entityID;
		std::string name;
		osm::Transform * transform;

		std::vector<MeshRenderer> meshRenderers;			//filter & renderers are ptrs to ones created by resource manager
		std::vector<MeshRenderer> skinnedMeshRenderes;		//filter & renderers are ptrs to ones created by resource manager
		///std::vector<TextureFilter *> textureRenderers;	//For standalone textures, excludes mesh/material textures

		std::vector<AnimationControllerFilter> animControllers;

		//The indexes are mapped to their names within the entity
		std::map<size_t, std::string> dirLightIndexes;
		std::map<size_t, std::string> pointLightIndexes;
		std::map<size_t, std::string> spotLightIndexes;

		std::vector<osr::Skybox> skyboxes;
	};





	class ResourceManager
	{
	public:
		ResourceManager(const std::string & projectPath);
		virtual ~ResourceManager();

		unsigned char * loadTexture(const std::string & path, int * width, int * height) const;
		GLuint loadCubemapTexture(std::vector<std::string> paths, std::vector<unsigned char *> * imgs,
				   std::vector<int> * widths, std::vector<int> * heights) const;
		Mesh * loadMesh(const std::string & path, const std::string & dir, bool shouldLoadBones = false);
		//GLuint createShader(GLenum eShaderType, const std::string &strShaderFile);
		//GLuint createShaderProgram(const std::vector<GLuint> &shaders);
		void loadAllResources();

		std::vector<Entity> entities;

		std::vector<osi::Input> inputs;

		std::vector<UIFilter> uis;
		uint32_t defaultUIIndex;

		std::vector<osr::DirectionLight> dirLights;
		std::vector<osr::PointLight> pointLights;
		std::vector<osr::SpotLight> spotLights;

		std::vector<osr::Skybox> skyboxes;
		uint32_t defaultSkyboxIndex;

		//std::vector<Shader> loadedShaders;
		//std::vector<ShaderProgram> loadedShaderPrograms;
		std::vector<MeshFilter> loadedMeshes;
		std::vector<TextureFilter> loadedTextures;
		std::vector<CubemapFilter> loadedCubemaps;


		//Loading uses a queue, First item added to queue is first item loaded
		GLuint		 registerVertShader(const VertShaderFilter shader);
		GLuint		 registerFragShader(const FragShaderFilter shader);
		GLuint		 registerShaderProgram(const std::vector<os_uint_t> & shaders);
		GLuint		 registerMesh(const std::string & meshFilterPath);
		GLuint		 registerTexture(const std::string & texturePath, GLuint * texWaitingAddress);
		GLuint		 registerCubemap(const std::vector<std::string> & paths, GLuint * texWaitingAddress);

		GLuint		 registerSkybox(Entity & entity, GLuint cubemapIndex);
		GLuint		 registerPlaneRenderer(Entity & entity, const std::string & name, GLuint texFilter, GLuint shaderProg);
		GLuint 		 registerMeshRenderer(Entity & entity, const std::string & name, GLuint filter, GLuint shaderProg, bool skinned = false);
		GLuint		 registerAnimationController(Entity & entity, AnimationControllerFilter controller);
		GLuint		 registerPointLight(Entity & entity, const std::string & name, const osr::PointLight & light);
		GLuint		 registerDirLight(Entity & entity, const std::string & name,   const osr::DirectionLight & light);
		GLuint		 registerSpotLight(Entity & entity, const std::string & name,  const osr::SpotLight & light);

		const std::string & getProjectPath() const {return projectPath;}
	private:
		std::string projectPath;
		Assimp::Importer * importer;

		TextureFilter texDefault;	//The default texture - Mapped to meshes etc. when no textures provided

		std::deque<MeshFilter> meshesToLoad;
		std::deque<TextureFilter> texturesToLoad;
		std::deque<CubemapFilter> cubemapsToLoad;

		std::deque<VertShaderFilter> vertShadersToCreate;
		std::deque<FragShaderFilter> fragShadersToCreate;
		std::deque<ShaderProgramFilter> shaderProgramsToCreate;

		void loadBones(GLuint meshVertexOffset, const aiMesh * mesh, osan::VertexBoneData * boneData,
			std::map<std::string, GLuint> & boneMappings, std::vector<osan::Bone> & bones, GLuint * numBonesProcessed);
		void loadNodeHierarchy(const aiNode * node, std::map<std::string, GLuint> & boneMappings, std::vector<osan::Bone> & bones, std::vector<osan::SkeletalNode *> & nodes, int parent);
		void loadAnimations(const aiScene * scene, osrm::Mesh * mesh);
		void setNodeAnimation(std::vector<osan::SkeletalNode *> nodes, osan::NodeAnimation & nodeAnim, os_int_t currentAnimation, GLfloat duration, GLfloat ticksPerSecond);
		void setBlankNodeAnimations(std::vector<osan::SkeletalNode *> nodes, int currentSizeNodeAnims, GLfloat duration, GLfloat ticksPerSecond);

		//Utility function for splitting path into filename and directory
		std::string splitFilename(const std::string & path, const int retVal);
		bool endsWith(const std::string & str, const std::string & suffix);
	};
} /* namespace osrm */

#endif /* IO_RESOURCE_MANAGEMENT_RESOURCEMANAGER_H_ */
