//=================================================================================
// Event
// Author: David Hanna
//
// Encapsulates the parameter of an event.
//=================================================================================

#include "Event.h"

namespace HeatStroke
{
	// Initial value to assign to events as their ID.
	// Incremented with the creation of each new Event.
	unsigned int Event::m_uiEventIDCounter = 0;

	//------------------------------------------------------------------------------------
	// Event::Event
	// Parameters:
	//	const std::string& p_strEventType - The type of event, which determines who
	//		will want to listen to the event and respond to it.
	//
	// Creates a new Event of type p_strEventType and increments m_uiEventIDCounter.
	//------------------------------------------------------------------------------------
	Event::Event(const std::string& p_strEventType) :
		m_uiEventID(m_uiEventIDCounter),
		m_fTimeStamp(static_cast<float>(glfwGetTime())),
		m_strEventType(p_strEventType),
		m_mIntParameterMap(),
		m_mFloatParameterMap(),
		m_mStringParameterMap(),
		m_mGameObjectParameterMap()
	{
		++m_uiEventIDCounter;
	}

	//-----------------------------------------------------------------------------------------
	// Event::SetIntParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name with which to reference this parameter.
	//	const int p_iParameterValue - The value of the parameter.
	//
	// Sets a parameter of type int on this event.
	//-----------------------------------------------------------------------------------------
	void Event::SetIntParameter(const std::string& p_strParameterName, const int p_iParameterValue)
	{
		m_mIntParameterMap.insert(std::pair<std::string, const int>(p_strParameterName, p_iParameterValue));
	}

	//-----------------------------------------------------------------------------------------
	// Event::GetOptionalIntParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name of the parameter to get the value of.
	//	int* p_iParameterOut - The output variable where the result is set.
	//	const int p_iDefault - If the parameter did not exist on this event, set the output
	//		to the value of p_iDefault.
	//
	// Sets p_iParameterOut to the value of the parameter by the name of p_strParameterName
	// of this event. If p_strParameterName did not exist, p_iParameterOut is set to p_iDefault.
	//-----------------------------------------------------------------------------------------
	void Event::GetOptionalIntParameter(const std::string& p_strParameterName, int& p_iParameterOut, const int p_iDefault) const
	{
		IntParameterMap::const_iterator find = m_mIntParameterMap.find(p_strParameterName);
		if (find == m_mIntParameterMap.end())
		{
			p_iParameterOut = p_iDefault;
		}
		else
		{
			p_iParameterOut = find->second;
		}
	}

	//----------------------------------------------------------------------------------------
	// Event::GetRequiredIntParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name of the parameter to get the value of.
	//	int* p_iParameterOut - The output variable where the result is set.
	//
	// Sets p_iParameterOut to the value of the parameter by the name of p_strParameterName
	// of this event. If p_strParameterName did not exist, an assertion fails.
	//----------------------------------------------------------------------------------------
	void Event::GetRequiredIntParameter(const std::string& p_strParameterName, int& p_iParameterOut) const
	{
		IntParameterMap::const_iterator find = m_mIntParameterMap.find(p_strParameterName);
		if (find == m_mIntParameterMap.end())
		{
			assert(false && "Missing Required Int Parameter");
		}
		else
		{
			p_iParameterOut = find->second;
		}
	}

	//-----------------------------------------------------------------------------------------
	// Event::SetFloatParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name with which to reference this parameter.
	//	const float p_fParameterValue - The value of the parameter.
	//
	// Sets a parameter of type float on this event.
	//-----------------------------------------------------------------------------------------
	void Event::SetFloatParameter(const std::string& p_strParameterName, const float p_fParameterValue)
	{
		m_mFloatParameterMap.insert(std::pair<std::string, const float>(p_strParameterName, p_fParameterValue));
	}

	//-----------------------------------------------------------------------------------------
	// Event::GetOptionalFloatParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name of the parameter to get the value of.
	//	float* p_fParameterOut - The output variable where the result is set.
	//	const float p_fDefault - If the parameter did not exist on this event, set the output
	//		to the value of p_fDefault.
	//
	// Sets p_fParameterOut to the value of the parameter by the name of p_strParameterName
	// of this event. If p_strParameterName did not exist, p_fParameterOut is set to p_fDefault.
	//-----------------------------------------------------------------------------------------
	void Event::GetOptionalFloatParameter(const std::string& p_strParameterName, float& p_fParameterOut, const float p_fDefault) const
	{
		FloatParameterMap::const_iterator find = m_mFloatParameterMap.find(p_strParameterName);
		if (find == m_mFloatParameterMap.end())
		{
			p_fParameterOut = p_fDefault;
		}
		else
		{
			p_fParameterOut = find->second;
		}
	}

	//----------------------------------------------------------------------------------------
	// Event::GetRequiredFloatParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name of the parameter to get the value of.
	//	float* p_fParameterOut - The output variable where the result is set.
	//
	// Sets p_fParameterOut to the value of the parameter by the name of p_strParameterName
	// of this event. If p_strParameterName did not exist, an assertion fails.
	//----------------------------------------------------------------------------------------
	void Event::GetRequiredFloatParameter(const std::string& p_strParameterName, float& p_fParameterOut) const
	{
		FloatParameterMap::const_iterator find = m_mFloatParameterMap.find(p_strParameterName);
		if (find == m_mFloatParameterMap.end())
		{
			assert(false && "Missing Required Float Parameter");
		}
		else
		{
			p_fParameterOut = m_mFloatParameterMap.at(p_strParameterName);
		}
	}

	//-----------------------------------------------------------------------------------------
	// Event::SetStringParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name with which to reference this parameter.
	//	const std::string& p_strParameterValue - The value of the parameter.
	//
	// Sets a parameter of type string on this event.
	//-----------------------------------------------------------------------------------------
	void Event::SetStringParameter(const std::string& p_strParameterName, const std::string& p_strParameterValue)
	{
		m_mStringParameterMap.insert(std::pair<std::string, const std::string>(p_strParameterName, p_strParameterValue));
	}

	//-----------------------------------------------------------------------------------------
	// Event::GetOptionalStringParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name of the parameter to get the value of.
	//	std::string* p_strParameterOut - The output variable where the result is set.
	//	std::string* p_strDefault - If the parameter did not exist on this event, set the output
	//		to the value of p_strDefault.
	//
	// Sets p_strParameterOut to the value of the parameter by the name of p_strParameterName
	// of this event. If p_strParameterName did not exist, p_strParameterOut is set to p_strDefault.
	//-----------------------------------------------------------------------------------------
	void Event::GetOptionalStringParameter(const std::string& p_strParameterName, std::string& p_strParameterOut, std::string& p_strDefault) const
	{
		StringParameterMap::const_iterator find = m_mStringParameterMap.find(p_strParameterName);
		if (find == m_mStringParameterMap.end())
		{
			p_strParameterOut = p_strDefault;
		}
		else
		{
			p_strParameterOut = find->second;
		}
	}

	//----------------------------------------------------------------------------------------
	// Event::GetRequiredStringParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name of the parameter to get the value of.
	//	std::string* p_strParameterOut - The output variable where the result is set.
	//
	// Sets p_strParameterOut to the value of the parameter by the name of p_strParameterName
	// of this event. If p_strParameterName did not exist, an assertion fails.
	//----------------------------------------------------------------------------------------
	void Event::GetRequiredStringParameter(const std::string& p_strParameterName, std::string& p_strParameterOut) const
	{
		StringParameterMap::const_iterator find = m_mStringParameterMap.find(p_strParameterName);
		if (find == m_mStringParameterMap.end())
		{
			assert(false && "Missing Required String Parameter");
		}
		else
		{
			p_strParameterOut = find->second;
		}
	}

	//-----------------------------------------------------------------------------------------
	// Event::SetGameObjectParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name with which to reference this parameter.
	//	GameObject* p_pParameterValue - The value of the parameter.
	//
	// Sets a parameter of type Game Object on this event.
	//-----------------------------------------------------------------------------------------
	void Event::SetGameObjectParameter(const std::string& p_strParameterName, const std::string& p_pParameterValue)
	{
		m_mGameObjectParameterMap.insert(std::pair<std::string, const std::string>(p_strParameterName, p_pParameterValue));
	}

	//-----------------------------------------------------------------------------------------
	// Event::GetOptionalGameObjectParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name of the parameter to get the value of.
	//	GameObject* p_pParameterOut - The output variable where the result is set.
	//	GameObject* p_pDefault - If the parameter did not exist on this event, set the output
	//		to the value of p_pDefault.
	//
	// Sets p_pParameterOut to the value of the parameter by the name of p_strParameterName
	// of this event. If p_strParameterName did not exist, p_pParameterOut is set to p_pDefault.
	//-----------------------------------------------------------------------------------------
	void Event::GetOptionalGameObjectParameter(const std::string& p_strParameterName, std::string& p_pParameterOut, const std::string& p_pDefault) const
	{
		GameObjectParameterMap::const_iterator find = m_mGameObjectParameterMap.find(p_strParameterName);
		if (find == m_mGameObjectParameterMap.end())
		{
			p_pParameterOut = p_pDefault;
		}
		else
		{
			p_pParameterOut = find->second;
		}
	}

	//----------------------------------------------------------------------------------------
	// Event::GetRequiredGameObjectParameter
	// Parameters:
	//	const std::string& p_strParameterName - The name of the parameter to get the value of.
	//	GameObject* p_pParameterOut - The output variable where the result is set.
	//
	// Sets p_pParameterOut to the value of the parameter by the name of p_strParameterName
	// of this event. If p_strParameterName did not exist, an assertion fails.
	//----------------------------------------------------------------------------------------
	void Event::GetRequiredGameObjectParameter(const std::string& p_strParameterName, std::string& p_pParameterOut) const
	{
		GameObjectParameterMap::const_iterator find = m_mGameObjectParameterMap.find(p_strParameterName);
		if (find == m_mGameObjectParameterMap.end())
		{
			assert(false && "Missing Required Game Object Parameter");
		}
		else
		{
			p_pParameterOut = find->second;
		}
	}
}