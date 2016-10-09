//=================================================================================
// SceneQuadTreeNode
// Author: David Hanna
//
// Manages a Node and four children Nodes.
//=================================================================================

#include "SceneQuadTreeNode.h"

namespace HeatStroke
{
	SceneQuadTreeNode* MakeQuadTree(
		const std::vector<Node*>& p_vNodes,
		const HeatStroke::AABB& p_mAABB,
		LineDrawer& p_mLineDrawer
	)
	{
		SceneQuadTreeNode* pRoot = new SceneQuadTreeNode(p_mAABB);

		std::vector<Node*>::const_iterator it = p_vNodes.begin(), end = p_vNodes.end();
		for (; it != end; it++)
		{
			// <TO DO> This is app-specific, assuming everything is a sphere. Work out a way to do this generically.
			const HeatStroke::Sphere* pSphere = dynamic_cast<const HeatStroke::Sphere*>(&(*it)->GetTotalCollidable());

			if (pSphere != nullptr)
			{
				if (HeatStroke::Collide(*pSphere, pRoot->aabb) != HeatStroke::CTR_OUTSIDE)
				{
					pRoot->nodesInRegion.push_back(*it);
				}
			}
		}

		Split(pRoot, p_mAABB, 0, p_mLineDrawer);

		return pRoot;
	}

	void Split(
		SceneQuadTreeNode* p_pSceneQuadTreeNode,
		const HeatStroke::AABB& p_mAABB,
		const unsigned int p_uiRecursionDepth,
		LineDrawer& p_mLineDrawer
	)
	{
		if (p_pSceneQuadTreeNode == nullptr)
		{
			return;
		}

		float fMinX = p_mAABB.GetMinX();
		float fMidX = p_mAABB.GetMidX();
		float fMaxX = p_mAABB.GetMaxX();

		float fMinZ = p_mAABB.GetMinZ();
		float fMidZ = p_mAABB.GetMidZ();
		float fMaxZ = p_mAABB.GetMaxZ();

		p_mLineDrawer.AddLine(glm::vec3(fMinX, 0.0f, fMinZ), glm::vec3(fMinX, 0.0f, fMaxZ));
		p_mLineDrawer.AddLine(glm::vec3(fMaxX, 0.0f, fMinZ), glm::vec3(fMaxX, 0.0f, fMaxZ));
		p_mLineDrawer.AddLine(glm::vec3(fMinX, 0.0f, fMinZ), glm::vec3(fMaxX, 0.0f, fMinZ));
		p_mLineDrawer.AddLine(glm::vec3(fMinX, 0.0f, fMaxZ), glm::vec3(fMaxX, 0.0f, fMaxZ));

		if (p_pSceneQuadTreeNode->nodesInRegion.size() < 3
			|| p_uiRecursionDepth > 6)
		{
			return;
		}

		HeatStroke::AABB mTopLeftAABB(fMinX, fMidX, -1000.0f, 1000.0f, fMidZ, fMaxZ);
		HeatStroke::AABB mTopRightAABB(fMidX, fMaxX, -1000.0f, 1000.0f, fMidZ, fMaxZ);
		HeatStroke::AABB mBottomLeftAABB(fMinX, fMidX, -1000.0f, 1000.0f, fMinZ, fMidZ);
		HeatStroke::AABB mBottomRightAABB(fMidX, fMaxX, -1000.0f, 1000.0f, fMinZ, fMidZ);

		std::vector<Node*>::iterator it = p_pSceneQuadTreeNode->nodesInRegion.begin();
		std::vector<Node*>::iterator end = p_pSceneQuadTreeNode->nodesInRegion.end();
		for (; it != end; it++)
		{
			const HeatStroke::Sphere* pSphere = dynamic_cast<const HeatStroke::Sphere*>(&(*it)->GetTotalCollidable());

			if (pSphere != nullptr)
			{
				if (HeatStroke::Collide(*pSphere, mTopLeftAABB) != HeatStroke::CTR_OUTSIDE)
				{
					p_pSceneQuadTreeNode->topLeft = AddNode(p_pSceneQuadTreeNode->topLeft, mTopLeftAABB, *it);
				}
				if (HeatStroke::Collide(*pSphere, mTopRightAABB) != HeatStroke::CTR_OUTSIDE)
				{
					p_pSceneQuadTreeNode->topRight = AddNode(p_pSceneQuadTreeNode->topRight, mTopRightAABB, *it);
				}
				if (HeatStroke::Collide(*pSphere, mBottomLeftAABB) != HeatStroke::CTR_OUTSIDE)
				{
					p_pSceneQuadTreeNode->bottomLeft = AddNode(p_pSceneQuadTreeNode->bottomLeft, mBottomLeftAABB, *it);
				}
				if (HeatStroke::Collide(*pSphere, mBottomRightAABB) != HeatStroke::CTR_OUTSIDE)
				{
					p_pSceneQuadTreeNode->bottomRight = AddNode(p_pSceneQuadTreeNode->bottomRight, mBottomRightAABB, *it);
				}
			}
		}
		
		p_pSceneQuadTreeNode->nodesInRegion.clear(); // Must have added every element to at least one child.

		const unsigned int uiNextRecursionDepth = p_uiRecursionDepth + 1;
		Split(p_pSceneQuadTreeNode->topLeft, mTopLeftAABB, uiNextRecursionDepth, p_mLineDrawer);
		Split(p_pSceneQuadTreeNode->topRight, mTopRightAABB, uiNextRecursionDepth, p_mLineDrawer);
		Split(p_pSceneQuadTreeNode->bottomLeft, mBottomLeftAABB, uiNextRecursionDepth, p_mLineDrawer);
		Split(p_pSceneQuadTreeNode->bottomRight, mBottomRightAABB, uiNextRecursionDepth, p_mLineDrawer);
	}

	SceneQuadTreeNode* AddNode(SceneQuadTreeNode* p_pSceneQuadTreeNode, const HeatStroke::AABB& p_mAABB, Node* p_pNode)
	{
		if (p_pSceneQuadTreeNode == nullptr)
		{
			p_pSceneQuadTreeNode = new SceneQuadTreeNode(p_mAABB);
		}
		p_pSceneQuadTreeNode->nodesInRegion.push_back(p_pNode);
		return p_pSceneQuadTreeNode;
	}

	void DestroyQuadTree(SceneQuadTreeNode* p_mSceneQuadTreeNode)
	{
		if (p_mSceneQuadTreeNode == nullptr)
		{
			return;
		}

		DestroyQuadTree(p_mSceneQuadTreeNode->topLeft);
		DestroyQuadTree(p_mSceneQuadTreeNode->topRight);
		DestroyQuadTree(p_mSceneQuadTreeNode->bottomLeft);
		DestroyQuadTree(p_mSceneQuadTreeNode->bottomRight);

		delete p_mSceneQuadTreeNode;
		p_mSceneQuadTreeNode = nullptr;
	}

	bool IsLeafNode(const SceneQuadTreeNode& p_mSceneQuadTreeNode)
	{
		return
			p_mSceneQuadTreeNode.topLeft == nullptr
			&& p_mSceneQuadTreeNode.topRight == nullptr
			&& p_mSceneQuadTreeNode.bottomLeft == nullptr
			&& p_mSceneQuadTreeNode.bottomRight == nullptr;
	}

	unsigned int Size(const SceneQuadTreeNode* p_pSceneQuadTreeNode)
	{
		if (p_pSceneQuadTreeNode == nullptr)
			return 0;

		unsigned int count = 1;

		count += Size(p_pSceneQuadTreeNode->topLeft);
		count += Size(p_pSceneQuadTreeNode->topRight);
		count += Size(p_pSceneQuadTreeNode->bottomLeft);
		count += Size(p_pSceneQuadTreeNode->bottomRight);

		return count;
	}
}