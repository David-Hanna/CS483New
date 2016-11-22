//------------------------------------------------------------------------
// ComponentTrackPiece
//
// Author:	David MacIntosh
//	
// Manages some useful info about a specific track piece
//------------------------------------------------------------------------

#ifndef COMPONENT_TRACK_PIECE_H
#define COMPONENT_TRACK_PIECE_H

#include "Component.h"
#include "GameObject.h"

namespace Kartaclysm
{
	class ComponentTrackPiece : public HeatStroke::Component
	{
		public:
			~ComponentTrackPiece();

			virtual const std::string FamilyID() const override { return "GOC_TrackPiece"; }
			virtual const std::string ComponentID() const override { return "GOC_TrackPiece"; }

			static HeatStroke::Component* CreateComponent(
				HeatStroke::GameObject* p_pGameObject,
				tinyxml2::XMLNode* p_pBaseNode,
				tinyxml2::XMLNode* p_pOverrideNode);

			virtual void Init() override;
			virtual void Update(const float p_fDelta) override;

			bool CheckInBounds(glm::vec3 p_pPosition);

		protected:
			ComponentTrackPiece(HeatStroke::GameObject* p_pGameObject, float p_fWidthX, float p_fWidthZ);

		private:
			float m_fWidthX;
			float m_fWidthZ;
	};
}

#endif // COMPONENT_TRACK_PIECE_H