//=================================================================================
// SceneQuadTreeNode
// Author: David Hanna
//
// Manages a Node and four children Nodes.
//=================================================================================

#ifndef SCENE_QUAD_TREE_NODE_H
#define SCENE_QUAD_TREE_NODE_H

#include <vector>
#include <glm.hpp>
#include <Collisions.h>

#include "Node.h"
#include "LineDrawer.h"
#include "AABB.h"

namespace HeatStroke
{
	struct SceneQuadTreeNode
	{
		HeatStroke::AABB aabb;

		std::vector<Node*> nodesInRegion;

		SceneQuadTreeNode* topLeft;
		SceneQuadTreeNode* topRight;
		SceneQuadTreeNode* bottomLeft;
		SceneQuadTreeNode* bottomRight;

		SceneQuadTreeNode(const HeatStroke::AABB& p_mAABB)
			:
			aabb(p_mAABB),
			nodesInRegion(),
			topLeft(nullptr),
			topRight(nullptr),
			bottomLeft(nullptr),
			bottomRight(nullptr)
		{}
	};

	SceneQuadTreeNode* MakeQuadTree(const std::vector<Node*>& p_vNodes, const HeatStroke::AABB& p_mAABB, LineDrawer& p_mLineDrawer);
	void Split(SceneQuadTreeNode* p_pSceneQuadTreeNode, const HeatStroke::AABB& p_mAABB, const unsigned int p_uiRecursionDepth, LineDrawer& p_mLineDrawer);
	SceneQuadTreeNode* AddNode(SceneQuadTreeNode* p_pSceneQuadTreeNode, const HeatStroke::AABB& p_mAABB, Node* p_pNode);
	void DestroyQuadTree(SceneQuadTreeNode* p_mSceneQuadTreeNode);
	bool IsLeafNode(const SceneQuadTreeNode& p_mSceneQuadTreeNode);
	unsigned int Size(const SceneQuadTreeNode* p_pSceneQuadTreeNode);
}

#endif // SCENE_QUAD_TREE_NODE_H