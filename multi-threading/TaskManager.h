/*
 * TaskManager.h
 *
 *  Created on: 9 Oct 2015
 *      Author: james_000
 */

#ifndef MULTI_THREADING_TASKMANAGER_H_
#define MULTI_THREADING_TASKMANAGER_H_

#include <vector>

#include "../physics/PhysicsEngine.h"
#include "../rendering/RenderingEngine.h"
#include "../input/InputManager.h"
#include "../user-interface/UIComponent.h"
#include "../user-interface/UIButton.h"
#include "../user-interface/UIPanel.h"
#include "../user-interface/UIWindow.h"
#include "../user-interface/UserInterface.h"
#include "../animation/AnimationEngine.h"
#include "../io-resource-management/Mesh.h"
#include "../windowing/WindowManager.h"
#include "../maths/Camera.h"
#include "../maths/CameraFPSDefault.h"
#include "../terrain/Terrain.h"

typedef unsigned int ID;

//Each thread contains a subset of the entity components
//All of the components for one entity will be contained in the same
//thread to reduce the amount of communication between threads
/*typedef struct Thread
{
	std::vector<osp::RigidBody> rigidBodies;//List of rigid bodies (physics objects)
	std::vector<osp::SoftBody> softBodies;	//List of soft bodies (physics objects)
} Thread;
*/

class TaskManager
{
public:
	TaskManager(WindowManager * windowManager);
	virtual ~TaskManager();

	void setupThreads();	//Create threads for computer's cores
	void startThreads();	//Start the threads and therefore start the game
private:
	GLFWwindow * window;
	WindowManager * windowManager;

	ID nextEntityID;	//Increments by 1 every time and entity is added

	osr::RenderingEngine * renderingEngine;	//The rendering engine used by the main thread
	osi::InputManager * inputManager;		//Manages input from the GLFW window
	osi::UserInterface * userInterface;
	//osi::GUI * gui;
	osan::AnimationEngine * animationEngine;
	osp::PhysicsEngine * physicsEngine;
	

	osm::Camera * mainCam;


	/*
	std::vector<Thread> threads;				//List of all threads, one for each core, does not include main thread
	*/

	double lastTimeSecondsPerUpdate;			//The system time of the last frame in seconds
	double lastTimeSecondsPerSecond;			//The system time of the last second in seconds
	double millisPerFrame;						//The number of milliseconds it takes to render one frame
	int numFrames;
	int framesPerSecond;						//The number of frames rendered in one second
	double calcDeltaTime();						//Calculates and returns the delta time in seconds
	void calcFPS(double currentTime);			//Calculates the fps and the mpf


	void updateThread();	//Do updating of all components in specific order, every thread has copy of function
};

#endif /* MULTI_THREADING_TASKMANAGER_H_ */
