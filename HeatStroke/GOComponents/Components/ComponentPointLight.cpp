//-----------------------------------------------------------------------
// ComponentPointLight.h
// Author: David Hanna
//
// Represents a component which acts as a point light from the
// parent GameObject's transform.
//-----------------------------------------------------------------------

#include "ComponentPointLight.h"

HeatStroke::ComponentPointLight::ComponentPointLight(
	GameObject* p_pGameObject,
	const glm::vec3& p_vAmbient,
	const glm::vec3& p_vDiffuse,
	const glm::vec3& p_vSpecular,
	const glm::vec3& p_vAttenuation,
	const float      p_fRange)
	:
	Component(p_pGameObject),
	m_mScenePointLight(
		p_pGameObject->GetTransform().GetTranslation(),
		p_vAmbient,
		p_vDiffuse,
		p_vSpecular,
		p_vAttenuation,
		p_fRange
	)
{
	SceneManager::Instance()->AddPointLight(&m_mScenePointLight);
}

HeatStroke::ComponentPointLight::~ComponentPointLight()
{
	SceneManager::Instance()->RemovePointLight(&m_mScenePointLight);
}

HeatStroke::Component* HeatStroke::ComponentPointLight::CreateComponent(
	HeatStroke::GameObject* p_pGameObject, 
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	assert(p_pGameObject != nullptr);

	// All attributes are optional and default to all zeros.
	glm::vec3 vAmbient, vDiffuse, vSpecular, vAttenuation;
	float fRange(0.0f);

	// Read the base node first, then override with attributes from the override node.
	if (p_pBaseNode != nullptr)
	{
		ParseNode(p_pBaseNode, vAmbient, vDiffuse, vSpecular, vAttenuation, fRange);
	}
	if (p_pOverrideNode != nullptr)
	{
		ParseNode(p_pOverrideNode, vAmbient, vDiffuse, vSpecular, vAttenuation, fRange);
	}

	return new ComponentPointLight(p_pGameObject, vAmbient, vDiffuse, vSpecular, vAttenuation, fRange);
}

void HeatStroke::ComponentPointLight::Update(float p_fDelta)
{
	m_mScenePointLight.SetPosition(m_pGameObject->GetTransform().GetTranslation());
}

void HeatStroke::ComponentPointLight::ParseNode(
	tinyxml2::XMLNode* p_pNode,
	glm::vec3& p_vAmbient,
	glm::vec3& p_vDiffuse,
	glm::vec3& p_vSpecular,
	glm::vec3& p_vAttenuation,
	float& p_fRange)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_PointLight") == 0);

	// Iterate elements in the XML
	for (tinyxml2::XMLNode* pChildNode = p_pNode->FirstChild(); pChildNode != nullptr; pChildNode = pChildNode->NextSibling())
	{
		// skip comments
		if (pChildNode->ToComment() != nullptr)
			continue;

		const char* szNodeName = pChildNode->Value();
		tinyxml2::XMLElement* pElement = pChildNode->ToElement();

		if (strcmp(pElement->Value(), "Ambient") == 0)
		{
			ParseColorVectorElement(pElement, p_vAmbient);
		}
		else if (strcmp(pElement->Value(), "Diffuse") == 0)
		{
			ParseColorVectorElement(pElement, p_vDiffuse);
		}
		else if (strcmp(pElement->Value(), "Specular") == 0)
		{
			ParseColorVectorElement(pElement, p_vSpecular);
		}
		else if (strcmp(pElement->Value(), "Attenuation") == 0)
		{
			EasyXML::GetOptionalFloatAttribute(pElement, "constant", p_vAttenuation.x, p_vAttenuation.x);
			EasyXML::GetOptionalFloatAttribute(pElement, "linear", p_vAttenuation.y, p_vAttenuation.y);
			EasyXML::GetOptionalFloatAttribute(pElement, "quadratic", p_vAttenuation.z, p_vAttenuation.z);
		}
		else if (strcmp(pElement->Value(), "Range") == 0)
		{
			EasyXML::GetOptionalFloatAttribute(pElement, "value", p_fRange, p_fRange);
		}
	}
}

void HeatStroke::ComponentPointLight::ParseColorVectorElement(tinyxml2::XMLElement* p_pElement, glm::vec3& p_vColorVector)
{
	EasyXML::GetOptionalFloatAttribute(p_pElement, "r", p_vColorVector.r, p_vColorVector.r);
	EasyXML::GetOptionalFloatAttribute(p_pElement, "g", p_vColorVector.g, p_vColorVector.g);
	EasyXML::GetOptionalFloatAttribute(p_pElement, "b", p_vColorVector.b, p_vColorVector.b);
}