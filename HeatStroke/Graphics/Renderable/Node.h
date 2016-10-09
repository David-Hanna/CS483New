//======================================================================================
// File: Node
// Author: David Hanna
//
// A Node is an object in the world with a name, transform, bounding volume for culling,
// a parent, and a list of children.
//======================================================================================

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <algorithm>

#include "Collidable.h"
#include "HierarchicalTransform.h"
#include "Renderable.h"
#include "Frustum.h"

namespace HeatStroke
{
	class Node : public HeatStroke::HierarchicalTransform, public Renderable
	{
	public:
		Node(const std::string& p_strName) : HierarchicalTransform(p_strName) {}
		virtual ~Node() = 0 {}

		// Updates bounding spheres based on changes to transforms and children.
		// If overridden, subclass must call this Update when implementing its own.
		virtual void Update(const float p_fDelta);

		// Subclasses are responsible for providing some way of determining whether
		// they should be culled given a frustum.
		virtual bool ShouldBeRendered(const HeatStroke::Frustum& p_pFrustum) const = 0;

		// Renders the node to the screen.
		// If overridden, subclass must call this Render when implementing its own.
		virtual void Render(const Camera* p_pCamera) override;

		// Local and total collidable objects for comparing against frustum, aabb, etc.
		virtual const HeatStroke::Collidable& GetLocalCollidable() const = 0;
		virtual const HeatStroke::Collidable& GetTotalCollidable() const = 0;
	};
}

#endif // W_NODE_H