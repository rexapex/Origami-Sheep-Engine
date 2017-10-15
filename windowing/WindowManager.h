#pragma once

#include "../rendering/RenderingEngine.h"
#include "../input/InputManager.h"
#include "../user-interface/UserInterface.h"

#include <string>

class WindowManager
{
public:
	WindowManager();
	~WindowManager();

	virtual int		setMouseVisibility(int value) = 0;
	
	virtual void	setWindowSize(int width, int height) = 0;
	virtual void	setWindowPos(int x, int y) = 0;

	virtual void	setTitle(const std::string & title) = 0;

	virtual void	setNumSamples(int numSamples) = 0;

	virtual void *  getWindow() const = 0;
	virtual void	createWindow(int windowMode) = 0;

	virtual void	update() = 0;

	virtual double  getTimeSeconds() const = 0;

	virtual void	setupCallbackClasses(osi::UserInterface * ui, osr::RenderingEngine * r, osi::InputManager * i) = 0;	//Sets the class to receive callbacks for changes to the window & input
private:
	virtual int		initWindowingToolkit() const = 0;
};

