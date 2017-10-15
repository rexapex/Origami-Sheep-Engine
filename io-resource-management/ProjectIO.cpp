/*
 * ProjectIO.cpp
 *
 *  Created on: 9 Oct 2015
 *      Author: james_000
 */

#include "ProjectIO.h"
#include "ResourceManager.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

namespace osrm
{
	void loadScene(ResourceManager * resourceManager, const std::string & path)	//TODO - Temporary fnc for testing, to be replaced
	{
		std::string headerPath = path + ".ossh";
		std::string bodyPath   = path + ".ossb";

		parseSceneHeaderFile(resourceManager, headerPath);

		resourceManager->loadAllResources();	//Load all resources requested for loading

		parseSceneBodyFile(resourceManager, bodyPath);
	}




	void parseSceneHeaderFile(ResourceManager * resourceManager, std::string path)
	{
		std::ifstream fleSceneHeader;
		fleSceneHeader.open(path.c_str(), std::ifstream::in);

		std::cerr << "Opening Scene Header: " << path.c_str() << std::endl;

		if(fleSceneHeader.is_open())
		{
			std::stringstream iss;
			iss << fleSceneHeader.rdbuf();

			while(iss)
			{
				std::string next;
				iss >> next;

				/*if(next == "s")			//Single Shader
				{
					GLuint typeInt;
					GLenum type;
					std::string path;

					iss >> typeInt;
					iss >> path;

					switch(typeInt)
					{
					case 0:
						type = GL_VERTEX_SHADER;
						break;
					case 1:
						type = GL_FRAGMENT_SHADER;
						break;
					}

		//			resourceManager->registerShader(path, type);
				}
				else if(next == "p")	//Single Shader Program
				{
					GLuint numShaders;
					std::vector<GLuint> shaderIndexes;

					iss >> numShaders;

					for(GLuint n = 0; n < numShaders; n++)
					{
						GLuint s;
						iss >> s;
						shaderIndexes.push_back(s);
					}

		//			resourceManager->registerShaderProgram(shaderIndexes);
				}*/
				if(next == "m")
				{
					std::string path;

					iss >> path;

					resourceManager->registerMesh(path);
				}
				else if(next == "t")
				{
					std::string tex;
					
					iss >> tex;

					resourceManager->registerTexture(tex, nullptr);
				}
				else if(next == "c")	//Cubemap texture
				{
					std::string ft, bk, up, dn, rt, lf;

					iss >> ft;
					iss >> bk;
					iss >> up;
					iss >> dn;
					iss >> rt;
					iss >> lf;

					std::vector<std::string> faces;
					faces.push_back(ft);
					faces.push_back(bk);
					faces.push_back(up);
					faces.push_back(dn);
					faces.push_back(rt);
					faces.push_back(lf);

					resourceManager->registerCubemap(faces, NULL);
				}
				else if(next == "s")	//Gives the ID of the default skybox to be used
				{
					GLuint index;	//Index into the skyboxes array of the default skybox

					iss >> index;

					resourceManager->defaultSkyboxIndex = index;
				}
				else if(next == "i")	//One of the input files used by this scene
				{
					std::string path;

					iss >> path;

					parseInputFile(resourceManager, resourceManager->getProjectPath() + path);
				}
				else if(next == "u")	//One of the user interface files used by this scene
				{
					std::string path;

					iss >> path;

					parseUIFile(resourceManager, resourceManager->getProjectPath() + path);
					std::cerr << "UI File Parsed\n";
				}
			}
		}

		fleSceneHeader.close();
	}






	void parseSceneBodyFile(ResourceManager * resourceManager, std::string path)
	{
		std::ifstream fleSceneBody;
		fleSceneBody.open(path.c_str(), std::ifstream::in);

		std::cerr << "Opening Scene Body: " << path.c_str() << std::endl;

		//std::vector<Entity> finishedEntities;

		/*
		 * Types in Files:
		 * s - Shader Header
		 * p - Shader Program
		 * e - Entity
		 *
		 * Shader IDs:
		 * 0:Vertex Shader
		 * 1:Fragment Shader
		 *
		 * Component IDs:
		 * 0-9:   Rendering -> 0:PlaneRenderer, 1:MeshRenderer
		 * 10-12: Lights    -> 10:DirLight, 11:PointLight, 12:SpotLight
		 * 20-29: Animation -> 20:SkinnedMeshRenderer, 21:Animation Controller
		 */

		if(fleSceneBody.is_open())
		{
			ID nextEntityID = 0;
			std::vector<Entity> entities;
			std::stringstream iss;
			iss << fleSceneBody.rdbuf();
			//std::cout << iss.str() << std::endl;

			int numSkinnedMeshRenderers = 0;						//The number of skinned mesh renderers registered up to and excluding current entity
			std::vector<int> numSkinnedMeshRenderersThisEntity;		//The number of skinned mesh renderers registered for the current entity - vector as entities can be nested
			int meshFilterIndexForAnimation = 0;					//The mesh fitler index used by following animation controllers

			while(iss)
			{
				std::string next;
				iss >> next;

				if(next == "e")	//Create a new entity
				{
					numSkinnedMeshRenderersThisEntity.push_back(0);

					std::string name;
					iss >> name;		//Get the name of the entity
					//std::cout << "New Entity: Name:" << name << ", ID:" << nextEntityID << std::endl;
					Entity e;
					e.entityID = nextEntityID;
					e.name = name;

					glm::vec3 pos, scale;
					glm::quat orientation;
					iss >> pos.x;
					iss >> pos.y;
					iss >> pos.z;
					iss >> scale.x;
					iss >> scale.y;
					iss >> scale.z;
					iss >> orientation.x;
					iss >> orientation.y;
					iss >> orientation.z;
					iss >> orientation.w;

					osm::Transform * transform = new osm::Transform;
					transform->translation = pos;
					transform->scale = scale;
					transform->orientation = orientation;
					osm::calcModelMatrix(transform);
					e.transform = transform;
					//std::cout << e.entityID << ": Transform: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;

					if(entities.size() > 0)	//If entity definition is within another entity definition then set as child
					{
						entities[entities.size() - 1].transform->children.push_back(transform);
						osm::calcModelMatrixChild(entities[entities.size() - 1].transform, entities[entities.size() - 1].transform->children.size()-1);
					}

					entities.push_back(e);
					nextEntityID++;
				}
				else if(next == "0")	//0 = Plane Renderer Component
				{
					std::string name;
					unsigned int texFilter, shaderProg;
					iss >> name;
					iss >> texFilter;
					iss >> shaderProg;

					resourceManager->registerPlaneRenderer(entities.back(), name, texFilter, shaderProg);
				}
				else if(next == "1")	//1 = Mesh Renderer Component
				{
					std::string name;
					unsigned int filter, shaderProg;
					iss >> name;
					iss >> filter;
					iss >> shaderProg;
					//std::cerr << name << ", filter: " << filter << std::endl;

					resourceManager->registerMeshRenderer(entities.back(), name, filter, shaderProg);

					//std::cout << entities[entities.size()-1].entityID << ": Mesh Renderer: Name:" << name << ", Mesh Filter:" << filter << std::endl;
				}
				else if(next == "10")
				{
					std::string name;	//The lights component name
					float x, y, z, aR, aB, aG, dR, dG, dB, sR, sG, sB, inten;	//Ambient, Diffuse, Specular RGB Values & Light Intensity

					iss >> name;
					iss >> x;
					iss >> y;
					iss >> z;
					iss >> aR;
					iss >> aG;
					iss >> aB;
					iss >> dR;
					iss >> dG;
					iss >> dB;
					iss >> sR;
					iss >> sG;
					iss >> sB;
					iss >> inten;

					osr::DirectionLight dirLight;
					dirLight.dir = glm::vec4(x, y, z, 1);
					dirLight.ambientColour = glm::vec4(aR, aG, aB, 1);
					dirLight.diffuseColour = glm::vec4(dR, dG, dB, 1);
					dirLight.specularColour = glm::vec4(sR, sG, sB, 1);
					dirLight.intensity = inten;

					//dirLight.dir = entities.back().transform->orientation * dirLight.dir;
				
					resourceManager->registerDirLight(entities.back(), name, dirLight);
				}
				else if(next == "11")
				{
					std::string name;	//The lights component name
					//Ambient, Diffuse, Specular RGB Values & Light Intensity & constant, linear, quadratic for use in attenuation formula
					float x, y, z, aR, aB, aG, dR, dG, dB, sR, sG, sB, inten, constant, linear, quadratic;

					iss >> name;
					iss >> x;
					iss >> y;
					iss >> z;
					iss >> aR;
					iss >> aG;
					iss >> aB;
					iss >> dR;
					iss >> dG;
					iss >> dB;
					iss >> sR;
					iss >> sG;
					iss >> sB;
					iss >> constant;
					iss >> linear;
					iss >> quadratic;
					iss >> inten;

					osr::PointLight pointLight;
					pointLight.relativePos = glm::vec4(x, y, z, 1);
					pointLight.ambientColour = glm::vec4(aR, aG, aB, 1);
					pointLight.diffuseColour = glm::vec4(dR, dG, dB, 1);
					pointLight.specularColour = glm::vec4(sR, sG, sB, 1);
					pointLight.intensity = inten;
					pointLight.constant = constant;
					pointLight.linear = linear;
					pointLight.quadratic = quadratic;

					pointLight.relativePos = glm::vec4(entities.back().transform->translation, 0.0f) + pointLight.relativePos;

					resourceManager->registerPointLight(entities.back(), name, pointLight);
				}
				else if(next == "12")
				{
					std::string name;	//The lights component name
					//Ambient, Diffuse, Specular RGB Values & Light Intensity & constant, linear, quadratic for use in attenuation formula
					float x, y, z, dirx, diry, dirz, cosInner, cosOuter, constant, linear, quadratic, inten, aR, aB, aG, dR, dG, dB, sR, sG, sB;

					iss >> name;
					iss >> x;
					iss >> y;
					iss >> z;
					iss >> dirx;
					iss >> diry;
					iss >> dirz;
					iss >> aR;
					iss >> aG;
					iss >> aB;
					iss >> dR;
					iss >> dG;
					iss >> dB;
					iss >> sR;
					iss >> sG;
					iss >> sB;
					iss >> cosInner;
					iss >> cosOuter;
					iss >> constant;
					iss >> linear;
					iss >> quadratic;
					iss >> inten;

					osr::SpotLight spotLight;
					spotLight.relativePos = glm::vec4(x, y, z, 1);
					spotLight.dir = glm::vec4(dirx, diry, dirz, 1);
					spotLight.cosInnerCutoff = cosInner;
					spotLight.cosOuterCutoff = cosOuter;
					spotLight.ambientColour = glm::vec4(aR, aG, aB, 1);
					spotLight.diffuseColour = glm::vec4(dR, dG, dB, 1);
					spotLight.specularColour = glm::vec4(sR, sG, sB, 1);
					spotLight.intensity = inten;
					spotLight.constant = constant;
					spotLight.linear = linear;
					spotLight.quadratic = quadratic;

					spotLight.relativePos = glm::vec4(entities.back().transform->translation, 0.0f) + spotLight.relativePos;

					resourceManager->registerSpotLight(entities.back(), name, spotLight);
				}
				else if(next == "20")	//Skinned Mesh Renderer
				{
					numSkinnedMeshRenderersThisEntity.back()++;

					std::string name;
					os_uint_t filter, shaderProg;
					iss >> name;
					iss >> filter;
					iss >> shaderProg;

					meshFilterIndexForAnimation = filter;	//Set the mesh filter to be used by following animation controllers - Note: Index for all meshes, static and skinned

					//std::cerr << name << ", filter: " << filter << std::endl;
					resourceManager->registerMeshRenderer(entities.back(), name, filter, shaderProg, true);
				}
				else if(next == "21")
				{
					std::string name;
					os_int_t index;				//The index into the array of this entities skinned mesh renderers
					os_int_t startAnimation;	//The animation to begin playing when the game begins
					os_int_t numConnectors;		//The number of connectors therefore the number of times to loop and read data is known
					iss >> name;
					iss >> index;
					iss >> startAnimation;
					iss >> numConnectors;

					AnimationControllerFilter controller;
					controller.name = name;
					controller.meshIndex = meshFilterIndexForAnimation;
					controller.skinnedMeshInstanceIndex = index + numSkinnedMeshRenderers;		//Gives the skinned mesh renderer to use in the full array
					controller.startAnimation = startAnimation;

					//std::cerr << "MESH FILTER INDEX: " << meshFilterIndexForAnimation << ", SKINNED MESH INSTANCE INDEX: " << index + numSkinnedMeshRenderers << std::endl;

					for(int i = 0; i < numConnectors; i++)
					{
						std::string conName;
						os_int_t animationTo, animationFrom;		//Indices into array of node animations
						os_int_t mask;
						std::string condition;						//Condition from input manager
						iss >> conName;
						iss >> animationTo;
						iss >> animationFrom;
						iss >> condition;
						iss >> mask;

						AnimationConnectorFilter connector;
						connector.name = conName;
						connector.animationTo = animationTo;
						connector.animationFrom = animationFrom;
						connector.condition = condition;
						connector.mask = mask;						//Will be 1 if entire animation is played

						controller.connectors.push_back(connector);
					}

					resourceManager->registerAnimationController(entities.back(), controller);
				}
				else if(next == "/")	//End of an entity
				{
					numSkinnedMeshRenderers += numSkinnedMeshRenderersThisEntity.back();
					numSkinnedMeshRenderersThisEntity.pop_back();	//Remove the number corresponding to finished entity
					///numSkinnedMeshRenderersThisEntity = 0;

					resourceManager->entities.push_back(entities.back());	//Add the finished entity to the list of finished entities
					entities.pop_back();		//Remove the finished entity from the list of entities under construction
				}
				else if(next == "30")	//Defines a skybox as a component of an entity
				{
					std::string name;
					GLuint cubemapIndex;

					iss >> name;
					iss >> cubemapIndex;

					resourceManager->registerSkybox(entities.back(), cubemapIndex);

					//TODO - Use cube/sphere mesh as skybox - Temporary - Cube used as default
				}
				else if(next == "#")
				{
				//	iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}
			}
		} else
		{
			std::cerr << "Error Opening Scene File" << std::endl;
		}

		fleSceneBody.close();
	}






	void parseInputFile(ResourceManager * resourceManager, std::string path)
	{
		std::ifstream fle;
		fle.open(path.c_str(), std::ifstream::in);

		std::cerr << "Opening Input File: " << path.c_str() << std::endl;

		if(fle.is_open())
		{
			std::stringstream iss;
			iss << fle.rdbuf();

			while(iss)
			{
				std::string next;
				iss >> next;

				if(next == "0")			//Single Input
				{
					std::string name;
					int inputID, requiredState, type;

					iss >> name;
					iss >> inputID;
					iss >> requiredState;
					iss >> type;

					osi::Input input;
					input.setName(name);
					input.setInputMethod1(inputID, requiredState, type);
					input.setEnabledInput2(false);

					resourceManager->inputs.push_back(input);
				}
				else if(next == "1")	//Multi Input
				{
					std::string name;
					int inputID, requiredState, type;
					int inputID2, requiredState2, type2;
					bool input2Enabled;

					iss >> name;
					iss >> inputID;
					iss >> requiredState;
					iss >> type;
					iss >> inputID2;
					iss >> requiredState2;
					iss >> type2;
					iss >> input2Enabled;

					osi::Input input;
					input.setName(name);
					input.setInputMethod1(inputID, requiredState, type);
					input.setInputMethod2(inputID2, requiredState2, type2, input2Enabled);

					resourceManager->inputs.push_back(input);
				}
			}
		}
	}






	/*
		0 - Panel
		1 - Button
		2 - Window
		3 - List Box
		4 - Decoration
	*/
	void parseUIFile(ResourceManager * resourceManager, std::string path)
	{
		std::ifstream fle;
		fle.open(path.c_str(), std::ifstream::in);

		std::cerr << "Opening User Interface File: " << path.c_str() << std::endl;

		if(fle.is_open())
		{
			std::stringstream iss;
			iss << fle.rdbuf();

			UIFilter ui;

			std::vector<osi::UIPanel *> parentPnls;	//The last in the list is the current parent to be added to

			while(iss)
			{
				std::string next;
				iss >> next;

				if(next == "0")
				{
					std::string name;
					int origin, x, y, width, height, texStd, texHover, texClick, listDirection, horizontalGap, verticalGap;
					bool xExtended, yExtended, scrollBar;

					iss >> name;
					iss >> origin;
					iss >> x;
					iss >> y;
					iss >> width;
					iss >> height;
					iss >> xExtended;
					iss >> yExtended;
					iss >> texStd;
					iss >> texHover;
					iss >> texClick;
					iss >> listDirection;
					iss >> horizontalGap;
					iss >> verticalGap;
					iss >> scrollBar;

					osi::UIPanel * pnl = new osi::UIPanel(name, static_cast<osi::Origin>(origin), x, y, width, height, xExtended, yExtended, texStd, texHover, texClick, listDirection, horizontalGap, verticalGap);
				
					if(parentPnls.size() > 0)	//Add the component to the last parent pnl entered into the list
						///parentPnls.back()->addComponent(pnl);
						ui.comps.push_back({pnl, parentPnls.back()});
					else
						ui.comps.push_back({pnl, nullptr});

					parentPnls.push_back(pnl);	//Add the new panel to the parent pnls list
				}
				else if(next == "1")
				{
					std::string name, text;
					int origin, x, y, width, height, texStd, texHover, texClick;
					bool xExtended, yExtended;

					iss >> name;
					iss >> origin;
					iss >> x;
					iss >> y;
					iss >> width;
					iss >> height;
					iss >> xExtended;
					iss >> yExtended;
					iss >> texStd;
					iss >> texHover;
					iss >> texClick;
					iss >> text;

					osi::UIButton * btn = new osi::UIButton(name, static_cast<osi::Origin>(origin), x, y, width, height, xExtended, yExtended, texStd, texHover, texClick, text, osi::Origin::CENTREX_CENTREY);
				
					if(parentPnls.size() > 0)	//Add the component to the last parent pnl entered into the list
						///parentPnls.back()->addComponent(btn);
						ui.comps.push_back({btn, parentPnls.back()});
					else
						ui.comps.push_back({btn, nullptr});
				}
				else if(next == "2")
				{
					std::string name;
					int origin, x, y, width, height, texStd, texHover, texClick, texTitleBar, texMinimise, texFullscreen, texClose;
					bool xExtended, yExtended, movable, resizable, minimisable, closable;

					iss >> name;
					iss >> origin;
					iss >> x;
					iss >> y;
					iss >> width;
					iss >> height;
					iss >> xExtended;
					iss >> yExtended;
					iss >> texStd;
					iss >> texHover;
					iss >> texClick;
					iss >> movable;
					iss >> resizable;
					iss >> minimisable;
					iss >> closable;
					iss >> texTitleBar;
					iss >> texMinimise;
					iss >> texFullscreen;
					iss >> texClose;

					osi::UIWindow * win = new osi::UIWindow(name, static_cast<osi::Origin>(origin), x, y, width, height, xExtended, yExtended, texStd, texHover, texClick,
						movable, resizable, minimisable, closable, texTitleBar, texMinimise, texFullscreen, texClose);

					if(parentPnls.size() > 0)	//Add the component to the last parent pnl entered into the list
						///parentPnls.back()->addComponent(win);
						ui.comps.push_back({win, parentPnls.back()});
					else
						ui.comps.push_back({win, nullptr});
				}
				/*else if(next == "3")
				{
					std::string name;
					int origin, x, y, width, height, texStd, texHover, texClick, listDirection, horizontalGap, verticalGap;
					bool xExtended, yExtended, scrollBar;

					iss >> name;
					iss >> origin;
					iss >> x;
					iss >> y;
					iss >> width;
					iss >> height;
					iss >> xExtended;
					iss >> yExtended;
					iss >> texStd;
					iss >> texHover;
					iss >> texClick;
					iss >> listDirection;
					iss >> horizontalGap;
					iss >> verticalGap;
					iss >> scrollBar;

					osi::UIListBox * pnl = new osi::UIListBox(name, static_cast<osi::Origin>(origin), x, y, width, height, xExtended, yExtended, texStd, texHover, texClick, listDirection, horizontalGap, verticalGap);

					if(parentPnls.size() > 0)	//Add the component to the last parent pnl entered into the list
						parentPnls.back()->addComponent(pnl);
					else
						ui.rootComps.push_back(pnl);
				}*/
				else if(next == "/")	//End of a container (panel)
				{
					if(parentPnls.size() > 0)
					{
						parentPnls.pop_back();
					}
				}
				else if(next == "t")	//All textures to be made into a texture atlas for this ui
				{
					std::string path;

					iss >> path;

					ui.textures.push_back(TextureFilter(path));
				}
			}

			resourceManager->uis.push_back(ui);
		}
	}















	void buildSceneHeader(ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osi::InputManager * inputManager, osi::FontManager * fontManager)
	{
		//TODO - Temporary font loading test code
		std::cerr << "Loading Font: Consolas" << std::endl;
		fontManager->loadFont("C:/Windows/Fonts/segoeui.ttf");	//Breaks entity point light data - unknown reasoning - rendering engine lights overwritten
		std::cerr << "Font Loaded: Consolas" << std::endl;

		//std::cerr << "NUM INPUTS: " << resourceManager->inputs.size() << std::endl;
		for(int i = 0; i < resourceManager->inputs.size(); i++)
		{
			inputManager->addInput(resourceManager->inputs[i]);
		}

		std::cerr << "Added Inputs" << std::endl;

		//std::cerr << "Creating " << resourceManager->loadedTextures.size() << " Textures" << std::endl;
		for(size_t i = 0; i < resourceManager->loadedTextures.size(); i++)
		{
			GLuint texID = renderingEngine->createTexture(resourceManager->loadedTextures[i].img,
					resourceManager->loadedTextures[i].width, resourceManager->loadedTextures[i].height, false);

			for(size_t a = 0; a < resourceManager->loadedTextures[i].addressOfTexturesWaiting.size(); a++)
			{
				(*resourceManager->loadedTextures[i].addressOfTexturesWaiting[a]) = texID;
			}
		}

		std::cerr << "Created Textures" << std::endl;

		//std::cerr << "Creating " << resourceManager->loadedCubemaps.size() << " Cubemap Textures" << std::endl;
		for(size_t i = 0; i < resourceManager->loadedCubemaps.size(); i++)
		{
			GLuint texID = renderingEngine->createCubemapTexture(resourceManager->loadedCubemaps[i].imgs,
				resourceManager->loadedCubemaps[i].widths, resourceManager->loadedCubemaps[i].heights);

			for(size_t a = 0; a < resourceManager->loadedCubemaps[i].addressOfTexturesWaiting.size(); a++)
			{
				(*resourceManager->loadedCubemaps[i].addressOfTexturesWaiting[a]) = texID;
			}
		}

		std::cerr << "Created Cubemap Textures" << std::endl;

		for(size_t i = 0; i < resourceManager->uis.size(); i++)	//Generate a texture atlas for each user interface
		{
			UIFilter & ui = resourceManager->uis[i];

			std::vector<unsigned char *> imgs;
			std::vector<int> widths, heights;

			for(size_t j = 0; j < ui.textures.size(); j++)
			{
				const TextureFilter & tf = ui.textures[j];

				imgs.push_back(tf.img);
				widths.push_back(tf.width);
				heights.push_back(tf.height);
			}

			ui.texAtlasID = renderingEngine->createTextureAtlas(imgs, widths, heights, ui.texCoords);
		}

		std::cerr << "Created UI Texture Atlas" << std::endl;

		for(size_t i = 0; i < fontManager->getFonts().size(); i++)	//Generate a texture atlas for each font containing all loaded characters
		{
			osi::Font & font = fontManager->getFont(i);
			font.texAtlasID = renderingEngine->createTextureAtlas(font.imgs, font.widths, font.heights, font.texCoords, true, GL_RED, GL_RED);
		}

		std::cerr << "Created Font Texture Atlas" << std::endl;
	}






	void buildSceneBody(ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osan::AnimationEngine * animationEngine, osi::UserInterface * ui, bool genFBO)
	{
		std::cerr << "Building Scene Body, No. Loaded Meshes: " << resourceManager->loadedMeshes.size() << std::endl;

		std::vector<osr::RenderObject> staticRenderObjects;
		std::vector<osr::RenderObject> skinnedRenderObjects;
		osr::RenderObject skyboxRenderObject, uiRenderObject;
		osr::RenderObject textRenderObject;




		
		//Create render objects for use with OpenGL
		SetupManager * setupManager = new SetupManagerGL(renderingEngine, animationEngine);



		//Create render objects for all renderers
		std::vector<osan::SkinnedMesh *> skinnedMeshes;
		setupManager->setupSkinnedMeshRenderers(resourceManager->loadedMeshes, skinnedMeshes, skinnedRenderObjects);
		setupManager->setupStaticMeshRenderers(resourceManager->loadedMeshes, staticRenderObjects);

		GLuint skyboxCubemapID = 0;
		if(resourceManager->skyboxes.size() > resourceManager->defaultSkyboxIndex)
		{
			skyboxRenderObject = setupManager->setupSkybox(resourceManager->skyboxes[resourceManager->defaultSkyboxIndex]);
			skyboxCubemapID = resourceManager->skyboxes[resourceManager->defaultSkyboxIndex].cubemapID;
		}

		GLuint uiTexAtlasID = 0;
		if(resourceManager->uis.size() > resourceManager->defaultUIIndex)
		{
			uiRenderObject = setupManager->setupUI(resourceManager->uis[resourceManager->defaultUIIndex], ui, textRenderObject);
			uiTexAtlasID = resourceManager->uis[resourceManager->defaultUIIndex].texAtlasID;
		}

		GLuint textTexAtlasID = ui->getFontManager()->getFont(0).texAtlasID;

		//TODO - Invent a way of freeing mesh resources whilst keeping the paths
		///resourceManager->loadedMeshes.clear();



		std::cerr << "Building Shaders" << std::endl;

		buildShaders(setupManager, resourceManager, renderingEngine, animationEngine,
			&staticRenderObjects, &skinnedRenderObjects, skinnedMeshes,
			skyboxRenderObject, skyboxCubemapID, uiRenderObject, uiTexAtlasID,
			textRenderObject, textTexAtlasID, genFBO);

		std::cerr << "Shaders Built" << std::endl;
	}















	void buildShaders(SetupManager * setupManager, ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osan::AnimationEngine * animationEngine,
		std::vector<osr::RenderObject> * staticRenderObjects, std::vector<osr::RenderObject> * skinnedRenderObjects, std::vector<osan::SkinnedMesh *> & skinnedMeshes,
		osr::RenderObject skyboxRenderObject, GLuint skyboxCubemapID, osr::RenderObject uiRenderObject, GLuint uiTexAtlasID,
		osr::RenderObject textRenderObject, GLuint textTexAtlasID, bool genFBO)
	{
		GLuint uboIDMatrices = renderingEngine->getUBOIDMatrices();
		GLuint blockIndexMatrices = 0;			//block index ~= uniform location
		GLuint bindingPointMatrices = 0;		//0 = The binding point to be used by this ubo, should be different for each ubo

											//Uniform block index used to set which uniform buffer binding location this uniform block uses
											//glUniformBlockBinding(shaderProg, uniformBlockIndex, bindingPoint);
											//glBindBufferRange(GL_UNIFORM_BUFFER, uboBindingPoint, uboID, 0, sizeof(glm::mat4));


		std::vector<std::pair<os_uint_t, os_uint_t>> vertShaderSkinnedLocs, vertShaderStaticLocs, vertShaderOrthoLocs;
		std::stringstream vertShaderSkinnedSourceStream, vertShaderStaticSourceStream, fragShaderSourceStream, vertShaderOrthoSourceStream, fragShaderOrthoSourceStream;
		setupManager->buildVertShader(vertShaderStaticSourceStream, vertShaderStaticLocs, false, 3, 3, 2, 0, true, false, false, 0, false);
		setupManager->buildVertShader(vertShaderSkinnedSourceStream, vertShaderSkinnedLocs, false, 3, 3, 2, 0, true, false, true, 1024, false);
		setupManager->buildFragShader(fragShaderSourceStream, false, 3, 3, 2, 0, true, false, true, false);
		setupManager->buildVertShader(vertShaderOrthoSourceStream, vertShaderOrthoLocs, true, 3, 0, 2, 0, true, false, false, 0, false);
		setupManager->buildFragShader(fragShaderOrthoSourceStream, true, 3, 0, 2, 0, true, false, false, false);
		const std::string & vertShaderSkinnedSource = vertShaderSkinnedSourceStream.str();
		const std::string & vertShaderStaticSource  = vertShaderStaticSourceStream.str();
		const std::string & fragShaderSource		= fragShaderSourceStream.str();
		const std::string & vertShaderOrthoSource	= vertShaderOrthoSourceStream.str();
		const std::string & fragShaderOrthoSource	= fragShaderOrthoSourceStream.str();



		std::cerr << "\n\n\n" << vertShaderStaticSource << "\n\n\n";
		std::cerr << "\n\n\n" << fragShaderSource << "\n\n\n";

		GLuint fragShader =			renderingEngine->createShader(GL_FRAGMENT_SHADER, fragShaderSource);
		GLuint vertShaderStatic =	renderingEngine->createShader(GL_VERTEX_SHADER,	  vertShaderStaticSource);
		GLuint vertShaderSkinned =	renderingEngine->createShader(GL_VERTEX_SHADER,   vertShaderSkinnedSource);
		GLuint vertShaderOrtho =    renderingEngine->createShader(GL_VERTEX_SHADER, vertShaderOrthoSource);
		GLuint fragShaderOrtho =    renderingEngine->createShader(GL_FRAGMENT_SHADER, fragShaderOrthoSource);
		GLuint vertShaderSkybox =	renderingEngine->createShader(GL_VERTEX_SHADER,   setupManager->loadTextFile("res/default_engine_res/skybox.vert"));
		GLuint fragShaderSkybox =	renderingEngine->createShader(GL_FRAGMENT_SHADER, setupManager->loadTextFile("res/default_engine_res/skybox.frag"));
		GLuint fragShaderUI =		renderingEngine->createShader(GL_FRAGMENT_SHADER, setupManager->loadTextFile("res/default_engine_res/user_interface.frag"));
		GLuint vertShaderUI =		renderingEngine->createShader(GL_VERTEX_SHADER,	  setupManager->loadTextFile("res/default_engine_res/user_interface.vert"));
		GLuint fragShaderText =		renderingEngine->createShader(GL_FRAGMENT_SHADER, setupManager->loadTextFile("res/default_engine_res/user_interface_text.frag"));



		//MAX_NUM_LIGHTS = 128 - TODO - Find automatic way of looking this up incase changed
		GLuint uboIDLighting = renderingEngine->genUBO(sizeof(glm::vec4) * (5*16 + 5*1 + 7*16));
		GLuint bindingPointLighting = 1;
		GLuint blockIndexLighting = 0;	// = glGetUniformBlockIndex(shaderProg, "LightBlock");


		//std::cerr << "Num Point Lights: " << resourceManager->pointLights.size() << std::endl;
		//std::cerr << "Num Dir Lights: "   << resourceManager->dirLights.size() << std::endl;
		//std::cerr << "Num Spot Lights: "  << resourceManager->spotLights.size() << std::endl;

		glBindBuffer(GL_UNIFORM_BUFFER, uboIDLighting);
		GLfloat * uboLightingData = (GLfloat*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

		int numPointLights = (int)resourceManager->pointLights.size();
		if(numPointLights > 16)
			numPointLights = 16;	//Max num point lights = 16
		if(numPointLights > 0)
			memcpy(uboLightingData, &resourceManager->pointLights[0], sizeof(osr::PointLight) * numPointLights);
		
		int numDirLights = (int)resourceManager->dirLights.size();
		if(numDirLights > 1)
			numDirLights = 1;	//Max num direction lights = 1
		if(numDirLights > 0)
			memcpy(uboLightingData + 16*20, &resourceManager->dirLights[0], sizeof(osr::DirectionLight) * numDirLights);

		int numSpotLights = (int)resourceManager->spotLights.size();
		if(numSpotLights > 16)
			numSpotLights = 16;	//Max num spot lights = 16
		if(numSpotLights > 0)
			memcpy(uboLightingData + 16*20 + 1*20, &resourceManager->spotLights[0], sizeof(osr::SpotLight) * numSpotLights);


		glUnmapBuffer(GL_UNIFORM_BUFFER);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointLighting, uboIDLighting);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		renderingEngine->setLightUBO(uboIDLighting);
		renderingEngine->setLightPtrs(&resourceManager->pointLights, &resourceManager->dirLights, &resourceManager->spotLights);






		//Block Index Currently Un-used Variable by Setup-Manager

		//ERROR - Called twice due to editor
		if(genFBO)
		{
			setupManager->setupDeferredShadingShaders(renderingEngine, renderingEngine->getFramebufferWidth(), renderingEngine->getFramebufferHeight(),
				blockIndexLighting, bindingPointLighting, uboIDLighting, numDirLights, numPointLights, numSpotLights);
		
			setupManager->setupShadowMapShaders(renderingEngine, 1024, 1024);
		}

		setupManager->setupShaders(staticRenderObjects, vertShaderStatic, fragShader, blockIndexMatrices, uboIDMatrices, bindingPointMatrices,
				blockIndexLighting, bindingPointLighting, uboIDLighting, numDirLights, numPointLights, numSpotLights, vertShaderStaticLocs);

		setupManager->setupShaders(skinnedRenderObjects, vertShaderSkinned, fragShader, blockIndexMatrices, uboIDMatrices, bindingPointMatrices,
			blockIndexLighting, bindingPointLighting, uboIDLighting, numDirLights, numPointLights, numSpotLights, vertShaderSkinnedLocs);

		if(skyboxCubemapID > 0)
			setupManager->setupSkyboxShaders(skyboxRenderObject, skyboxCubemapID, vertShaderSkybox, fragShaderSkybox, blockIndexMatrices, uboIDMatrices, bindingPointMatrices);
		
		if(uiTexAtlasID > 0)
			setupManager->setupUIShaders(uiRenderObject, uiTexAtlasID, vertShaderUI, fragShaderUI);

		if(textTexAtlasID > 0 && uiTexAtlasID > 0)
			setupManager->setupTextShaders(textRenderObject, textTexAtlasID, vertShaderUI, fragShaderText);



		//TO BE IMPLEMENTED - COMPLEMENTS NEW SETUP SHADER METHOD
		/*std::vector<osan::AnimationController> & vecRef = animationEngine->getAnimationControllers();
		for(size_t a = 0; a < vecRef.size(); a++)
		{
			vecRef[a].skinnedMesh.shaderProg = shaderProg;
		}*/




		renderingEngine->deleteShader(fragShader);
		renderingEngine->deleteShader(vertShaderStatic);
		renderingEngine->deleteShader(vertShaderSkinned);
		renderingEngine->deleteShader(vertShaderSkybox);
		renderingEngine->deleteShader(fragShaderSkybox);
		renderingEngine->deleteShader(fragShaderUI);
		renderingEngine->deleteShader(vertShaderUI);
		renderingEngine->deleteShader(fragShaderText);
	}






	/*
	Save the current scene to the file given by the specified path
	*/
	void saveScene(const std::string & path, ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osan::AnimationEngine * animationEngine, osp::PhysicsEngine * physicsEngine)
	{
		saveSceneHeader(path + ".ossh", resourceManager);
		saveSceneBody(path + ".ossb", resourceManager, renderingEngine, animationEngine, physicsEngine);
	}

	void saveSceneHeader(const std::string & path, ResourceManager * resourceManager)
	{
		std::ofstream fle;
		fle.open(path.c_str(), std::ofstream::out);

		for(auto cubemap : resourceManager->loadedCubemaps)	//Write the cubemaps to file
		{
			fle << "c";
			for(auto texPath : cubemap.paths)
			{
				fle << " " << texPath;
			}
			fle << std::endl;
		}

		for(auto mesh : resourceManager->loadedMeshes)		//Write the meshes to file
		{
			fle << "m " << mesh.path << std::endl;
		}

		//TODO - Write the textures to file - complicated by fact not all need to be written due to meshes

		fle.close();
	}

	/*
	* Types in Files:
	* Component IDs:
	* 0-9:   Mesh      -> 1:MeshRenderer
	* 10-12: Lights    -> 10:DirLight, 11:PointLight, 12:SpotLight
	* 20-29: Animation -> 20:SkinnedMeshRenderer, 21:Animation Controller
	*/
	void saveSceneBody(const std::string & path, ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osan::AnimationEngine * animationEngine, osp::PhysicsEngine * physicsEngine)
	{
		std::ofstream fle;
		fle.open(path.c_str(), std::ofstream::out);

		for(auto e : resourceManager->entities)
		{
			fle << "e " << e.name << " " << e.transform->translation.x << " " << e.transform->translation.y << " " << e.transform->translation.z << " " <<
				e.transform->scale.x << " " << e.transform->scale.y << " " << e.transform->scale.z << " " << e.transform->orientation.x << " " << e.transform->orientation.y << " " <<
				e.transform->orientation.z << " " << e.transform->orientation.w;

			for(size_t meshIndex = 0; meshIndex < resourceManager->loadedMeshes.size(); meshIndex++)
			{
				auto mesh = resourceManager->loadedMeshes[meshIndex];

				for(auto staticRenderer : mesh.meshRenderers)
				{
					if(staticRenderer.eTransform == e.transform)
						fle << " 1 " << staticRenderer.name << " " << meshIndex << " 0";	//0 = shader program, 0 as not supported atm
				}

				for(auto skinnedRenderer : mesh.skinnedMeshRenderers)
				{
					if(skinnedRenderer.eTransform == e.transform)
						fle << " 20 " << skinnedRenderer.name << " " << meshIndex << " 0";	//0 = shader program, 0 as not supported atm
				}
			}

			for(auto indexNamePair : e.dirLightIndexes)
			{
				auto light = resourceManager->dirLights[indexNamePair.first];
				fle << " 10 " << indexNamePair.second << " " << light.dir.x << " " << light.dir.y << " " << light.dir.z << " " << light.ambientColour.x << " " << light.ambientColour.y << " " << light.ambientColour.z << " " <<
					light.diffuseColour.x << " " << light.diffuseColour.y << " " << light.diffuseColour.z << " " << light.specularColour.x << " " << light.specularColour.y << " " << light.specularColour.z <<
					" " << light.intensity;
			}

			for(auto indexNamePair : e.pointLightIndexes)
			{
				auto light = resourceManager->pointLights[indexNamePair.first];
				glm::vec3 pos = glm::vec3(light.relativePos) - e.transform->translation;
				fle << " 11 " << indexNamePair.second << " " << pos.x << " " << pos.y << " " << pos.z << " " <<
					light.ambientColour.x << " " << light.ambientColour.y << " " << light.ambientColour.z << " " <<
					light.diffuseColour.x << " " << light.diffuseColour.y << " " << light.diffuseColour.z << " " <<
					light.specularColour.x << " " << light.specularColour.y << " " << light.specularColour.z <<
					" " << light.constant << " " << light.linear << " " << light.quadratic << " " << light.intensity;
			}

			for(auto indexNamePair : e.spotLightIndexes)
			{
				auto light = resourceManager->spotLights[indexNamePair.first];
				glm::vec3 pos = glm::vec3(light.relativePos) - e.transform->translation;
				fle << " 12 " << indexNamePair.second << " " << pos.x << " " << pos.y << " " << pos.z << " " <<
					light.dir.x << " " << light.dir.y << " " << light.dir.z << " " << light.ambientColour.x << " " << light.ambientColour.y << " " << light.ambientColour.z
					<< " " << light.diffuseColour.x << " " << light.diffuseColour.y << " " << light.diffuseColour.z << " " <<
					light.specularColour.x << " " << light.specularColour.y << " " << light.specularColour.z << " " << light.cosInnerCutoff << " " << light.cosOuterCutoff <<
					" " << light.constant << " " << light.linear << " " << light.quadratic << " " << light.intensity;
			}

			fle << " /" << std::endl;
		}

		fle.close();
	}
}









