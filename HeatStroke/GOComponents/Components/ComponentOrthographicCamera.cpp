//----------------------------------------------------------------------------
// ComponentOrthographicCamera
// Author: David Hanna
//
// Adds an orthographic-view scene camera into the scene manager.
//----------------------------------------------------------------------------

#include "ComponentOrthographicCamera.h"

HeatStroke::ComponentOrthographicCamera::ComponentOrthographicCamera(
	GameObject* p_pGameObject,
	SceneManager::SceneViewportSelection p_eViewportSelection,
	const glm::vec3& p_vPosition,
	const glm::vec3& p_vTarget,
	const glm::vec3& p_vUp,
	const float p_fLeft,
	const float p_fRight,
	const float p_fBottom,
	const float p_fTop)
	:
	Component(p_pGameObject),
	m_mSceneOrthographicCamera(
		p_vPosition,
		p_vTarget,
		p_vUp,
		p_fLeft,
		p_fRight,
		p_fBottom,
		p_fTop
	),
	m_eViewportSelection(p_eViewportSelection)
{
	SceneManager::Instance()->AddOrthographicCamera(&m_mSceneOrthographicCamera, m_eViewportSelection);
}

HeatStroke::ComponentOrthographicCamera::~ComponentOrthographicCamera()
{
	SceneManager::Instance()->RemoveOrthographicCamera(m_eViewportSelection);
}

HeatStroke::Component* HeatStroke::ComponentOrthographicCamera::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	assert(p_pGameObject != nullptr);

	// Defaults
	std::string strViewportSelection("full");
	glm::vec3 vPosition(0.0f, 0.0f, 0.0f);
	glm::vec3 vTarget(0.0f, 0.0f, -5.0f);
	glm::vec3 vUp(0.0f, 1.0f, 0.0f);
	float fLeft(-100.0f);
	float fRight(100.0f);
	float fBottom(-100.0f);
	float fTop(100.0f);

	if (p_pBaseNode != nullptr)
	{
		ParseNode(p_pBaseNode, strViewportSelection, vPosition, vTarget, vUp, fLeft, fRight, fBottom, fTop);
	}
	if (p_pOverrideNode != nullptr)
	{
		ParseNode(p_pBaseNode, strViewportSelection, vPosition, vTarget, vUp, fLeft, fRight, fBottom, fTop);
	}

	return new ComponentOrthographicCamera(
		p_pGameObject, 
		SceneManager::ParseViewportSelection(strViewportSelection), 
		vPosition, 
		vTarget, 
		vUp, 
		fLeft, 
		fRight, 
		fBottom, 
		fTop
	);
}

void HeatStroke::ComponentOrthographicCamera::ParseNode(
	tinyxml2::XMLNode* p_pNode,
	std::string& p_strViewportSelection,
	glm::vec3& p_vPosition,
	glm::vec3& p_vTarget,
	glm::vec3& p_vUp,
	float& p_fLeft,
	float& p_fRight,
	float& p_fBottom,
	float& p_fTop
)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_OrthographicCamera") == 0);

	EasyXML::GetRequiredStringAttribute(p_pNode->ToElement(), "viewport", p_strViewportSelection);

	for (tinyxml2::XMLElement*  pChildElement = p_pNode->FirstChildElement();
		pChildElement != nullptr;
		pChildElement = pChildElement->NextSiblingElement())
	{
		const char* szNodeName = pChildElement->Value();

		if (strcmp(szNodeName, "Position") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "x", p_vPosition.x);
			EasyXML::GetRequiredFloatAttribute(pChildElement, "y", p_vPosition.y);
			EasyXML::GetRequiredFloatAttribute(pChildElement, "z", p_vPosition.z);
		}
		else if (strcmp(szNodeName, "Target") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "x", p_vTarget.x);
			EasyXML::GetRequiredFloatAttribute(pChildElement, "y", p_vTarget.y);
			EasyXML::GetRequiredFloatAttribute(pChildElement, "z", p_vTarget.z);
		}
		else if (strcmp(szNodeName, "Up") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "x", p_vUp.x);
			EasyXML::GetRequiredFloatAttribute(pChildElement, "y", p_vUp.y);
			EasyXML::GetRequiredFloatAttribute(pChildElement, "z", p_vUp.z);
		}
		else if (strcmp(szNodeName, "Left") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fLeft);
		}
		else if (strcmp(szNodeName, "Right") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fRight);
		}
		else if (strcmp(szNodeName, "Bottom") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fBottom);
		}
		else if (strcmp(szNodeName, "Top") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fTop);
		}
	}
}