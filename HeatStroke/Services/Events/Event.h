//=================================================================================
// Event
// Author: David Hanna
//
// Encapsulates the parameter of an event.
//=================================================================================

#ifndef EVENT_H
#define EVENT_H

#include <GLFW/glfw3.h>			// Timestamp.
#include <map>					// Parameters.

#include "GameObjectManager.h"	// Ability to look up a GameObject by GUID.

namespace HeatStroke
{
	class Event
	{
	public:
		// Creates a new Event of type p_strEventType and increments m_uiEventIDCounter.
		Event(const std::string& p_strEventType);
		virtual ~Event() {}

		// Accessors for required event data.
		unsigned int GetEventID() const			{ return m_uiEventID; }
		float GetTimeStamp() const				{ return m_fTimeStamp; }
		const std::string& GetEventType() const { return m_strEventType; }

		// Set and access int-type parameters of this event, created and referenced by look-up string.
		void SetIntParameter(const std::string& p_strParameterName, const int p_iParameterValue);
		void GetOptionalIntParameter(const std::string& p_strParameterName, int& p_iParameterOut, const int p_iDefault) const;
		void GetRequiredIntParameter(const std::string& p_strParameterName, int& p_iParameterOut) const;

		// Set and access float-type parameters of this event, created and referenced by look-up string.
		void SetFloatParameter(const std::string& p_strParameterName, const float p_fParameterValue);
		void GetOptionalFloatParameter(const std::string& p_strParameterName, float& p_fParameterOut, const float p_fDefault) const;
		void GetRequiredFloatParameter(const std::string& p_strParameterName, float& p_fParameterOut) const;

		// Set and access string-type parameters of this event, created and referenced by look-up string.
		void SetStringParameter(const std::string& p_strParameterName, const std::string& p_strParameterValue);
		void GetOptionalStringParameter(const std::string& p_strParameterName, std::string& p_strParameterOut, std::string& p_strDefault) const;
		void GetRequiredStringParameter(const std::string& p_strParameterName, std::string& p_strParameterOut) const;

		// Set and access GameObject-type parameters of this event, created and referenced by look-up string.
		void SetGameObjectParameter(const std::string& p_strParameterName, const std::string& p_strParameterValue);
		void GetOptionalGameObjectParameter(const std::string& p_strParameterName, std::string& p_strParameterOut, const std::string& p_strDefault) const;
		void GetRequiredGameObjectParameter(const std::string& p_strParameterName, std::string& p_pParameterOut) const;

	private:
		//------------------------------------------------------------------------------
		// Private map typedefs
		//------------------------------------------------------------------------------
		typedef std::map<std::string, const int>			IntParameterMap;
		typedef	std::map<std::string, const float>			FloatParameterMap;
		typedef	std::map<std::string, const std::string>	StringParameterMap;
		typedef std::map<std::string, const std::string>	GameObjectParameterMap;

		//------------------------------------------------------------------------------
		// Private members.
		//------------------------------------------------------------------------------
		// This starts at zero and is incremented for each event created so they are all unique.
		static unsigned int m_uiEventIDCounter;

		const unsigned int m_uiEventID;

		// Initialized to the value given by glfwGetTime() at the time this event is created.
		const float m_fTimeStamp;

		// The event type, which narrows down the listeners to only those who care.
		const std::string m_strEventType;

		// Various types of maps corresponding to the allowed parameter types for events.
		IntParameterMap m_mIntParameterMap;
		FloatParameterMap m_mFloatParameterMap;
		StringParameterMap m_mStringParameterMap;
		GameObjectParameterMap m_mGameObjectParameterMap;
	};
}

#endif // EVENT_H