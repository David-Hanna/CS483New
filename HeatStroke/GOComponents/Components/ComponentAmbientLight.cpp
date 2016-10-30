//------------------------------------------------------------------------
// ComponentAmbientLight
// Author:	David Hanna
//	
// Adds an Ambient Light to the scene for a GameObject.
//------------------------------------------------------------------------

#include "ComponentAmbientLight.h"

HeatStroke::Component* HeatStroke::ComponentAmbientLight::CreateComponent(HeatStroke::GameObject* p_pGameObject, tinyxml2::XMLNode* p_pBaseNode, tinyxml2::XMLNode* p_pOverrideNode)
{
	assert(p_pGameObject != nullptr);

	glm::vec3 vAmbientLightColor(0.2f, 0.2f, 0.2f);

	if (p_pBaseNode != nullptr)
	{
		ParseNode(p_pBaseNode, vAmbientLightColor);
	}
	if (p_pOverrideNode != nullptr)
	{
		ParseNode(p_pOverrideNode, vAmbientLightColor);
	}

	return new ComponentAmbientLight(p_pGameObject, vAmbientLightColor);
}

HeatStroke::ComponentAmbientLight::ComponentAmbientLight(HeatStroke::GameObject* p_pGameObject, const glm::vec3& p_vAmbientLightColor) :
	Component(p_pGameObject),
	m_mAmbientLight(p_vAmbientLightColor)
{
}

HeatStroke::ComponentAmbientLight::~ComponentAmbientLight()
{
	SceneManager::Instance()->RemoveAmbientLight(&m_mAmbientLight);
}

void HeatStroke::ComponentAmbientLight::Init()
{
	SceneManager::Instance()->AddAmbientLight(&m_mAmbientLight);
}

void HeatStroke::ComponentAmbientLight::ParseNode(tinyxml2::XMLNode* p_pNode, glm::vec3& p_vAmbientLightColor)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_AmbientLight") == 0);

	for (tinyxml2::XMLNode* pChildNode = p_pNode->FirstChild();
		pChildNode != nullptr;
		pChildNode = pChildNode->NextSibling())
	{
		// skip comments
		if (pChildNode->ToComment() != nullptr)
			continue;

		const char* szNodeName = pChildNode->Value();
		tinyxml2::XMLElement* pElement = pChildNode->ToElement();

		if (strcmp(szNodeName, "AmbientLightColor") == 0)
		{
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "r", p_vAmbientLightColor.r, p_vAmbientLightColor.r);
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "g", p_vAmbientLightColor.g, p_vAmbientLightColor.g);
			HeatStroke::EasyXML::GetOptionalFloatAttribute(pElement, "b", p_vAmbientLightColor.b, p_vAmbientLightColor.b);
		}
	}
}