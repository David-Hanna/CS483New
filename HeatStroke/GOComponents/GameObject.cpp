//------------------------------------------------------------------------
// GameObject
//
// Created:	2012/12/14
// Author:	Carel Boers
// Editor:	David Hanna
// Editor:	Matthew White
//	
// This class defines a "GameObject". An GameObject for our purpose is 
// essentially just an empty container to define any object in our game 
// world. The definition of the object is through the aggregation of various
// components.
//------------------------------------------------------------------------

#include "GameObject.h"
#include <sstream>

using namespace HeatStroke;

//------------------------------------------------------------------------------
// Method:    GameObject
// Parameter: GameObjectManager * p_pGameObjectManager
// Returns:   
// 
// Constructor.
//------------------------------------------------------------------------------
GameObject::GameObject(GameObjectManager* p_pGameObjectManager, const std::string& p_strGUID)
	:
	m_pGameObjectManager(p_pGameObjectManager),
	m_strGUID(p_strGUID),
	m_Transform(p_strGUID),
	m_pParent(nullptr),
	m_mComponentMap(),
	m_mChildMap(),
	m_mTagList()
{
}

//------------------------------------------------------------------------------
// Method:    ~GameObject
// Returns:   
// 
// Destructor.
//------------------------------------------------------------------------------
GameObject::~GameObject()
{
	// TODO - I don't know what's happening, but it's started throwing errors again when trying to remove itself as a child from its parent.
	//			I'm removing this for now so it doesn't throw errors, but I'll need to figure this out soon.
	DeleteAllComponents();
	DeleteAllChildren();
}

//------------------------------------------------------------------------------
// Method:    AddComponent
// Parameter: BaseComponent * p_pComponent
// Returns:   void
// 
// Add a component to the GameObject.
//------------------------------------------------------------------------------
void GameObject::AddComponent(Component* p_pComponent)
{
	if (p_pComponent != nullptr)
	{
		m_mComponentMap.insert(std::pair<std::string, Component*>(p_pComponent->FamilyID(), p_pComponent));
	}
}

//------------------------------------------------------------------------------
// Method:    GetComponent
// Parameter: const std::string & p_strFamilyId
// Returns:   BaseComponent*
// 
// Get a component with the given family Id. Returns NULL if it isn't found.
//------------------------------------------------------------------------------
Component* GameObject::GetComponent(const std::string& p_strFamilyId) const
{
	ComponentMap::const_iterator it = m_mComponentMap.find(p_strFamilyId);
	if (it != m_mComponentMap.end())
	{
		return it->second;
	}

	// Don't have it.
	return nullptr;
}

//------------------------------------------------------------------------------
// Method:    RemoveComponent
// Parameter: const std::string & p_strFamilyId
// Returns:   BaseComponent*
// 
// Remove a component with the given family Id and returns it. It is not 
// deleted to allow moving this component to another GameObject if desired. 
//------------------------------------------------------------------------------
Component* GameObject::RemoveComponent(const std::string& p_strFamilyId)
{
	Component* pComponent = nullptr;

	ComponentMap::iterator it = m_mComponentMap.find(p_strFamilyId);
	if (it != m_mComponentMap.end())
	{
		pComponent = it->second;
		m_mComponentMap.erase(it);
	}

	return pComponent;
}

//------------------------------------------------------------------------------
// Method:    DeleteAllComponents
// Returns:   void
// 
// Remove all components from the GameObject and deletes them.
//------------------------------------------------------------------------------
void GameObject::DeleteAllComponents()
{
	for (ComponentMap::iterator it = m_mComponentMap.begin(); it != m_mComponentMap.end(); ++it)
	{
		delete it->second;
	}
	m_mComponentMap.clear();
}


void GameObject::AddChild(GameObject* p_pChild)
{
	assert(p_pChild != nullptr);
	p_pChild->SetParent(this);
}

void GameObject::SetParent(GameObject* p_pParent)
{
	if (m_pParent == p_pParent)
	{
		return;
	}

	if (m_pParent != nullptr)
	{
		m_pParent->RemoveChild(m_strGUID);
	}

	m_pParent = p_pParent;
	if (m_pParent != nullptr)
	{
		m_pParent->m_mChildMap.insert(std::pair<std::string, GameObject*>(m_strGUID, this));
		m_Transform.SetParent(&(m_pParent->GetTransform()));
	}
}

GameObject* GameObject::RemoveChild(const std::string& p_strChildGuid)
{
	GameObject* pChildGameObject = nullptr;

	ChildMap::iterator it = m_mChildMap.find(p_strChildGuid);
	if (it != m_mChildMap.end())
	{
		pChildGameObject = it->second;
		pChildGameObject->m_pParent = nullptr;
		pChildGameObject->m_Transform.SetParent(nullptr);
		m_mChildMap.erase(it);
	}

	return pChildGameObject;
}

GameObject* GameObject::GetChild(const std::string& p_strChildGuid)
{
	GameObject* pChildGameObject = nullptr;

	ChildMap::iterator it = m_mChildMap.find(p_strChildGuid);
	if (it != m_mChildMap.end())
	{
		pChildGameObject = it->second;
	}

	return pChildGameObject;
}

void GameObject::DeleteAllChildren()
{
	for (ChildMap::iterator it = m_mChildMap.begin(); it != m_mChildMap.end(); ++it)
	{
		m_pGameObjectManager->DestroyGameObject(it->second);
	}
	m_mChildMap.clear();
}

//------------------------------------------------------------------------------
// Method:    Init
// Returns:   void
// 
// Calls the post-construction init function on all components.
//------------------------------------------------------------------------------
void GameObject::Init()
{
	for (ComponentMap::iterator it = m_mComponentMap.begin(); it != m_mComponentMap.end(); ++it)
	{
		it->second->Init();
	}
}

//------------------------------------------------------------------------------
// Method:    Update
// Parameter: float p_fDelta
// Returns:   void
// 
// Updates the internal components of the GameObject.
//------------------------------------------------------------------------------
void GameObject::Update(float p_fDelta)
{
	for (ComponentMap::iterator it = m_mComponentMap.begin(); it != m_mComponentMap.end(); ++it)
	{
		it->second->Update(p_fDelta);
	}
}

//TODO: delete this (once all bugs are worked out)
//Matt: Just using this to check that children are being added properly
void GameObject::Print()
{
	printf("guid: %s\n", m_strGUID.c_str());
	m_Transform.Print();
	printf("parent: %s\n", m_pParent == nullptr ? "none" : m_pParent->GetGUID().c_str());
	printf("children:\n");
	if (m_mChildMap.size() == 0)
	{
		printf("none\n");
	}
	else
	{
		for (ChildMap::iterator it = m_mChildMap.begin(); it != m_mChildMap.end(); ++it)
		{
			printf("%s\n", it->second->GetGUID().c_str());
		}
	}
	printf("\n");
}