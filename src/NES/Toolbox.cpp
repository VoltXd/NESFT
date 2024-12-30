#include "NES/Toolbox.hpp"

#include <iostream>
#include <fstream>

void testAndExitWithMessage(bool condition, const std::string &message)
{
	if (condition)
	{
		std::cout << "Error: " << message << std::endl;
		exit(EXIT_FAILURE);
	}
}

bool gIsTraceLogCpuEnabled = false;
bool gIsTraceLogPpuEnabled = false;
std::ofstream gLogFile("log.txt");
void traceLog(const std::string &log)
{
	// No log if log mode is disabled
	bool isTraceLogEnabled = gIsTraceLogCpuEnabled || gIsTraceLogPpuEnabled;
	if (!isTraceLogEnabled)
		return;

	// Open file
	gLogFile << log;
}
