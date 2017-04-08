//-----------------------------------------------------------------------------
// File:			TimeFormat.cpp
// Original Author:	Bradley Cooper
//
// Methods for converting between float time and string time for races.
//-----------------------------------------------------------------------------

#include "TimeFormat.h"

std::string Kartaclysm::Common::TimeStringFromFloat(float p_fTimeInSeconds)
{
	// Warning: If changing default value, ensure that the new default value conforms
	// with string comparison (i.e. [default value <= "00:00.00"] must remain true)
	if (p_fTimeInSeconds == 0.0f) return "--:--.--";

	if (p_fTimeInSeconds > 3599.99f)
	{
		printf("TimeFormat.cpp - Formatting time greater than 1 hour: %.2f\n", p_fTimeInSeconds);
		return "59:59.99";
	}

	if (p_fTimeInSeconds < 0.0f)
	{
		printf("TimeFormat.cpp - Formatting negative time: %.2f\n", p_fTimeInSeconds);
		return "--:--.--";
	}

	int iMinutes = static_cast<int>(p_fTimeInSeconds) / 60;
	float fSeconds = fmod(p_fTimeInSeconds, 60.0f);

	std::string strMinutes = std::to_string(iMinutes);
	std::string strSeconds = std::to_string(fSeconds);
	if (iMinutes < 10)
	{
		strMinutes = "0" + strMinutes;
	}
	if (fSeconds < 10.0f)
	{
		strSeconds = "0" + strSeconds;
	}
	strSeconds = strSeconds.substr(0, 5); // ##.##

	return strMinutes + ":" + strSeconds;
}

float Kartaclysm::Common::FloatFromTimeString(const std::string& p_strFormattedTime)
{
	if (p_strFormattedTime == "--:--.--") return 0.0f;

	if (p_strFormattedTime == "")
	{
		printf("TimeFormat.cpp - Converting empty string\n");
		return 0.0f;
	}

	if (!std::regex_match(p_strFormattedTime, std::regex("^\\d\\d:\\d\\d\\.\\d\\d$"))) // ##:##.##
	{
		printf("TimeFormat.cpp - Converting unformatted string: %s\n", p_strFormattedTime.c_str());
		return 0.0f;
	}

	int iMinutes = std::stoi(p_strFormattedTime.substr(0, 2));
	float fSeconds = std::stof(p_strFormattedTime.substr(3));

#ifdef _DEBUG
	assert(iMinutes < 60);
	assert(fSeconds < 60.0f);
#endif

	return (iMinutes * 60.0f) + fSeconds;
}

std::string Kartaclysm::Common::FormatHudTime(float p_fTimeInSeconds)
{
	std::string strTime = TimeStringFromFloat(p_fTimeInSeconds);
	return (strTime == "--:--.--" ? "00:00" : strTime.substr(0, 5));
}