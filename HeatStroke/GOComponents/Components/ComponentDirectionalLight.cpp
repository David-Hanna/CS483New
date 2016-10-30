//------------------------------------------------------------------------
// ComponentDirectionalLight
// Author:	David Hanna
//	
// Adds a Directional Light to the scene for a GameObject.
//------------------------------------------------------------------------

#include "ComponentDirectionalLight.h"

HeatStroke::Component* HeatStroke::ComponentDirectionalLight::CreateComponent(HeatStroke::GameObject* p_pGameObject, tinyxml2::XMLNode* p_pBaseNode, tinyxml2::XMLNode* p_pOverrideNode)
{
	assert(p_pGameObject != nullptr);

	glm::vec3 vDirectionalLightDirection(0.0f, -1.0f, 0.0f);
	glm::vec3 vDirectionalLightColor(1.0f, 1.0f, 1.0f);

	if (p_pBaseNode != nullptr)
	{
		ParseNode(p_pBaseNode, vDirectionalLightDirection, vDirectionalLightColor);
	}
	if (p_pOverrideNode != nullptr)
	{
		ParseNode(p_pOverrideNode, vDirectionalLightDirection, vDirectionalLightColor);
	}

	return new ComponentDirectionalLight(p_pGameObject, vDirectionalLightDirection, vDirectionalLightColor);
}

HeatStroke::ComponentDirectionalLight::ComponentDirectionalLight(HeatStroke::GameObject* p_pGameObject, const glm::vec3& p_vDirectionalLightDirection, const glm::vec3& p_vDirectionalLightColor) :
	Component(p_pGameObject),
	m_mDirectionalLight(p_vDirectionalLightDirection, p_vDirectionalLightColor)
{
}

HeatStroke::ComponentDirectionalLight::~ComponentDirectionalLight()
{
	SceneManager::Instance()->RemoveDirectionalLight(&m_mDirectionalLight);
}

void HeatStroke::ComponentDirectionalLight::Init()
{
	SceneManager::Instance()->AddDirectionalLight(&m_mDirectionalLight);
}

void HeatStroke::ComponentDirectionalLight::ParseNode(tinyxml2::XMLNode* p_pNode, glm::vec3& p_vDirectionalLightDirection, glm::vec3& p_vDirectionalLightColor)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_DirectionalLight") == 0);

	for (tinyxml2::XMLNode* pChildNode = p_pNode->FirstChild();
		pChildNode != nullptr;
		pChildNode = pChildNode->NextSibling())
	{
		// skip comments
		if (pChildNode->ToComment() != nullptr)
			continue;

		const char* szNodeName = pChildNode->Value();
		tinyxml2::XMLElement* pElement = pChildNode->ToElement();

		if (strcmp(szNodeName, "DirectionalLightDirection") == 0)
		{
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "x", p_vDirectionalLightDirection.x, p_vDirectionalLightDirection.x);
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "y", p_vDirectionalLightDirection.y, p_vDirectionalLightDirection.y);
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "z", p_vDirectionalLightDirection.z, p_vDirectionalLightDirection.z);
		}
		else if (strcmp(szNodeName, "DirectionalLightColor") == 0)
		{
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "r", p_vDirectionalLightColor.r, p_vDirectionalLightColor.r);
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "g", p_vDirectionalLightColor.g, p_vDirectionalLightColor.g);
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "b", p_vDirectionalLightColor.b, p_vDirectionalLightColor.b);
		}
	}
}