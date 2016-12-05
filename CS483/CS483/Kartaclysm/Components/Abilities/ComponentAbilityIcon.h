//----------------------------------------------------------------------------
// ComponentAbilityIcon.h
// Author: Bradley Cooper
//
// Component that sends event to set HUD icon for ability.
//----------------------------------------------------------------------------

#ifndef COMPONENT_ABILITY_ICON_H
#define COMPONENT_ABILITY_ICON_H

#include <tinyxml2.h>

#include "Component.h"
#include "GameObject.h"
#include "EventManager.h"

namespace Kartaclysm
{
	class ComponentAbilityIcon : public HeatStroke::Component
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_AbilityIcon"; }
		virtual const std::string ComponentID() const override	{ return "GOC_AbilityIcon"; }

		// Destruction.
		virtual ~ComponentAbilityIcon();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override;
		virtual void Update(const float p_fDelta) override {}

	protected:
		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentAbilityIcon(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strAbility,
			const std::string& p_strActiveMTLFileName,
			const std::string& p_strActiveMaterialName,
			const std::string& p_strInactiveMTLFileName,
			const std::string& p_strInactiveMaterialName
			);

		static void ParseNode(
			tinyxml2::XMLNode* p_pNode,
			std::string& p_strAbility,
			std::string& p_strActiveMTLFileName,
			std::string& p_strActiveMaterialName,
			std::string& p_strInactiveMTLFileName,
			std::string& p_strInactiveMaterialName);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		std::string m_strAbility;
		std::string m_strActiveMTLFileName;
		std::string m_strActiveMaterialName;
		std::string m_strInactiveMTLFileName;
		std::string m_strInactiveMaterialName;
	};
}

#endif // COMPONENT_ABILITY_ICON_H