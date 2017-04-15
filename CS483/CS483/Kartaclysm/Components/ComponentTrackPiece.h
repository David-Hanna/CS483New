//------------------------------------------------------------------------
// ComponentTrackPiece
//
// Author:	David MacIntosh
//	
// Manages some useful info about a specific track piece
//------------------------------------------------------------------------

#ifndef COMPONENT_TRACK_PIECE_H
#define COMPONENT_TRACK_PIECE_H

#include "Common.h"
#include "Component.h"
#include "GameObject.h"

namespace Kartaclysm
{
	enum HeightFunction
	{
		Flat,
		Linear,
		Sin1,
		Sin2,
		Sin3,
		Sin4
	};

	enum PositionFunction
	{
		Straight,
		Turn
	};
	
	struct OffroadSquare
	{
		float x1;
		float z1;
		float x2;
		float z2;
	};

	struct OffroadTriangle
	{
		float x1;
		float z1;
		float x2;
		float z2;
		float x3;
		float z3;
	};

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
			float HeightAtPosition(glm::vec3 p_pPosition);
			bool IsOffroadAtPosition(const glm::vec3 &p_pPosition);

			bool GetIsUnderJump() const { return m_bIsUnderJump; }

			bool IsAhead(const glm::vec3& p_vFirstRacerPosition, const glm::vec3& p_vSecondRacerPosition) const;

		protected:
			ComponentTrackPiece(
				HeatStroke::GameObject* p_pGameObject,
				float p_fWidthX,
				float p_fWidthZ,
				HeightFunction p_eHeightFunction,
				float p_fHeight1,
				float p_fHeight2,
				PositionFunction p_ePositionFunction,
				glm::vec3 p_vPivotPosition,
				glm::vec3 p_vPivotAxis,
				std::vector<OffroadSquare> p_vOffroadSquares,
				std::vector<OffroadTriangle> p_vOffroadTriangles,
				bool p_bIsUnderJump);

		private:
			float m_fWidthX;
			float m_fWidthZ;

			HeightFunction m_eHeightFunction;
			float m_fHeight1;
			float m_fHeight2;

			PositionFunction m_ePositionFunction;
			glm::vec3 m_vPivotPosition;
			glm::vec3 m_vPivotAxis;

			bool IsAheadOnStraight(const glm::vec3& p_vFirstRacerPosition, const glm::vec3& p_vSecondRacerPosition) const;
			bool IsAheadOnTurn(const glm::vec3& p_vFirstRacerPosition, const glm::vec3& p_vSecondRacerPosition) const;

			glm::vec4 CalculatePlane() const;
			float GetDistanceToPlane(const glm::vec4& p_vPlane, const glm::vec3& p_vPoint) const;
			glm::vec3 CalculatePivot() const;
			glm::vec3 CalculatePivotAxis() const;

			static PositionFunction ParsePositionFunction(const tinyxml2::XMLNode* p_pNode);
			static glm::vec3 ParsePivotPosition(const tinyxml2::XMLNode* p_pNode);
			static glm::vec3 ParsePivotAxis(const tinyxml2::XMLNode* p_pNode);

			std::vector<OffroadSquare> m_vOffroadSquares;
			std::vector<OffroadTriangle> m_vOffroadTriangles;

			bool m_bIsUnderJump;

			bool PointInTriangle(const glm::vec2 &p, const glm::vec2 &p0, const glm::vec2 &p1, const glm::vec2 &p2);
	};
}

#endif // COMPONENT_TRACK_PIECE_H