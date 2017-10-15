#pragma once

#include "animation\AnimationEngine.h"
#include "physics\PhysicsEngine.h"
#include "rendering\RenderingEngine.h"
#include "user-interface\UserInterface.h"
#include "user-interface\UIList.h"
#include "user-interface\UISlider.h"
#include "user-interface\UILabel.h"
#include "user-interface\UIButton.h"
#include "user-interface\UITextField.h"
#include "io-resource-management\ProjectIO.h"
#include "io-resource-management\ResourceManager.h"

#include <string>
#include <vector>
#include <iostream>

namespace script
{
	osi::UserInterface * editorUI;
	osr::RenderingEngine * rEngine;
	osp::PhysicsEngine * pEngine;
	osan::AnimationEngine * animEngine;
	osrm::ResourceManager * projResManager;

	osi::UIList<osrm::Entity *> * lstLeft1;
	osi::UIList<osrm::Entity> * lstRight1;

	osi::UILabel * lblFPS;

	class PnlLeft1ComponentPressedCallback : public osi::ComponentPressedCallback
	{
	public:
		void componentPressed(osi::UIComponent * comp) const;
	};

	class FloatRefSliderCallback : public osi::SliderCallback
	{
	public:
		FloatRefSliderCallback(float & valToModify) : valToModify(valToModify) {}
		void valueChanged(osi::UISlider * sld, float val) const;
		float & valToModify;
	};

	class StringRefCallback : public osi::StringCallback
	{
	public:
		StringRefCallback(std::string & valToModify) : valToModify(valToModify) {}
		void valueChanged(osi::UITextField * txt, std::string & str) const;
		std::string & valToModify;
	};

	class EditorButtonPressedCallback : public osi::ButtonPressedCallback
	{
		void buttonPressed(osi::UIButton * btn) const;
	};

	void editor_on_startup()
	{
		std::cerr << std::endl << "Welcome to Origami Sheep Engine!" << std::endl;
		//std::cerr << "Enter n to create a new project or o to open an existing project" << std::endl;
		
		/*while(true)
		{
			std::string input;
			//getline(std::cin, input);

			std::cerr << input << "\n";

			if(input == "n")
			{
				getline(std::cin, input);

			} else if(input == "o")
			{
				getline(std::cin, input);
			}

			std::cerr << input << "\n";
			break;
		}*/
	}

	void editor_update(os_int_t framesPerSecond, osr::RenderingEngine * rEngine)
	{
		std::stringstream ssTitle;
		ssTitle << "FPS: " << framesPerSecond;
		lblFPS->setText(ssTitle.str());
		//rEngine->threadsafe_postShouldUpdateUI();
	}

	void editor_init(osrm::ResourceManager * resourceManager, osr::RenderingEngine * renderingEngine, osp::PhysicsEngine * physicsEngine,
		osan::AnimationEngine * animationEngine, osi::InputManager * inputManager, osi::UserInterface * ui, std::vector<osrm::Entity> & entities)
	{
		editorUI = ui;
		rEngine = renderingEngine;
		pEngine = physicsEngine;
		animEngine = animationEngine;

		osrm::ResourceManager * projResourceManager = new osrm::ResourceManager("zproj_test/");
		projResManager = projResourceManager;

		osi::UserInterface * projUI = new osi::UserInterface(editorUI->getFramebufferWidth(), editorUI->getFramebufferHeight(), inputManager);


		osi::UIPanel * pnlTransform = static_cast<osi::UIPanel *>(editorUI->getComponent("pnlTools"));
		lblFPS = new osi::UILabel("lblFPS", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "", osi::Origin::CENTREX_CENTREY);
		//pnlTransform->addComponent(lblFPS);
		editorUI->addComponent(pnlTransform, lblFPS);

		EditorButtonPressedCallback * cllbkButtons = new EditorButtonPressedCallback;
		osi::UIPanel * pnlTop1 = static_cast<osi::UIPanel *>(editorUI->getComponent("pnlTop1"));
		osi::UIButton * btnImport = static_cast<osi::UIButton *>(pnlTop1->getComponent("btnImport"));
		osi::UIButton * btnSave = static_cast<osi::UIButton *>(pnlTop1->getComponent("btnSave"));
		btnImport->addButtonPressedCallback(cllbkButtons);
		btnSave->addButtonPressedCallback(cllbkButtons);


		osi::UIPanel * pnlLeft1 = static_cast<osi::UIPanel *>(editorUI->getComponent("pnlLeft1"));
		lstLeft1 = new osi::UIList<osrm::Entity *>(*pnlLeft1);

		osrm::loadScene(projResourceManager, "zproj_test/scenes/scene1");
		osrm::buildSceneHeader(projResourceManager, renderingEngine, inputManager, projUI->getFontManager());
		osrm::buildSceneBody(projResourceManager, renderingEngine, animationEngine, projUI, false);
		renderingEngine->updateUBOLighting();





		for(size_t i = 0; i < projResourceManager->entities.size(); i++)
		{
			//Add a button into the entity list
			std::string name = projResourceManager->entities[i].name;

			//std::cerr << "Entity Name: " << projResourceManager->entities[i].name << ", ID: " << projResourceManager->entities[i].entityID << std::endl;
			osi::UIButton * btn1 = new osi::UIButton(name, osi::Origin::LEFT_CENTREY, 0, 0, 80, 25, false, false, -1, -1, -1, name, osi::Origin::LEFT_CENTREY);

			lstLeft1->addComponent(editorUI, &projResourceManager->entities[i], btn1);
		}

		pnlLeft1->addComponentPressedCallback(new PnlLeft1ComponentPressedCallback);
		editorUI->setScrollBarShown(pnlLeft1, true, 3, 3, 3);
	//	pnlLeft1->setScrollBarShown(true, 3, 3, 3);

		renderingEngine->fullUpdateUserInterface(editorUI);
		renderingEngine->fullUpdateUserInterfaceText(editorUI);
	}

	
	void PnlLeft1ComponentPressedCallback::componentPressed(osi::UIComponent * comp) const
	{
		osi::UIPanel * pnlRight1 = static_cast<osi::UIPanel *>(editorUI->getComponent("pnlRight1"));
		///pnlRight1->clear(editorUI->getComponents());
		if(lstRight1 != nullptr)
		{
			lstRight1->clear(editorUI);
			delete lstRight1;
		}

		lstRight1 = new osi::UIList<osrm::Entity>(*pnlRight1);

		osrm::Entity & entity = **(lstLeft1->getLinkedElement(comp));

		osi::UIPanel * pnlDefault = new osi::UIPanel("pnlDefault", osi::Origin::RIGHT_CENTREY, 0, 0, 80, 125, false, false, 8, 8, 8, 1, 5, 5);
		osi::UIPanel * pnlName     = new osi::UIPanel("pnlName", osi::Origin::LEFT_CENTREY, 0, 0, 170, 25, false, false, -1, -1, -1, 0, 0, 0);
		osi::UILabel * lblName = new osi::UILabel("lblName", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Name:", osi::Origin::CENTREX_CENTREY);
		osi::UITextField * txtName = new osi::UITextField(entity.name, osi::Origin::LEFT_CENTREY, 0, 0, 90, 25, false, false, -1, -1, -1);
		osi::UIPanel * pnlTranslation = new osi::UIPanel("pnlTranslation", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
		osi::UIPanel * pnlOrientation = new osi::UIPanel("pnlOrientation", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
		osi::UIPanel * pnlScale = new osi::UIPanel("pnlScale", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
		osi::UILabel * lblTranslation = new osi::UILabel("lblTranslation", osi::Origin::RIGHT_CENTREY, 0, 0, 25, 25, false, false, 5, 5, 5, "", osi::Origin::CENTREX_CENTREY);
		osi::UILabel * lblOrientation = new osi::UILabel("lblOrientation", osi::Origin::RIGHT_CENTREY, 0, 0, 25, 25, false, false, 7, 7, 7, "", osi::Origin::CENTREX_CENTREY);
		osi::UILabel * lblScale = new osi::UILabel("lblScale", osi::Origin::RIGHT_CENTREY, 0, 0, 25, 25, false, false, 6, 6, 6, "", osi::Origin::CENTREX_CENTREY);
		txtName->setText(entity.name);
		/*pnlName->addComponent(lblName);
		pnlName->addComponent(txtName);
		pnlTranslation->addComponent(lblTranslation);
		pnlOrientation->addComponent(lblOrientation);
		pnlScale->addComponent(lblScale);
		pnlDefault->addComponent(pnlName);
		pnlDefault->addComponent(pnlTranslation);
		pnlDefault->addComponent(pnlOrientation);
		pnlDefault->addComponent(pnlScale);*/
		editorUI->addComponent(pnlName, lblName);
		editorUI->addComponent(pnlName, txtName);
		editorUI->addComponent(pnlTranslation, lblTranslation);
		editorUI->addComponent(pnlOrientation, lblOrientation);
		editorUI->addComponent(pnlScale, lblScale);
		editorUI->addComponent(pnlDefault, pnlName);
		editorUI->addComponent(pnlDefault, pnlTranslation);
		editorUI->addComponent(pnlDefault, pnlOrientation);
		editorUI->addComponent(pnlDefault, pnlScale);

		txtName->addStringCallback(new StringRefCallback(entity.name));

		lstRight1->addComponent(editorUI, entity, pnlDefault);

		for(size_t i = 0; i < entity.meshRenderers.size(); i++)
		{
			osrm::MeshRenderer & r = entity.meshRenderers[i];

			osi::UIPanel * pnl = new osi::UIPanel("MR", osi::Origin::RIGHT_CENTREY, 0, 0, 80, 65, false, false, 8, 8, 8, 1, 5, 5);
			lstRight1->addComponent(editorUI, entity, pnl);

			osi::UIPanel * pnlFilter = new osi::UIPanel("pnlFilter", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UILabel * lblFilter = new osi::UILabel("lblFilter", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Filter", osi::Origin::CENTREX_CENTREY);
			osi::UITextField * txtFilter = new osi::UITextField("txtFilter", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 2, 2, 2);
			//pnlFilter->addComponent(lblFilter);
			//pnlFilter->addComponent(txtFilter);
			editorUI->addComponent(pnlFilter, lblFilter);
			editorUI->addComponent(pnlFilter, txtFilter);

			//pnl->addComponent(pnlFilter);
			editorUI->addComponent(pnl, pnlFilter);
		}

		for(size_t i = 0; i < entity.skinnedMeshRenderes.size(); i++)
		{
			osrm::MeshRenderer & r = entity.skinnedMeshRenderes[i];

			osi::UIPanel * pnl = new osi::UIPanel("SMR", osi::Origin::RIGHT_CENTREY, 0, 0, 80, 65, false, false, 8, 8, 8, 1, 5, 5);
			lstRight1->addComponent(editorUI, entity, pnl);

			osi::UIPanel * pnlFilter = new osi::UIPanel("pnlFilter", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UILabel * lblFilter = new osi::UILabel("lblFilter", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Filter", osi::Origin::CENTREX_CENTREY);
			osi::UITextField * txtFilter = new osi::UITextField("txtFilter", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3);
			osi::UIButton * btnProg = new osi::UIButton("btnProg", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, 3, 3, 3, "Shader", osi::Origin::CENTREX_CENTREY);

			txtFilter->setText("MR");
			//pnlFilter->addComponent(lblFilter);
			//pnlFilter->addComponent(txtFilter);
			editorUI->addComponent(pnlFilter, lblFilter);
			editorUI->addComponent(pnlFilter, txtFilter);

			//pnl->addComponent(pnlFilter);
			//pnl->addComponent(btnProg);
			editorUI->addComponent(pnl, pnlFilter);
			editorUI->addComponent(pnl, btnProg);
		}

		for(size_t i = 0; i < entity.animControllers.size(); i++)
		{
			osrm::AnimationControllerFilter & r = entity.animControllers[i];

			osi::UIPanel * pnl = new osi::UIPanel("ACF", osi::Origin::RIGHT_CENTREY, 0, 0, 80, 25, false, false, 8, 8, 8, 1, 5, 5);
			lstRight1->addComponent(editorUI, entity, pnl);

			//TODO - Add UIComponents here
		}

		for(auto dirLight : entity.dirLightIndexes)
		{
			osr::DirectionLight & r = projResManager->dirLights[dirLight.first];

			osi::UIPanel * pnl = new osi::UIPanel("DL", osi::Origin::RIGHT_CENTREY, 0, 0, 80, 215, false, false, 8, 8, 8, 1, 5, 5);
			lstRight1->addComponent(editorUI, entity, pnl);

			osi::UIPanel * pnlCompName = new osi::UIPanel("pnlCompName", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UILabel * lblCompType = new osi::UILabel("lblType", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Direction Light", osi::Origin::CENTREX_CENTREY);
			osi::UILabel * lblCompName = new osi::UILabel("lblName", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Name:", osi::Origin::CENTREX_CENTREY);
			osi::UITextField * txtCompName = new osi::UITextField("txtCompName", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 2, 2, 2);
			txtCompName->setText(dirLight.second);
			editorUI->addComponent(pnl, lblCompType);
			editorUI->addComponent(pnl, pnlCompName);
			editorUI->addComponent(pnlCompName, lblCompName);


			osi::UILabel * lblDir = new osi::UILabel("lblDir", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Direction", osi::Origin::CENTREX_CENTREY);
			osi::UILabel * lblInten = new osi::UILabel("lblIten", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Inten", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldInten = new osi::UISlider("sldIten", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UIPanel * pnlInten = new osi::UIPanel("pnlInten", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			//pnlInten->addComponent(lblInten);
			//pnlInten->addComponent(sldInten);
			editorUI->addComponent(pnlInten, lblInten);
			editorUI->addComponent(pnlInten, sldInten);
			osi::UILabel * lblRed = new osi::UILabel("lblRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Red", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldRed = new osi::UISlider("sldRed", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UILabel * lblGreen = new osi::UILabel("lblRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Green", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldGreen = new osi::UISlider("sldGreen", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UILabel * lblBlue = new osi::UILabel("lblRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Blue", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldBlue = new osi::UISlider("sldBlue", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UIPanel * pnlRed = new osi::UIPanel("pnlRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UIPanel * pnlGreen = new osi::UIPanel("pnlGreen", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UIPanel * pnlBlue = new osi::UIPanel("pnlBlue", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			/*pnlRed->addComponent(lblRed);
			pnlRed->addComponent(sldRed);
			pnlGreen->addComponent(lblGreen);
			pnlGreen->addComponent(sldGreen);
			pnlBlue->addComponent(lblBlue);
			pnlBlue->addComponent(sldBlue);*/
			editorUI->addComponent(pnlRed, lblRed);
			editorUI->addComponent(pnlRed, sldRed);
			editorUI->addComponent(pnlGreen, lblGreen);
			editorUI->addComponent(pnlGreen, sldGreen);
			editorUI->addComponent(pnlBlue, lblBlue);
			editorUI->addComponent(pnlBlue, sldBlue);

			sldInten->setStartVal(r.intensity);
			sldInten->addSliderCallback(new FloatRefSliderCallback(r.intensity));
			sldRed->setStartVal(r.diffuseColour.r);
			sldRed->addSliderCallback(new FloatRefSliderCallback(r.diffuseColour.r));
			sldGreen->setStartVal(r.diffuseColour.g);
			sldGreen->addSliderCallback(new FloatRefSliderCallback(r.diffuseColour.g));
			sldBlue->setStartVal(r.diffuseColour.b);
			sldBlue->addSliderCallback(new FloatRefSliderCallback(r.diffuseColour.b));
			
			/*pnl->addComponent(lblDir);
			pnl->addComponent(pnlInten);
			pnl->addComponent(pnlRed);
			pnl->addComponent(pnlGreen);
			pnl->addComponent(pnlBlue);*/
			editorUI->addComponent(pnl, lblDir);
			editorUI->addComponent(pnl, pnlInten);
			editorUI->addComponent(pnl, pnlRed);
			editorUI->addComponent(pnl, pnlGreen);
			editorUI->addComponent(pnl, pnlBlue);
		}

		for(auto pointLight : entity.pointLightIndexes)
		{
			osr::PointLight & r = projResManager->pointLights[pointLight.first];

			osi::UIPanel * pnl = new osi::UIPanel("PL", osi::Origin::RIGHT_CENTREY, 0, 0, 80, 331, false, false, 8, 8, 8, 1, 5, 5);
			lstRight1->addComponent(editorUI, entity, pnl);

			osi::UIPanel * pnlCompName = new osi::UIPanel("pnlCompName", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UILabel * lblCompType = new osi::UILabel("lblType", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Point Light", osi::Origin::CENTREX_CENTREY);
			osi::UILabel * lblCompName = new osi::UILabel("lblName", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Name:", osi::Origin::CENTREX_CENTREY);
			osi::UITextField * txtCompName = new osi::UITextField("txtCompName", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 2, 2, 2);
			txtCompName->setText(pointLight.second);
			editorUI->addComponent(pnl, lblCompType);
			editorUI->addComponent(pnl, pnlCompName);
			editorUI->addComponent(pnlCompName, lblCompName);

			osi::UILabel * lblPos = new osi::UILabel("lblPos", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Position", osi::Origin::CENTREX_CENTREY);
			osi::UILabel * lblInten = new osi::UILabel("lblIten", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Inten", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldInten = new osi::UISlider("sldIten", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UIPanel * pnlInten = new osi::UIPanel("pnlInten", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);

			editorUI->addComponent(pnlInten, lblInten);
			editorUI->addComponent(pnlInten, sldInten);
			osi::UILabel * lblRed = new osi::UILabel("lblRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Red", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldRed = new osi::UISlider("sldRed", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UILabel * lblGreen = new osi::UILabel("lblRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Green", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldGreen = new osi::UISlider("sldGreen", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UILabel * lblBlue = new osi::UILabel("lblRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Blue", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldBlue = new osi::UISlider("sldBlue", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UIPanel * pnlRed = new osi::UIPanel("pnlRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UIPanel * pnlGreen = new osi::UIPanel("pnlGreen", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UIPanel * pnlBlue = new osi::UIPanel("pnlBlue", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			editorUI->addComponent(pnlRed, lblRed);
			editorUI->addComponent(pnlRed, sldRed);
			editorUI->addComponent(pnlGreen, lblGreen);
			editorUI->addComponent(pnlGreen, sldGreen);
			editorUI->addComponent(pnlBlue, lblBlue);
			editorUI->addComponent(pnlBlue, sldBlue);
			osi::UILabel * lblAtten = new osi::UILabel("lblAtten", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Attenuation", osi::Origin::CENTREX_CENTREY);
			osi::UILabel * lblConst = new osi::UILabel("lblConst", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Constant", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldConst = new osi::UISlider("sldConst", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UILabel * lblLin = new osi::UILabel("lblLin", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Linear", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldLin = new osi::UISlider("sldLin", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UILabel * lblQuad = new osi::UILabel("lblQuad", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Quadratic", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldQuad = new osi::UISlider("sldQuad", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UIPanel * pnlConst = new osi::UIPanel("pnlConst", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UIPanel * pnlLin = new osi::UIPanel("pnlLin", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UIPanel * pnlQuad = new osi::UIPanel("pnlQuad", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			editorUI->addComponent(pnlConst, lblConst);
			editorUI->addComponent(pnlConst, sldConst);
			editorUI->addComponent(pnlLin, lblLin);
			editorUI->addComponent(pnlLin, sldLin);
			editorUI->addComponent(pnlQuad, lblQuad);
			editorUI->addComponent(pnlQuad, sldQuad);

			sldInten->setStartVal(r.intensity);
			sldInten->addSliderCallback(new FloatRefSliderCallback(r.intensity));
			sldRed->setStartVal(r.diffuseColour.r);
			sldRed->addSliderCallback(new FloatRefSliderCallback(r.diffuseColour.r));
			sldGreen->setStartVal(r.diffuseColour.g);
			sldGreen->addSliderCallback(new FloatRefSliderCallback(r.diffuseColour.g));
			sldBlue->setStartVal(r.diffuseColour.b);
			sldBlue->addSliderCallback(new FloatRefSliderCallback(r.diffuseColour.b));
			sldConst->setStartVal(r.constant);
			sldConst->addSliderCallback(new FloatRefSliderCallback(r.constant));
			sldLin->setStartVal(r.linear);
			sldLin->addSliderCallback(new FloatRefSliderCallback(r.linear));
			sldQuad->setStartVal(r.quadratic);
			sldQuad->addSliderCallback(new FloatRefSliderCallback(r.quadratic));

			editorUI->addComponent(pnl, lblPos);
			editorUI->addComponent(pnl, pnlInten);
			editorUI->addComponent(pnl, pnlRed);
			editorUI->addComponent(pnl, pnlGreen);
			editorUI->addComponent(pnl, pnlBlue);
			editorUI->addComponent(pnl, lblAtten);
			editorUI->addComponent(pnl, pnlConst);
			editorUI->addComponent(pnl, pnlLin);
			editorUI->addComponent(pnl, pnlQuad);
		}

		for(auto spotLight : entity.spotLightIndexes)
		{
			osr::SpotLight & r = projResManager->spotLights[spotLight.first];

			osi::UIPanel * pnl = new osi::UIPanel("SL", osi::Origin::RIGHT_CENTREY, 0, 0, 80, 300, false, false, 8, 8, 8, 1, 5, 5);
			lstRight1->addComponent(editorUI, entity, pnl);

			osi::UIPanel * pnlCompName = new osi::UIPanel("pnlCompName", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UILabel * lblCompType = new osi::UILabel("lblType", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Spot Light", osi::Origin::CENTREX_CENTREY);
			osi::UILabel * lblCompName = new osi::UILabel("lblName", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Name:", osi::Origin::CENTREX_CENTREY);
			osi::UITextField * txtCompName = new osi::UITextField("txtCompName", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 2, 2, 2);
			txtCompName->setText(spotLight.second);
			editorUI->addComponent(pnl, lblCompType);
			editorUI->addComponent(pnl, pnlCompName);
			editorUI->addComponent(pnlCompName, lblCompName);
			
			osi::UILabel * lblDir = new osi::UILabel("lblDir", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Direction", osi::Origin::CENTREX_CENTREY);
			osi::UILabel * lblInten = new osi::UILabel("lblIten", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Inten", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldInten = new osi::UISlider("sldIten", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UIPanel * pnlInten = new osi::UIPanel("pnlInten", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			editorUI->addComponent(pnlInten, lblInten);
			editorUI->addComponent(pnlInten, sldInten);
			osi::UILabel * lblRed = new osi::UILabel("lblRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Red", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldRed = new osi::UISlider("sldRed", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UILabel * lblGreen = new osi::UILabel("lblRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Green", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldGreen = new osi::UISlider("sldGreen", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UILabel * lblBlue = new osi::UILabel("lblRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Blue", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldBlue = new osi::UISlider("sldBlue", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UIPanel * pnlRed = new osi::UIPanel("pnlRed", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UIPanel * pnlGreen = new osi::UIPanel("pnlGreen", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UIPanel * pnlBlue = new osi::UIPanel("pnlBlue", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			editorUI->addComponent(pnlRed, lblRed);
			editorUI->addComponent(pnlRed, sldRed);
			editorUI->addComponent(pnlGreen, lblGreen);
			editorUI->addComponent(pnlGreen, sldGreen);
			editorUI->addComponent(pnlBlue, lblBlue);
			editorUI->addComponent(pnlBlue, sldBlue);

			osi::UILabel * lblCosInner = new osi::UILabel("lblCosInner", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Inner Cutoff", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldCosInner = new osi::UISlider("sldCosInner", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UIPanel * pnlCosInner = new osi::UIPanel("pnlCosInner", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			osi::UILabel * lblCosOuter = new osi::UILabel("lblCosOuter", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, "Outer Cutoff", osi::Origin::CENTREX_CENTREY);
			osi::UISlider * sldCosOuter = new osi::UISlider("sldCosOuter", osi::Origin::RIGHT_CENTREY, 0, 0, 90, 25, false, false, 3, 3, 3, 3, 0.0f, 1.0f);
			osi::UIPanel * pnlCosOuter = new osi::UIPanel("pnlCosOuter", osi::Origin::RIGHT_CENTREY, 0, 0, 70, 25, false, false, -1, -1, -1, 0, 0, 0);
			editorUI->addComponent(pnlCosInner, lblCosInner);
			editorUI->addComponent(pnlCosInner, sldCosInner);
			editorUI->addComponent(pnlCosOuter, lblCosOuter);
			editorUI->addComponent(pnlCosOuter, sldCosOuter);

			sldInten->setStartVal(r.intensity);
			sldInten->addSliderCallback(new FloatRefSliderCallback(r.intensity));
			sldRed->setStartVal(r.diffuseColour.r);
			sldRed->addSliderCallback(new FloatRefSliderCallback(r.diffuseColour.r));
			sldGreen->setStartVal(r.diffuseColour.g);
			sldGreen->addSliderCallback(new FloatRefSliderCallback(r.diffuseColour.g));
			sldBlue->setStartVal(r.diffuseColour.b);
			sldBlue->addSliderCallback(new FloatRefSliderCallback(r.diffuseColour.b));
			sldCosInner->setStartVal(r.cosInnerCutoff);
			sldCosInner->addSliderCallback(new FloatRefSliderCallback(r.cosInnerCutoff));
			sldCosOuter->setStartVal(r.cosOuterCutoff);
			sldCosOuter->addSliderCallback(new FloatRefSliderCallback(r.cosOuterCutoff));

			editorUI->addComponent(pnl, lblDir);
			editorUI->addComponent(pnl, pnlInten);
			editorUI->addComponent(pnl, pnlRed);
			editorUI->addComponent(pnl, pnlGreen);
			editorUI->addComponent(pnl, pnlBlue);
			editorUI->addComponent(pnl, pnlCosInner);
			editorUI->addComponent(pnl, pnlCosOuter);
		}

		for(size_t i = 0; i < entity.skyboxes.size(); i++)
		{
			osr::Skybox & r = entity.skyboxes[i];

			osi::UIPanel * pnl = new osi::UIPanel("Skybox", osi::Origin::RIGHT_CENTREY, 0, 0, 80, 25, false, false, 8, 8, 8, 1, 5, 5);
			lstRight1->addComponent(editorUI, entity, pnl);

			//TODO - Add UIComponents here
		}

		//pnlRight1->setScrollBarShown(true, 3, 3, 3);
		editorUI->setScrollBarShown(pnlRight1, true, 3, 3, 3);

		rEngine->fullUpdateUserInterface(editorUI);
		rEngine->fullUpdateUserInterfaceText(editorUI);
	}

	void FloatRefSliderCallback::valueChanged(osi::UISlider * sld, float val) const
	{
		valToModify = val;
		rEngine->threadsafe_postShouldUpdateUBOLighting();
	}

	void StringRefCallback::valueChanged(osi::UITextField * txt, std::string & str) const
	{
		valToModify = str;
	}

	void EditorButtonPressedCallback::buttonPressed(osi::UIButton * btn) const
	{
		if(btn->getName() == "btnImport")
		{
			osi::UIWindow * win = new osi::UIWindow("winFileBrowser", osi::Origin::CENTREX_CENTREY, 0, 0, 800, 600, false, false, 3, 0, 0,
				true, true, true, true, 2, 3, 3, 3);

			win->setRestraintPanel(new osi::UIPanel("pnlRESTRAINT"+win->getName(), osi::Origin::LEFT_TOP, 50, 50, 1200, 1000, false, false, -1, -1, -1, 1, 5, 5));

			editorUI->addComponent(win);
			//editorUI->addComponent(win->get, new osi::UILabel("lblTest", osi::Origin::CENTREX_CENTREY, 0, 0, 100, 100, false, false, -1, -1, -1, "Hello World!", osi::Origin::CENTREX_CENTREY));
			rEngine->threadsafe_postShouldUpdateUI();
		}
		else if(btn->getName() == "btnSave")
		{
			osrm::saveScene("zproj_test/scenes/scene_test", projResManager, rEngine, animEngine, pEngine);
		}
	}
}
