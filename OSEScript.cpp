#include "OSEScript.h"

namespace script
{
	void init(osrm::ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osp::PhysicsEngine * physicsEngine,
		osan::AnimationEngine * animationEngine, osi::InputManager * inputManager, std::vector<osrm::Entity> & entities)
	{
		osrm::ResourceManager * projResourceManager = new osrm::ResourceManager("zproj_test/");

		osi::UserInterface * projUI = new osi::UserInterface(0, 0, inputManager);

		osrm::loadScene(projResourceManager, "zproj_test/scenes/scene1");
		osrm::buildSceneHeader(projResourceManager, renderingEngine, inputManager, projUI->getFontManager());
		osrm::buildSceneBody(projResourceManager, renderingEngine, animationEngine, projUI, false);
		renderingEngine->updateUBOLighting();

	/*	CEGUI::Listbox * lstEntityViewer = dynamic_cast<CEGUI::Listbox *>(gui->getWidget("root/entityViewer"));

		if(lstEntityViewer == nullptr)
		{
			std::cerr << "ERROR: lstEntityViewer is nullptr" << std::endl;
			return;
		}*/

	//	for(osrm::Entity & entity : projResourceManager->entities)
	//	{
			//CEGUI::ListboxItem * itm = new CEGUI::ListboxTextItem("test", 0, 0, false, false);
	//		CEGUI::ListboxItem * itm = dynamic_cast<CEGUI::ListboxItem *>(gui->createWidget("WindowsLook/ListboxItem", glm::vec4(0, 0, 100, 100), glm::vec4(0, 0, 0, 0)));
	//		lstEntityViewer->addItem(itm);
	//	}
	}
}
