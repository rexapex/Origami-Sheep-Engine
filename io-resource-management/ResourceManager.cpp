/*
 * ResourceManager.cpp
 *
 *  Created on: 9 Oct 2015
 *      Author: james_000
 */

#include "ResourceManager.h"

#include <assimp/material.h>
#include <assimp/material.inl>
#include <assimp/mesh.h>
#include <assimp/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <cstring>


namespace osrm
{
	ResourceManager::ResourceManager(const std::string & projectPath)
	{
		defaultSkyboxIndex = defaultUIIndex = 0;
		this->importer = new Assimp::Importer;

		if(endsWith(projectPath, "/"))
			this->projectPath = projectPath;
		else
			this->projectPath = projectPath + "/";

		std::cerr << "Project: " << this->projectPath << std::endl;



		//Load the default plane object as the first mesh
		//MeshFilter mfPlane;
		//mfPlane.mesh = new osrm::Mesh;			//TODO - DO SOMETHING HERE HELP ME PLS?!?!?!?!/!?
	}

	ResourceManager::~ResourceManager()
	{

	}





	GLuint ResourceManager::registerVertShader(const VertShaderFilter shader)
	{
		vertShadersToCreate.push_back(shader);
		return 0;
	}

	GLuint ResourceManager::registerFragShader(const FragShaderFilter shader)
	{
		fragShadersToCreate.push_back(shader);
		return 0;
	}

	GLuint ResourceManager::registerShaderProgram(const std::vector<os_uint_t> & shaders)
	{
		ShaderProgramFilter prog;
		///prog.vertShaders.push_back(vertShadersToCreate)
		return 0;
	}




	//Registers mesh filter as needed to be loaded
	//If the mesh filter has already been loaded, a pointer to the mesh filter is returned
	//Else, if the mesh filter is in the load queue, the calling entity will be added to the interest list
	//Else, the mesh filter will be added to the load queue
	GLuint ResourceManager::registerMesh(const std::string & meshFilterPath)
	{
		for(GLuint i = 0; i < loadedMeshes.size(); i++)
		{
			if(loadedMeshes[i].path == meshFilterPath)	//If the mesh filter requested has already been loaded
			{
				return 0;	//Then return as the mesh does not need to be added to load queue
			}
		}

		///std::cout << "Mesh Got Here" <<std::endl;

		//Mesh filter is neither loaded nor in the load queue
		//Therefore, add the mesh filter to the load queue
		MeshFilter mf;
		mf.path = meshFilterPath;
		mf.meshRenderers = std::vector<MeshRenderer>();
		mf.skinnedMeshRenderers = std::vector<MeshRenderer>();
		///std::cerr << "IDs Size: " << m.IDs.size() << std::endl;
		///m.meshFilter = new MeshFilter;
		///m.meshFilter->path = meshFilterPath;
		meshesToLoad.push_back(mf);

		return 0;
	}

	//Registers texture as needed to be loaded
	//If the texture has already been loaded, the OpenGL ID of the texture is returned
	//Else, if the texture is in the load queue, the calling entity will be added to the interest list
	//Else, the texture will be added to the load queue
	GLuint ResourceManager::registerTexture(const std::string & texturePath, GLuint * texWaitingAddress)
	{
		for(GLuint i = 0; i < texturesToLoad.size(); i++)
		{
			if(texturesToLoad[i].path == texturePath)	//If the texture requested has already been loaded
			{
				if(texWaitingAddress != nullptr)
					texturesToLoad[i].addressOfTexturesWaiting.push_back(texWaitingAddress);	//Add address to wait for creation of texture in OpenGL
				return 0;		//Return the OpenGL texture ID
			}
		}

		for(GLuint i = 0; i < loadedTextures.size(); i++)
		{
			if(loadedTextures[i].path == texturePath)	//If the texture requested has already been loaded
			{
				if(texWaitingAddress != nullptr)
					loadedTextures[i].addressOfTexturesWaiting.push_back(texWaitingAddress);	//Add address to wait for creation of texture in OpenGL
				return 0;		//Return the OpenGL texture ID
			}
		}

		//std::cerr << "Loading Texture: " << texturePath <<std::endl;

		//Texture is neither loaded nor in the load queue
		//Therefore, add the texture to the load queue
		TextureFilter tf(texturePath);
		if(texWaitingAddress != nullptr)
			tf.addressOfTexturesWaiting.push_back(texWaitingAddress);
		texturesToLoad.push_back(tf);

		//std::cerr << "Size Textures To Load: " << texturesToLoad.size() << std::endl;

		return 0;	//0 is a none valid texture ID
	}


	//Registers cubemap texture as needed to be loaded
	//Max number of paths = 6
	GLuint ResourceManager::registerCubemap(const std::vector<std::string> & paths, GLuint * texWaitingAddress)
	{
		for(GLuint i = 0; i < cubemapsToLoad.size(); i++)
		{
			if(cubemapsToLoad[i].paths == paths)	//If the texture requested has already been loaded
			{
				if(texWaitingAddress != nullptr)
					cubemapsToLoad[i].addressOfTexturesWaiting.push_back(texWaitingAddress);	//Add address to wait for creation of texture in OpenGL
				return 0;		//Return the OpenGL texture ID
			}
		}

		for(GLuint i = 0; i < loadedCubemaps.size(); i++)
		{
			if(loadedCubemaps[i].paths == paths)	//If the texture requested has already been loaded
			{
				if(texWaitingAddress != nullptr)
					loadedCubemaps[i].addressOfTexturesWaiting.push_back(texWaitingAddress);	//Add address to wait for creation of texture in OpenGL
				return 0;		//Return the OpenGL texture ID
			}
		}

		//Texture is neither loaded nor in the load queue
		//Therefore, add the texture to the load queue
		CubemapFilter cf;
		cf.paths = paths;
		if(cf.paths.size() > 6)
			cf.paths.resize(6);
		if(texWaitingAddress != nullptr)
			cf.addressOfTexturesWaiting.push_back(texWaitingAddress);
		this->cubemapsToLoad.push_back(cf);

		//std::cerr << "Size Textures To Load: " << texturesToLoad.size() << std::endl;

		return 0;	//0 is a none valid texture ID
	}








	GLuint ResourceManager::registerPlaneRenderer(Entity & entity, const std::string & name, GLuint texFilter, GLuint shaderProg)
	{
		/*MeshRenderer mr;
		mr.name = name;
		mr.eTransform = entity.transform;
		mr.shaderProgram = shaderProg;

		loadedMeshes[filter].meshRenderers.push_back(mr);
		entity.meshRenderers.push_back(loadedMeshes[filter].meshRenderers.back());
		*/
		return 0;
	}

	GLuint ResourceManager::registerMeshRenderer(Entity & entity, const std::string & name, GLuint filter, GLuint shaderProg, bool skinned)
	{
		MeshRenderer mr;
		mr.name = name;
		mr.eTransform = entity.transform;
		mr.shaderProgram = shaderProg;

		if(skinned)
		{
			loadedMeshes[filter].skinnedMeshRenderers.push_back(mr);
			entity.meshRenderers.push_back(loadedMeshes[filter].skinnedMeshRenderers.back());
		}
		else
		{
			loadedMeshes[filter].meshRenderers.push_back(mr);
			entity.meshRenderers.push_back(loadedMeshes[filter].meshRenderers.back());
		}

		return 0;
	}

	GLuint ResourceManager::registerSkybox(Entity & entity, GLuint cubemapIndex)
	{
		osr::Skybox skybox;
		skyboxes.push_back(skybox);
		entity.skyboxes.push_back(skyboxes.back());

		if(cubemapIndex < loadedCubemaps.size())
			loadedCubemaps[cubemapIndex].addressOfTexturesWaiting.push_back(&skyboxes[skyboxes.size()-1].cubemapID);
		else
			std::cerr << "Cubemap Index out of Range when Registering Skybox" << std::endl;

		return 0;
	}

	GLuint ResourceManager::registerAnimationController(Entity & entity, AnimationControllerFilter controller)
	{
		loadedMeshes[controller.meshIndex].skinnedAnimControllers.push_back(controller);
		entity.animControllers.push_back(loadedMeshes[controller.meshIndex].skinnedAnimControllers.back());	//Must be added this way so refers to correct object

		return 0;
	}

	GLuint ResourceManager::registerPointLight(Entity & entity, const std::string & name, const osr::PointLight & light)
	{
		pointLights.push_back(light);
		entity.pointLightIndexes.insert(std::make_pair(pointLights.size()-1, name));

		return 0;
	}

	GLuint ResourceManager::registerDirLight(Entity & entity, const std::string & name, const osr::DirectionLight & light)
	{
		dirLights.push_back(light);
		entity.dirLightIndexes.insert(std::make_pair(dirLights.size()-1, name));

		return 0;
	}

	GLuint ResourceManager::registerSpotLight(Entity & entity, const std::string & name, const osr::SpotLight & light)
	{
		spotLights.push_back(light);
		entity.spotLightIndexes.insert(std::make_pair(spotLights.size()-1, name));

		return 0;
	}













	void ResourceManager::loadAllResources()
	{
		std::cerr << "Loading All Resources!" << std::endl;

		//First load the default texture - required for models etc. with no textures
		texDefault.img = loadTexture("res/default_engine_res/default_tex.png", &texDefault.width, &texDefault.height);

		//Load meshes before textures as meshes can add more textures to load queue
		size_t numCycles = meshesToLoad.size();	//Cannot use meshesToLoad.size directly because value changes during loop
		for(size_t i = 0; i < numCycles; i++)
		{
			MeshFilter mf = meshesToLoad[0];
			meshesToLoad.pop_front();		//First in, first out - destroys the element in the list

			//if(mf.mesh == nullptr)			//Plane mesh already loaded, therefore, does not attempt another load
				mf.mesh = loadMesh(projectPath + mf.path, splitFilename(mf.path, 0), true);

			loadedMeshes.push_back(mf);
		}

		//std::cerr << "Size Textures To Load: " << texturesToLoad.size() << std::endl;
		numCycles = texturesToLoad.size();	//Cannot use texturesToLoad.size directly because value changes during loop
		for(size_t i = 0; i < numCycles; i++)
		{
			TextureFilter tf = texturesToLoad[0];
			texturesToLoad.pop_front();		//First in, first out

			tf.img = loadTexture(projectPath + tf.path, &tf.width, &tf.height);

			loadedTextures.push_back(tf);
		}

		numCycles = cubemapsToLoad.size();
		for(size_t i = 0; i < numCycles; i++)
		{
			CubemapFilter cf = cubemapsToLoad[0];
			cubemapsToLoad.pop_front();

			std::vector<std::string> correctedPaths;
			for(size_t j = 0; j < cf.paths.size(); j++)
			{
				correctedPaths.push_back(projectPath + cf.paths[j]);
			}

			loadCubemapTexture(correctedPaths, &cf.imgs, &cf.widths, &cf.heights);

			loadedCubemaps.push_back(cf);
		}

		numCycles = uis.size();
		for(size_t i = 0; i < numCycles; i++)
		{
			for(size_t j = 0; j < uis[i].textures.size(); j++)
			{
				TextureFilter & tf = uis[i].textures[j];
				
				tf.img = loadTexture(projectPath + tf.path, &tf.width, &tf.height);
			}
		}
	}











	//Loads the texture into unsigned char array and stores the width and height in pointers provided
	//Returns the unsigned char pointer containing an array of image data
	unsigned char * ResourceManager::loadTexture(const std::string & path, int * width, int * height) const
	{
		return SOIL_load_image(path.c_str(), width, height, 0, SOIL_LOAD_RGBA);
	}



	GLuint ResourceManager::loadCubemapTexture(std::vector<std::string> paths, std::vector<unsigned char *> * imgs,
											   std::vector<int> * widths, std::vector<int> * heights) const
	{
		int width, height;
		unsigned char * img;

		for(GLuint i = 0; i < (paths.size()) && (i < 6); i++)
		{
			img = SOIL_load_image(paths[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			imgs->push_back(img);
			widths->push_back(width);
			heights->push_back(height);
		}

		return 0;
	}








	//Dir string should be relative excluding project folder
	Mesh * ResourceManager::loadMesh(const std::string & path, const std::string & dir, bool shouldLoadBones)
	{
	//	if(*dir.rbegin() != '/')
	//		(*dir) += "/";

		///std::cerr << "Loading Mesh: Path: " << path << ", Dir: " << dir << std::endl;

		Assimp::Importer importer;
		const aiScene * scene = importer.ReadFile(path,
				aiProcess_CalcTangentSpace        |
				aiProcess_Triangulate             |
				aiProcess_JoinIdenticalVertices   |
				aiProcess_SortByPType			  |
				aiProcess_RemoveRedundantMaterials|
				aiProcess_FlipUVs);

		//If the import failed, report it
		if(!scene)
		{
			std::cerr << importer.GetErrorString() << std::endl;
			return nullptr;
		}


		osrm::Mesh * mesh = new osrm::Mesh;

		///std::cerr << "No. Meshes: " << scene->mNumMeshes << std::endl;

		unsigned int numVertices = 0;

		for(size_t n = 0; n < scene->mNumMeshes; ++n)
		{
			numVertices += scene->mMeshes[n]->mNumVertices;
		}

		///std::cerr << "No. Vertices: " << numVertices << std::endl;


		mesh->numVertices = numVertices;
		mesh->positions = (GLfloat *)malloc(numVertices * 3 * sizeof(GLfloat));
		mesh->normals  	= (GLfloat *)malloc(numVertices * 3 * sizeof(GLfloat));
		mesh->uvs      	= (GLfloat *)malloc(numVertices * 2 * sizeof(GLfloat));

		//For normal mapped meshes
		mesh->tangents	= (GLfloat *)malloc(numVertices * 3 * sizeof(GLfloat));
		mesh->bitangents= (GLfloat *)malloc(numVertices * 3 * sizeof(GLfloat));




		//Bone/Skinned Mesh Data
		GLuint * numBonesProcessed = new GLuint;
		(*numBonesProcessed) = 0;
		osan::VertexBoneData * boneData = new osan::VertexBoneData[numVertices];
		//std::cerr << "Mesh: " << path << ", Size Vertex Bone Data: " << numVertices << std::endl;;
		std::map<std::string, GLuint> vBoneMappings;
		std::vector<osan::Bone> vBoneInfo;
		std::map<std::string, GLuint> & boneMappings = vBoneMappings;
		std::vector<osan::Bone> & bones = vBoneInfo;





		GLuint offset = 0;
		for(size_t n = 0; n < scene->mNumMeshes; ++n)
		{
			MeshSection section;

			section.numFaces = scene->mMeshes[n]->mNumFaces;
			///std::cerr << "Num Faces: " << section.numFaces << std::endl;

			section.sizeBytesFaces = sizeof(GLuint) * scene->mMeshes[n]->mNumFaces * 3;
			section.faces = (GLuint *)malloc(section.sizeBytesFaces);
			unsigned int faceIndex = 0;

			for(size_t t = 0; t < scene->mMeshes[n]->mNumFaces; ++t)
			{
				const aiFace * face = &scene->mMeshes[n]->mFaces[t];

				face->mIndices[0] += offset;
				face->mIndices[1] += offset;
				face->mIndices[2] += offset;

				memcpy(&section.faces[faceIndex], face->mIndices, 3 * sizeof(GLuint));	//TODO - Use ushorts if possible, else use uints
				faceIndex += 3;
			}
			///std::cerr << "Faces Added to Section" << std::endl;

			if(scene->mMeshes[n]->HasPositions())
			{
				///std::cerr << "Position: " << offset << ", Size: " << mesh->sizeArrayVertices << std::endl;
				memcpy(mesh->positions + offset*3, scene->mMeshes[n]->mVertices, scene->mMeshes[n]->mNumVertices * 3 * sizeof(GLfloat));
			}
			if(scene->mMeshes[n]->HasNormals())
			{
				///std::cerr << "Normal: " << offset << ", Size: " << mesh->sizeArrayVertices << std::endl;
				memcpy(mesh->normals + offset*3, scene->mMeshes[n]->mNormals, scene->mMeshes[n]->mNumVertices * 3 * sizeof(GLfloat));
			}
			if(scene->mMeshes[n]->HasTextureCoords(0))
			{
				///std::cerr << "Tex Coord: " << offset << ", Size: " << mesh->sizeArrayVertices << std::endl;
				GLfloat * texCoords = (GLfloat *)malloc(scene->mMeshes[n]->mNumVertices * 2 * sizeof(GLfloat));
				for(unsigned int k = 0; k < scene->mMeshes[n]->mNumVertices; k++)
				{
					texCoords[k * 2]     = scene->mMeshes[n]->mTextureCoords[0][k].x;
					texCoords[k * 2 + 1] = scene->mMeshes[n]->mTextureCoords[0][k].y;
				}

				memcpy(mesh->uvs + offset*2, texCoords, scene->mMeshes[n]->mNumVertices * 2 * sizeof(GLfloat));
			}
			if(scene->mMeshes[n]->HasTangentsAndBitangents())
			{

			}

			mesh->meshSections.push_back(section);


			if(scene->mMeshes[n]->HasBones() && shouldLoadBones)	//Load the bones for this mesh
			{
				for(GLuint v = 0; v < scene->mMeshes[n]->mNumVertices; v++)
				{
					boneData[v + offset] = osan::VertexBoneData();
				}
				this->loadBones(offset, scene->mMeshes[n], boneData, boneMappings, bones, numBonesProcessed);
			}


			offset += scene->mMeshes[n]->mNumVertices;
		}
		delete numBonesProcessed;


		if(shouldLoadBones)	//If bones are being loaded then add them to the mesh
		{
			osan::SkinnedMesh * skinnedMesh = new osan::SkinnedMesh;
			std::vector<osan::SkeletalNode *> nodes;
			loadNodeHierarchy(scene->mRootNode, boneMappings, bones, nodes, -1);	//-1 siginifies no parent aka root node
			skinnedMesh->nodes = nodes;
			skinnedMesh->bones = bones;
			skinnedMesh->numVertexBoneData = numVertices;
			skinnedMesh->vertexBoneData = boneData;

			///std::cerr << "Set Mesh" << std::endl;
			mesh->skinnedMesh = skinnedMesh;

			this->loadAnimations(scene, mesh);
			///std::cerr << "Loaded Animations" << std::endl;
		}
		///std::cerr << "Done Loading Bones" << std::endl;



		///std::cerr << "Num Materials: " << scene->mNumMaterials << std::endl;

		for(unsigned m = 0; m < scene->mNumMaterials; m++)
		{
			int texIndex = 0;
			aiString path;  // filename

			aiColor3D ambient (0.5f, 0.5f, 0.5f), diffuse (0.5f, 0.5f, 0.5f), specular (0.5f, 0.5f, 0.5f);
			GLfloat shininessStrength, shininess;
			scene->mMaterials[m]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			scene->mMaterials[m]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			scene->mMaterials[m]->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			scene->mMaterials[m]->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
			scene->mMaterials[m]->Get(AI_MATKEY_SHININESS, shininess);

			mesh->meshSections[m].material.ambientColour = glm::vec3(ambient.r, ambient.g, ambient.b);
			mesh->meshSections[m].material.diffuseColour = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
			mesh->meshSections[m].material.specularColour = glm::vec3(specular.r, specular.g, specular.b);
			mesh->meshSections[m].material.specularShininess = shininess / 1000.0f;

			///std::cerr << "Ambient: " << ambient.r << ", " << ambient.g << ", " << ambient.b << std::endl;
			///std::cerr << "Diffuse: " << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << std::endl;
			///std::cerr << "Specular: " << specular.r << ", " << specular.g << ", " << specular.b << std::endl;
			///std::cerr << "Shininess: " << shininess / 128.0f << std::endl;

			aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
			///std::cerr << "Tex Found: " << texFound << std::endl;
			while(texFound == AI_SUCCESS)	//TODO - Register Mesh Textures but load Immediately - RegisterTexture fnc not suitable atm
			{
				registerTexture(dir + "/" + path.C_Str(), &mesh->meshSections[m].texID);	//Register texture & pass address where to store ID

				texIndex++;
				texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
			}

			if(texIndex == 0)
			{
				//No textures for mesh section, therefore, use default texture
				texDefault.addressOfTexturesWaiting.push_back(&mesh->meshSections[m].texID);
			}
		}

		///std::cerr << "Mesh Loaded: " << path << std::endl;
		///std::cerr << "Num Vertices: " << mesh->numVertices << std::endl;

		return mesh;
	}









	void ResourceManager::loadBones(GLuint meshVertexOffset, const aiMesh * mesh, osan::VertexBoneData * boneData,
			std::map<std::string, GLuint> & boneMappings, std::vector<osan::Bone> & bones, GLuint * numBonesProcessed)
	{
		//std::cerr << "Loading Bones" << std::endl;
		for(os_uint_t i = 0; i < mesh->mNumBones; i++)
		{
			os_uint_t boneIndex = 0;
			std::string boneName(mesh->mBones[i]->mName.data);
			//std::cerr << "Bone: " << boneName << std::endl;

			if(boneMappings.find(boneName) == boneMappings.end())	//If bone name has not been added to the map
			{
				//boneIndex = (*numBonesProcessed);
				//(*numBonesProcessed)++;
				osan::Bone bone;
				bones.push_back(bone);
				boneIndex = (os_uint_t)bones.size() - 1;
			} else
			{
				boneIndex = boneMappings[boneName];
			}

			boneMappings[boneName] = boneIndex;

			//std::cerr << "1 - Bone Name: " << boneName << ", Bone Index: " << boneIndex << std::endl;
			aiMatrix4x4 m = mesh->mBones[i]->mOffsetMatrix;
			//memcpy((void*)&boneInfo[boneIndex].offsetMatrix, (void*)&m, sizeof(glm::mat4));
			bones[boneIndex].offsetMatrix = glm::transpose(glm::make_mat4(&m.a1));


			for(os_uint_t j = 0; j < mesh->mBones[i]->mNumWeights; j++)
			{
				GLuint vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
				GLfloat weight = mesh->mBones[i]->mWeights[j].mWeight;
				//std::cerr << "Weight = " << weight << ", Vertex ID = " << vertexID << ", Bone Index = " << boneIndex << std::endl;
				//TODO - Bone data with more than 1 mesh section might need an offset
				addBoneData(&boneData[vertexID], boneIndex, weight);
			}
		}
	}

	//Recursive function used to set children in bone info struct - required for hierarchical updating of bones
	void ResourceManager::loadNodeHierarchy(const aiNode * node, std::map<std::string, GLuint> & boneMappings, std::vector<osan::Bone> & bones, std::vector<osan::SkeletalNode *> & nodes, int parent)
	{
		osan::SkeletalNode * skeleNode = new osan::SkeletalNode;
		std::string name(node->mName.data);
		skeleNode->name = name;
		skeleNode->parent = parent;

		//-1 will be max +ve integer since boneInfoIndex is unsigned
		skeleNode->boneIndex = (boneMappings.find(name) != boneMappings.end()) && (boneMappings[name] < bones.size()) ? boneMappings[name] : -1;
		//std::cerr << "2 - Skele Node Name: " << skeleNode->name << ", ID: " << skeleNode->boneIndex << std::endl;

		aiMatrix4x4 m = node->mTransformation;
		skeleNode->transform = glm::transpose(glm::make_mat4(&m.a1));	//Convert from assimp matrix to glm matrix

		//skeleNode->numChildren = node->mNumChildren;
		//if(node->mNumChildren > 0)
		//	skeleNode->children = new osan::SkeletalNode[node->mNumChildren];

		int nextParent = (int)nodes.size();
		nodes.push_back(skeleNode);

		for(os_uint_t i = 0; i < node->mNumChildren; i++)	//Load the node hierarchy for this nodes children
		{
			loadNodeHierarchy(node->mChildren[i], boneMappings, bones, nodes, nextParent);
			//skeleNode->children[i] = *childNode;
			//childNode->children = nullptr;		//Children will have already been deleted by recursive nature of function
			//delete childNode;
		}

		//return skeleNode;
	}


	void ResourceManager::loadAnimations(const aiScene * scene, osrm::Mesh * mesh)
	{
		osan::SkinnedMesh * skinnedMesh = mesh->skinnedMesh;

		//skinnedMesh->animations = new osan::SkeletalAnimation[scene->mNumAnimations];
		//skinnedMesh->numAnimations = scene->mNumAnimations;

		os_float_t previousDuration = 0;
		os_float_t previousTicksPerSecond = 0;

		for(size_t i = 0; i < scene->mNumAnimations; i++)	//Load each animation belonging to the scene
		{
			os_float_t ticksPerSecond = scene->mAnimations[i]->mTicksPerSecond != 0.0 ? (os_float_t)scene->mAnimations[i]->mTicksPerSecond : 25.0f;
			os_float_t duration = (os_float_t)scene->mAnimations[i]->mDuration;

			/*osan::SkeletalAnimation animation;
			animation.ticksPerSecond = ticksPerSecond;
			animation.duration = duration;
			animation.rootNode = new SkeletalNode;
			(*animation.rootNode) = (*mesh->rootNode);*/

			/*animation.boneAnims = new osan::BoneAnimation[scene->mAnimations[i]->mNumChannels];	//Construct array of nodes of exact length
			animation.numBoneAnims = scene->mAnimations[i]->mNumChannels;
			//animation.boneInfo = &mesh->boneInfo[0];	//Does not copy the data, just creates a pointer to it
			animation.boneInfo = new BoneInfo[mesh->boneInfo.size()];
			std::copy(mesh->boneInfo.begin(), mesh->boneInfo.end(), animation.boneInfo);	//Copy the bone info data from the mesh to the animation
			animation.sizeBoneInfo = mesh->boneInfo.size();*/

			for(size_t j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
			{
				osan::NodeAnimation nodeAnim;
				nodeAnim.name = std::string(scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str());	//Store the name of the node/bone

				nodeAnim.posKeys = new osan::KeyFrame[scene->mAnimations[i]->mChannels[j]->mNumPositionKeys];	//Construct array in struct of exact size
				nodeAnim.scaleKeys = new osan::KeyFrame[scene->mAnimations[i]->mChannels[j]->mNumScalingKeys];
				nodeAnim.orientKeys = new osan::KeyFrameOrient[scene->mAnimations[i]->mChannels[j]->mNumRotationKeys];
				nodeAnim.numPosKeys = scene->mAnimations[i]->mChannels[j]->mNumPositionKeys;			//Store the sizes of each array
				nodeAnim.numScaleKeys = scene->mAnimations[i]->mChannels[j]->mNumScalingKeys;
				nodeAnim.numOrientKeys = scene->mAnimations[i]->mChannels[j]->mNumRotationKeys;

				for(size_t k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumPositionKeys; k++)	//Load position keyframes of all bones
				{
					osan::KeyFrame frame;
					frame.time = (float)scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mTime;
					frame.value = glm::vec3(
							scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.x,
							scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.y,
							scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue.z);
					nodeAnim.posKeys[k] = frame;
				}
				for(size_t k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumScalingKeys; k++)	//Load scale keyframes for all bones
				{
					osan::KeyFrame frame;
					frame.time = (float)scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mTime;
					frame.value = glm::vec3(
							scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.x,
							scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.y,
							scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue.z);
					nodeAnim.scaleKeys[k] = frame;
				}
				for(size_t k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumRotationKeys; k++)	//Load orientation keyframes for all bones
				{
					osan::KeyFrameOrient frame;
					frame.time = (float)scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mTime;
					frame.orientation = glm::quat(
							scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.w,
							scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.x,
							scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.y,
							scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue.z);
					nodeAnim.orientKeys[k] = frame;
				}

				nodeAnim.ticksPerSecond = ticksPerSecond;
				nodeAnim.duration = duration;

				setNodeAnimation(skinnedMesh->nodes, nodeAnim, (os_int_t)i, previousDuration, previousTicksPerSecond);

				previousDuration = duration;
				previousTicksPerSecond = ticksPerSecond;
			}

			setBlankNodeAnimations(skinnedMesh->nodes, scene->mNumAnimations, previousDuration, previousTicksPerSecond);
		}
	}


	void ResourceManager::setNodeAnimation(std::vector<osan::SkeletalNode *> nodes, osan::NodeAnimation & nodeAnim, os_int_t currentAnimation, GLfloat duration, GLfloat ticksPerSecond)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(nodes[i]->name == nodeAnim.name)
			{
				nodes[i]->nodeAnims.push_back(nodeAnim);

				return;
			}
		}
	}

	//To be called after each set of keyframes has been set using setnodeanimation
	void ResourceManager::setBlankNodeAnimations(std::vector<osan::SkeletalNode *> nodes, int currentSizeNodeAnims, GLfloat duration, GLfloat ticksPerSecond)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(currentSizeNodeAnims > nodes[i]->nodeAnims.size())
			{
				osan::NodeAnimation blankNodeAnim;
				blankNodeAnim.duration = duration;
				blankNodeAnim.ticksPerSecond = ticksPerSecond;
				blankNodeAnim.posKeys = nullptr;

				nodes[i]->nodeAnims.push_back(blankNodeAnim);
			}
		}
	}








	//Split a path into it's directory and file name
	//If retVal = 0, returns the directory
	//Else, returns the name of the file
	std::string ResourceManager::splitFilename(const std::string & path, const int retVal)
	{
		size_t found;
		//cout << "Splitting: " << path << endl;
		found = path.find_last_of("/\\");
		//std::cerr << "Folder: " << path.substr(0,found) << std::endl;
		//std::cerr << "File: " << path.substr(found+1) << std::endl;

		return retVal == 0 ? path.substr(0, found) : path.substr(found + 1);
	}

	bool ResourceManager::endsWith(const std::string & str, const std::string & suffix)
	{
		return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
	}




} /* namespace osrm */




























