//-----------------------------------------------------------------------------
// File:			RNGesus.h
// Original Author:	Bradley Cooper
//
// Accessors for common random utilities.
//-----------------------------------------------------------------------------
#ifndef RNGESUS_H
#define RNGESUS_H

#include <random>

namespace HeatStroke
{
	namespace Common
	{
		std::mt19937& GetRNGesus();
	}
}

#endif