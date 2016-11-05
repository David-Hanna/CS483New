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
		m_strFragmentProgramPath(p_strFragmentProgramPath)
	{
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
			SectionList::iterator it2 = it->second.begin();
			while (it2 != it->second.end())
			{
				// Unregister event
				HUDSection section = *it2;
				if (section.m_pDelegate != nullptr)
				{
					pEventManager->RemoveListener(section.m_strEventRegistered, section.m_pDelegate);
					section.m_pDelegate = nullptr;
				}

				// Iterate and delete all TextBoxes
				TextBoxMap::iterator it3 = section.m_mTextBoxMap.begin();
				while (it3 != section.m_mTextBoxMap.end())
				{
					if (it3->second != nullptr)
					{
						delete it3->second;
						it3->second = nullptr;
					}

					it3 = section.m_mTextBoxMap.erase(it3); // TextBox
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
		std::string strFontFilePath = "";
		std::string strFontTextureFilesPath = "";
		std::string strVertexProgramPath = "";
		std::string strFragmentProgramPath = "";

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

	void ComponentHUD::Update(const float p_fDelta)
	{
		// do nothing for Update - everything is handled through Events
	}
}