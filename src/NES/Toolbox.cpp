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
bool gIsTraceLogMMC3IrqEnabled = false;
void traceLog(const std::string &log)
{
	// No log if log mode is disabled
	bool isTraceLogEnabled = gIsTraceLogCpuEnabled || 
	                         gIsTraceLogPpuEnabled ||
							 gIsTraceLogMMC3IrqEnabled;
	if (!isTraceLogEnabled)
		return;

	// Open file
	static std::ofstream gLogFile("log.txt");
	gLogFile << log;
}

float limitToInterval(float value, float min, float max)
{
	// Clamp value to [min, max]
	if (value < min)
		return min;
	else if (value > max)
		return max;

    return value;
}
