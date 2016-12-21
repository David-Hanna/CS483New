//------------------------------------------------------------------------
// ComponentSphereCollider
// Author:	David MacIntosh
//	
// Collider that takes the shape of a sphere
//------------------------------------------------------------------------

#include "ComponentSphereCollider.h"

#include "Collisions\CollisionManager.h"

HeatStroke::ComponentSphereCollider::ComponentSphereCollider(
	HeatStroke::GameObject* p_pGameObject,
	glm::vec3 p_pOffset,
	float p_fRadius,
	bool p_bAppliesPhysics)
	:
	ComponentCollider(p_pGameObject, p_bAppliesPhysics),
	m_pOffset(p_pOffset),
	m_fRadius(p_fRadius)
{
	HeatStroke::CollisionManager::Instance()->RegisterCollider(this, GetGameObject()->GetGUID());
}

HeatStroke::ComponentSphereCollider::~ComponentSphereCollider()
{
	HeatStroke::CollisionManager::Instance()->UnregisterCollider(GetGameObject()->GetGUID());
}

HeatStroke::Component* HeatStroke::ComponentSphereCollider::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	// Make some assertions on the parameters.
	assert(p_pGameObject != nullptr);

	// The values we need to fill by the end of parsing.
	glm::vec3 offset = glm::vec3();
	float radius = 0.0f;
	bool physics = true;

	// Neither base nor override node are mandatory, but it is mandatory
	// that all strings are given values between the two of them.

	// Parse the elements of the base node.
	if (p_pBaseNode != nullptr)
	{
		//ParseNode(p_pBaseNode, strOBJFileName);
		ParseNode(p_pBaseNode, offset, radius, physics);
	}
	// Then override with the Override node.
	if (p_pOverrideNode != nullptr)
	{
		//ParseNode(p_pOverrideNode, strOBJFileName);
		ParseNode(p_pOverrideNode, offset, radius, physics);
	}

	// Check that we got everything we needed.
	//assert(radius > 0.0f);

	// Now we can create and return the Component.
	return new ComponentSphereCollider(
		p_pGameObject,
		offset,
		radius,
		physics
		);
}

void HeatStroke::ComponentSphereCollider::Init()
{
}

void HeatStroke::ComponentSphereCollider::Update(const float p_fDelta)
{
	m_pPreviousPosition = m_pPosition;
	m_pPosition = m_pGameObject->GetTransform().GetTranslation();
}

void HeatStroke::ComponentSphereCollider::SyncTransform()
{
}

void HeatStroke::ComponentSphereCollider::ParseNode(
	tinyxml2::XMLNode* p_pNode,
	glm::vec3& p_pOffset,
	float& p_fRadius,
	bool& p_bAppliesPhysics)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_SphereCollider") == 0);

	for (tinyxml2::XMLNode* pChildNode = p_pNode->FirstChild();
		pChildNode != nullptr;
		pChildNode = pChildNode->NextSibling())
	{
		// skip comments
		if (pChildNode->ToComment() != nullptr)
			continue;

		const char* szNodeName = pChildNode->Value();
		tinyxml2::XMLElement* pElement = pChildNode->ToElement();

		if (strcmp(szNodeName, "Offset") == 0)
		{
			HeatStroke::EasyXML::GetRequiredFloatAttribute(pElement, "x", p_pOffset.x);
			HeatStroke::EasyXML::GetRequiredFloatAttribute(pElement, "y", p_pOffset.y);
			HeatStroke::EasyXML::GetRequiredFloatAttribute(pElement, "z", p_pOffset.z);
		}
		else if (strcmp(szNodeName, "Sphere") == 0)
		{
			HeatStroke::EasyXML::GetRequiredFloatAttribute(pElement, "radius", p_fRadius);
		}
		else if (strcmp(szNodeName, "Physics") == 0)
		{
			HeatStroke::EasyXML::GetRequiredBoolAttribute(pElement, "applies", p_bAppliesPhysics);
		}
	}
}

void HeatStroke::ComponentSphereCollider::DebugRender(LineDrawer* p_pLineDrawer)
{
	glm::vec3 center = m_pGameObject->GetTransform().GetTranslation() + m_pOffset;

	// Top
	p_pLineDrawer->AddLine(center + glm::vec3(0.0f, m_fRadius, 0.0f), center + glm::vec3(m_fRadius, 0.0f, 0.0f), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));
	p_pLineDrawer->AddLine(center + glm::vec3(0.0f, m_fRadius, 0.0f), center + glm::vec3(-m_fRadius, 0.0f, 0.0f), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));
	p_pLineDrawer->AddLine(center + glm::vec3(0.0f, m_fRadius, 0.0f), center + glm::vec3(0.0f, 0.0f, m_fRadius), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));
	p_pLineDrawer->AddLine(center + glm::vec3(0.0f, m_fRadius, 0.0f), center + glm::vec3(0.0f, 0.0f, -m_fRadius), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));

	// Equator
	p_pLineDrawer->AddLine(center + glm::vec3(m_fRadius, 0.0f, 0.0f), center + glm::vec3(0.0f, 0.0f, m_fRadius), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));
	p_pLineDrawer->AddLine(center + glm::vec3(0.0f, 0.0f, m_fRadius), center + glm::vec3(-m_fRadius, 0.0f, 0.0f), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));
	p_pLineDrawer->AddLine(center + glm::vec3(-m_fRadius, 0.0f, 0.0f), center + glm::vec3(0.0f, 0.0f, -m_fRadius), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));
	p_pLineDrawer->AddLine(center + glm::vec3(0.0f, 0.0f, -m_fRadius), center + glm::vec3(m_fRadius, 0.0f, 0.0f), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));

	// Bottom
	p_pLineDrawer->AddLine(center + glm::vec3(0.0f, -m_fRadius, 0.0f), center + glm::vec3(m_fRadius, 0.0f, 0.0f), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));
	p_pLineDrawer->AddLine(center + glm::vec3(0.0f, -m_fRadius, 0.0f), center + glm::vec3(-m_fRadius, 0.0f, 0.0f), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));
	p_pLineDrawer->AddLine(center + glm::vec3(0.0f, -m_fRadius, 0.0f), center + glm::vec3(0.0f, 0.0f, m_fRadius), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));
	p_pLineDrawer->AddLine(center + glm::vec3(0.0f, -m_fRadius, 0.0f), center + glm::vec3(0.0f, 0.0f, -m_fRadius), HeatStroke::Color4(0.0f, 1.0f, 0.0f, 1.0f));
}