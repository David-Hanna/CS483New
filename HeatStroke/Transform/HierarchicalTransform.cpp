//-------------------------------------------------------------------------------------
// HierarchicalTransform
// Author: David Hanna
//
// Subclass of Transform which adds parents, children, and derived transformations.
//-------------------------------------------------------------------------------------

#include "HierarchicalTransform.h"

namespace HeatStroke
{
	HierarchicalTransform::HierarchicalTransform(const std::string& p_strName) 
		:
		Transform(),
		m_strName(p_strName),
		m_bScaleDirty(true),
		m_bRotationDirty(true),
		m_vParentScale(1.0f, 1.0f, 1.0f),
		m_qParentRotation(1.0f, 0.0f, 0.0f, 0.0f),
		m_vParentTranslation(0.0f, 0.0f, 0.0f),
		m_vWorldTranslation(0.0f, 0.0f, 0.0f),
		m_pParent(nullptr),
		m_vChildren()
	{
	}


	void HierarchicalTransform::Scale(const glm::vec3& p_vScale)
	{
		Transform::Scale(p_vScale);
		UpdateChildrensParentScale();
	}


	void HierarchicalTransform::SetScale(const glm::vec3& p_vScale)
	{
		Transform::SetScale(p_vScale);
		UpdateChildrensParentScale();
	}


	void HierarchicalTransform::Rotate(const glm::vec3& p_vRotation)
	{
		Transform::Rotate(p_vRotation);
		UpdateChildrensParentRotation();
	}


	void HierarchicalTransform::SetRotation(const float p_fAngle, const glm::vec3& p_vRotation)
	{
		Transform::SetRotation(p_fAngle, p_vRotation);
		UpdateChildrensParentRotation();
	}


	void HierarchicalTransform::SetRotation(const glm::quat& p_qRotation)
	{
		Transform::SetRotation(p_qRotation);
		UpdateChildrensParentRotation();
	}


	void HierarchicalTransform::SetRotationXYZ(const float p_fAngle, float x, float y, float z)
	{
		Transform::SetRotationXYZ(p_fAngle, x, y, z);
		UpdateChildrensParentRotation();
	}


	void HierarchicalTransform::Translate(const glm::vec3 &p_vTranslation)
	{
		Transform::Translate(p_vTranslation);
		UpdateChildrensParentTranslation();
	}


	void HierarchicalTransform::SetTranslation(const glm::vec3 &p_vTranslation)
	{
		Transform::SetTranslation(p_vTranslation);
		UpdateChildrensParentTranslation();
	}


	const glm::vec3& HierarchicalTransform::GetScale() const
	{
		UpdateComponents();

		if (m_pParent == nullptr)
			return GetRelativeScale();

		if (m_bScaleDirty)
		{
			m_bScaleDirty = false;
			m_vScale = glm::vec3(
				m_vParentScale.x * m_vScale.x,
				m_vParentScale.y * m_vScale.y,
				m_vParentScale.z * m_vScale.z
			);
		}

		return m_vScale;
	}


	const glm::quat& HierarchicalTransform::GetRotation() const
	{
		UpdateComponents();

		if (m_pParent == nullptr)
			return GetRelativeRotation();

		if (m_bRotationDirty)
		{
			m_bRotationDirty = false;
			m_qDerivedRotation = m_qParentRotation * GetRelativeRotation();
		}

		return m_qDerivedRotation;
	}


	const glm::vec3& HierarchicalTransform::GetTranslation() const
	{
		UpdateComponents();

		if (m_pParent == nullptr)
			return GetRelativeTranslation();

		// Trigger a recompute of the final derived transformation. The final
		// position can depend on applying a rotation around the parent.
		if (m_bTransformDirty)
		{
			GetTransform(); // Updates m_vWorldTranslation
		}

		return m_vWorldTranslation;
	}


	const glm::mat4& HierarchicalTransform::GetTransform() const
	{
		// If there's no parent, just use this one's own transform.
		if (m_pParent == nullptr)
			return this->GetRelativeTransform();

		// Run this block only if the derived transform has changed,
		// because it's expensive.
		if (m_bTransformDirty)
		{
			// Set flag to false because we're about to update it.
			m_bTransformDirty = false;

			// Start from the identity matrix and build up the new transform based
			// on components.
			m_mTransform = glm::mat4();

			//-----------------------------------------------------
			// Read the comments following in this block bottom-up.
			//-----------------------------------------------------

			// Last: Apply the parent translation, followed by this translation.
			m_mTransform = glm::translate(m_mTransform, m_vTranslation);
			m_mTransform = glm::translate(m_mTransform, m_vParentTranslation);

			// Third: Apply own rotation around the origin.
			m_mTransform *= glm::mat4_cast(m_qRotation);

			// Second: Rotate it around the center point of the parent.
			m_mTransform = glm::translate(m_mTransform, -m_vTranslation);
			m_mTransform *= glm::mat4_cast(m_qParentRotation);
			m_mTransform = glm::translate(m_mTransform, m_vTranslation);

			// First: apply the scales of the parent and this.
			m_mTransform = glm::scale(m_mTransform, m_vScale);
			m_mTransform = glm::scale(m_mTransform, m_vParentScale);

			// Update World Translation at the same time as the Transform
			m_vWorldTranslation = glm::vec3(m_mTransform[3]);
		}

		return m_mTransform;
	}


	void HierarchicalTransform::SetParent(HierarchicalTransform* p_pParent)
	{
		if (m_pParent == p_pParent)
			return;

		if (m_pParent != nullptr)
		{
			m_pParent->RemoveChild(m_strName);
		}

		m_pParent = p_pParent;

		if (p_pParent != nullptr)
		{
			m_vParentScale = m_pParent->GetScale();
			m_qParentRotation = m_pParent->GetRotation();
			m_vParentTranslation = m_pParent->GetTranslation();
			m_bScaleDirty = true;
			m_bRotationDirty = true;
			m_bTransformDirty = true;
			m_pParent->m_vChildren.push_back(this);
		}
	}


	void HierarchicalTransform::AddChild(HierarchicalTransform* p_pChild)
	{
		assert(p_pChild != nullptr);
		p_pChild->SetParent(this);
	}


	void HierarchicalTransform::RemoveChild(const std::string& p_strChildName)
	{
		std::vector<HierarchicalTransform*>::iterator it = m_vChildren.begin(), end = m_vChildren.end();
		for (; it != end; it++)
		{
			HierarchicalTransform* pChild = *it;
			if (p_strChildName.compare(pChild->GetName()) == 0)
			{
				pChild->m_pParent = nullptr;
				m_vChildren.erase(it);
				return;
			}
		}
	}


	void HierarchicalTransform::RemoveAllChildren()
	{
		std::vector<HierarchicalTransform*>::iterator it = m_vChildren.begin();
		// Ask for end again each iteration, since the iterator is invalidated 
		// when erase is called and we get a new one.
		for (; it != m_vChildren.end(); it++)
		{
			HierarchicalTransform* pChild = *it;
			pChild->m_pParent = nullptr;
			it = m_vChildren.erase(it);
		}
	}


	void HierarchicalTransform::DeleteChild(const std::string& p_strChildName)
	{
		std::vector<HierarchicalTransform*>::iterator it = m_vChildren.begin(), end = m_vChildren.end();
		for (; it != end; it++)
		{
			HierarchicalTransform* pChild = *it;
			if (p_strChildName.compare(pChild->GetName()) == 0)
			{
				pChild->DeleteAllChildren();
				m_vChildren.erase(it);
				delete pChild;
				pChild = nullptr;
				return;
			}
		}
	}


	void HierarchicalTransform::DeleteAllChildren()
	{
		std::vector<HierarchicalTransform*>::iterator it = m_vChildren.begin();
		// Ask for end again each iteration, since the iterator is invalidated 
		// when erase is called and we get a new one.
		for (; it != m_vChildren.end(); it++)
		{
			HierarchicalTransform* pChild = *it;
			pChild->DeleteAllChildren();
			it = m_vChildren.erase(it);
			delete pChild;
			pChild = nullptr;
		}
	}


	HierarchicalTransform* HierarchicalTransform::GetChild(const std::string& p_strChildName)
	{
		std::vector<HierarchicalTransform*>::iterator it = m_vChildren.begin(), end = m_vChildren.end();
		for (; it != end; it++)
		{
			if ((*it)->GetName().compare(p_strChildName) == 0)
			{
				return (*it);
			}
		}
		return nullptr;
	}


	glm::vec3 HierarchicalTransform::GetCenterTranslation() const
	{
		// Take the average of all the centers of itself and its children.
		glm::vec3 vCenterTranslation = GetTranslation();

		float fDivisor = 1.0f;
		std::vector<HierarchicalTransform*>::const_iterator it = this->begin(), end = this->end();
		for (; it != end; it++)
		{
			vCenterTranslation += (*it)->GetTranslation();
			fDivisor += 1.0f;
		}

		return vCenterTranslation /= fDivisor;
	}


	void HierarchicalTransform::ManuallyUpdateParent()
	{
		UpdateChildrensParentScale();
		UpdateChildrensParentRotation();
		UpdateChildrensParentTranslation();
	}


	void HierarchicalTransform::UpdateChildrensParentScale()
	{
		std::vector<HierarchicalTransform*>::iterator it = m_vChildren.begin(), end = m_vChildren.end();
		for (; it != end; it++)
		{
			// Update the value for the child and let it know it needs to recalculate.
			(*it)->m_vParentScale = GetScale();
			(*it)->m_bScaleDirty = true;
			(*it)->m_bTransformDirty = true;
			// Propagate the update to its children.
			(*it)->UpdateChildrensParentScale();
		}
	}


	void HierarchicalTransform::UpdateChildrensParentRotation()
	{
		std::vector<HierarchicalTransform*>::iterator it = m_vChildren.begin(), end = m_vChildren.end();
		for (; it != end; it++)
		{
			// Update the value for the child and let it know it needs to recalculate.
			(*it)->m_qParentRotation = GetRotation();
			(*it)->m_bRotationDirty = true;
			(*it)->m_bTransformDirty = true;
			// Propagate the update to its children.
			(*it)->UpdateChildrensParentRotation();
		}
	}


	void HierarchicalTransform::UpdateChildrensParentTranslation()
	{
		std::vector<HierarchicalTransform*>::iterator it = m_vChildren.begin(), end = m_vChildren.end();
		for (; it != end; it++)
		{
			// Update the value for the child and let it know it needs to recalculate.
			(*it)->m_vParentTranslation = GetTranslation();
			(*it)->m_bTransformDirty = true;
			// Propagate the update to its children.
			(*it)->UpdateChildrensParentTranslation();
		}
	}

	//TODO: delete this (once all bugs are worked out)
	//Matt: just using this to make sure transform values are correct
	void HierarchicalTransform::Print() const
	{
		const glm::vec3 scale = GetScale();
		const glm::vec3 trans = GetTranslation();
		printf("scale: %f, %f, %f\n", scale.x, scale.y, scale.z);
		printf("translate: %f, %f, %f\n", trans.x, trans.y, trans.z);
	}
}