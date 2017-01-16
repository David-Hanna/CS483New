//-------------------------------------------------------------------------------------
// HierarchicalTransform
// Author: David Hanna
//
// Subclass of Transform which adds parents, children, and derived transformations.
//-------------------------------------------------------------------------------------

#ifndef HIERARCHICAL_TRANSFORM_H
#define HIERARCHICAL_TRANSFORM_H

#include "Transform.h"

#include <string>
#include <glm.hpp>
#include <vector>

namespace HeatStroke
{
	class HierarchicalTransform : public Transform
	{
	public:
		HierarchicalTransform(const std::string& p_strName);
		virtual ~HierarchicalTransform() {} 

		// The unique name of this Transform for the purposes of searching through children.
		const std::string& GetName() const { return m_strName; }

		//------------------------------------------------------------------------------
		// Transform-Related Methods
		//------------------------------------------------------------------------------
		// Overrides for some functions from Transform which now need to
		// consider the fact there may be a parent and children.
		virtual void Scale(const glm::vec3& p_vScale) override;
		virtual void SetScale(const glm::vec3& p_vScale) override;

		virtual void Rotate(const glm::vec3& p_vRotation) override;
		virtual void SetRotation(const float p_fAngle, const glm::vec3& p_vRotation) override;
		virtual void SetRotation(const glm::quat& p_qRotation) override;
		virtual void SetRotationXYZ(const float p_fAngle, float x, float y, float z) override;

		virtual void Translate(const glm::vec3 &p_vTranslation) override;
		virtual void SetTranslation(const glm::vec3 &p_vTranslation) override;

		// Relative accessors give the transform of this object relative to the transform of the parent.
		virtual const glm::vec3& GetRelativeScale() const				{ return Transform::GetScale(); }
		virtual const glm::quat& GetRelativeRotation() const			{ return Transform::GetRotation(); }
		virtual const glm::vec3& GetRelativeTranslation() const			{ return Transform::GetTranslation(); }
		virtual const glm::mat4& GetRelativeTransform() const			{ return Transform::GetTransform(); }

		// "Finalized" accessors give the transform of this object relative to (0, 0, 0).
		virtual const glm::vec3& GetScale() const override;
		virtual const glm::quat& GetRotation() const override;
		virtual const glm::vec3& GetTranslation() const override;
		virtual const glm::mat4& GetTransform() const override;
		
		//----------------------------------------------------------------------------
		// Hierarchy-Related Methods
		//----------------------------------------------------------------------------
		// Parent Transform
		virtual void SetParent(HierarchicalTransform* p_pParent);
		virtual HierarchicalTransform* GetParent()									{ return m_pParent; }

		// Child Transforms
		virtual void AddChild(HierarchicalTransform* p_pChild);
		virtual void RemoveChild(const std::string& p_strChildName);
		virtual void RemoveAllChildren();
		virtual void DeleteChild(const std::string& p_strChildName);
		virtual void DeleteAllChildren();
		virtual HierarchicalTransform* GetChild(const std::string& p_strChildName);

		// The Average of all the Translations of this and all children.
		virtual glm::vec3 GetCenterTranslation() const;

		// Iterators over children.
		virtual std::vector<HierarchicalTransform*>::iterator begin()				{ return m_vChildren.begin(); }
		virtual std::vector<HierarchicalTransform*>::iterator end()					{ return m_vChildren.end(); }
		virtual std::vector<HierarchicalTransform*>::const_iterator begin() const	{ return m_vChildren.begin(); }
		virtual std::vector<HierarchicalTransform*>::const_iterator end() const		{ return m_vChildren.end(); }

		// Manual update for use when creating GameObjects
		virtual void ManuallyUpdateParent();

		//TODO: delete this (once all bugs are worked out)
		//Matt: just using this to check that values of transform are correct
		void Print() const;

	protected:
		// The name of this transform, which distinguishes it from other transforms.
		const std::string m_strName;

		// Whether the parent's scale has changed, invalidating this one's derived scale.
		mutable bool m_bScaleDirty;

		// Whether the parent's rotation has changed, invalidating this one's derived rotation.
		mutable bool m_bRotationDirty;

		// We don't need a derived translation dirty flag because the derived transform dirty doubles
		// as one for translation as well, since it needs to be recomputed to find the correct transformation
		// (after rotating around the parent) anyway.

		// The parent's scale, rotation and translation which also incorporates
		// the values of its parent, and so on up the chain.
		glm::vec3 m_vParentScale;
		glm::quat m_qParentRotation;
		glm::vec3 m_vParentTranslation;
		mutable glm::quat m_qDerivedRotation;

		// The overall translation of this object
		mutable glm::vec3 m_vWorldTranslation;

		// Parent transform reference
		HierarchicalTransform* m_pParent;

		// List of children transforms
		std::vector<HierarchicalTransform*> m_vChildren;
		
	protected:
		// Helper methods for a parent to use to update the Parent
		// components of its children.
		void UpdateChildrensParentScale();
		void UpdateChildrensParentRotation();
		void UpdateChildrensParentTranslation();
	};
}

#endif // HIERARCHICAL_TRANSFORM_H