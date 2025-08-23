#include "iostream"
#include <limits>

/*
Clears all '\n' leftovers in the cin input buffer
Not to be used after std::getline() because that already gets rid of the '\n'
*/
void cleanCin()
{
	// Cleaning all input that stayed in the buffer so it won't 
	// go to other things when doing std::cin
	std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}
