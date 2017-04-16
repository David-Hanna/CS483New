//------------------------------------------------------------------------
// ComponentRotateOverTime
// Author:	David Hanna
//	
// Provides a rotation over time behaviour to an object.
//------------------------------------------------------------------------

#include "ComponentRotateOverTime.h"

HeatStroke::ComponentRotateOverTime::ComponentRotateOverTime(
	GameObject* p_pGameObject,
	float p_fRotationPerSecondX,
	float p_fRotationPerSecondY,
	float p_fRotationPerSecondZ)
	:
	Component(p_pGameObject),
	m_fRotationPerSecondX(p_fRotationPerSecondX),
	m_fRotationPerSecondY(p_fRotationPerSecondY),
	m_fRotationPerSecondZ(p_fRotationPerSecondZ)
{
}

HeatStroke::Component* HeatStroke::ComponentRotateOverTime::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode
	)
{
	// Make some assertions on the parameters.
	assert(p_pGameObject != nullptr);

	// The values we need to fill by the end of parsing.
	float fRotationPerSecondX = 0.0f;
	float fRotationPerSecondY = 0.0f;
	float fRotationPerSecondZ = 0.0f;

	// Parse the elements of the base node.
	if (p_pBaseNode != nullptr)
	{
		ParseNode(p_pBaseNode, fRotationPerSecondX, fRotationPerSecondY, fRotationPerSecondZ);
	}
	// Then override with the Override node.
	if (p_pOverrideNode != nullptr)
	{
		ParseNode(p_pBaseNode, fRotationPerSecondX, fRotationPerSecondY, fRotationPerSecondZ);
	}

	// Change seconds to milliseconds for ease of computation in Update()
	return new ComponentRotateOverTime(
		p_pGameObject,
		fRotationPerSecondX,
		fRotationPerSecondY,
		fRotationPerSecondZ
	);
}

void HeatStroke::ComponentRotateOverTime::Update(const float p_fDelta)
{
	m_pGameObject->GetTransform().RotateXYZ(
		m_fRotationPerSecondX * p_fDelta,
		m_fRotationPerSecondY * p_fDelta,
		m_fRotationPerSecondZ * p_fDelta);
}

void HeatStroke::ComponentRotateOverTime::ParseNode(
	tinyxml2::XMLNode* p_pNode,
	float& p_fRotationPerSecondX,
	float& p_fRotationPerSecondY,
	float& p_fRotationPerSecondZ
)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_RotateOverTime") == 0);

	for (tinyxml2::XMLNode* pChildNode = p_pNode->FirstChild();
		pChildNode != nullptr;
		pChildNode = pChildNode->NextSibling())
	{
		// skip comments
		if (pChildNode->ToComment() != nullptr)
			continue;

		const char* szNodeName = pChildNode->Value();
		tinyxml2::XMLElement* pElement = pChildNode->ToElement();

		if (strcmp(szNodeName, "RotationPerSecond") == 0)
		{
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "x", p_fRotationPerSecondX, p_fRotationPerSecondX);
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "y", p_fRotationPerSecondY, p_fRotationPerSecondY);
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "z", p_fRotationPerSecondZ, p_fRotationPerSecondZ);
		}
	}
}