//------------------------------------------------------------------------
// main.cpp
// Author:	David Hanna
//	
// Program entry point.
//------------------------------------------------------------------------

#include <cstdlib>
#include "KartGame.h"

int main(int argc, char* argv[])
{
	Kartaclysm::KartGame *game = new Kartaclysm::KartGame();
	return game->Run("Kartaclysm", glm::vec4(0.43f, 0.48f, 0.88f, 0.0f));
}