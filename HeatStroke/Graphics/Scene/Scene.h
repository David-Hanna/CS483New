//==================================================================================
// Scene
// Author: David Hanna
//
// Provides an abstraction for a collection of related renderables.
//==================================================================================

#ifndef SCENE_H
#define SCENE_H

#ifndef __APPLE__
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <vector>
#include <set>

#include "Node.h"
#include "Renderable.h"
#include "SceneQuadTreeNode.h"
#include "LineDrawer.h"

namespace HeatStroke
{
	class Scene : public Renderable 
	{
	public:
		static void CreateInstance(GLFWwindow* p_pWindow);
		static void DestroyInstance();
		static Scene* Instance();

		void BuildQuadTree(const float p_fMinX, const float p_fMaxX, const float p_fMinZ, const float p_fMaxZ);

		virtual void Update(const float p_fDelta);
		virtual void Render(const Camera* p_pActiveCameraOverride = nullptr) override;

		void		SetWindow(GLFWwindow* p_pWindow)			{ m_pWindow = p_pWindow; }
		GLFWwindow*	GetWindow()									{ return m_pWindow; }
		GLFWwindow*	GetWindow() const							{ return m_pWindow; }

		void		SetActiveCamera(Camera* p_pCamera)			{ m_pActiveCamera = p_pCamera; }
		Camera*		GetActiveCamera()							{ return m_pActiveCamera; }
		Camera*		GetActiveCamera() const						{ return m_pActiveCamera; }

		void		AddNode(Node* p_pNode)						{ m_vTopLevelNodes.push_back(p_pNode); }
		Node*		GetNode(const std::string& p_strName);
		Node*		RemoveNode(const std::string& p_strName);

		void		SetUseLineDrawer(const bool p_bUseLineDrawer) { m_bUseLineDrawer = p_bUseLineDrawer; }
		bool		IsUsingLineDrawer() const					{ return m_bUseLineDrawer; }

	private:
		static Scene* s_pSceneInstance;

		GLFWwindow* m_pWindow;
		Camera* m_pActiveCamera;
		std::vector<Node*> m_vTopLevelNodes;
		std::set<Node*> m_vNodesToRender;	// set avoids duplicates
		SceneQuadTreeNode* m_pRoot;
		LineDrawer m_mLineDrawer;
		bool m_bUseLineDrawer;

		Scene(GLFWwindow* p_pWindow)
			:
			m_pWindow(p_pWindow),
			m_pActiveCamera(nullptr),
			m_vTopLevelNodes(),
			m_vNodesToRender(),
			m_pRoot(nullptr),
			m_mLineDrawer(),
			m_bUseLineDrawer(false)
		{
		}

		virtual ~Scene() {}

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;

		virtual void DoCulling(const HeatStroke::Frustum& p_mFrustum, SceneQuadTreeNode* p_pSceneQuadTreeNode);
	};
}

#endif