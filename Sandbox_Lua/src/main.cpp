#include <filesystem>
#include <TestGameObjects.h>


namespace fs = std::filesystem;

TestGameObjects testGameObjects;

int main() 
{	
	testGameObjects.Run();	
	return 0;
}