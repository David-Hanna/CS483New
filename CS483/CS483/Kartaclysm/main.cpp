//------------------------------------------------------------------------
// main.cpp
// Author:	David Hanna
//	
// Program entry point.
//------------------------------------------------------------------------

//#include "ExampleGame.h"
#include <cstdlib>

#include "KartGame.h"

int main(int argc, char* argv[])
{
	Kartaclysm::KartGame *game = new Kartaclysm::KartGame();
	return game->Run();
}