//==================================================================================
// Scene
// Author: David Hanna
//
// Provides an abstraction for a collection of related renderables.
//==================================================================================

#include "Scene.h"

namespace HeatStroke
{
	// Static singleton instance
	Scene* Scene::s_pSceneInstance = nullptr;

	void Scene::CreateInstance(GLFWwindow* p_pWindow)
	{
		assert(s_pSceneInstance == nullptr);
		s_pSceneInstance = new Scene(p_pWindow);
	}

	void Scene::DestroyInstance()
	{
		assert(s_pSceneInstance != nullptr);
		delete s_pSceneInstance;
		s_pSceneInstance = nullptr;
	}

	Scene* Scene::Instance()
	{
		assert(s_pSceneInstance != nullptr);
		return s_pSceneInstance;
	}

	void Scene::BuildQuadTree(const float p_fMinX, const float p_fMaxX, const float p_fMinZ, const float p_fMaxZ)
	{
		HeatStroke::AABB mAABB(p_fMinX, p_fMaxX, -1000.0f, 1000.0f, p_fMinZ, p_fMaxZ);
		m_pRoot = MakeQuadTree(m_vTopLevelNodes, mAABB, m_mLineDrawer);
		//m_mLineDrawer.Init(ShaderNames::VS_POSITION_3_COLOR_4, ShaderNames::FS_COLOR_4);
	}

	void Scene::Update(const float p_fDelta)
	{
		std::vector<Node*>::iterator update_it = m_vTopLevelNodes.begin(), update_end = m_vTopLevelNodes.end();
		for (; update_it != update_end; update_it++)
		{
			(*update_it)->Update(p_fDelta);
		}

		//m_pActiveCamera->Update(p_fDelta);

		// Do culling.
		m_vNodesToRender.clear();
		DoCulling(m_pActiveCamera->GetViewFrustum(), m_pRoot);
	}

	void Scene::DoCulling(const HeatStroke::Frustum& p_mFrustum, SceneQuadTreeNode* p_pSceneQuadTreeNode)
	{
		if (p_pSceneQuadTreeNode == nullptr)
			return;

		if (HeatStroke::Collide(p_pSceneQuadTreeNode->aabb, p_mFrustum) != HeatStroke::CTR_OUTSIDE)
		{
			std::vector<Node*>::const_iterator it = p_pSceneQuadTreeNode->nodesInRegion.begin();
			std::vector<Node*>::const_iterator end = p_pSceneQuadTreeNode->nodesInRegion.end();

			// This will be empty unless it is a leaf node.
			for (; it != end; it++)
			{
				if ((*it)->ShouldBeRendered(p_mFrustum))
				{
					m_vNodesToRender.insert(*it);
				}
			}

			DoCulling(p_mFrustum, p_pSceneQuadTreeNode->topLeft);
			DoCulling(p_mFrustum, p_pSceneQuadTreeNode->topRight);
			DoCulling(p_mFrustum, p_pSceneQuadTreeNode->bottomLeft);
			DoCulling(p_mFrustum, p_pSceneQuadTreeNode->bottomRight);
		}
	}

	void Scene::Render(const Camera* p_pCamera)
	{
		if (p_pCamera != nullptr)
		{
			std::set<Node*>::iterator it = m_vNodesToRender.begin(), end = m_vNodesToRender.end();
			//std::vector<Node*>::iterator it = m_vTopLevelNodes.begin(), end = m_vTopLevelNodes.end();
			for (; it != end; it++)
			{
				(*it)->Render(p_pCamera);
			}
			if (m_bUseLineDrawer)
			{
				m_mLineDrawer.Render(p_pCamera->GetProjectionMatrix(), p_pCamera->GetViewMatrix());
			}
		}
		else if (m_pActiveCamera != nullptr)
		{
			std::set<Node*>::iterator it = m_vNodesToRender.begin(), end = m_vNodesToRender.end();
			//std::vector<Node*>::iterator it = m_vTopLevelNodes.begin(), end = m_vTopLevelNodes.end();
			for (; it != end; it++)
			{
				(*it)->Render(m_pActiveCamera);
			}
			if (m_bUseLineDrawer)
			{
				m_mLineDrawer.Render(m_pActiveCamera->GetProjectionMatrix(), m_pActiveCamera->GetViewMatrix());
			}
		}
	}

	Node* Scene::GetNode(const std::string& p_strName)
	{
		std::vector<Node*>::iterator it = m_vTopLevelNodes.begin(), end = m_vTopLevelNodes.end();
		for (; it != end; it++)
		{
			if ((*it)->GetName() == p_strName)
			{
				return *it;
			}
		}
		return nullptr;
	}

	Node* Scene::RemoveNode(const std::string& p_strName)
	{
		std::vector<Node*>::iterator it = m_vTopLevelNodes.begin(), end = m_vTopLevelNodes.end();
		for (; it != end; it++)
		{
			if ((*it)->GetName() == p_strName)
			{
				Node* pNode = *it;
				m_vTopLevelNodes.erase(it);
				return pNode;
			}
		}
		return nullptr;
	}
}