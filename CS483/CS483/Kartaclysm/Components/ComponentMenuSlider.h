//----------------------------------------------------------------------------
// ComponentMenuSlider.h
// Author: Bradley Cooper
//
// Component for menu slider ranging from [0,100].
//----------------------------------------------------------------------------

#ifndef COMPONENT_MENU_SLIDER_H
#define COMPONENT_MENU_SLIDER_H

#include <tinyxml2.h>

#include "GameObject.h"
#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "FontManager.h"
#include "TextBox.h"
#include "SpriteInstance.h"

namespace Kartaclysm
{
	class ComponentMenuSlider : public HeatStroke::ComponentRenderable
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_MenuSlider"; }

		virtual ~ComponentMenuSlider();

		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override {}
		virtual void SyncTransform() override {}

		void SetSliderValue(float p_fSliderOffset) { m_fSliderOffset = p_fSliderOffset; }
		virtual bool IncreaseSlider();
		virtual bool DecreaseSlider();
		int GetSliderValue() const { return m_iCurrent; }

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentMenuSlider(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strFontFilePath,
			const std::string& p_strLabelMessage,
			const std::string& p_strOutlineMTLFileName,
			const std::string& p_strOutlineMaterialName,
			const std::string& p_strSliderMTLFileName,
			const std::string& p_strSliderMaterialName,
			float p_fSliderOffset,
			int p_iChangeFactor,
			int p_iCurrent
			);

		virtual void UpdateSlider();

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strFontFilePath,
			std::string& p_strLabelMessage,
			std::string& p_strOutlineMTLFileName,
			std::string& p_strOutlineMaterialName,
			std::string& p_strSliderMTLFileName,
			std::string& p_strSliderMaterialName,
			float& p_fSliderOffset,
			int& p_iChangeFactor,
			int& p_iCurrent
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------

		HeatStroke::Font* m_pFont;
		HeatStroke::TextBox m_mLabelTextBox;
		HeatStroke::SpriteInstance m_mOutline;
		HeatStroke::SpriteInstance m_mSlider;

		float m_fSliderOffset;
		int m_iChangeFactor;
		int m_iCurrent;
	};
}

#endif