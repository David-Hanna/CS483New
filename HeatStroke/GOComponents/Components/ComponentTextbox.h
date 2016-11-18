//------------------------------------------------------------------------
// ComponentTextBox
// Author:	Bradley Cooper
//	
// Provides 2D rendering of a textbox.
//------------------------------------------------------------------------

#ifndef COMPONENT_TEXTBOX_H
#define COMPONENT_TEXTBOX_H

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EasyXML.h"
#include "GameObject.h"

namespace HeatStroke
{
	class ComponentTextBox : public ComponentRenderable
	{
	public:
		virtual const std::string ComponentID() const override { return std::string("GOC_TextBox"); }

		virtual ~ComponentTextBox();

		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		virtual void Init() override						{}
		virtual void Update(const float p_fDelta) override	{}

	protected:
		ComponentTextBox(GameObject* p_pGameObject,
			const std::string& p_strFontFilePath,
			const std::string& p_strMessage,
			float p_fWidth,
			float p_fHeight);

		virtual void SyncTransform();

	private:
		Font* m_pFont;
		TextBox m_mTextBox;

		// Prevent copying
		ComponentTextBox(const ComponentTextBox&) = delete;
		ComponentTextBox& operator=(const ComponentTextBox&) = delete;

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strFontFilePath,
			std::string& p_strMessage,
			float& p_fWidth,
			float& p_fHeight
			);
	};
}

#endif