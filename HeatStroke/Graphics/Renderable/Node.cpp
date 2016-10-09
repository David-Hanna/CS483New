//======================================================================================
// File: Node
// Author: David Hanna
//
// A Node is an object in the world with a name, transform, bounding volume for culling,
// a parent, and a list of children.
//======================================================================================

#include "Node.h"

namespace HeatStroke
{
	void Node::Update(const float p_fDelta)
	{
		std::vector<HierarchicalTransform*>::iterator it = m_vChildren.begin(), end = m_vChildren.end();
		for (; it != end; it++)
		{
			Node* pNode = dynamic_cast<Node*>(*it);
			if (pNode != nullptr)
			{
				pNode->Update(p_fDelta);
			}
		}
	}

	void Node::Render(const Camera* p_pCamera)
	{
		std::vector<HierarchicalTransform*>::iterator it = m_vChildren.begin(), end = m_vChildren.end();
		for (; it != end; it++)
		{
			Node* pNode = dynamic_cast<Node*>(*it);
			if (pNode != nullptr)
			{
				pNode->Render(p_pCamera);
			}
		}
	}
}