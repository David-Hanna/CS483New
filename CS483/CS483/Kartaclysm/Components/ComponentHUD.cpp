//----------------------------------------------------------------------------
// ComponentHUD.cpp
// Author: Bradley Cooper
//
// Component that handles camera positioning.
//----------------------------------------------------------------------------

#include "ComponentHUD.h"

namespace Kartaclysm
{
	ComponentHUD::ComponentHUD(
		HeatStroke::GameObject* p_pGameObject,
		const std::string& p_strFontFilePath,
		const std::string& p_strFontTextureFilesPath,
		const std::string& p_strVertexProgramPath,
		const std::string& p_strFragmentProgramPath
		) :
		HeatStroke::ComponentRenderable(p_pGameObject),
		m_pFont(new HeatStroke::Font(p_strFontFilePath, p_strFontTextureFilesPath)),
		m_strVertexProgramPath(p_strVertexProgramPath),
		m_strFragmentProgramPath(p_strFragmentProgramPath),
		m_bPaused(true),
		m_fRaceTime(0.0f)
	{
		assert(m_pFont->GetFontName() != "" && "Error creating font.");
	}

	ComponentHUD::~ComponentHUD()
	{
		if (m_pFont != nullptr)
		{
			delete m_pFont;
			m_pFont = nullptr;
		}

		// Iterate through all layers
		HeatStroke::EventManager* pEventManager = HeatStroke::EventManager::Instance();
		LayerMap::iterator it = m_mLayerMap.begin();
		while (it != m_mLayerMap.end())
		{
			// Iterate through SectionList for this layer
			SectionMap::iterator it2 = it->second.begin();
			while (it2 != it->second.end())
			{
				// Unregister event
				HUDSection section = it2->second;
				if (section.m_pDelegate != nullptr)
				{
					pEventManager->RemoveListener(section.m_strEventRegistered, section.m_pDelegate);
					section.m_pDelegate = nullptr;
				}

				// Iterate and delete all TextBoxes
				RenderMap::iterator it3 = section.m_mRenderMap.begin();
				while (it3 != section.m_mRenderMap.end())
				{
					if (it3->second != nullptr)
					{
						delete it3->second;
						it3->second = nullptr;
					}

					it3 = section.m_mRenderMap.erase(it3); // TextBox
				}
				it2 = it->second.erase(it2); // SectionList
			}
			it = m_mLayerMap.erase(it); // Layer
		}
	}

	HeatStroke::Component* ComponentHUD::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		//assert(p_pGameObject != nullptr);

		// Defaults
		std::string strFontFilePath = "Assets/Fonts/ComicSans/Comic_Sans.fnt";
		std::string strFontTextureFilesPath = "Assets/Fonts/ComicSans/";
		std::string strVertexProgramPath = "Assets/Fonts/font.vsh";
		std::string strFragmentProgramPath = "Assets/Fonts/font.fsh";

		// All parameters are optional.
		if (p_pBaseNode != nullptr)
		{
			//ParseNode(p_pBaseNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
		}
		if (p_pOverrideNode != nullptr)
		{
			//ParseNode(p_pOverrideNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
		}

		return new ComponentHUD(
			p_pGameObject,
			strFontFilePath,
			strFontTextureFilesPath,
			strVertexProgramPath,
			strFragmentProgramPath
		);
	}

	void ComponentHUD::Init()
	{
		// Break up an otherwise insanely long method
		// TO DO, find another way to get the player number
		AssignEventDelegates(0);
		AssignInitialRenderables();
	}

	void ComponentHUD::Update(const float p_fDelta)
	{
		// Update racing time and the accompanying HUD
		if (!m_bPaused)
		{
			m_fRaceTime += p_fDelta;
		}

		// TO DO, also reduce ability cooldowns
	}

	void ComponentHUD::AssignEventDelegates(const int p_iPlayerNumber)
	{
		// Some common variables
		std::string strPlayer = "HUD_Player" + p_iPlayerNumber;
		HeatStroke::EventManager* pEventManager = HeatStroke::EventManager::Instance();

		// Assign the callbacks for events
		HUDSection section = m_mLayerMap[eRaceInfo]["Timer"];
		section.m_strEventRegistered = "HUD_Pause"; // not attached to any player
		section.m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHUD::PauseCallback, this, std::placeholders::_1));
		pEventManager->AddListener(section.m_strEventRegistered, section.m_pDelegate);

		section = m_mLayerMap[eRaceInfo]["DriverAbility1"];
		section.m_strEventRegistered = strPlayer + "_DriverAbility1";
		section.m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHUD::DriverAbility1Callback, this, std::placeholders::_1));
		pEventManager->AddListener(section.m_strEventRegistered, section.m_pDelegate);

		section = m_mLayerMap[eRaceInfo]["DriverAbility2"];
		section.m_strEventRegistered = strPlayer + "_DriverAbility2";
		section.m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHUD::DriverAbility2Callback, this, std::placeholders::_1));
		pEventManager->AddListener(section.m_strEventRegistered, section.m_pDelegate);

		section = m_mLayerMap[eRaceInfo]["KartAbility1"];
		section.m_strEventRegistered = strPlayer + "_KartAbility1";
		section.m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHUD::KartAbility1Callback, this, std::placeholders::_1));
		pEventManager->AddListener(section.m_strEventRegistered, section.m_pDelegate);

		section = m_mLayerMap[eRaceInfo]["KartAbility2"];
		section.m_strEventRegistered = strPlayer + "_KartAbility2";
		section.m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHUD::KartAbility2Callback, this, std::placeholders::_1));
		pEventManager->AddListener(section.m_strEventRegistered, section.m_pDelegate);

		section = m_mLayerMap[eRaceInfo]["Position"];
		section.m_strEventRegistered = strPlayer + "_Position";
		section.m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHUD::PositionCallback, this, std::placeholders::_1));
		pEventManager->AddListener(section.m_strEventRegistered, section.m_pDelegate);

		section = m_mLayerMap[eRaceInfo]["Lap"];
		section.m_strEventRegistered = strPlayer + "_Lap";
		section.m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHUD::LapCallback, this, std::placeholders::_1));
		pEventManager->AddListener(section.m_strEventRegistered, section.m_pDelegate);

		section = m_mLayerMap[ePopups]["Popup"];
		section.m_strEventRegistered = strPlayer + "_Popup";
		section.m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHUD::PopupCallback, this, std::placeholders::_1));
		pEventManager->AddListener(section.m_strEventRegistered, section.m_pDelegate);

		section = m_mLayerMap[eDebug]["Debug"];
		section.m_strEventRegistered = strPlayer + "_Debug";
		section.m_pDelegate = new std::function<void(const HeatStroke::Event*)>(std::bind(&ComponentHUD::DebugCallback, this, std::placeholders::_1));
		pEventManager->AddListener(section.m_strEventRegistered, section.m_pDelegate);
	}

	void ComponentHUD::AssignInitialRenderables()
	{
		// TO DO, currently everything renders but practice mode might not need to render position (as example)
		// Assign the callbacks for events
		HUDSection section = m_mLayerMap[eRaceInfo]["Timer"];
		section.m_mRenderMap["Time"] = new HeatStroke::TextBox(m_pFont, "0.0", 320.0f, 40.0f, m_strVertexProgramPath, m_strFragmentProgramPath);
		static_cast<HeatStroke::TextBox*>(section.m_mRenderMap["Time"])->SetPosition(1000.0f, 40.0f);

		section = m_mLayerMap[eRaceInfo]["DriverAbility1"];
		section.m_mRenderMap["Icon"] = new HeatStroke::TextBox(m_pFont, "D1", 80.0f, 40.0f, m_strVertexProgramPath, m_strFragmentProgramPath);
		static_cast<HeatStroke::TextBox*>(section.m_mRenderMap["Icon"])->SetPosition(900.0f, 700.0f);

		section = m_mLayerMap[eRaceInfo]["DriverAbility2"];
		section.m_mRenderMap["Icon"] = new HeatStroke::TextBox(m_pFont, "D2", 80.0f, 40.0f, m_strVertexProgramPath, m_strFragmentProgramPath);
		static_cast<HeatStroke::TextBox*>(section.m_mRenderMap["Icon"])->SetPosition(1000.0f, 700.0f);

		section = m_mLayerMap[eRaceInfo]["KartAbility1"];
		section.m_mRenderMap["Icon"] = new HeatStroke::TextBox(m_pFont, "K1", 80.0f, 40.0f, m_strVertexProgramPath, m_strFragmentProgramPath);
		static_cast<HeatStroke::TextBox*>(section.m_mRenderMap["Icon"])->SetPosition(1100.0f, 700.0f);

		section = m_mLayerMap[eRaceInfo]["KartAbility2"];
		section.m_mRenderMap["Icon"] = new HeatStroke::TextBox(m_pFont, "K2", 80.0f, 40.0f, m_strVertexProgramPath, m_strFragmentProgramPath);
		static_cast<HeatStroke::TextBox*>(section.m_mRenderMap["Icon"])->SetPosition(1200.0f, 700.0f);

		section = m_mLayerMap[eRaceInfo]["Position"];
		section.m_mRenderMap["Number"] = new HeatStroke::TextBox(m_pFont, "1st", 120.0f, 40.0f, m_strVertexProgramPath, m_strFragmentProgramPath);
		static_cast<HeatStroke::TextBox*>(section.m_mRenderMap["Number"])->SetPosition(100.0f, 700.0f);

		section = m_mLayerMap[eRaceInfo]["Lap"];
		section.m_mRenderMap["Current"] = new HeatStroke::TextBox(m_pFont, "1/3", 120.0f, 40.0f, m_strVertexProgramPath, m_strFragmentProgramPath);
		static_cast<HeatStroke::TextBox*>(section.m_mRenderMap["Current"])->SetPosition(1000.0f, 120.0f);

		section = m_mLayerMap[ePopups]["Popup"];
		section.m_mRenderMap["Warning"] = new HeatStroke::TextBox(m_pFont, "", 200.0f, 80.0f, m_strVertexProgramPath, m_strFragmentProgramPath);
		static_cast<HeatStroke::TextBox*>(section.m_mRenderMap["Warning"])->SetPosition(540.0f, 320.0f);

		section = m_mLayerMap[eDebug]["Debug"];
		section.m_mRenderMap["Message"] = new HeatStroke::TextBox(m_pFont, "", 200.0f, 80.0f, m_strVertexProgramPath, m_strFragmentProgramPath);
		static_cast<HeatStroke::TextBox*>(section.m_mRenderMap["Message"])->SetPosition(40.0f, 40.0f);
	}

	void ComponentHUD::PauseCallback(const HeatStroke::Event* p_pEvent)
	{

	}
	void ComponentHUD::DriverAbility1Callback(const HeatStroke::Event* p_pEvent)
	{

	}
	void ComponentHUD::DriverAbility2Callback(const HeatStroke::Event* p_pEvent)
	{

	}
	void ComponentHUD::KartAbility1Callback(const HeatStroke::Event* p_pEvent)
	{

	}
	void ComponentHUD::KartAbility2Callback(const HeatStroke::Event* p_pEvent)
	{

	}
	void ComponentHUD::PositionCallback(const HeatStroke::Event* p_pEvent)
	{

	}
	void ComponentHUD::LapCallback(const HeatStroke::Event* p_pEvent)
	{

	}
	void ComponentHUD::PopupCallback(const HeatStroke::Event* p_pEvent)
	{

	}
	void ComponentHUD::DebugCallback(const HeatStroke::Event* p_pEvent)
	{

	}
}