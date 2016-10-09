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

#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <map>
#include <vector>
#include <functional>
#include <queue>
#include <assert.h>

#include "Event.h"

namespace HeatStroke
{
	class EventManager
	{
	public:
		//-----------------------------------------------------------------------------
		// Public Interface
		//-----------------------------------------------------------------------------
		// Singleton methods
		static void CreateInstance();
		static void DestroyInstance();
		static EventManager* Instance();
		
		// Empties the event queue by distributing all queued events to all listeners
		// of the type of each event.
		void Update(const float pDelta);

		// Adds a listener method to call when an event of type p_strEventType is sent.
		void AddListener(const std::string& p_strEventType, std::function<void(const Event*)>* p_pListener);

		// Removes a listener method from listening to events of type p_strEventType.
		void RemoveListener(const std::string& p_strEventType, std::function<void(const Event*)>* p_pListener);

		// Adds an event to the queue which will be fired to all listeners on the next call
		// to the Update() method.
		void QueueEvent(const Event* p_pEvent);

		// Immediately dispatches p_pEvent to all listeners.
		void TriggerEvent(const Event* p_pEvent);

	private:
		//---------------------------------------------------------------------------
		// Private typedefs for data structures.
		//---------------------------------------------------------------------------
		typedef std::function<void(const Event*)>* Listener;
		typedef std::vector<Listener> ListenerList;
		typedef std::map<const std::string, ListenerList> ListenerMap;
		typedef std::queue<const Event*> EventQueue;

		//---------------------------------------------------------------------------
		// Private members
		//---------------------------------------------------------------------------
		// The map of listeners organized by event type.
		ListenerMap m_mListeners;

		// The queue of events which will be dispatched and emptied every Update() call.
		EventQueue m_mEventQueue;

		// Static singleton instance
		static EventManager* s_pEventManagerInstance;

		//---------------------------------------------------------------------------
		// Private methods
		//---------------------------------------------------------------------------
		// Hide constructor and destructor for singleton.
		EventManager() : m_mListeners(), m_mEventQueue() {}
		virtual ~EventManager() {}

		// EventManagers shouldn't be copied.
		EventManager(const EventManager&) = delete;
		EventManager& operator=(const EventManager&) = delete;
	};
}

#endif // EVENT_MANAGER_H