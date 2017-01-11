//------------------------------------------------------------------------
// ComponentWallCollider
// Author:	David MacIntosh
//	
// Collider that takes the shape of a wall
//------------------------------------------------------------------------

#include "ComponentWallCollider.h"

#include "Collisions\CollisionManager.h"

HeatStroke::ComponentWallCollider::ComponentWallCollider(
	HeatStroke::GameObject* p_pGameObject,
	glm::vec3 p_pOffset,
	glm::vec3 p_pSurfaceNormal,
	float p_fHeight,
	float p_fWidth)
	:
	ComponentCollider(p_pGameObject, true),
	m_pOffset(p_pOffset),
	m_pSurfaceNormal(p_pSurfaceNormal),
	m_fHeight(p_fHeight),
	m_fWidth(p_fWidth),
	m_pDebugLineDrawer(nullptr)
{
	HeatStroke::CollisionManager::Instance()->RegisterCollider(this, GetGameObject()->GetGUID());
}

HeatStroke::ComponentWallCollider::~ComponentWallCollider()
{
	HeatStroke::CollisionManager::Instance()->UnregisterCollider(GetGameObject()->GetGUID());

	if (m_pDebugLineDrawer != nullptr)
	{
		SceneManager::Instance()->RemoveLineDrawer(m_pDebugLineDrawer);
		delete m_pDebugLineDrawer;
		m_pDebugLineDrawer = nullptr;
	}
}

HeatStroke::Component* HeatStroke::ComponentWallCollider::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	// Make some assertions on the parameters.
	assert(p_pGameObject != nullptr);

	// The values we need to fill by the end of parsing.
	glm::vec3 offset;
	glm::vec3 surfaceNormal;
	float height;
	float width;

	// Neither base nor override node are mandatory, but it is mandatory
	// that all strings are given values between the two of them.

	// Parse the elements of the base node.
	if (p_pBaseNode != nullptr)
	{
		//ParseNode(p_pBaseNode, strOBJFileName);
		ParseNode(p_pBaseNode, offset, surfaceNormal, height, width);
	}
	// Then override with the Override node.
	if (p_pOverrideNode != nullptr)
	{
		//ParseNode(p_pOverrideNode, strOBJFileName);
		ParseNode(p_pOverrideNode, offset, surfaceNormal, height, width);
	}

	// Check that we got everything we needed.
	//assert(strOBJFileName != "");

	// Now we can create and return the Component.
	return new ComponentWallCollider(
		p_pGameObject,
		offset,
		surfaceNormal,
		height,
		width
		);
}

void HeatStroke::ComponentWallCollider::Init()
{
}

void HeatStroke::ComponentWallCollider::Update(const float p_fDelta)
{
	m_pPreviousPosition = m_pPosition;
	m_pPosition = m_pGameObject->GetTransform().GetTranslation();
	// PreRender(); // TODO: Isn't called naturally because FamilyID is not "GOC_Renderable"
}

void HeatStroke::ComponentWallCollider::PreRender()
{
	SyncTransform();
	if (m_bDebugRender)
	{
		if (m_pDebugLineDrawer == nullptr)
		{
			m_pDebugLineDrawer = new LineDrawer("Assets/Kart/kart.vsh", "Assets/Kart/kart.fsh");
			SceneManager::Instance()->AddLineDrawer(m_pDebugLineDrawer);

			glm::vec3 top = m_pOffset + glm::vec3(0.0f, m_fHeight, 0.0f);
			glm::vec3 bottom = m_pOffset + glm::vec3(0.0f, -m_fHeight, 0.0f);
			glm::vec3 left = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_pSurfaceNormal) * m_fWidth;
			glm::vec3 right = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_pSurfaceNormal) * -m_fWidth;

			// Border
			m_pDebugLineDrawer->AddLine(top + left, top + right, HeatStroke::Color4(1.0f, 1.0f, 0.0f, 1.0f));
			m_pDebugLineDrawer->AddLine(top + right, bottom + right, HeatStroke::Color4(1.0f, 1.0f, 0.0f, 1.0f));
			m_pDebugLineDrawer->AddLine(bottom + right, bottom + left, HeatStroke::Color4(1.0f, 1.0f, 0.0f, 1.0f));
			m_pDebugLineDrawer->AddLine(bottom + left, top + left, HeatStroke::Color4(1.0f, 1.0f, 0.0f, 1.0f));

			// Inside
			m_pDebugLineDrawer->AddLine(top + left, bottom + right, HeatStroke::Color4(1.0f, 1.0f, 0.0f, 1.0f));
			m_pDebugLineDrawer->AddLine(top + right, bottom + left, HeatStroke::Color4(1.0f, 1.0f, 0.0f, 1.0f));
		}
	}
}

void HeatStroke::ComponentWallCollider::SyncTransform()
{
	if (m_pDebugLineDrawer != nullptr)
	{
		m_pDebugLineDrawer->SetTransform(GetGameObject()->GetTransform().GetTransform());
	}
}

void HeatStroke::ComponentWallCollider::ParseNode(
	tinyxml2::XMLNode* p_pNode,
	glm::vec3& p_pOffset,
	glm::vec3& p_pSurfaceNormal,
	float& p_fHeight,
	float& p_fWidth)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_WallCollider") == 0);

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

		if (strcmp(szNodeName, "SurfaceNormal") == 0)
		{
			HeatStroke::EasyXML::GetRequiredFloatAttribute(pElement, "x", p_pSurfaceNormal.x);
			HeatStroke::EasyXML::GetRequiredFloatAttribute(pElement, "y", p_pSurfaceNormal.y);
			HeatStroke::EasyXML::GetRequiredFloatAttribute(pElement, "z", p_pSurfaceNormal.z);
		}

		if (strcmp(szNodeName, "Wall") == 0)
		{
			HeatStroke::EasyXML::GetRequiredFloatAttribute(pElement, "height", p_fHeight);
			HeatStroke::EasyXML::GetRequiredFloatAttribute(pElement, "width", p_fWidth);
		}
	}
}