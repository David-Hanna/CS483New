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
		HeatStroke::SceneManager* pSceneManager = HeatStroke::SceneManager::Instance();
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

				// Iterate and delete all renderables
				RenderMap::iterator it3 = section.m_mRenderMap.begin();
				while (it3 != section.m_mRenderMap.end())
				{
					if (it3->second != nullptr)
					{
						// Remove renderable from Scene Management
						if (HeatStroke::TextBox* pTextBox = dynamic_cast<HeatStroke::TextBox*>(it3->second))
						{
							pSceneManager->RemoveTextBox(pTextBox);
						}
						else if (HeatStroke::Sprite* pSprite = dynamic_cast<HeatStroke::Sprite*>(it3->second))
						{
							pSceneManager->RemoveSprite(pSprite);
						}

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