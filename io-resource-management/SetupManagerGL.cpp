#include "SetupManagerGL.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


namespace osrm
{
	SetupManagerGL::SetupManagerGL(osr::RenderingEngine * rEngine, osan::AnimationEngine * aEngine) : renderingEngine(rEngine), animationEngine(aEngine)
	{

	}


	SetupManagerGL::~SetupManagerGL()
	{

	}




	


	const OSError SetupManagerGL::setupStaticMeshRenderers(std::vector<osrm::MeshFilter> loadedMeshes, std::vector<osr::RenderObject> & staticRenderObjects)
	{
		for(size_t m = 0; m < loadedMeshes.size(); m++)
		{
			osrm::Mesh * mesh = loadedMeshes[m].mesh;

			if(loadedMeshes[m].meshRenderers.size() > 0 && loadedMeshes[m].mesh != nullptr)
			{
				osr::RenderObject renderObject;
				renderObject.numInstances = loadedMeshes[m].meshRenderers.size();

				//Transform Instance VBO
				os_uint_t dataSize = loadedMeshes[m].meshRenderers.size() * 16;
				GLfloat * instanceVBOData = new GLfloat[dataSize];
				for(size_t i = 0; i < loadedMeshes[m].meshRenderers.size(); i++)
				{
					//sizeof(glm::vec4) as 64 bytes in but array is array of GLfloats so will automatically multiply by 4
					renderObject.transforms.push_back(&loadedMeshes[m].meshRenderers[i].eTransform->modelMatrix);
					memcpy(instanceVBOData + i * 16, &loadedMeshes[m].meshRenderers[i].eTransform->modelMatrix, sizeof(glm::mat4));
				}
				renderObject.camSpaceTransforms.resize(loadedMeshes[m].meshRenderers.size());	//Ensure camSpaceTransforms is same size as transforms

				GLuint instanceVBO = renderingEngine->genVBO(instanceVBOData, dataSize*sizeof(GLfloat), GL_DYNAMIC_DRAW);
				renderObject.instanceVBO = instanceVBO;

				delete[] instanceVBOData;





				dataSize = mesh->numVertices * 8;	//3 * Vertices, 3 * Normals, 2 * UV Coords

				GLfloat * vboData = new GLfloat[dataSize];	//Create an interleaved array of mesh data

				for(size_t i = 0, j = 0, k = 0; j < mesh->numVertices * 3; i += 8, j += 3, k += 2)
				{
					vboData[i] 	   = mesh->positions[j];
					vboData[i + 1] = mesh->positions[j + 1];
					vboData[i + 2] = mesh->positions[j + 2];
					vboData[i + 3] = mesh->normals[j];
					vboData[i + 4] = mesh->normals[j + 1];
					vboData[i + 5] = mesh->normals[j + 2];
					vboData[i + 6] = mesh->uvs[k];
					vboData[i + 7] = mesh->uvs[k + 1];
				}

				GLuint vbo = renderingEngine->genVBO(vboData, dataSize*sizeof(GLfloat), GL_STATIC_DRAW);
				delete[] vboData;

				//Setup the render object
				renderObject.numVerticesPerRenderCall = new GLuint[mesh->meshSections.size()];
				renderObject.sizeNumVerticesPerRenderCall = renderObject.sizeNumTexturesPerRenderCall = mesh->meshSections.size();
				renderObject.materials = new osr::Material[mesh->meshSections.size()];
				renderObject.textures = new osr::Texture[mesh->meshSections.size()];
				renderObject.sizeTextures = mesh->meshSections.size();
				renderObject.sizeMaterials = mesh->meshSections.size();
				renderObject.numTexturesPerRenderCall = new os_uint_t[mesh->meshSections.size()];

				//Setup the number of textures bound for every render call
				for(size_t t = 0; t < renderObject.sizeNumTexturesPerRenderCall; t++)
				{
					renderObject.numTexturesPerRenderCall[t] = 1;
				}



				dataSize = 0;
				for(size_t i = 0; i < mesh->meshSections.size(); i++)	//Determine size of IBO data array
				{
					unsigned int numVertices = mesh->meshSections[i].numFaces * 3;	//Number of Faces * 3 Vertices per Face
					dataSize += numVertices;
					renderObject.numVerticesPerRenderCall[i] = numVertices;
					renderObject.materials[i] = mesh->meshSections[i].material;
					renderObject.textures[i] = osr::Texture{mesh->meshSections[i].texID, GL_TEXTURE_2D};
				}

				GLuint * iboData = new GLuint[dataSize];

				unsigned offset = 0;
				for(size_t i = 0; i < mesh->meshSections.size(); i++)	//Copy ibo data from all mesh sections of mesh structure to one array
				{
					memcpy(iboData + offset, mesh->meshSections[i].faces, mesh->meshSections[i].numFaces * 3 * sizeof(GLuint));
					offset += mesh->meshSections[i].numFaces * 3;
				}

				GLuint ibo = renderingEngine->genIBOINT(iboData, dataSize, GL_STATIC_DRAW);
				delete[] iboData;

				renderObject.VBOs.push_back(vbo);
				renderObject.IBO = ibo;
				renderObject.renderPrimitive = GL_TRIANGLES;

				staticRenderObjects.push_back(renderObject);
			}
		}

		return OSError::NONE;
	}






	//Fills skinned meshes reference with pointers to skinned meshes - Indexes into render objects and skinned meshes match for the same object
	const OSError SetupManagerGL::setupSkinnedMeshRenderers(std::vector<osrm::MeshFilter> loadedMeshes, std::vector<osan::SkinnedMesh *> & skinnedMeshes, std::vector<osr::RenderObject> & skinnedRenderObjects)
	{
		size_t boneOffset = 0;

		for(size_t m = 0; m < loadedMeshes.size(); m++)
		{
			osrm::Mesh * mesh = loadedMeshes[m].mesh;

			if(loadedMeshes[m].skinnedMeshRenderers.size() > 0 && loadedMeshes[m].mesh != nullptr && loadedMeshes[m].mesh->skinnedMesh != nullptr)
			{
				osr::RenderObject renderObject;
				renderObject.numInstances = loadedMeshes[m].skinnedMeshRenderers.size();

				//Transform Instance VBO
				size_t dataSize = loadedMeshes[m].skinnedMeshRenderers.size() * 16;
				GLfloat * instanceVBOData = new GLfloat[dataSize];
				for(size_t i = 0; i < loadedMeshes[m].skinnedMeshRenderers.size(); i++)
				{
					//sizeof(glm::vec4) as 64 bytes in but array is array of GLfloats so will automatically multiply by 4
					renderObject.transforms.push_back(&loadedMeshes[m].skinnedMeshRenderers[i].eTransform->modelMatrix);
					memcpy(instanceVBOData + i * 16, &loadedMeshes[m].skinnedMeshRenderers[i].eTransform->modelMatrix, sizeof(glm::mat4));
				}
				renderObject.camSpaceTransforms.resize(loadedMeshes[m].skinnedMeshRenderers.size());	//Ensure camSpaceTransforms is same size as transforms

				GLuint instanceVBO = renderingEngine->genVBO(instanceVBOData, (GLsizeiptr)dataSize*sizeof(GLfloat), GL_DYNAMIC_DRAW);
				renderObject.instanceVBO = instanceVBO;

				delete[] instanceVBOData;




				dataSize = mesh->numVertices * 16;	//3 * Positions, 3 * Normals, 2 * UV Coords, 4 Bone IDs, 4 Bone Weights

				GLfloat * vboData = new GLfloat[dataSize];	//Create an interleaved array of mesh data
															//GLvoid * vboData = calloc(dataSize, sizeof(GLfloat));

				for(size_t i = 0, j = 0, k = 0, p = 0; j < mesh->numVertices * 3; i += 16, j += 3, k += 2, p += 1)
				{
					vboData[i] 	   = mesh->positions[j];
					vboData[i + 1] = mesh->positions[j + 1];
					vboData[i + 2] = mesh->positions[j + 2];

					vboData[i + 3] = mesh->normals[j];
					vboData[i + 4] = mesh->normals[j + 1];
					vboData[i + 5] = mesh->normals[j + 2];

					vboData[i + 6] = mesh->uvs[k];
					vboData[i + 7] = mesh->uvs[k + 1];

					vboData[i + 8]  = (GLfloat)mesh->skinnedMesh->vertexBoneData[p].IDs[0];
					vboData[i + 9]  = (GLfloat)mesh->skinnedMesh->vertexBoneData[p].IDs[1];
					vboData[i + 10] = (GLfloat)mesh->skinnedMesh->vertexBoneData[p].IDs[2];
					vboData[i + 11] = (GLfloat)mesh->skinnedMesh->vertexBoneData[p].IDs[3];

					vboData[i + 12] = mesh->skinnedMesh->vertexBoneData[p].weights[0];
					vboData[i + 13] = mesh->skinnedMesh->vertexBoneData[p].weights[1];
					vboData[i + 14] = mesh->skinnedMesh->vertexBoneData[p].weights[2];
					vboData[i + 15] = mesh->skinnedMesh->vertexBoneData[p].weights[3];
				}

				GLuint vbo = renderingEngine->genVBO(vboData, dataSize*sizeof(GLfloat), GL_STATIC_DRAW);
				delete[] vboData;




				//Create a VBO containing only bone offsets
				dataSize = loadedMeshes[m].skinnedMeshRenderers.size();
				GLuint * vboInstanceBoneOffsetData = new GLuint[dataSize];
				for(size_t i = 0; i < loadedMeshes[m].skinnedMeshRenderers.size(); i++)
				{
					vboInstanceBoneOffsetData[i] = i * loadedMeshes[m].mesh->skinnedMesh->bones.size();
				}
				GLuint vboInstanceBoneOffset = renderingEngine->genVBO(vboInstanceBoneOffsetData, dataSize*sizeof(GLint), GL_STATIC_DRAW);
				delete[] vboInstanceBoneOffsetData;



				//Setup the render object
				renderObject.numVerticesPerRenderCall = new GLuint[mesh->meshSections.size()];
				renderObject.sizeNumVerticesPerRenderCall = renderObject.sizeNumTexturesPerRenderCall = mesh->meshSections.size();
				renderObject.materials = new osr::Material[mesh->meshSections.size()];
				renderObject.textures = new osr::Texture[mesh->meshSections.size()];
				renderObject.sizeTextures = mesh->meshSections.size();
				renderObject.sizeMaterials = mesh->meshSections.size();
				renderObject.numTexturesPerRenderCall = new os_uint_t[mesh->meshSections.size()];

				//Setup the number of textures bound for every render call
				for(size_t t = 0; t < renderObject.sizeNumTexturesPerRenderCall; t++)
				{
					renderObject.numTexturesPerRenderCall[t] = 1;
				}



				dataSize = 0;
				for(size_t i = 0; i < mesh->meshSections.size(); i++)	//Determine size of IBO data array
				{
					unsigned int numVertices = mesh->meshSections[i].numFaces * 3;	//Number of Faces * 3 Vertices per Face
					dataSize += numVertices;
					renderObject.numVerticesPerRenderCall[i] = numVertices;
					renderObject.materials[i] = mesh->meshSections[i].material;
					renderObject.textures[i] = osr::Texture {mesh->meshSections[i].texID, GL_TEXTURE_2D};
				}

				//Create the IBO
				GLuint * iboData = new GLuint[dataSize];
				unsigned offset = 0;
				for(size_t i = 0; i < mesh->meshSections.size(); i++)	//Copy ibo data from all mesh sections of mesh structure to one array
				{
					memcpy(iboData + offset, mesh->meshSections[i].faces, mesh->meshSections[i].numFaces * 3 * sizeof(GLuint));
					offset += mesh->meshSections[i].numFaces * 3;
				}

				GLuint ibo = renderingEngine->genIBOINT(iboData, dataSize, GL_STATIC_DRAW);
				delete[] iboData;

				renderObject.VBOs.push_back(vbo);
				renderObject.VBOs.push_back(vboInstanceBoneOffset);
				renderObject.IBO = ibo;
				renderObject.renderPrimitive = GL_TRIANGLES;


				//Setup the animations relating to the skinned mesh
				for(size_t i = 0; i < loadedMeshes[m].skinnedAnimControllers.size(); i++)
				{
					osan::AnimationController animController(*mesh->skinnedMesh, loadedMeshes[m].skinnedAnimControllers[i].skinnedMeshInstanceIndex);
					for(size_t j = 0; j < loadedMeshes[m].skinnedAnimControllers[i].connectors.size(); j++)
					{
						AnimationConnectorFilter & connector = loadedMeshes[m].skinnedAnimControllers[i].connectors[j];
						std::bitset<32> mask(connector.mask);
						animController.addConnector(connector.animationFrom, connector.animationTo, connector.condition, mask);
					}
					animationEngine->addAnimationController(animController);
					animationEngine->setAnimation(animationEngine->getAnimationControllers().size()-1, loadedMeshes[m].skinnedAnimControllers[i].startAnimation, 0, std::bitset<32>(1));
				}


				loadedMeshes[m].mesh->skinnedMesh->totalBoneOffset = boneOffset;
				boneOffset += loadedMeshes[m].mesh->skinnedMesh->bones.size();
				skinnedMeshes.push_back(loadedMeshes[m].mesh->skinnedMesh);
				skinnedRenderObjects.push_back(renderObject);
			}
		}

		return OSError::NONE;
	}



	const OSError SetupManagerGL::setupStaticPlaneRenderers(std::vector<osr::RenderObject> & outRenderObjects)
	{
	/*	osr::RenderObject renderObject;
	//	renderObject.numInstances = loadedMeshes[m].meshRenderers.size();

		//Transform Instance VBO
		os_uint_t dataSize = loadedMeshes[m].meshRenderers.size() * 16;
		GLfloat * instanceVBOData = new GLfloat[dataSize];
		for(size_t i = 0; i < loadedMeshes[m].meshRenderers.size(); i++)
		{
			//sizeof(glm::vec4) as 64 bytes in but array is array of GLfloats so will automatically multiply by 4
			renderObject.transforms.push_back(&loadedMeshes[m].meshRenderers[i].eTransform->modelMatrix);
			memcpy(instanceVBOData + i * 16, &loadedMeshes[m].meshRenderers[i].eTransform->modelMatrix, sizeof(glm::mat4));
		}
		renderObject.camSpaceTransforms.resize(loadedMeshes[m].meshRenderers.size());	//Ensure camSpaceTransforms is same size as transforms

		GLuint instanceVBO = renderingEngine->genVBO(instanceVBOData, dataSize*sizeof(GLfloat), GL_DYNAMIC_DRAW);
		renderObject.instanceVBO = instanceVBO;

		delete[] instanceVBOData;





		dataSize = 4 * 8;	//3 * Vertices, 3 * Normals, 2 * UV Coords

		GLfloat * vboData = new GLfloat[dataSize];	//Create an interleaved array of mesh data

		for(size_t i = 0, j = 0, k = 0; j < mesh->numVertices * 3; i += 8, j += 3, k += 2)
		{
			vboData[i] 	   = mesh->positions[j];
			vboData[i + 1] = mesh->positions[j + 1];
			vboData[i + 2] = mesh->positions[j + 2];
			vboData[i + 3] = mesh->normals[j];
			vboData[i + 4] = mesh->normals[j + 1];
			vboData[i + 5] = mesh->normals[j + 2];
			vboData[i + 6] = mesh->uvs[k];
			vboData[i + 7] = mesh->uvs[k + 1];
		}

		GLuint vbo = renderingEngine->genVBO(vboData, dataSize*sizeof(GLfloat), GL_STATIC_DRAW);
		delete[] vboData;

		//Setup the render object
		renderObject.numVerticesPerRenderCall = new GLuint[mesh->meshSections.size()];
		renderObject.sizeNumVerticesPerRenderCall = renderObject.sizeNumTexturesPerRenderCall = mesh->meshSections.size();
		renderObject.materials = new osr::Material[mesh->meshSections.size()];
		renderObject.textures = new osr::Texture[mesh->meshSections.size()];
		renderObject.sizeTextures = mesh->meshSections.size();
		renderObject.sizeMaterials = mesh->meshSections.size();
		renderObject.numTexturesPerRenderCall = new os_uint_t[mesh->meshSections.size()];

		//Setup the number of textures bound for every render call
		for(size_t t = 0; t < renderObject.sizeNumTexturesPerRenderCall; t++)
		{
			renderObject.numTexturesPerRenderCall[t] = 1;
		}



		dataSize = 0;
		for(size_t i = 0; i < mesh->meshSections.size(); i++)	//Determine size of IBO data array
		{
			unsigned int numVertices = mesh->meshSections[i].numFaces * 3;	//Number of Faces * 3 Vertices per Face
			dataSize += numVertices;
			renderObject.numVerticesPerRenderCall[i] = numVertices;
			renderObject.materials[i] = mesh->meshSections[i].material;
			renderObject.textures[i] = osr::Texture{mesh->meshSections[i].texID, GL_TEXTURE_2D};
		}

		GLuint * iboData = new GLuint[dataSize];

		unsigned offset = 0;
		for(size_t i = 0; i < mesh->meshSections.size(); i++)	//Copy ibo data from all mesh sections of mesh structure to one array
		{
			memcpy(iboData + offset, mesh->meshSections[i].faces, mesh->meshSections[i].numFaces * 3 * sizeof(GLuint));
			offset += mesh->meshSections[i].numFaces * 3;
		}

		GLuint ibo = renderingEngine->genIBOINT(iboData, dataSize, GL_STATIC_DRAW);
		delete[] iboData;

		renderObject.VBOs.push_back(vbo);
		renderObject.IBO = ibo;
		renderObject.renderPrimitive = GL_TRIANGLES;

		staticRenderObjects.push_back(renderObject);
		*/
		return OSError::NONE;
	}






	osr::RenderObject SetupManagerGL::setupSkybox(osr::Skybox skybox)
	{
		//osrm::Mesh * cubeMesh = resourceManager->loadMesh("res/cube.obj", false);
		//unsigned dataSize = cubeMesh->numVertices * 3;
		size_t dataSize;
		//std::cerr << "Cube Mesh No. Vertices: " << cubeMesh->numVertices << std::endl;
		GLfloat vboData[] =
		{
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};
		dataSize = 108;
		GLuint vbo = renderingEngine->genVBO(vboData, dataSize*sizeof(GLfloat), GL_STATIC_DRAW);

		osr::RenderObject skyboxRenderObject;
		skyboxRenderObject.numVerticesPerRenderCall = new GLuint[1];
		skyboxRenderObject.numVerticesPerRenderCall[0] = dataSize/3;
		skyboxRenderObject.IBO = 0;
		skyboxRenderObject.VBOs.push_back(vbo);
		skyboxRenderObject.textures = new osr::Texture[1];
		skyboxRenderObject.sizeTextures = 1;
		skyboxRenderObject.numTexturesPerRenderCall = new os_uint_t[1];
		skyboxRenderObject.sizeNumTexturesPerRenderCall = 1;
		skyboxRenderObject.numTexturesPerRenderCall[0] = 1;
		skyboxRenderObject.textures[0] = osr::Texture {skybox.cubemapID, GL_TEXTURE_CUBE_MAP};
		skyboxRenderObject.renderPrimitive = GL_TRIANGLES;

		return skyboxRenderObject;
	}





	osr::RenderObject SetupManagerGL::setupUI(const osrm::UIFilter & uiFilter, osi::UserInterface * userInterface, osr::RenderObject & textRenderObject)
	{
		for(size_t i = 0; i < uiFilter.comps.size(); i++)	//Add all components to the user interface
		{
			userInterface->addComponent(uiFilter.comps[i].parent, uiFilter.comps[i].comp);
		}

		userInterface->calcComponentsScreenCoords();

		size_t numComps = userInterface->getNumComponents();
		os_uint_t numChars = static_cast<os_uint_t>(userInterface->getNumTextChars());
		GLuint dataSize = 16 * numComps;
		GLfloat * vboData = new GLfloat[dataSize];
		GLfloat * vboTextData = new GLfloat[numChars * 16];

		///std::cerr << "NUM CHARS: " << numChars <<"\n\n";

		userInterface->texCoords = uiFilter.texCoords;

		/*for(size_t i = 0, j = 0, n = 0; i < numComps; i++)
		{
			osi::UIComponent * comp = userInterface->getComponent(i);

			std::vector<glm::vec4> coords {comp->getComponentScreenCoords()};
			///std::vector<glm::vec4> coords = comp->getComponentScreenCoords(0, 0, userInterface->getFramebufferWidth(), userInterface->getFramebufferHeight());
			
			for(size_t k = 0; k < coords.size(); k++, j++)	//coords cotains coordinates of components and all sub-components
			{
				//Two position coordinates followed by two texture co-ordinates
				int t = comp->getCurrentTex() < uiFilter.texCoords.size() ? comp->getCurrentTex() : 0;	//Determine which set of texture co-ordinates to use

				vboData[j * 16]      = coords[k].x;
				vboData[j * 16 + 1]  = coords[k].y;
				vboData[j * 16 + 2]  = uiFilter.texCoords[t][0];	//0, 1
				vboData[j * 16 + 3]  = uiFilter.texCoords[t][3];

				vboData[j * 16 + 4]  = coords[k].x + coords[k].z;
				vboData[j * 16 + 5]  = coords[k].y;
				vboData[j * 16 + 6]  = uiFilter.texCoords[t][1];	//1, 1
				vboData[j * 16 + 7]  = uiFilter.texCoords[t][3];

				vboData[j * 16 + 8]  = coords[k].x + coords[k].z;
				vboData[j * 16 + 9]  = coords[k].y + coords[k].w;
				vboData[j * 16 + 10] = uiFilter.texCoords[t][1];	//1, 0
				vboData[j * 16 + 11] = uiFilter.texCoords[t][2];

				vboData[j * 16 + 12] = coords[k].x;
				vboData[j * 16 + 13] = coords[k].y + coords[k].w;
				vboData[j * 16 + 14] = uiFilter.texCoords[t][0];	//0, 0
				vboData[j * 16 + 15] = uiFilter.texCoords[t][2];





				const std::string & text = comp->getText();

				if(text == "") continue;

				osi::Font & font = userInterface->getFontManager()->getFont(0);

				GLfloat scale = 0.28f;
				GLfloat sumWidths = 0, maxHeight = 0;
				for(std::string::const_iterator c = text.begin(); c != text.end(); ++c)
				{
					osi::Character & ch = font.chars[*c];

					sumWidths += (GLfloat)(ch.size.x + ch.bearing.x);//* scale;
					maxHeight = std::max(maxHeight, (GLfloat)(ch.size.y));//* scale);
				}

				osi::Origin orig = comp->getTextOrigin();
				GLfloat x = 0.0f, y = 0.0f;
				switch(orig)	//TODO - Add remainder of origins
				{
				case osi::Origin::LEFT_CENTREY:
					x = coords[k].x - font.chars[*text.begin()].bearing.x * scale;
					y = coords[k].y + coords[k].w/2 - maxHeight/2;
					break;
				case osi::Origin::CENTREX_CENTREY:
					x = coords[k].x + coords[k].z/2 - sumWidths/2 - font.chars[*text.begin()].bearing.x * scale;
					y = coords[k].y + coords[k].w/2 - maxHeight/2;
					break;
				default:
					x = coords[k].x + coords[k].z/2 - sumWidths/2 - font.chars[*text.begin()].bearing.x * scale;
					y = coords[k].y + coords[k].w/2 - maxHeight/2;
					break;
				}

				//Iterate through all characters
				std::string::const_iterator c;
				for(c = text.begin(); c != text.end(); c++)
				{
					osi::Character & ch = font.chars[*c];

					GLfloat scale = 1.0f;

					GLfloat xpos = x + (GLfloat)(ch.bearing.x * scale);
					GLfloat ypos = y - (GLfloat)((ch.size.y - ch.bearing.y) * scale);

					GLfloat w = (GLfloat)ch.size.x * scale;
					GLfloat h = (GLfloat)ch.size.y * scale;

					///std::cerr << "CHAR: " << (*c) << std::endl;
					///std::cerr << "x = " << xpos << ", y = " << ypos << ", w = " << w << ", h = " << h << std::endl;
					///std::cerr << "Tex: " << font.texCoords[(*c)*2][0] << ", " << font.texCoords[(*c)*2][1] << std::endl << std::endl;

					//Update VBO for each character
					vboTextData[n * 16]       = xpos;
					vboTextData[n * 16 + 1]   = ypos;
					vboTextData[n * 16 + 2]   = font.texCoords[(*c)][0];
					vboTextData[n * 16 + 3]   = font.texCoords[(*c)][3];	//0, 1

					vboTextData[n * 16 + 4]   = xpos + w;
					vboTextData[n * 16 + 5]   = ypos;
					vboTextData[n * 16 + 6]   = font.texCoords[(*c)][1];
					vboTextData[n * 16 + 7]   = font.texCoords[(*c)][3];	//1, 1

					vboTextData[n * 16 + 8]   = xpos + w;
					vboTextData[n * 16 + 9]   = ypos + h;
					vboTextData[n * 16 + 10]  = font.texCoords[(*c)][1];
					vboTextData[n * 16 + 11]  = font.texCoords[(*c)][2];	//1, 0

					vboTextData[n * 16 + 12]  = xpos;
					vboTextData[n * 16 + 13]  = ypos + h;
					vboTextData[n * 16 + 14]  = font.texCoords[(*c)][0];
					vboTextData[n * 16 + 15]  = font.texCoords[(*c)][2];	//0, 0

					n++;	//Increment total character count

					//Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
					x += (ch.advance.x >> 6) * scale; //Bitshift by 6 to get value in pixels (2^6 = 64)
					///y += (ch.advance.y >> 6);
				}
			}
		}*/

		GLuint vboID = renderingEngine->genVBO(vboData, dataSize * sizeof(GLfloat), GL_STATIC_DRAW);
		delete vboData;

		GLuint vboIDText = renderingEngine->genVBO(vboTextData, numChars * 16 * sizeof(GLfloat), GL_STATIC_DRAW);
		delete vboTextData;

		if(textRenderObject.numVerticesPerRenderCall == nullptr)	delete[] textRenderObject.numVerticesPerRenderCall;
		textRenderObject.numVerticesPerRenderCall = new GLuint[1];
		textRenderObject.numVerticesPerRenderCall[0] = numChars * 4;	//A quad for each character
		textRenderObject.IBO = 0;
		textRenderObject.instanceVBO = 0;
		textRenderObject.VBOs.clear();
		textRenderObject.VBOs.push_back(vboIDText);
		textRenderObject.textures = new osr::Texture[1];
		textRenderObject.sizeTextures = 1;
		textRenderObject.numTexturesPerRenderCall = new os_uint_t[1];
		textRenderObject.numTexturesPerRenderCall[0] = 1;
		textRenderObject.sizeNumTexturesPerRenderCall = 1;
		textRenderObject.renderPrimitive = GL_QUADS;

		osr::RenderObject ui;
		ui.numVerticesPerRenderCall = new GLuint[1];
		ui.numVerticesPerRenderCall[0] = 4 * numComps;
		ui.IBO = 0;
		ui.instanceVBO = 0;
		ui.VBOs.push_back(vboID);
		ui.textures = new osr::Texture[1];
		ui.textures[0] = osr::Texture {uiFilter.texAtlasID, GL_TEXTURE_2D};
		ui.sizeTextures = 1;
		ui.numTexturesPerRenderCall = new os_uint_t[1];
		ui.numTexturesPerRenderCall[0] = 1;
		ui.sizeNumTexturesPerRenderCall = 1;
		ui.renderPrimitive = GL_QUADS;					//Shader program uses quads for rendering rather than default triangles
		
		return ui;
	}












	

	void SetupManagerGL::setupDeferredShadingShaders(osr::RenderingEngine * renderingEngine, os_int_t framebufferWidth, os_int_t framebufferHeight,
		os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting, os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights)
	{
		os_uint_t gBufferFBO, gPos, gNormal, gColourSpec, gDepthRBO, framebufferVBO, framebufferVAO, framebufferProg;

		//Create the g-buffer fbo
		glGenFramebuffers(1, &gBufferFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

		//Position colour buffer
		glGenTextures(1, &gPos);
		glBindTexture(GL_TEXTURE_2D, gPos);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, framebufferWidth, framebufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPos, 0);

		//Normal colour buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, framebufferWidth, framebufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		//Colour + specular colour buffer
		glGenTextures(1, &gColourSpec);
		glBindTexture(GL_TEXTURE_2D, gColourSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, framebufferWidth, framebufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColourSpec, 0);

		//Tell OpenGL which colour attachments to use for rendering this frame buffer
		GLenum attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
		glDrawBuffers(3, attachments);

		glGenRenderbuffers(1, &gDepthRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, gDepthRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, framebufferWidth, framebufferHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gDepthRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Frame Buffer Complete" << std::endl;
		}
		else
		{
			std::cerr << "Frame Buffer Could not be Completed" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Create the VAO and VBO for rendering the fbo
		os_float_t quadVertices[] =
		{
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};
		glGenVertexArrays(1, &framebufferVAO);
		glGenBuffers(1, &framebufferVBO);
		glBindVertexArray(framebufferVAO);
		glBindBuffer(GL_ARRAY_BUFFER, framebufferVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(os_float_t), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(os_float_t), (GLvoid*)(2 * sizeof(os_float_t)));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		std::vector<os_uint_t> shaders;
		shaders.push_back(renderingEngine->createShader(GL_VERTEX_SHADER,   loadTextFile("res/default_engine_res/framebuffer_quad.vert")));
		shaders.push_back(renderingEngine->createShader(GL_FRAGMENT_SHADER, loadTextFile("res/default_engine_res/framebuffer_quad.frag")));
		framebufferProg = renderingEngine->createShaderProgram(shaders);

		glUseProgram(framebufferProg);
		if((blockIndexLighting = glGetUniformBlockIndex(framebufferProg, "LightBlock")) != GL_INVALID_INDEX)
		{
			glUniform1i(glGetUniformLocation(framebufferProg, "numPointLights"), numPointLights);
			glUniform1i(glGetUniformLocation(framebufferProg, "numDirLights"), numDirLights);
			glUniform1i(glGetUniformLocation(framebufferProg, "numSpotLights"), numSpotLights);
			glBindBuffer(GL_UNIFORM_BUFFER, uboIDLighting);
			glUniformBlockBinding(framebufferProg, blockIndexLighting, bindingPointLighting);//Bind the Uniform Shader Block to the correct binding point
		}

		glUniform1i(glGetUniformLocation(framebufferProg, "gPos"), 0);
		glUniform1i(glGetUniformLocation(framebufferProg, "gNormal"), 1);
		glUniform1i(glGetUniformLocation(framebufferProg, "gColourSpec"), 2);
	//	glUniform1i(glGetUniformLocation(framebufferProg, "shadowMap"), 3);
		glUniform4f(glGetUniformLocation(framebufferProg, "ambientIntensity"), 0.005f, 0.005f, 0.005f, 1.0f);
		glUseProgram(0);

		renderingEngine->setFramebuffer(gBufferFBO, gPos, gNormal, gColourSpec, gDepthRBO, framebufferVBO, framebufferVAO, framebufferProg);




		/**TODO - REMOVE FROM THIS FUNCTION AND PLACE IN PROJECT IO BUILD SHADERS*/
		osr::RenderPass renderPassObjects {gBufferFBO, renderingEngine->getFramebufferWidth(), renderingEngine->getFramebufferHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, true, false};
		osr::RenderPass renderPassDeferred {0, renderingEngine->getFramebufferWidth(), renderingEngine->getFramebufferHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, false, false};
		osr::RenderPass renderPassUI {0, renderingEngine->getFramebufferWidth(), renderingEngine->getFramebufferHeight(), GL_NONE, false, true};
		renderingEngine->addRenderPass(renderPassObjects);
		renderingEngine->addRenderPass(renderPassDeferred);
		renderingEngine->addRenderPass(renderPassUI);


		
		osr::RenderObject renderObject;
		renderObject.IBO = 0;
		renderObject.instanceVBO = 0;
		renderObject.materials = nullptr;
		renderObject.sizeMaterials = 0;
		renderObject.numInstances = 0;
		renderObject.numVerticesPerRenderCall = new os_uint_t[1];
		renderObject.numVerticesPerRenderCall[0] = 6;
		renderObject.VAO = framebufferVAO;
		renderObject.VBOs.push_back(framebufferVBO);
		renderObject.textures = new osr::Texture[3];
		renderObject.textures[0] = osr::Texture {gPos, GL_TEXTURE_2D};
		renderObject.textures[1] = osr::Texture {gNormal, GL_TEXTURE_2D};
		renderObject.textures[2] = osr::Texture {gColourSpec, GL_TEXTURE_2D};
		renderObject.numTexturesPerRenderCall = new os_uint_t[1];
		renderObject.numTexturesPerRenderCall[0] = 3;
		renderObject.sizeNumTexturesPerRenderCall = 1;
		renderObject.sizeTextures = 3;
		renderObject.renderPrimitive = GL_TRIANGLES;

		osr::ShaderProgram shaderProgram;
		shaderProgram.program = framebufferProg;
		//shaderProgram.uniforms = {0, 0, 0, 0, 0, 0, 0};

		osr::ShaderGroup shaderGroup;
		shaderGroup.renderObjects.push_back(renderObject);
		shaderGroup.shaderPrograms.push_back(shaderProgram);

		renderingEngine->addRenderable(1, shaderGroup);
	}





	void SetupManagerGL::setupShadowMapShaders(osr::RenderingEngine * renderingEngine, os_int_t shadowMapWidth, os_int_t shadowMapHeight)
	{
		os_uint_t shadowMapFBO, shadowMap, shaderProg, shadowMapPos0, shadowMapPos1;

		//Generate an fbo for the shadow map
		glGenFramebuffers(1, &shadowMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);

		//Generate a depth map texture to store the shadow information
		glGenTextures(1, &shadowMap);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Attach the depth map to the fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Shadow Map Frame Buffer Complete" << std::endl;
		}
		else
		{
			std::cerr << "Shadow Map Frame Buffer Could not be Completed" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		std::vector<os_uint_t> shaders;
		shaders.push_back(renderingEngine->createShader(GL_VERTEX_SHADER,   loadTextFile("res/default_engine_res/shadow_map.vert")));
		shaders.push_back(renderingEngine->createShader(GL_FRAGMENT_SHADER, loadTextFile("res/default_engine_res/shadow_map.frag")));
		shaderProg = renderingEngine->createShaderProgram(shaders);

		glUseProgram(shaderProg);
		shadowMapPos0 = glGetUniformLocation(shaderProg, "lightSpaceMatrix");
		shadowMapPos1 = glGetUniformLocation(shaderProg, "model");
		glUseProgram(0);

		renderingEngine->setShadowMap(shadowMapFBO, shadowMap, shadowMapWidth, shadowMapHeight, shaderProg, shadowMapPos0, shadowMapPos1);
	}





	/*void SetupManagerGL::setupStaticMeshShaders(std::vector<osr::RenderObject> * staticRenderObjects, os_uint_t vertShader, os_uint_t fragShader,
		os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices, os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting,
		os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights, const std::vector<std::pair<os_uint_t, os_uint_t>> & locs)
	{
		//Static Render Objects
		if((*staticRenderObjects).size() > 0)
		{
			std::vector<GLuint> shaders {vertShader, fragShader};
			osr::ShaderGroup shaderGroup;
			os_uint_t shaderProg = renderingEngine->createShaderProgram(shaders);

			glUseProgram(shaderProg);
			if((blockIndexMatrices = glGetUniformBlockIndex(shaderProg, "GlobalMatrices")) != GL_INVALID_INDEX)
			{
				glBindBuffer(GL_UNIFORM_BUFFER, uboIDMatrices);
				glUniformBlockBinding(shaderProg, blockIndexMatrices, bindingPointMatrices);
			}
			if((blockIndexLighting = glGetUniformBlockIndex(shaderProg, "LightBlock")) != GL_INVALID_INDEX)
			{
				glUniform1i(glGetUniformLocation(shaderProg, "numPointLights"), numPointLights);
				glUniform1i(glGetUniformLocation(shaderProg, "numDirLights"), numDirLights);
				glUniform1i(glGetUniformLocation(shaderProg, "numSpotLights"), numSpotLights);
				glBindBuffer(GL_UNIFORM_BUFFER, uboIDLighting);
				glUniformBlockBinding(shaderProg, blockIndexLighting, bindingPointLighting);//Bind the Uniform Shader Block to the correct binding point
			}

			glUseProgram(shaderProg);
			glUniformMatrix4fv(glGetUniformLocation(shaderProg, "camToClipMat"), 1, GL_FALSE, glm::value_ptr(renderingEngine->getPerspectiveProjectionMatrix()));
			glUniform1i(glGetUniformLocation(shaderProg, "texSampler"), 0);		//0 = GL_TEXTURE0 binding point
			glUniform1i(glGetUniformLocation(shaderProg, "shadowMap"), 1);
			glUniform1f(glGetUniformLocation(shaderProg, "attenuation"), 0.75f);
			glUniform4f(glGetUniformLocation(shaderProg, "ambientIntensity"), 0.2f, 0.2f, 0.2f, 1.0f);
			glUniform1f(glGetUniformLocation(shaderProg, "exposure"), 0.6f);
			glUseProgram(0);

			glUseProgram(shaderProg);

			osr::ShaderProgram shaderProgram(shaderProg);

			osr::Uniform unf1 {glGetUniformLocation(shaderProg, "material.ambient"), 0b00110100, 0};
			osr::Uniform unf2 {glGetUniformLocation(shaderProg, "material.diffuse"), 0b00110100, 0};
			osr::Uniform unf3 {glGetUniformLocation(shaderProg, "material.specular"), 0b00110100, 0};
			osr::Uniform unf4 {glGetUniformLocation(shaderProg, "material.shininess"), 0b00000000, 0};
			//osr::Uniform unf5 {glGetUniformLocation(shaderProg, "normWorldToCamMat"), 0b00110000};
			//osr::Uniform unf5 {glGetUniformLocation(shaderProg, "lightSpaceMatrix"), 0b00110000};

			shaderGroup.UBO = uboIDMatrices;
			shaderGroup.shaderPrograms.push_back(shaderProgram);
			shaderGroup.renderObjects = (*staticRenderObjects);

			//std::cerr << "SROs: " << (*staticRenderObjects).size() << std::endl;


			//VAO Generation & Setup
			{
				GLsizei vec4Size = sizeof(glm::vec4);
				GLsizei floatSize = sizeof(os_float_t);

				osr::VertexAttrib posAttrib {0, 3, GL_FLOAT, GL_FALSE, 8*floatSize, 0, 0, 4};
				osr::VertexAttrib normalsAttrib {1, 3, GL_FLOAT, GL_FALSE, 8*floatSize, 3*floatSize, 0, 4};
				osr::VertexAttrib uvAttrib {2, 2, GL_FLOAT, GL_FALSE, 8*floatSize, 6*floatSize, 0, 4};
				osr::VertexAttrib iAttrib1 {3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 0*vec4Size, 0, 1};
				osr::VertexAttrib iAttrib2 {4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 1*vec4Size, 0, 1};
				osr::VertexAttrib iAttrib3 {5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 2*vec4Size, 0, 1};
				osr::VertexAttrib iAttrib4 {6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 3*vec4Size, 0, 1};

				std::vector<osr::VertexAttrib> attribs {posAttrib, normalsAttrib, uvAttrib, iAttrib1, iAttrib2, iAttrib3, iAttrib4};

				for(size_t i = 0; i < shaderGroup.renderObjects.size(); i++)
				{
					attribs[0].VBO = shaderGroup.renderObjects[i].VBOs[0];
					attribs[1].VBO = shaderGroup.renderObjects[i].VBOs[0];
					attribs[2].VBO = shaderGroup.renderObjects[i].VBOs[0];
					attribs[3].VBO = shaderGroup.renderObjects[i].instanceVBO;
					attribs[4].VBO = shaderGroup.renderObjects[i].instanceVBO;
					attribs[5].VBO = shaderGroup.renderObjects[i].instanceVBO;
					attribs[6].VBO = shaderGroup.renderObjects[i].instanceVBO;
					shaderGroup.renderObjects[i].VAO = renderingEngine->genVAO(attribs, shaderGroup.renderObjects[i].IBO);
					shaderGroup.renderObjects[i].uniforms.push_back(unf1);
					shaderGroup.renderObjects[i].uniforms.back().data = glm::value_ptr(shaderGroup.renderObjects[i].materials[0].ambientColour);
					shaderGroup.renderObjects[i].uniforms.push_back(unf2);
					shaderGroup.renderObjects[i].uniforms.back().data = glm::value_ptr(shaderGroup.renderObjects[i].materials[0].diffuseColour);
					shaderGroup.renderObjects[i].uniforms.push_back(unf3);
					shaderGroup.renderObjects[i].uniforms.back().data = glm::value_ptr(shaderGroup.renderObjects[i].materials[0].specularColour);
					shaderGroup.renderObjects[i].uniforms.push_back(unf4);
					shaderGroup.renderObjects[i].uniforms.back().data = &(shaderGroup.renderObjects[i].materials[0].specularShininess);
				}

				glBindVertexArray(0);
				glUseProgram(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			renderingEngine->addRenderable(0, shaderGroup);
		}
	}*/



	/*void SetupManagerGL::setupSkinnedMeshShaders(std::vector<osr::RenderObject> * skinnedRenderObjects, std::vector<osan::SkinnedMesh *> & skinnedMeshes, os_uint_t vertShader, os_uint_t fragShader,
		os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices, os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting,
		os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights, const std::vector<std::pair<os_uint_t, os_uint_t>> & locs)
	{
		std::cerr << "Setting up Skinned Mesh Shaders" <<  std::endl;

		if((*skinnedRenderObjects).size() > 0)
		{
			std::vector<GLuint> shaders {vertShader, fragShader};
			os_uint_t shaderProg = renderingEngine->createShaderProgram(shaders);


			glUseProgram(shaderProg);
			if((blockIndexMatrices = glGetUniformBlockIndex(shaderProg, "GlobalMatrices")) != GL_INVALID_INDEX)
			{
				//GLuint bindingPoint = 0;		//0 = The binding point to be used by this ubo, should be different for each ubo
				glBindBuffer(GL_UNIFORM_BUFFER, uboIDMatrices);
				glUniformBlockBinding(shaderProg, blockIndexMatrices, bindingPointMatrices);
			}
			if((blockIndexLighting = glGetUniformBlockIndex(shaderProg, "LightBlock")) != GL_INVALID_INDEX)
			{
				glUniform1i(glGetUniformLocation(shaderProg, "numPointLights"), numPointLights);
				glUniform1i(glGetUniformLocation(shaderProg, "numDirLights"), numDirLights);
				glUniform1i(glGetUniformLocation(shaderProg, "numSpotLights"), numSpotLights);
				glBindBuffer(GL_UNIFORM_BUFFER, uboIDLighting);
				glUniformBlockBinding(shaderProg, blockIndexLighting, bindingPointLighting);//Bind the Uniform Shader Block to the correct binding point
			}


			glUniformMatrix4fv(glGetUniformLocation(shaderProg, "camToClipMat"), 1, GL_FALSE, glm::value_ptr(renderingEngine->getPerspectiveProjectionMatrix()));
			glUniform1i(glGetUniformLocation(shaderProg, "texSampler"), 0);		//0 = GL_TEXTURE0 binding point
			glUniform1i(glGetUniformLocation(shaderProg, "shadowMap"), 1);
			glUniform1f(glGetUniformLocation(shaderProg, "attenuation"), 0.75f);
			glUniform4f(glGetUniformLocation(shaderProg, "ambientIntensity"), 0.2f, 0.2f, 0.2f, 1.0f);
			glUniform1f(glGetUniformLocation(shaderProg, "exposure"), 0.6f);


			osr::ShaderProgram shaderProgram;
			shaderProgram.program = shaderProg;
			glUseProgram(shaderProg);
			osr::Uniform unf1 {glGetUniformLocation(shaderProg, "material.ambient"), 0b00110100, 0};
			osr::Uniform unf2 {glGetUniformLocation(shaderProg, "material.diffuse"), 0b00110100, 0};
			osr::Uniform unf3 {glGetUniformLocation(shaderProg, "material.specular"), 0b00110100, 0};
			osr::Uniform unf4 {glGetUniformLocation(shaderProg, "material.shininess"), 0b00000000, 0};

			osr::ShaderGroup shaderGroup;
			shaderGroup.UBO = uboIDMatrices;
			shaderGroup.shaderPrograms.push_back(shaderProgram);
			shaderGroup.renderObjects = (*skinnedRenderObjects);




			/*for(size_t r = 0; r < skinnedRenderObjects->size() && r < skinnedMeshes.size(); r++)
			{
				//std::cerr << "NUM INSTANCES: " << skinnedRenderObjects->at(r).numInstances << std::endl;

				osan::SkinnedMesh * skinnedMesh = skinnedMeshes[r];

				for(size_t i = 0; i < skinnedRenderObjects->at(r).numInstances; i++)	//For each instance of the render object
				{
					//glUniform1i(glGetUniformLocation(shaderProg, "numBonesPerInstance"), skinnedMesh.bones.size());

					for(size_t j = 0; j < skinnedMesh->bones.size(); j++)
					{
						std::string locName("boneTransforms[");
						std::stringstream ss;
						ss << (skinnedMesh->totalBoneOffset + j + i * skinnedMesh->bones.size());
						locName += ss.str();
						locName += "]";

						glUniformMatrix4fv(glGetUniformLocation(shaderProg, locName.c_str()), 1,
							GL_FALSE, glm::value_ptr(skinnedMesh->bones[j].offsetMatrix));
					}
				}
			}///////////////////////////////////////////////




			//VAO Generation & Setup - Skeletal Animation
			{
				GLsizei vec4Size = sizeof(glm::vec4);
				GLsizei floatSize = sizeof(os_float_t);

				osr::VertexAttrib posAttrib		{locs[0].second,   3, GL_FLOAT, GL_FALSE, 16*floatSize, 0, 0, 4};
				osr::VertexAttrib normalsAttrib {locs[1].second,   3, GL_FLOAT, GL_FALSE, 16*floatSize, 3*floatSize, 0, 4};
				osr::VertexAttrib uvAttrib		{locs[2].second,   2, GL_FLOAT, GL_FALSE, 16*floatSize, 6*floatSize, 0, 4};
				osr::VertexAttrib boneIDSAttrib {locs[4].second,   4, GL_FLOAT, GL_FALSE, 16*floatSize, 8*floatSize, 0, 4};
				osr::VertexAttrib weightsAttrib {locs[5].second,   4, GL_FLOAT, GL_FALSE, 16*floatSize, 12*floatSize, 0, 4};
				osr::VertexAttrib iAttrib1		{locs[3].second,   4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 0 * vec4Size, 0, 1};
				osr::VertexAttrib iAttrib2		{locs[3].second+1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 1 * vec4Size, 0, 1};
				osr::VertexAttrib iAttrib3		{locs[3].second+2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 2 * vec4Size, 0, 1};
				osr::VertexAttrib iAttrib4		{locs[3].second+3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 3 * vec4Size, 0, 1};
				osr::VertexAttrib iBoneOffset	{locs[6].second,   1, GL_FLOAT, GL_FALSE, floatSize, 0, 0, 1};

				std::vector<osr::VertexAttrib> attribs {posAttrib, normalsAttrib, uvAttrib, boneIDSAttrib, weightsAttrib, iAttrib1, iAttrib2, iAttrib3, iAttrib4, iBoneOffset};

				for(size_t i = 0; i < shaderGroup.renderObjects.size(); i++)
				{
					attribs[0].VBO = shaderGroup.renderObjects[i].VBOs[0];
					attribs[1].VBO = shaderGroup.renderObjects[i].VBOs[0];
					attribs[2].VBO = shaderGroup.renderObjects[i].VBOs[0];
					attribs[3].VBO = shaderGroup.renderObjects[i].VBOs[0];
					attribs[4].VBO = shaderGroup.renderObjects[i].VBOs[0];
					attribs[5].VBO = shaderGroup.renderObjects[i].instanceVBO;
					attribs[6].VBO = shaderGroup.renderObjects[i].instanceVBO;
					attribs[7].VBO = shaderGroup.renderObjects[i].instanceVBO;
					attribs[8].VBO = shaderGroup.renderObjects[i].instanceVBO;
					attribs[9].VBO = shaderGroup.renderObjects[i].VBOs[1];
					shaderGroup.renderObjects[i].VAO = renderingEngine->genVAO(attribs, shaderGroup.renderObjects[i].IBO);
					shaderGroup.renderObjects[i].uniforms.push_back(unf1);
					shaderGroup.renderObjects[i].uniforms.push_back(unf2);
					shaderGroup.renderObjects[i].uniforms.push_back(unf3);
					shaderGroup.renderObjects[i].uniforms.push_back(unf4);
					shaderGroup.renderObjects[i].uniforms.back().data = glm::value_ptr(shaderGroup.renderObjects[i].materials[0].ambientColour);
					shaderGroup.renderObjects[i].uniforms.back().data = glm::value_ptr(shaderGroup.renderObjects[i].materials[0].diffuseColour);
					shaderGroup.renderObjects[i].uniforms.back().data = glm::value_ptr(shaderGroup.renderObjects[i].materials[0].specularColour);
					shaderGroup.renderObjects[i].uniforms.back().data = &(shaderGroup.renderObjects[i].materials[0].specularShininess);
				}

				std::vector<osan::AnimationController> & vecRef = animationEngine->getAnimationControllers();
				for(size_t a = 0; a < vecRef.size(); a++)
				{
					vecRef[a].skinnedMesh.shaderProg = shaderProg;
				}

				glBindVertexArray(0);
				glUseProgram(0);
			}

			renderingEngine->addRenderable(0, shaderGroup);
		}
	}*/






	void SetupManagerGL::setupSkyboxShaders(osr::RenderObject skyboxRenderObject, os_uint_t skyboxCubemapID, os_uint_t vertShader, os_uint_t fragShader,
		os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices)
	{
		osr::VertexAttrib posAttrib {0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(os_float_t), 0, skyboxRenderObject.VBOs[0], 4};

		std::vector<osr::VertexAttrib> attribs {posAttrib};

		skyboxRenderObject.VAO = renderingEngine->genVAO(attribs, 0);

		std::vector<os_uint_t> shaders {vertShader, fragShader};
		os_uint_t shaderProg = renderingEngine->createShaderProgram(shaders);

		glUseProgram(shaderProg);
		glUniformMatrix4fv(glGetUniformLocation(shaderProg, "camToClipMat"), 1, GL_FALSE, glm::value_ptr(renderingEngine->getPerspectiveProjectionMatrix()));
		glUniform1i(glGetUniformLocation(shaderProg, "cubemapTex"), 0);		//0 = GL_TEXTURE0 binding point
		glUniform4f(glGetUniformLocation(shaderProg, "ambientIntensity"), 0.9f, 0.9f, 0.9f, 1.0f);
		glUseProgram(0);

		osr::ShaderProgram shaderProgram {shaderProg};

		osr::ShaderGroup shaderGroup;
		shaderGroup.shaderPrograms.push_back(shaderProgram);
		skyboxRenderObject.uniforms.push_back(osr::Uniform {glGetUniformLocation(shaderProg, "viewMat"), 0b10111000, 0, glm::value_ptr(*renderingEngine->mainCamTransformInv)});
		//shaderGroup.shaderPrograms[0].uniforms.push_back(glGetUniformLocation(shaderProg, "viewMat"));	//TODO - OOPS ERROR FIX PLOX
		shaderGroup.renderObjects.push_back(skyboxRenderObject);

		renderingEngine->setSkybox(shaderGroup, skyboxCubemapID);
		renderingEngine->addRenderable(0, shaderGroup);
	}





	void SetupManagerGL::setupUIShaders(osr::RenderObject uiRenderObject, os_uint_t uiTexAtlasID, os_uint_t vertShader, os_uint_t fragShader)
	{
		osr::VertexAttrib posAttrib {0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(os_float_t), 0, uiRenderObject.VBOs[0], 4};
		osr::VertexAttrib uvAttrib {1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(os_float_t), 2*sizeof(os_float_t), uiRenderObject.VBOs[0], 4};

		std::vector<osr::VertexAttrib> attribs {posAttrib, uvAttrib};

		uiRenderObject.VAO = renderingEngine->genVAO(attribs, 0);
		uiRenderObject.numInstances = 0;	//Only required for instanced rendering

		std::vector<GLuint> shaders {vertShader, fragShader};
		os_uint_t shaderProg = renderingEngine->createShaderProgram(shaders);

		glUseProgram(shaderProg);
		glUniform1i(glGetUniformLocation(shaderProg, "texSampler"), 0);
		glUniformMatrix4fv(glGetUniformLocation(shaderProg, "orthoProjection"), 1, GL_FALSE, glm::value_ptr(renderingEngine->getOrthographicProjectionMatrix()));
		glUseProgram(0);

		osr::ShaderProgram shaderProgram;
		shaderProgram.program = shaderProg;

		osr::ShaderGroup shaderGroup;
		shaderGroup.shaderPrograms.push_back(shaderProgram);
		shaderGroup.renderObjects.push_back(uiRenderObject);

		renderingEngine->setUI(shaderGroup, uiTexAtlasID);
		renderingEngine->addRenderable(2, shaderGroup);
	}





	void SetupManagerGL::setupTextShaders(osr::RenderObject textRenderObject, os_uint_t textTexAtlasID, os_uint_t vertShader, os_uint_t fragShader)
	{
		osr::VertexAttrib posAttrib {0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(os_float_t), 0, textRenderObject.VBOs[0], 4};
		osr::VertexAttrib uvAttrib {1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(os_float_t), 2*sizeof(os_float_t), textRenderObject.VBOs[0], 4};
		
		std::vector<osr::VertexAttrib> attribs {posAttrib, uvAttrib};

		textRenderObject.VAO = renderingEngine->genVAO(attribs, 0);
		textRenderObject.numInstances = 0;

		std::vector<os_uint_t> shaders {vertShader, fragShader};
		os_uint_t textShaderProg = renderingEngine->createShaderProgram(shaders);

		glUseProgram(textShaderProg);
		os_int_t texSampler = 0;
		os_float_t textColour[3] {1.0f, 1.0f, 1.0f};
		renderingEngine->uniform(0b00000010, glGetUniformLocation(textShaderProg, "texSampler"), &texSampler);
		renderingEngine->uniform(0b00100000, glGetUniformLocation(textShaderProg, "textColour"), &textColour);
		renderingEngine->uniform(0b00111000, glGetUniformLocation(textShaderProg, "orthoProjection"), (void *)glm::value_ptr(renderingEngine->getOrthographicProjectionMatrix()));
		glUseProgram(0);

		osr::ShaderProgram shaderProgram;
		shaderProgram.program = textShaderProg;

		osr::ShaderGroup textShaderGroup;
		textRenderObject.textures[0] = osr::Texture {textTexAtlasID, GL_TEXTURE_2D};
		textShaderGroup.shaderPrograms.push_back(shaderProgram);
		textShaderGroup.renderObjects.push_back(textRenderObject);

		renderingEngine->setText(textShaderGroup, textTexAtlasID);
		renderingEngine->addRenderable(2, textShaderGroup);
	}




	void SetupManagerGL::setupShaders(std::vector<osr::RenderObject> * renderObjects, os_uint_t vertShader, os_uint_t fragShader,
		os_uint_t blockIndexMatrices, os_uint_t uboIDMatrices, os_uint_t bindingPointMatrices, os_uint_t blockIndexLighting, os_uint_t bindingPointLighting, os_uint_t uboIDLighting,
		os_int_t numDirLights, os_int_t numPointLights, os_int_t numSpotLights, const std::vector<std::pair<os_uint_t, os_uint_t>> & locs)
	{
		if((*renderObjects).size() > 0)
		{
			std::vector<GLuint> shaders {vertShader, fragShader};
			os_uint_t shaderProg = renderingEngine->createShaderProgram(shaders);

			
			//if skinned or static render object
			//{
				glUseProgram(shaderProg);
				if((blockIndexMatrices = glGetUniformBlockIndex(shaderProg, "GlobalMatrices")) != GL_INVALID_INDEX)
				{
					//GLuint bindingPoint = 0;		//0 = The binding point to be used by this ubo, should be different for each ubo
					glBindBuffer(GL_UNIFORM_BUFFER, uboIDMatrices);
					glUniformBlockBinding(shaderProg, blockIndexMatrices, bindingPointMatrices);
				}
				if((blockIndexLighting = glGetUniformBlockIndex(shaderProg, "LightBlock")) != GL_INVALID_INDEX)
				{
					glUniform1i(glGetUniformLocation(shaderProg, "numPointLights"), numPointLights);
					glUniform1i(glGetUniformLocation(shaderProg, "numDirLights"), numDirLights);
					glUniform1i(glGetUniformLocation(shaderProg, "numSpotLights"), numSpotLights);
					glBindBuffer(GL_UNIFORM_BUFFER, uboIDLighting);
					glUniformBlockBinding(shaderProg, blockIndexLighting, bindingPointLighting);//Bind the Uniform Shader Block to the correct binding point
				}


				glUniformMatrix4fv(glGetUniformLocation(shaderProg, "camToClipMat"), 1, GL_FALSE, glm::value_ptr(renderingEngine->getPerspectiveProjectionMatrix()));
				glUniform1i(glGetUniformLocation(shaderProg, "texSampler"), 0);		//0 = GL_TEXTURE0 binding point
				glUniform1i(glGetUniformLocation(shaderProg, "shadowMap"), 1);
				glUniform1f(glGetUniformLocation(shaderProg, "attenuation"), 0.75f);
				glUniform4f(glGetUniformLocation(shaderProg, "ambientIntensity"), 0.2f, 0.2f, 0.2f, 1.0f);
				glUniform1f(glGetUniformLocation(shaderProg, "exposure"), 0.6f);


				osr::ShaderProgram shaderProgram {shaderProg};
				osr::Uniform unf1 {glGetUniformLocation(shaderProg, "material.ambient"), 0b00110100, 0};
				osr::Uniform unf2 {glGetUniformLocation(shaderProg, "material.diffuse"), 0b00110100, 0};
				osr::Uniform unf3 {glGetUniformLocation(shaderProg, "material.specular"), 0b00110100, 0};
				osr::Uniform unf4 {glGetUniformLocation(shaderProg, "material.shininess"), 0b00000000, 0};
				
				osr::ShaderGroup shaderGroup;
				shaderGroup.UBO = uboIDMatrices;
				shaderGroup.shaderPrograms.push_back(shaderProgram);
				shaderGroup.renderObjects = (*renderObjects);
			//}

			glUseProgram(shaderProg);

			//Calculate the stride for VBO[0] in number of floats
			GLsizei strideNumFloats = 0;
			for(auto loc : locs)
			{
				//If the loc is a position, normal, texCoord, boneID, boneWeight or colour
				if(loc.first == 0 || loc.first == 1)
					strideNumFloats += 3;			//Then, add the number of floats used
				else if(loc.first == 2)
					strideNumFloats += 2;
				else if(loc.first == 4 || loc.first == 5 || loc.first == 7)
					strideNumFloats += 4;
			}

			//VAO Generation & Setup - Skeletal Animation
			GLsizei VBO0Offset = 0;

			std::vector<osr::VertexAttrib> attribs;
			for(auto loc : locs)
			{
				//Generate a set of attribs for each loc
				std::vector<osr::VertexAttrib> attribsForLoc;
				genVertexAttrib(attribsForLoc, loc, VBO0Offset, strideNumFloats * sizeof(os_float_t));	//offset incremented in genVertexAttrib function
				
				//Copy contents of loc attribs into attribs array
				attribs.insert(attribs.end(), attribsForLoc.begin(), attribsForLoc.end());
			}

			//Set each attrib to the correct VBO
			for(size_t i = 0; i < shaderGroup.renderObjects.size(); i++)
			{
				for(osr::VertexAttrib & attrib : attribs)
				{
					switch(attrib.attribID)			//attribID gives the type of attribute represented
					{
					case 0:			//Positions
					case 1:			//Normals
					case 2:			//Tex Coords
					case 4:			//Bone IDs
					case 5:			//Bone Weights
					case 7:			//Colours
					{
						if(shaderGroup.renderObjects[i].VBOs.size() >= 1)
							attrib.VBO = shaderGroup.renderObjects[i].VBOs[0];
						else
							std::cerr << "Error: VBO[0] attrib set VBO failed -> VBO array not large enough" << std::endl;
						break;
					}
					case 3:			//Instance transform matrix
					{
						attrib.VBO = shaderGroup.renderObjects[i].instanceVBO;
						break;
					}
					case 6:			//Bone Offset
					{
						if(shaderGroup.renderObjects[i].VBOs.size() >= 2)
							attrib.VBO = shaderGroup.renderObjects[i].VBOs[1];
						else
							std::cerr << "Error: Bone offset attrib set VBO failed -> VBO array not large enough" << std::endl;
						break;
					}
					}
				}

				//Generate a VAO for current renderObject
				shaderGroup.renderObjects[i].VAO = renderingEngine->genVAO(attribs, shaderGroup.renderObjects[i].IBO);

				//Setup material uniforms
				shaderGroup.renderObjects[i].uniforms.push_back(unf1);
				shaderGroup.renderObjects[i].uniforms.back().data = glm::value_ptr(shaderGroup.renderObjects[i].materials[0].ambientColour);
				shaderGroup.renderObjects[i].uniforms.push_back(unf2);
				shaderGroup.renderObjects[i].uniforms.back().data = glm::value_ptr(shaderGroup.renderObjects[i].materials[0].diffuseColour);
				shaderGroup.renderObjects[i].uniforms.push_back(unf3);
				shaderGroup.renderObjects[i].uniforms.back().data = glm::value_ptr(shaderGroup.renderObjects[i].materials[0].specularColour);
				shaderGroup.renderObjects[i].uniforms.push_back(unf4);
				shaderGroup.renderObjects[i].uniforms.back().data = &(shaderGroup.renderObjects[i].materials[0].specularShininess);
			}


			///////////////////////////////////////////////////////////////////////////
			////    CODE MAY CAUSE SEVERE BUGS -> FIND BETTER WAY OF DOING THIS    ////
			///////////////////////////////////////////////////////////////////////////
			for(auto loc : locs)				//Prevents some bugs - i.e. ordering of calls
			{
				if(loc.first == 4 || loc.first == 5 || loc.first == 6)		//If the program supports skinning
				{
					std::vector<osan::AnimationController> & vecRef = animationEngine->getAnimationControllers();
					for(size_t a = 0; a < vecRef.size(); a++)				//Then, inform the corresponding animation controller
					{
						vecRef[a].skinnedMesh.shaderProg = shaderProg;
					}
					break;	
				}
			}
			///////////////////////////////////////////////////////////////////////////////////////////////////


			glBindVertexArray(0);
			glUseProgram(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			renderingEngine->addRenderable(0, shaderGroup);
		}
	}



	void SetupManagerGL::genVertexAttrib(std::vector<osr::VertexAttrib> & outAttribs, const std::pair<os_uint_t, os_uint_t> & loc, GLsizei & inOutOffset, GLsizei VBO0Stride)
	{
		GLsizei vec4Size = sizeof(glm::vec4);
		GLsizei floatSize = sizeof(os_float_t);

		//VertexAttrib {location, size, type, normalised, stride, offset, VBO, divisor, attribID (matches to loc.first)}

		switch(loc.first)
		{
			case 0:			//Positions (flow through to Normals)
			case 1:			//Normals
			{
				outAttribs = {{loc.second, 3, GL_FLOAT, GL_FALSE, VBO0Stride, inOutOffset, 0, 4, loc.first}};
				inOutOffset += 3 * floatSize;
				break;
			}
			case 2:			//Texture co-ordinates
			{
				outAttribs = {{loc.second, 2, GL_FLOAT, GL_FALSE, VBO0Stride, inOutOffset, 0, 4, loc.first}};
				inOutOffset += 2 * floatSize;
				break;
			}
			case 3:			//Instance transform matrix
			{
				//Assumes matrix is stored in separate VBO, therefore, do not increment inOutOffset
				outAttribs = {{loc.second+0, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 0 * vec4Size, 0, 1, loc.first},
							{loc.second+1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 1 * vec4Size, 0, 1, loc.first},
							{loc.second+2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 2 * vec4Size, 0, 1, loc.first},
							{loc.second+3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, 3 * vec4Size, 0, 1, loc.first}};
				break;
			}
			case 4:			//Bone IDs (flow through to Bone Weights)
			case 5:			//Bone Weights
			case 7:			//Colours
			{
				outAttribs = {{loc.second, 4, GL_FLOAT, GL_FALSE, VBO0Stride, inOutOffset, 0, 4, loc.first}};
				inOutOffset += 4 * floatSize;
				break;
			}
			case 6:			//Bone Offset
			{
				//Assume stored in separate VBO, therefore, do not increment inOutOffset
				outAttribs = {{loc.second, 1, GL_FLOAT, GL_FALSE, floatSize, 0, 0, 1, loc.first}};
				break;
			}
		}
	}
}
