//-----------------------------------------------------------------------------
// File:			RNGesus.cpp
// Original Author:	Bradley Cooper
//
// Accessors for common random utilities.
//-----------------------------------------------------------------------------

#include "RNGesus.h"

std::mt19937& HeatStroke::Common::GetRNGesus()
{
	static std::random_device s_Rand;
	static std::mt19937 s_RNGesus = std::mt19937(s_Rand());
	return s_RNGesus;
}