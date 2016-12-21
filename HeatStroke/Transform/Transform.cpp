//------------------------------------------------------------------------
// Transform
// Author:	David Hanna
//------------------------------------------------------------------------

#include "Transform.h"

HeatStroke::Transform::Transform()
	:
	m_bComponentsDirty(false),
	m_bTransformDirty(true),
	m_vScale(1.0f, 1.0f, 1.0f),
	m_qRotation(1.0f, 0.0f, 0.0f, 0.0f),
	m_vTranslation(0.0f, 0.0f, 0.0f),
	m_mTransform()
{
}

void HeatStroke::Transform::Scale(const glm::vec3& p_vScale)
{
	m_bTransformDirty = true;
	m_vScale *= p_vScale;
}

void HeatStroke::Transform::SetScale(const glm::vec3& p_vScale)
{
	m_bTransformDirty = true;
	m_vScale = p_vScale;
}

const glm::vec3& HeatStroke::Transform::GetScale() const
{
	UpdateComponents();
	return m_vScale;
}

void HeatStroke::Transform::Rotate(const glm::vec3& p_vRotation)
{
	m_bTransformDirty = true;
	m_qRotation *= glm::quat(glm::vec3(1.0f, 0.0f, 0.0f) * p_vRotation.x);
	m_qRotation *= glm::quat(glm::vec3(0.0f, 1.0f, 0.0f) * p_vRotation.y);
	m_qRotation *= glm::quat(glm::vec3(0.0f, 0.0f, 1.0f) * p_vRotation.z);
}

void HeatStroke::Transform::SetRotation(const float p_fAngle, const glm::vec3& p_vRotation)
{
	m_bTransformDirty = true;
	m_qRotation = glm::angleAxis(glm::radians(p_fAngle), p_vRotation);
}

const glm::quat& HeatStroke::Transform::GetRotation() const
{
	UpdateComponents();
	return m_qRotation;
}

void HeatStroke::Transform::Translate(const glm::vec3 &p_vTranslation)
{
	m_bTransformDirty = true;
	m_vTranslation += p_vTranslation;
}

void HeatStroke::Transform::SetTranslation(const glm::vec3 &p_vTranslation)
{
	m_bTransformDirty = true;
	m_vTranslation = p_vTranslation;
}

const glm::vec3& HeatStroke::Transform::GetTranslation() const
{
	UpdateComponents();
	return m_vTranslation;
}

void HeatStroke::Transform::SetTransform(const glm::mat4& p_mTransform)
{
	m_bComponentsDirty = true;
	m_mTransform = p_mTransform;
}

const glm::mat4& HeatStroke::Transform::GetTransform() const
{
	if (m_bTransformDirty)
	{
		m_bTransformDirty = false;
		m_mTransform = glm::mat4();
		m_mTransform = glm::translate(m_mTransform, m_vTranslation);
		m_mTransform *= glm::mat4_cast(m_qRotation);
		m_mTransform = glm::scale(m_mTransform, m_vScale);
	}

	return m_mTransform;
}

void HeatStroke::Transform::ParseTransformNode(tinyxml2::XMLNode* p_pTransformNode)
{
	if (p_pTransformNode == nullptr)
		return;

	assert(strcmp(p_pTransformNode->Value(), "Transform") == 0);

	for (tinyxml2::XMLNode* pTransformChildNode = p_pTransformNode->FirstChild();
		pTransformChildNode != nullptr;
		pTransformChildNode = pTransformChildNode->NextSibling())
	{
		// skip comment
		if (pTransformChildNode->ToComment() != nullptr)
			continue;

		tinyxml2::XMLElement* pTransformChildElement = pTransformChildNode->ToElement();

		if (strcmp(pTransformChildElement->Value(), "Translation") == 0)
		{
			float x, y, z;
			EasyXML::GetOptionalFloatAttribute(pTransformChildElement, "x", x, 0.0f);
			EasyXML::GetOptionalFloatAttribute(pTransformChildElement, "y", y, 0.0f);
			EasyXML::GetOptionalFloatAttribute(pTransformChildElement, "z", z, 0.0f);
			SetTranslation(glm::vec3(x, y, z));
		}
		else if (strcmp(pTransformChildElement->Value(), "Rotation") == 0)
		{
			float angle, x, y, z;
			EasyXML::GetOptionalFloatAttribute(pTransformChildElement, "angle", angle, 0.0f);
			EasyXML::GetOptionalFloatAttribute(pTransformChildElement, "x", x, 0.0f);
			EasyXML::GetOptionalFloatAttribute(pTransformChildElement, "y", y, 0.0f);
			EasyXML::GetOptionalFloatAttribute(pTransformChildElement, "z", z, 0.0f);
			SetRotation(angle, glm::vec3(x, y, z));
		}
		else if (strcmp(pTransformChildElement->Value(), "Scale") == 0)
		{
			float x, y, z;
			EasyXML::GetOptionalFloatAttribute(pTransformChildElement, "x", x, 1.0f);
			EasyXML::GetOptionalFloatAttribute(pTransformChildElement, "y", y, 1.0f);
			EasyXML::GetOptionalFloatAttribute(pTransformChildElement, "z", z, 1.0f);
			SetScale(glm::vec3(x, y, z));
		}
	}
}

void HeatStroke::Transform::UpdateComponents() const
{
	if (m_bComponentsDirty)
	{
		m_bComponentsDirty = false;
		glm::vec3 vSkew;
		glm::vec4 vPerspective;
		glm::decompose(m_mTransform, m_vScale, m_qRotation, m_vTranslation, vSkew, vPerspective);
	}
}