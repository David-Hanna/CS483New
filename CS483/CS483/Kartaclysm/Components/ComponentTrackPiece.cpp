#include "ComponentTrackPiece.h"

namespace Kartaclysm
{
	ComponentTrackPiece::ComponentTrackPiece(HeatStroke::GameObject* p_pGameObject, float p_fWidthX, float p_fWidthZ)
		:
		Component(p_pGameObject),
		m_fWidthX(p_fWidthX),
		m_fWidthZ(p_fWidthZ)
	{
	}

	ComponentTrackPiece::~ComponentTrackPiece()
	{
	}

	HeatStroke::Component* ComponentTrackPiece::CreateComponent(HeatStroke::GameObject* p_pGameObject, tinyxml2::XMLNode* p_pBaseNode, tinyxml2::XMLNode* p_pOverrideNode)
	{
		float fWidthX;
		HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Bounds"), "widthX", fWidthX);

		float fWidthZ;
		HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Bounds"), "widthZ", fWidthZ);

		return new ComponentTrackPiece(p_pGameObject, fWidthX, fWidthZ);
	}

	void ComponentTrackPiece::Init()
	{
	}

	void ComponentTrackPiece::Update(float p_fDelta)
	{
	}

	bool ComponentTrackPiece::CheckInBounds(glm::vec3 p_pPosition)
	{
		float xMin, xMax, zMin, zMax;

		//if (m_pGameObject->GetTransform().GetRotation().)

		glm::vec3 origin = m_pGameObject->GetTransform().GetTranslation();
		xMin = origin.x - (m_fWidthX / 2.0f);
		xMax = origin.x + (m_fWidthX / 2.0f);
		zMin = origin.z - (m_fWidthZ / 2.0f);
		zMax = origin.z + (m_fWidthZ / 2.0f);

		if (p_pPosition.x >= xMin && p_pPosition.x < xMax && p_pPosition.z >= zMin && p_pPosition.z < zMax)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}