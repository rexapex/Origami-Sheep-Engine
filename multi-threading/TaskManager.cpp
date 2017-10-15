/*
s * TaskManager.cpp
 *
 *  Created on: 9 Oct 2015
 *      Author: james_000
 */

#include "../glm/detail/type_mat.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtx/quaternion.hpp"

#include "../input/Input.h"
#include "../maths/Transform.h"
#include "../maths/GameMaths.h"

#include <cmath>

#include "../io-resource-management/Mesh.h"

#include <sstream>
#include <iostream>

#include "TaskManager.h"
#include "../io-resource-management/Mesh.h"
#include "../io-resource-management/ResourceManager.h"
#include "../io-resource-management/ProjectIO.h"
#include "../GLFWFunctions.h"
#include "../JNAWrapperFunctions.h"

#include "../ScriptTest.h"

TaskManager::TaskManager(WindowManager * windowManager)
{
	//script::editor_on_startup();

	GLFWwindow * win = (GLFWwindow*)windowManager->getWindow();
	this->windowManager = windowManager;

	numFrames = framesPerSecond = 0;
	millisPerFrame = 0.0;
	lastTimeSecondsPerSecond = lastTimeSecondsPerUpdate = windowManager->getTimeSeconds();

	//int windowWidth, windowHeight;
	//glfwGetWindowSize(win, &windowWidth, &windowHeight);
	os_int_t framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize(win, &framebufferWidth, &framebufferHeight);

	nextEntityID = 0;
	window = win;
	renderingEngine = new osr::RenderingEngine(framebufferWidth, framebufferHeight);
	animationEngine = new osan::AnimationEngine;
	physicsEngine	= new osp::PhysicsEngine;
	inputManager	= new osi::InputManager(win);
	userInterface	= new osi::UserInterface(framebufferWidth, framebufferHeight, inputManager);
	//gui = new osi::GUI(framebufferWidth, framebufferHeight);
	windowManager->setupCallbackClasses(userInterface, renderingEngine, inputManager);



	//GUI CODE:
	//gui->init("res/default_engine_res/datafiles");
	//gui->loadScheme("Generic.scheme");
	//gui->loadScheme("WindowsLook.scheme");
	//gui->loadScheme("TaharezLook.scheme");
	//gui->loadScheme("VanillaSkin.scheme");
	//gui->loadScheme("VanillaCommonDialogs.scheme");
	//gui->loadScheme("AlfiskoSkin.scheme");
	//gui->createContext("OSEUI.layout");
	//gui->setMouseCursor("WindowsLook/MouseArrow");
	//gui->setFont("DejaVuSans-12");
	//gui->showMouseCursor();
	//CEGUI::PushButton * btnTest = static_cast<CEGUI::PushButton *>(gui->createWidget("WindowsLook/Button", glm::vec4(0.5f, 0.5f, 0.1f, 0.05f), glm::vec4(0, 0, 0, 0), "TEST_BUTTON"));
	//btnTest->setText("Hello World!");
	//renderingEngine->setGUI(gui);

//	int framebufferWidth, framebufferHeight;
//	glfwGetFramebufferSize(win, &framebufferWidth, &framebufferHeight);
	glViewport(0, 0, framebufferWidth, framebufferHeight);

	renderingEngine->initDepthTesting();
	renderingEngine->initFaceCulling();
	renderingEngine->setFaceCulling(true);

	glEnable(GL_MULTISAMPLE);

	//glfwWindowHint(GLFW_SAMPLES, 4);

	//jnaPtrs.renderingEngine = renderingEngine;

	std::cerr << "Initialised Task Manager" << std::endl;



	//mainCam = new osm::CameraFPSDefault(renderingEngine->mainCamTransform);
	mainCam = new osm::CameraFPSDefault();
	renderingEngine->setCamera(mainCam);
	osm::translate(mainCam->getTransform(), glm::vec3(0.0f, 0.0f, -1.0f));

	osp::RigidBody * rigidbody1, * rigidbody2;
	rigidbody1 = new osp::RigidBody;
	rigidbody1->velocity = glm::vec3(0, 0, 0);
	rigidbody2 = new osp::RigidBody;
	rigidbody2->velocity = glm::vec3(0, -2, 0);

	osp::SphereCollider col1, col2;
	col1.rigidbody = rigidbody1;
	col2.rigidbody = rigidbody2;
	col1.parentTransform = renderingEngine->mainCamTransform;
	col2.parentTransform = new osm::Transform;
	osm::translate(col1.parentTransform, glm::vec3(0, 0, 0));
	osm::translate(col2.parentTransform, glm::vec3(0, 50, 0));
	//col1.lastParentPosition = renderingEngine->mainCamTransform->translation;
	//col2.lastParentPosition = col2.parentTransform->translation;
	col1.radius = 5.0f;
	col2.radius = 5.0f;
	col1.colliderPosition = glm::vec3(0, 0, 0);
	col2.colliderPosition = glm::vec3(0, 0, 0);

	physicsEngine->addSphereCollider(col1);
	physicsEngine->addSphereCollider(col2);






	//TODO - Remove this test code after testing
	{
		//std::cerr << "Call to Load Scene" << std::endl;
		osrm::ResourceManager * resourceManager = new osrm::ResourceManager("zproj_oseditor/");

		osrm::loadScene(resourceManager, "zproj_oseditor/scenes/scene1");
		osrm::buildSceneHeader(resourceManager, renderingEngine, inputManager, userInterface->getFontManager());
		osrm::buildSceneBody(resourceManager, renderingEngine, animationEngine, userInterface);

		script::editor_init(resourceManager, renderingEngine, physicsEngine, animationEngine, inputManager, userInterface, resourceManager->entities);

		//delete resourceManager;	//Bad idea - Deleting resource manager frees resources
	}






	//Terrain Temporary
	/*osutil::GridCell * grid = new osutil::GridCell[512];

	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			for(int k = 0; k < 8; k++)
			{
				osutil::GridCell cell;

				cell.p[0] = glm::vec3(i + 0.0, k + 0.0, j + 0.0);
				cell.p[1] = glm::vec3(i + 1.0, k + 0.0, j + 0.0);
				cell.p[2] = glm::vec3(i + 1.0, k + 0.0, j + 1.0);
				cell.p[3] = glm::vec3(i + 0.0, k + 0.0, j + 1.0);
				cell.p[4] = glm::vec3(i + 0.0, k + 1.0, j + 0.0);
				cell.p[5] = glm::vec3(i + 1.0, k + 1.0, j + 0.0);
				cell.p[6] = glm::vec3(i + 1.0, k + 1.0, j + 1.0);
				cell.p[7] = glm::vec3(i + 0.0, k + 1.0, j + 1.0);

				cell.val[0] = (double)k/8;
				cell.val[1] = (double)k/8;
				cell.val[2] = (double)k/8;
				cell.val[3] = (double)k/8;
				cell.val[4] = (double)(k+1)/8;
				cell.val[5] = (double)(k+1)/8;
				cell.val[6] = (double)(k+1)/8;
				cell.val[7] = (double)(k+1)/8;

				grid[i * 64 + j * 8 + k] = cell;
			}
		}
	}

	grid[64 * 4 + 8 * 4 + 5].val[0] -= 0.05;
	grid[64 * 4 + 8 * 4 + 5].val[1] -= 0.05;
	grid[64 * 4 + 8 * 4 + 5].val[2] -= 0.05;
	grid[64 * 4 + 8 * 4 + 5].val[3] -= 0.05;
	grid[64 * 4 + 8 * 4 + 5].val[4] -= 0.1;
	grid[64 * 4 + 8 * 4 + 5].val[5] -= 0.1;
	grid[64 * 4 + 8 * 4 + 5].val[6] -= 0.1;
	grid[64 * 4 + 8 * 4 + 5].val[7] -= 0.1;

	grid[64 * 4 + 8 * 4 + 5].val[0] -= 0.1;
	grid[64 * 4 + 8 * 4 + 5].val[1] -= 0.1;
	grid[64 * 4 + 8 * 4 + 5].val[2] -= 0.1;
	grid[64 * 4 + 8 * 4 + 5].val[3] -= 0.1;
	grid[64 * 4 + 8 * 4 + 5].val[4] -= 0.1;
	grid[64 * 4 + 8 * 4 + 5].val[5] -= 0.1;
	grid[64 * 4 + 8 * 4 + 5].val[6] -= 0.1;
	grid[64 * 4 + 8 * 4 + 5].val[7] -= 0.1;

	std::vector<osutil::Triangle> tris;
	osutil::marchingCubes(grid, 512, tris);
	delete[] grid;

	GLuint vbo, vao;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, tris.size() * 9 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

	GLfloat * buffer = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for(int i = 0; i < tris.size(); i++)
	{
		buffer[i*9]   = tris[i].p[0].x;
		buffer[i*9+1] = tris[i].p[0].y;
		buffer[i*9+2] = tris[i].p[0].z;

		buffer[i*9+3] = tris[i].p[1].x;
		buffer[i*9+4] = tris[i].p[1].y;
		buffer[i*9+5] = tris[i].p[1].z;

		buffer[i*9+6] = tris[i].p[2].x;
		buffer[i*9+7] = tris[i].p[2].y;
		buffer[i*9+8] = tris[i].p[2].z;

		//std::cerr << "Tri: " << tris[i].p[0].x << ", " << tris[i].p[0].y << ", " << tris[i].p[0].z << "\n";
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	//glBufferSubData(GL_ARRAY_BUFFER, 0, 9 * sizeof(GL_FLOAT), data);

	osr::RenderObject renderObject;
	renderObject.IBO = renderObject.instanceVBO = renderObject.numInstances = 0;
	renderObject.numVerticesPerRenderCall = new GLuint[1];
	renderObject.numVerticesPerRenderCall[0] = tris.size() * 3;
	renderObject.VBOs.push_back(vbo);

	osr::Material mtl;
	renderObject.materials = new osr::Material[1];
	renderObject.materials[0] = mtl;

	glGenBuffers(1, &vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	renderObject.VAO = vao;

	osrm::SetupManagerGL * s = new osrm::SetupManagerGL(renderingEngine, animationEngine);
	GLuint vertShader = renderingEngine->createShader(GL_VERTEX_SHADER, s->loadTextFile("res/default_engine_res/basic_test.vert"));
	GLuint fragShader = renderingEngine->createShader(GL_FRAGMENT_SHADER, s->loadTextFile("res/default_engine_res/basic_test.frag"));
	std::vector<GLuint> shaders;
	shaders.push_back(vertShader);
	shaders.push_back(fragShader);
	osr::ShaderGroup shaderGroup;
	GLuint shaderProg = renderingEngine->createShaderProgram(shaders);
	glUseProgram(shaderProg);
	glUniformMatrix4fv(glGetUniformLocation(shaderProg, "camToClipMat"), 1, GL_FALSE, glm::value_ptr(renderingEngine->getPerspectiveProjectionMatrix()));

	osr::ShaderProgram p;
	p.program = shaderProg;
	shaderGroup.shaderPrograms.push_back(p);
	shaderGroup.renderObjects.push_back(renderObject);
	shaderGroup.UBO = 0;

	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	renderingEngine->addRenderable(0, shaderGroup);*/



	renderingEngine->setDepthTesting(false);
	renderingEngine->setFaceCulling(true);
}

TaskManager::~TaskManager()
{
	delete renderingEngine;
	delete inputManager;
	delete animationEngine;
	delete userInterface;
}

void TaskManager::setupThreads()	//Create a thread for all but one core since main thread runs on first core
{

}

void TaskManager::startThreads()	//Start the game
{
	updateThread();
}

void TaskManager::updateThread()	//Update correct thread
{
	while(!glfwWindowShouldClose(window))
	{
		double deltaTimeSeconds = calcDeltaTime();

		std::stringstream ssTitle;
		ssTitle << "FPS: " << framesPerSecond;
		windowManager->setTitle(ssTitle.str());
//		script::editor_update(framesPerSecond, renderingEngine);

		osi::UICallback uiCallback {osi::UICallback::NONE};

		inputManager->updateInput();
		userInterface->updateUI(&uiCallback);

		if(uiCallback == osi::UICallback::POST_UPDATE_UI)
			renderingEngine->threadsafe_postShouldUpdateUI();



		{	//Temporary Camera Code
			const float & deltaX = (float)inputManager->getDeltaX();
			const float & deltaY = (float)inputManager->getDeltaY();


			mainCam->update(deltaTimeSeconds, deltaX, deltaY, inputManager->getInputValue("allow_cam_rotation"), inputManager->getInputValue("move_forward"),
				inputManager->getInputValue("move_back"), inputManager->getInputValue("move_right"),
				inputManager->getInputValue("move_left"), inputManager->getInputValue("move_up"), inputManager->getInputValue("move_down"));


			//
			//				TODO - Window manager needs to know about gui so it can update input
			//					   Possibly change render engine framebufferSize callback to remove gui framebuffer resize function


			//TODO - Temporary animation switcher
			for(int a = 0; a < animationEngine->getAnimationControllers().size(); a++)
			{
				osan::AnimationController & animController = animationEngine->getAnimationController(a);

				for(int c = 0; c < animController.connectors.size(); c++)
				{
					osan::Connector & connector = animController.connectors[c];

					//std::cerr << animController.currentAnimation << ", " << connector.animationFrom << std::endl;

					//Animation from of -1 signifies the animation can be switched to from any other animation
					if(connector.animationFrom == -1 || (animController.currentAnimation == connector.animationFrom))
					{
						if(inputManager->getInputValue(connector.condition))
						{
							animationEngine->setAnimation(a, connector.animationTo, (float)lastTimeSecondsPerUpdate, connector.bitmask);
						}
					}
				}
			}
		}

		physicsEngine->stepSimulation(deltaTimeSeconds);

		animationEngine->updateSkeletalAnimations((float)lastTimeSecondsPerUpdate);

		if(renderingEngine->getShouldUpdateUI())
			renderingEngine->updateUserInterface(userInterface);
		if(renderingEngine->getShouldUpdateUBOLighting())
			renderingEngine->updateUBOLighting();
		renderingEngine->updateSkinnedAnimations(animationEngine);
		renderingEngine->render();

		/*std::stringstream ss;
		ss << "FPS: ";
		ss << framesPerSecond;
		renderingEngine->renderText(userInterface->getFontManager(), ss.str(), 205, 630, 0.5, glm::vec3(1.0, 1.0, 1.0));
		ss.str("");
		ss.clear();
		ss << "MFP: ";
		ss << millisPerFrame;
		renderingEngine->renderText(userInterface->getFontManager(), ss.str(), 205, 595, 0.5, glm::vec3(1.0, 1.0, 1.0));*/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}



void TaskManager::calcFPS(double currentTime)
{
	numFrames++;
	if(currentTime - lastTimeSecondsPerSecond >= 1.0)
	{
		framesPerSecond = numFrames;
		millisPerFrame = 1000.0/double(numFrames);
		numFrames = 0;
		lastTimeSecondsPerSecond += 1.0;
	}
}



double TaskManager::calcDeltaTime()	//Calculates and returns the delta time in seconds
{
	double time = windowManager->getTimeSeconds();
	double delta = time - lastTimeSecondsPerUpdate;			//Calculate the time passed between frames

	calcFPS(time);

	lastTimeSecondsPerUpdate = time;

	return delta;
}