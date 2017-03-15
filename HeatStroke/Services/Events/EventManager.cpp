//==================================================================================
// EventManager
// Author: David Hanna
//
// Able to hold references to (Listener) functions and call these functions with
// (Event) context data when another object requests to send an event.
// Events are separated by EventType to focus the Events only towards those
// listeners who want to handle that EventType, and those who want to handle
// different EventTypes differently.
//==================================================================================

#include "EventManager.h"

namespace HeatStroke
{
	// Static singleton instance
	EventManager* EventManager::s_pEventManagerInstance = nullptr;

	//------------------------------------------------------------------------------
	// Method:    CreateInstance
	// Returns:   void
	// 
	// Creates the singletone instance.
	//------------------------------------------------------------------------------
	void EventManager::CreateInstance()
	{
		assert(s_pEventManagerInstance == nullptr);
		s_pEventManagerInstance = new EventManager;
	}

	//------------------------------------------------------------------------------
	// Method:    DestroyInstance
	// Returns:   void
	// 
	// Destroys the singleton instance.
	//------------------------------------------------------------------------------
	void EventManager::DestroyInstance()
	{
		assert(s_pEventManagerInstance != nullptr);
		delete s_pEventManagerInstance;
		s_pEventManagerInstance = nullptr;
	}

	//------------------------------------------------------------------------------
	// Method:    Instance
	// Returns:   SceneManager::SceneManager*
	// 
	// Access to singleton instance.
	//------------------------------------------------------------------------------
	EventManager* EventManager::Instance()
	{
		assert(s_pEventManagerInstance != nullptr);
		return s_pEventManagerInstance;
	}

	//----------------------------------------------------------------------------
	// EventManager::Update
	// Parameters:
	//	pDelta - the delta time since the last Update call.
	//
	// Empties the event queue by distributing all queued events to all listeners
	// of the type of each event.
	//----------------------------------------------------------------------------
	void EventManager::Update(const float pDelta)
	{
		// Iterate over the event queue in the order the events were received.
		while (!m_mEventQueue.empty())
		{
			// Grab the event we'll be sending out.
			const Event* pEvent = m_mEventQueue.front();
			m_mEventQueue.pop();

			// Dispatch the event to all listeners of the event's type.
			TriggerEvent(pEvent);
		}
	}

	//-----------------------------------------------------------------------------------------
	// EventManager::AddListener
	// Parameters:
	//	const std::string& p_strEventType - The type of event to listen to.
	//	std::function<void(Event*)>* - The listener method to call when an event of type
	//		type p_strEventType is sent. Return type must be void, parameters must be Event*.
	//
	// Adds a listener method to call when an event of type p_strEventType is sent.
	//-----------------------------------------------------------------------------------------
	void EventManager::AddListener(const std::string& p_strEventType, std::function<void(const Event*)>* p_pListener)
	{
		// If we didn't already have an item in the map matching p_strEventType,
		// operator[] adds a vector there using the default constructor.
		m_mListeners[p_strEventType].push_back(p_pListener);
	}

	//-----------------------------------------------------------------------------------------
	// EventManager::RemoveListener
	// Parameters:
	//	const std::string& p_strEventType - The type of event to remove the listener from.
	//	std::function<void(Event*)>* - The listener method to remove from listening to events
	//		of type p_strEventType.
	//
	// Removes a listener method from listening to events of type p_strEventType.
	//-----------------------------------------------------------------------------------------
	void EventManager::RemoveListener(const std::string& p_strEventType, std::function<void(const Event*)>* p_pListener)
	{
		m_mListeners[p_strEventType].erase(
			std::remove(m_mListeners[p_strEventType].begin(), m_mListeners[p_strEventType].end(), p_pListener),
			m_mListeners[p_strEventType].end());
	}

	//-------------------------------------------------------------------------------------------
	// EventManager::QueueEvent
	// Parameters:
	//	Event* p_pEvent - The event object to be sent to all 
	//		listeners of type p_pEvent->GetEventType().
	//
	// Adds an event to the queue which will be fired to all listeners on the next call
	// to the Update() method.
	//-------------------------------------------------------------------------------------------
	void EventManager::QueueEvent(const Event* p_pEvent)
	{
		m_mEventQueue.push(p_pEvent);
	}

	//-------------------------------------------------------------------------------------------
	// EventManager::TriggerEvent
	// Parameters:
	//	Event* p_pEvent - The event object to be sent to all
	//		listeners of type p_pEvent->GetEventType().
	//
	// Immediately dispatches p_pEvent to all listeners.
	//-------------------------------------------------------------------------------------------
	void EventManager::TriggerEvent(const Event* p_pEvent)
	{
		// Confirm the event type so we can send only to those listening.
		ListenerMap::iterator findListenerList = m_mListeners.find(p_pEvent->GetEventType());
		if (findListenerList != m_mListeners.end())
		{
			// Iterate over the listeners and call their handler method with
			// the Event parameter.
			// TO DO, currently uses local copy to avoid iterator problems if
			// AddListener() or RemoveListener() are called during event dispatch
			ListenerList mLocalCopy(findListenerList->second);
			ListenerList::iterator it = mLocalCopy.begin();
			ListenerList::iterator end = mLocalCopy.end();

			for (; it != end; ++it)
			{
				std::function<void(const Event*)>* pListener = (*it);
				(*pListener)(p_pEvent);
			}
		}

		// Everyone has seen the event at this point, so we can clean it up.
		delete p_pEvent;
		p_pEvent = nullptr;
	}
}