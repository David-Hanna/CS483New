//----------------------------------------------------------------------------
// ComponentPerspectiveCamera
// Author: David Hanna
//
// Adds a perspective-view scene camera into the scene manager.
//----------------------------------------------------------------------------

#include "ComponentPerspectiveCamera.h"

HeatStroke::ComponentPerspectiveCamera::ComponentPerspectiveCamera(
	GameObject* p_pGameObject,
	const float p_fFOV,
	const float p_fAspectRatio,
	const float p_fNearClip,
	const float p_fFarClip,
	GameObject* p_pTargetGameObject)
	:
	Component(p_pGameObject),
	m_mScenePerspectiveCamera(
		glm::vec3(),
		glm::vec3(),
		glm::vec3(0.0f, 1.0f, 0.0f),
		p_fFOV,
		p_fAspectRatio,
		p_fNearClip,
		p_fFarClip
	),
	m_pTargetGameObject(p_pTargetGameObject)
{
	SceneManager::Instance()->AddPerspectiveCamera(&m_mScenePerspectiveCamera);
}

HeatStroke::ComponentPerspectiveCamera::~ComponentPerspectiveCamera()
{
	SceneManager::Instance()->RemovePerspectiveCamera(&m_mScenePerspectiveCamera);
}

HeatStroke::Component* HeatStroke::ComponentPerspectiveCamera::CreateComponent(
	HeatStroke::GameObject* p_pGameObject, 
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	assert(p_pGameObject != nullptr);

	// Defaults
	float fFOV = 45.0f;
	float fAspectRatioWidth = 1280.0f;
	float fAspectRatioHeight = 720.0f;
	float fNearClip = 0.1f;
	float fFarClip = 1000.0f;
	std::string strTargetGUID("");
	GameObject* pTargetGameObject = nullptr;

	if (p_pBaseNode != nullptr)
	{
		ParseNode(p_pBaseNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
	}
	if (p_pOverrideNode != nullptr)
	{
		ParseNode(p_pOverrideNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
	}

	// Apply the target if one was set.
	if (strTargetGUID != "")
	{
		pTargetGameObject = p_pGameObject->GetManager()->GetGameObject(strTargetGUID);
	}

	return new ComponentPerspectiveCamera(
		p_pGameObject, 
		fFOV, 
		fAspectRatioWidth / fAspectRatioHeight, 
		fNearClip, 
		fFarClip,
		pTargetGameObject
	);
}

void HeatStroke::ComponentPerspectiveCamera::Update(const float p_fDelta)
{
	glm::vec3 vPosition = m_pGameObject->GetTransform().GetTranslation();
	m_mScenePerspectiveCamera.SetPosition(vPosition);

	// If this is not nullptr, then we have a target we're following.
	if (m_pTargetGameObject != nullptr)
	{
		m_mScenePerspectiveCamera.SetTarget(m_pTargetGameObject->GetTransform().GetTranslation());
	}
	// Otherwise, just face in the same direction as the GameObject.
	else
	{
		glm::vec3 vForward = glm::normalize(m_pGameObject->GetTransform().GetRotation() * glm::vec3(0.0f, 0.0f, 1.0f));
		m_mScenePerspectiveCamera.SetTarget(vPosition + vForward);
	}
}


void HeatStroke::ComponentPerspectiveCamera::SetScenePerspectiveCamera(const HeatStroke::ScenePerspectiveCamera& p_mScenePerspectiveCamera)
{
	SceneManager::Instance()->RemovePerspectiveCamera(&m_mScenePerspectiveCamera);
	m_mScenePerspectiveCamera = p_mScenePerspectiveCamera;
	SceneManager::Instance()->AddPerspectiveCamera(&m_mScenePerspectiveCamera);
}


void HeatStroke::ComponentPerspectiveCamera::ParseNode(
	tinyxml2::XMLNode* p_pNode,
	float& p_fFOV,
	float& p_fAspectRatioWidth,
	float& p_fAspectRatioHeight,
	float& p_fNearClip,
	float& p_fFarClip,
	std::string& p_strTargetGUID
)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_PerspectiveCamera") == 0);

	for (tinyxml2::XMLNode*  pChildNode = p_pNode->FirstChild();
		pChildNode != nullptr;
		pChildNode = pChildNode->NextSibling())
	{
		// skip comments
		if (pChildNode->ToComment() != nullptr)
			continue;

		const char* szNodeName = pChildNode->Value();
		tinyxml2::XMLElement* pElement = pChildNode->ToElement();

		if (strcmp(szNodeName, "FieldOfView") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pElement, "value", p_fFOV);
		}
		else if (strcmp(szNodeName, "AspectRatioWidth") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pElement, "value", p_fAspectRatioWidth);
		}
		else if (strcmp(szNodeName, "AspectRatioHeight") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pElement, "value", p_fAspectRatioHeight);
		}
		else if (strcmp(szNodeName, "NearClip") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pElement, "value", p_fNearClip);
		}
		else if (strcmp(szNodeName, "FarClip") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pElement, "value", p_fFarClip);
		}
		else if (strcmp(szNodeName, "Target") == 0)
		{
			EasyXML::GetRequiredStringAttribute(pElement, "guid", p_strTargetGUID);
		}
	}
}