//------------------------------------------------------------------------
// Component3DModel
// Author:	David Hanna
//	
// Provides 3D rendering of a .obj model to the screen for a GameObject.
//------------------------------------------------------------------------

#include "Component3DModel.h"

HeatStroke::Component3DModel::Component3DModel(HeatStroke::GameObject* p_pGameObject, const std::string& p_strOBJFileName) :
	ComponentRenderable(p_pGameObject),
	m_mModel(p_strOBJFileName)
{
}

HeatStroke::Component3DModel::~Component3DModel()
{
	SceneManager::Instance()->RemoveModel(&m_mModel);
}

HeatStroke::Component* HeatStroke::Component3DModel::CreateComponent(
	HeatStroke::GameObject* p_pGameObject,
	tinyxml2::XMLNode* p_pBaseNode,
	tinyxml2::XMLNode* p_pOverrideNode)
{
	// Make some assertions on the parameters.
	assert(p_pGameObject != nullptr);

	// The values we need to fill by the end of parsing.
	std::string strOBJFileName("");

	// Neither base nor override node are mandatory, but it is mandatory
	// that all strings are given values between the two of them.

	// Parse the elements of the base node.
	if (p_pBaseNode != nullptr)
	{
		ParseNode(p_pBaseNode, strOBJFileName);
	}
	// Then override with the Override node.
	if (p_pOverrideNode != nullptr)
	{
		ParseNode(p_pOverrideNode, strOBJFileName);
	}

	// Check that we got everything we needed.
	assert(strOBJFileName != "");

	// Now we can create and return the Component.
	return new Component3DModel(
		p_pGameObject,
		strOBJFileName
	);
}

void HeatStroke::Component3DModel::Init()
{
	SceneManager::Instance()->AddModel(&m_mModel);
}

void HeatStroke::Component3DModel::Update(const float p_fDelta)
{
}

void HeatStroke::Component3DModel::SyncTransform()
{
	m_mModel.SetTransform(this->GetGameObject()->GetTransform().GetTransform());
}

void HeatStroke::Component3DModel::ParseNode(
	tinyxml2::XMLNode* p_pNode,
	std::string& p_strOBJFileName)
{
	assert(p_pNode != nullptr);
	assert(strcmp(p_pNode->Value(), "GOC_3DModel") == 0);

	for (tinyxml2::XMLNode* pChildNode = p_pNode->FirstChild();
		pChildNode != nullptr;
		pChildNode = pChildNode->NextSibling())
	{
		// skip comments
		if (pChildNode->ToComment() != nullptr)
			continue;

		const char* szNodeName = pChildNode->Value();
		tinyxml2::XMLElement* pElement = pChildNode->ToElement();

		if (strcmp(szNodeName, "OBJFileName") == 0)
		{
			HeatStroke::EasyXML::GetRequiredStringAttribute(pElement, "path", p_strOBJFileName);
		}
	}
}