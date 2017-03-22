#ifndef COMPONENT_HUD_RACE_RESULTS_H
#define COMPONENT_HUD_RACE_RESULTS_H

#include "ComponentRenderable.h"

// TODO - fix compiler error that is thrown if glew.h not included here
#include "GL\glew.h"

#include "EasyXML.h"
#include "EventManager.h"
#include "FontManager.h"
#include "GameObject.h"
#include "SceneManager.h"
#include "TextBox.h"

namespace Kartaclysm
{
	class ComponentHudRaceResults : public HeatStroke::ComponentRenderable
	{
		public:
			virtual const std::string ComponentID() const override { return "GOC_HUD_RaceResults"; }

			~ComponentHudRaceResults();

			static HeatStroke::Component* CreateComponent(
				HeatStroke::GameObject* p_pGameObject,
				tinyxml2::XMLNode* p_pBaseNode,
				tinyxml2::XMLNode* p_pOverrideNode
			);

			virtual void Init() override {}
			virtual void Update(const float p_fDelta) override;
			virtual void SyncTransform() override;

		protected:
			ComponentHudRaceResults(
				HeatStroke::GameObject* p_pGameObject,
				const std::string& p_strFontFilePath
			);

		private:
			HeatStroke::Font* m_pFont;
			HeatStroke::TextBox m_TextBox;
			std::string m_strRacerId;
			std::function<void(const HeatStroke::Event*)>* m_pRaceFinishedDelegate;

			void RaceFinishedCallback(const HeatStroke::Event* p_pEvent);
			std::string FormatTime(const float p_fRaceTime);
	};
}

#endif