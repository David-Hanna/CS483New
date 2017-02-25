//----------------------------------------------------------------------------
// ComponentPerspectiveCamera
// Author: David Hanna
//
// Adds a perspective-view scene camera into the scene manager.
//----------------------------------------------------------------------------

#include "ComponentPerspectiveCamera.h"
#include "ComponentKartController.h"

HeatStroke::ComponentPerspectiveCamera::ComponentPerspectiveCamera(
	GameObject* p_pGameObject,
	SceneManager::SceneViewportSelection p_eViewportSelection,
	const glm::vec3& p_vPosition,
	const glm::vec3& p_vTarget,
	const glm::vec3& p_vUp,
	const float p_fFOV,
	const float p_fAspectRatio,
	const float p_fNearClip,
	const float p_fFarClip,
	GameObject* p_pTargetGameObject)
	:
	Component(p_pGameObject),
	m_mScenePerspectiveCamera(
		p_vPosition,
		p_vTarget,
		p_vUp,
		p_fFOV,
		p_fAspectRatio,
		p_fNearClip,
		p_fFarClip
	),
	m_eViewportSelection(p_eViewportSelection),
	m_pTargetGameObject(p_pTargetGameObject)
{
	SceneManager::Instance()->AddPerspectiveCamera(&m_mScenePerspectiveCamera, m_eViewportSelection);
}

HeatStroke::ComponentPerspectiveCamera::~ComponentPerspectiveCamera()
{
	SceneManager::Instance()->RemovePerspectiveCamera(m_eViewportSelection);
}

HeatStroke::Component* HeatStroke::ComponentPerspectiveCamera::CreateComponent(
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
	float fFOV = 45.0f;
	float fAspectRatioWidth = 1280.0f;
	float fAspectRatioHeight = 720.0f;
	float fNearClip = 0.1f;
	float fFarClip = 1000.0f;
	std::string strTargetGUID("");
	GameObject* pTargetGameObject = nullptr;

	if (p_pBaseNode != nullptr)
	{
		ParseNode(p_pBaseNode, strViewportSelection, vPosition, vTarget, vUp, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
	}
	if (p_pOverrideNode != nullptr)
	{
		ParseNode(p_pOverrideNode, strViewportSelection, vPosition, vTarget, vUp, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
	}

	// Apply the target if one was set.
	if (strTargetGUID != "")
	{
		pTargetGameObject = p_pGameObject->GetManager()->GetGameObject(strTargetGUID);
	}

	return new ComponentPerspectiveCamera(
		p_pGameObject, 
		SceneManager::ParseViewportSelection(strViewportSelection),
		vPosition,
		vTarget,
		vUp,
		fFOV, 
		fAspectRatioWidth / fAspectRatioHeight, 
		fNearClip, 
		fFarClip,
		pTargetGameObject
	);
}

void HeatStroke::ComponentPerspectiveCamera::Update(const float p_fDelta)
{
	GameObject* pParent = m_pGameObject->GetParent();

	if (pParent != nullptr && pParent->HasTag("Racer"))
	{
		Kartaclysm::ComponentKartController* pKartController = dynamic_cast<Kartaclysm::ComponentKartController*>(pParent->GetComponent("GOC_KartController"));
		float fSwerve = pKartController->GetRotationMod();

		pParent->GetTransform().RotateXYZ((pKartController->IsInWheelie() ? -pKartController->GetWheelieRotation() : 0.0f) - pKartController->GetOffroadRumble(), -fSwerve, 0.0f);

		m_mScenePerspectiveCamera.SetPosition(m_pGameObject->GetTransform().GetTranslation());
		m_mScenePerspectiveCamera.SetTarget(m_pTargetGameObject->GetTransform().GetTranslation());

		pParent->GetTransform().RotateXYZ((pKartController->IsInWheelie() ? pKartController->GetWheelieRotation() : 0.0f) + pKartController->GetOffroadRumble(), fSwerve, 0.0f);
	}
	// If this is not nullptr, then we have a target we're following.
	else if (m_pTargetGameObject != nullptr)
	{
		glm::vec3 vPosition = m_pGameObject->GetTransform().GetTranslation();
		m_mScenePerspectiveCamera.SetPosition(vPosition);
		m_mScenePerspectiveCamera.SetTarget(m_pTargetGameObject->GetTransform().GetTranslation());
	}
	// Otherwise, just face in the same direction as the GameObject.
	else
	{
		glm::vec3 vPosition = m_pGameObject->GetTransform().GetTranslation();
		m_mScenePerspectiveCamera.SetPosition(vPosition);
		glm::vec3 vForward = glm::normalize(m_pGameObject->GetTransform().GetRotation() * glm::vec3(0.0f, 0.0f, 1.0f));
		m_mScenePerspectiveCamera.SetTarget(vPosition + vForward);
	}
}


void HeatStroke::ComponentPerspectiveCamera::ParseNode(
	tinyxml2::XMLNode* p_pNode,
	std::string& p_strViewportSelection,
	glm::vec3& p_vPosition,
	glm::vec3& p_vTarget,
	glm::vec3& p_vUp,
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
			EasyXML::GetOptionalFloatAttribute(pChildElement, "x", p_vTarget.x, p_vTarget.x);
			EasyXML::GetOptionalFloatAttribute(pChildElement, "y", p_vTarget.y, p_vTarget.y);
			EasyXML::GetOptionalFloatAttribute(pChildElement, "z", p_vTarget.z, p_vTarget.z);
			EasyXML::GetOptionalStringAttribute(pChildElement, "guid", p_strTargetGUID, p_strTargetGUID);
		}
		else if (strcmp(szNodeName, "Up") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "x", p_vUp.x);
			EasyXML::GetRequiredFloatAttribute(pChildElement, "y", p_vUp.y);
			EasyXML::GetRequiredFloatAttribute(pChildElement, "z", p_vUp.z);
		}
		else if (strcmp(szNodeName, "FieldOfView") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fFOV);
		}
		else if (strcmp(szNodeName, "AspectRatioWidth") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fAspectRatioWidth);
		}
		else if (strcmp(szNodeName, "AspectRatioHeight") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fAspectRatioHeight);
		}
		else if (strcmp(szNodeName, "NearClip") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fNearClip);
		}
		else if (strcmp(szNodeName, "FarClip") == 0)
		{
			EasyXML::GetRequiredFloatAttribute(pChildElement, "value", p_fFarClip);
		}
	}
}