//------------------------------------------------------------------------
// main.cpp
// Author:	David Hanna
//	
// Program entry point.
//------------------------------------------------------------------------

//#include "ExampleGame.h"
#include <cstdlib>

#include "OBJFile.h"

int main(int argc, char* argv[])
{
	//assignment1::ExampleGame exampleGame;
	//return exampleGame.Run();

	HeatStroke::OBJFile file(std::string("HeatStroke/Services/OBJ/test.obj"));
	file.ParseFile();

#ifdef _DEBUG
	std::cout << file.ToString();
#endif

	system("pause");
	return 0;
}