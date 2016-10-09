//------------------------------------------------------------------------
// GameObject
//
// Created:	2012/12/14
// Author:	Carel Boers
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
	m_mComponentMap()
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
	this->DeleteAllComponents();
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