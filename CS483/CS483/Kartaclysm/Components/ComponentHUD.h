//----------------------------------------------------------------------------
// ComponentHUD.h
// Author: Bradley Cooper
//
// Component that renders for the HUD.
//----------------------------------------------------------------------------

#ifndef COMPONENT_HUD_H
#define COMPONENT_HUD_H

#include <tinyxml2.h>

#include "ComponentRenderable.h"
#include "TextBox.h"
#include "Font.h"
#include "EventManager.h"
#include "SceneManager.h"
#include <map>

namespace Kartaclysm
{
	class ComponentHUD : public HeatStroke::ComponentRenderable
	{
	public:
		//--------------------------------------------------------------------------
		// Public methods
		//--------------------------------------------------------------------------
		virtual const std::string FamilyID() const override		{ return "GOC_HUD"; }
		virtual const std::string ComponentID() const override	{ return "GOC_HUD"; }

		// Destruction.
		virtual ~ComponentHUD();

		// Factory construction.
		static HeatStroke::Component* CreateComponent(
			HeatStroke::GameObject* p_pGameObject,
			tinyxml2::XMLNode* p_pBaseNode,
			tinyxml2::XMLNode* p_pOverrideNode
			);

		// Game Loop methods.
		virtual void Init() override;
		virtual void Update(const float p_fDelta) override;
		virtual void PreRender() { HeatStroke::ComponentRenderable::PreRender(); }

	protected:

		//--------------------------------------------------------------------------
		// Protected types
		//--------------------------------------------------------------------------
		enum eRenderLayer {
			eRaceInfo = 0,
			ePopups,
			eDebug
		};

		typedef std::function<void(const HeatStroke::Event*)>* EventDelegate;
		typedef std::map<std::string, HeatStroke::Renderable*> RenderMap;

		// A HUD section records TextBoxes associated with a section of the HUD,
		// and the information to register/unregister with EventManager
		struct HUDSection {
			std::string	m_strEventRegistered;
			EventDelegate m_pDelegate;
			RenderMap m_mRenderMap;

			HUDSection() :
				m_strEventRegistered(""),
				m_pDelegate(nullptr),
				m_mRenderMap() {}
		};

		typedef std::map<std::string, HUDSection> SectionMap;
		typedef std::map<eRenderLayer, SectionMap> LayerMap;

		//--------------------------------------------------------------------------
		// Protected methods
		//--------------------------------------------------------------------------
		ComponentHUD(
			HeatStroke::GameObject* p_pGameObject,
			const std::string& p_strFontFilePath,
			const std::string& p_strFontTextureFilesPath,
			const std::string& p_strVertexProgramPath,
			const std::string& p_strFragmentProgramPath
			);

		// Required for Renderable Components, but does nothing
		virtual void SyncTransform() {}

		// All of the callback methods for this HUD
		virtual void PauseCallback(const HeatStroke::Event* p_pEvent);
		virtual void DriverAbility1Callback(const HeatStroke::Event* p_pEvent);
		virtual void DriverAbility2Callback(const HeatStroke::Event* p_pEvent);
		virtual void KartAbility1Callback(const HeatStroke::Event* p_pEvent);
		virtual void KartAbility2Callback(const HeatStroke::Event* p_pEvent);
		virtual void PositionCallback(const HeatStroke::Event* p_pEvent);
		virtual void LapCallback(const HeatStroke::Event* p_pEvent);
		virtual void PopupCallback(const HeatStroke::Event* p_pEvent);
		virtual void DebugCallback(const HeatStroke::Event* p_pEvent);

		//--------------------------------------------------------------------------
		// Protected variables
		//--------------------------------------------------------------------------
		HeatStroke::GameObject* m_pGameObject;

		// Timer control
		bool m_bPaused;
		float m_fRaceTime;

		// Information for creating textboxes
		HeatStroke::Font*	m_pFont;
		std::string			m_strVertexProgramPath;
		std::string			m_strFragmentProgramPath;

		// Track HUD Sections as they are mapped by layers
		LayerMap m_mLayerMap;
	};
}

#endif // COMPONENT_HUD_H