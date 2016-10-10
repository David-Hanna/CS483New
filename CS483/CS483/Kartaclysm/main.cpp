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
	KartGame *game = new KartGame();
	return game->Run();
}