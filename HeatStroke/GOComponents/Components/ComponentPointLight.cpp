//-----------------------------------------------------------------------
// ComponentPointLight.h
// Author: David Hanna
//
// Represents a component which acts as a point light from the
// parent GameObject's transform.
//-----------------------------------------------------------------------

#include "ComponentPointLight.h"

namespace HeatStroke
{
	//------------------------------------------------------------------------------
	// ComponentPointLight::ComponentPointLight
	//
	// Constructor
	//------------------------------------------------------------------------------
	ComponentPointLight::ComponentPointLight(
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
		SceneManager::Instance()->AddLight(&m_mScenePointLight);
	}

	//------------------------------------------------------------------------------
	// ComponentPointLight::~ComponentPointLight
	//
	// Destructor.
	//------------------------------------------------------------------------------
	ComponentPointLight::~ComponentPointLight()
	{
		SceneManager::Instance()->RemoveLight(&m_mScenePointLight);
	}

	//------------------------------------------------------------------------------
	// ComponentPointLight::CreateComponent
	// Parameters:
	//	HeatStroke::GameObject* p_pGameObject - the game object that owns this component.
	//	tinyxml2::XMLNode* p_pNode - the node to read the initialization data from.
	// Returns:
	//	HeatStroke::Component* - the point light component created from the data.
	//
	// Factory construction method. Returns an initialized ComponentPointLight
	// instance if the passed in XML is valid, asserts otherwise. (fail loudly)
	//------------------------------------------------------------------------------
	HeatStroke::Component* ComponentPointLight::CreateComponent(
		HeatStroke::GameObject* p_pGameObject, 
		tinyxml2::XMLNode* p_pBaseNode,
		tinyxml2::XMLNode* p_pOverrideNode)
	{
		assert(p_pGameObject != nullptr);

		// All attributes are optional and default to all zeros.
		glm::vec3 vAmbient, vDiffuse, vSpecular, vAttenuation;
		float fRange;

		// Read the base node first, then override with attributes from the override node.
		if (p_pBaseNode != nullptr)
		{
			ParseNode(p_pBaseNode, vAmbient, vDiffuse, vSpecular, vAttenuation, fRange);
		}
		if (p_pOverrideNode != nullptr)
		{
			ParseNode(p_pOverrideNode, vAmbient, vDiffuse, vSpecular, vAttenuation, fRange);
		}

		return new ComponentPointLight(
			p_pGameObject,
			vAmbient,
			vDiffuse,
			vSpecular,
			vAttenuation,
			fRange);
	}

	//---------------------------------------------------------------------------------------------
	// Method: Update
	// Parameter: const float p_fDelta
	// Returns: void
	//
	// Updates the light position with the position of the parent Game Object.
	//---------------------------------------------------------------------------------------------
	void ComponentPointLight::Update(float p_fDelta)
	{
		m_mScenePointLight.SetPosition(m_pGameObject->GetTransform().GetTranslation());
	}

	//--------------------------------------------------------------------------------------------
	// ComponentPointLight::SetScenePointLight
	//
	// Sets this component to use a new ScenePointLight.
	//--------------------------------------------------------------------------------------------
	void ComponentPointLight::SetScenePointLight(const ScenePointLight& p_mScenePointLight)
	{
		SceneManager::Instance()->RemoveLight(&m_mScenePointLight);
		m_mScenePointLight = p_mScenePointLight;
		SceneManager::Instance()->AddLight(&m_mScenePointLight);
	}

	//---------------------------------------------------------------------------------------------
	// ComponentPointLight::ParseNode
	//
	// A helper method for parsing a node of type "GOC_PointLight".
	//---------------------------------------------------------------------------------------------
	void ComponentPointLight::ParseNode(
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

	//----------------------------------------------------------------------
	// ComponentPointLight::ParseColorVectorElement
	//
	// Helper method for parsing an xml element with rgb attributes.
	//----------------------------------------------------------------------
	void ComponentPointLight::ParseColorVectorElement(tinyxml2::XMLElement* p_pElement, glm::vec3& p_vColorVector)
	{
		EasyXML::GetOptionalFloatAttribute(p_pElement, "r", p_vColorVector.r, p_vColorVector.r);
		EasyXML::GetOptionalFloatAttribute(p_pElement, "g", p_vColorVector.g, p_vColorVector.g);
		EasyXML::GetOptionalFloatAttribute(p_pElement, "b", p_vColorVector.b, p_vColorVector.b);
	}
}