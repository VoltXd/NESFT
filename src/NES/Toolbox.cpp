#include "NES/Toolbox.hpp"

#include <iostream>

void testAndExitWithMessage(bool condition, const std::string &message)
{
	if (condition)
	{
		std::cout << "Error: " << message << std::endl;
		exit(EXIT_FAILURE);
	}
}
