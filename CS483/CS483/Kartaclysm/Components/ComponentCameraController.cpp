//----------------------------------------------------------------------------
// ComponentCameraController.cpp
// Author: David MacIntosh
//
// Component that handles camera positioning.
//----------------------------------------------------------------------------

#include "ComponentCameraController.h"

#include "Common.h"
#include "GameObject.h"

#include "GameObjectManager.h"

namespace Kartaclysm
{
	ComponentCameraController::ComponentCameraController(
		HeatStroke::GameObject* p_pGameObject)
		:
		Component(p_pGameObject),
		m_pGameObject(p_pGameObject)
	{
		m_pTargetKart = m_pGameObject->GetManager()->GetGameObject("Kart");
	}

	ComponentCameraController::~ComponentCameraController()
	{
	}

	HeatStroke::Component* ComponentCameraController::CreateComponent(
		HeatStroke::GameObject* p_pGameObject,
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		//assert(p_pGameObject != nullptr);

		// Defaults
		/*float fFOV = 45.0f;
		float fAspectRatioWidth = 1280.0f;
		float fAspectRatioHeight = 720.0f;
		float fNearClip = 0.1f;
		float fFarClip = 1000.0f;*/
		std::string strTargetGUID("");
		HeatStroke::GameObject* pTargetGameObject = nullptr;

		// All parameters are optional. Some cameras may be attached "as is". (example_level.xml)
		if (p_pBaseNode != nullptr)
		{
			//ParseNode(p_pBaseNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
		}
		if (p_pOverrideNode != nullptr)
		{
			//ParseNode(p_pOverrideNode, fFOV, fAspectRatioWidth, fAspectRatioHeight, fNearClip, fFarClip, strTargetGUID);
		}

		// Apply the target if one was set.
		/*if (strTargetGUID != "")
		{
		pTargetGameObject = p_pGameObject->GetManager()->GetGameObject(strTargetGUID);
		}*/

		return new ComponentCameraController(
			p_pGameObject/*,
						 fFOV,
						 fAspectRatioWidth / fAspectRatioHeight,
						 fNearClip,
						 fFarClip,
						 pTargetGameObject*/
						 );
	}

	void ComponentCameraController::Update(const float p_fDelta)
	{
		glm::vec3 offset = glm::vec3(0.0f, 0.5f, -1.0f);

		offset = offset * m_pTargetKart->GetTransform().GetRotation();
		offset = offset * glm::vec3(-1.0f, 1.0f, 1.0f);
		offset = offset + m_pTargetKart->GetTransform().GetTranslation();

		m_pGameObject->GetTransform().SetTranslation(offset);
	}
}