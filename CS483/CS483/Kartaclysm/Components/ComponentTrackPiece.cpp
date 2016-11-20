#include "ComponentTrackPiece.h"

namespace Kartaclysm
{
	ComponentTrackPiece::ComponentTrackPiece(HeatStroke::GameObject* p_pGameObject, float p_fWidth, float p_fHeight)
		:
		Component(p_pGameObject),
		m_fWidth(p_fWidth),
		m_fHeight(p_fHeight)
	{
	}

	ComponentTrackPiece::~ComponentTrackPiece()
	{
	}

	HeatStroke::Component* ComponentTrackPiece::CreateComponent(HeatStroke::GameObject* p_pGameObject, tinyxml2::XMLNode* p_pBaseNode, tinyxml2::XMLNode* p_pOverrideNode)
	{
		float fWidth;
		HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Bounds"), "width", fWidth);

		float fHeight;
		HeatStroke::EasyXML::GetRequiredFloatAttribute(p_pBaseNode->FirstChildElement("Bounds"), "height", fHeight);

		return new ComponentTrackPiece(p_pGameObject, fWidth, fHeight);
	}

	void ComponentTrackPiece::Init()
	{
	}

	void ComponentTrackPiece::Update(float p_fDelta)
	{
	}
}