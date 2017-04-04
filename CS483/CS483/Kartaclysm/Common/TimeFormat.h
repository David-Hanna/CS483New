//-----------------------------------------------------------------------------
// File:			TimeFormat.h
// Original Author:	Bradley Cooper
//
// Methods for converting between float time and string time for races.
//-----------------------------------------------------------------------------
#ifndef TIME_FORMAT_H
#define TIME_FORMAT_H

#include <string>
#include <regex>
#include <assert.h>

namespace Kartaclysm
{
	namespace Common
	{
		std::string TimeStringFromFloat(float p_fTimeInSeconds);
		float FloatFromTimeString(const std::string& p_strFormattedTime);
	}
}

#endif