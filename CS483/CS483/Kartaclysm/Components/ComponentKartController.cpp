//----------------------------------------------------------------------------
// ComponentKartController.cpp
// Author: David MacIntosh
//
// Component that handles generic kart behaviour.
//----------------------------------------------------------------------------

#include "ComponentKartController.h"

#define _USE_MATH_DEFINES

#include "GameObject.h"
#include "IO\KeyboardInputBuffer.h"
#include <math.h>

ComponentKartController::ComponentKartController(
	HeatStroke::GameObject* p_pGameObject)
	:
	Component(p_pGameObject)
{
	pGameObject = p_pGameObject;

	fSpeed = 0.0f;
	fDirection = 0.0f;
}

ComponentKartController::~ComponentKartController()
{
}

HeatStroke::Component* ComponentKartController::CreateComponent(
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

	return new ComponentKartController(
		p_pGameObject/*,
		fFOV,
		fAspectRatioWidth / fAspectRatioHeight,
		fNearClip,
		fFarClip,
		pTargetGameObject*/
		);
}

void ComponentKartController::Update(const float p_fDelta)
{

	if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown(GLFW_KEY_UP))
	{
		fSpeed += fmax((fMaxSpeedStat - fmax(fSpeed, 0.0f)) * fAccelerationStat * fSpeedScale, -fSpeed * fAccelerationFrictionStat * 2.0f) * p_fDelta;
	}
	else if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown(GLFW_KEY_DOWN))
	{
		
		fSpeed += fmin((-fMaxReverseSpeedStat - fmin(fSpeed, 0.0f)) * fReverseAccelerationStat * fSpeedScale, -fSpeed * fAccelerationFrictionStat * 2.0f) * p_fDelta;
	}
	else
	{
		fSpeed -= fSpeed * fAccelerationFrictionStat * p_fDelta;
	}

	if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown(GLFW_KEY_LEFT))
	{
		fDirection += fTurningStat * p_fDelta;

		if (fDirection >= M_PI * 2.0f)
		{
			fDirection -= M_PI * 2.0f;
		}
	}
	if (HeatStroke::KeyboardInputBuffer::Instance()->IsKeyDown(GLFW_KEY_RIGHT))
	{
		fDirection += -fTurningStat * p_fDelta;

		if (fDirection < 0)
		{
			fDirection += M_PI * 2.0f;
		}
	}

	pGameObject->GetTransform().TranslateXYZ(fSpeed * cosf(fDirection), 0.0f, fSpeed * sinf(fDirection));
	pGameObject->GetTransform().SetRotation(glm::quat(glm::vec3(0.0f, fDirection, 0.0f)));

	HeatStroke::HierarchicalTransform transform = pGameObject->GetTransform();
	printf("Position:\n  X: %f\n  Y: %f\n  Z: %f\nRotation:\n  X: %f\n  Y: %f\n  Z: %f\n\n", transform.GetTranslation().x, transform.GetTranslation().y, transform.GetTranslation().z, transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z);
}