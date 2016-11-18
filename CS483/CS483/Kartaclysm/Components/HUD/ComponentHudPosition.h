//----------------------------------------------------------------------------
// ComponentHudPosition.h
// Author: Bradley Cooper
//
// Component that handles an ability icon on a player's HUD.
//----------------------------------------------------------------------------

#ifndef COMPONENT_HUD_POSITION_H
#define COMPONENT_HUD_POSITION_H

#include <tinyxml2.h>

#include "ComponentRenderable.h"
#include "SceneManager.h"
#include "EventManager.h"
#include "Sprite.h"

namespace Kartaclysm
{
	class ComponentHudPosition : public HeatStroke::ComponentRenderable
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string ComponentID() const override	{ return "GOC_Hud_Position"; }

		virtual ~ComponentHudPosition();

		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		virtual void Init() override {}
		virtual void Update(const float p_fDelta) override {}
		virtual void SyncTransform() override { m_mSprite.SetTransform(this->GetGameObject()->GetTransform().GetTransform()); };

	protected:

		//--------------------------------------------------------------------------
		// Protected types
		//--------------------------------------------------------------------------
		typedef std::function<void(const HeatStroke::Event*)>* EventDelegate;

		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentHudPosition(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strPositionFilePrefix
			);

		virtual void PositionCallback(const HeatStroke::Event* p_pEvent);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strPositionFilePrefix
			);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------

		std::string m_strPositionFilePrefix;
		HeatStroke::Sprite m_mSprite;
		std::string m_strEventName;

		std::function<void(const HeatStroke::Event*)>* m_pDelegate;
	};
}

#endif // COMPONENT_HUD_POSITION_H