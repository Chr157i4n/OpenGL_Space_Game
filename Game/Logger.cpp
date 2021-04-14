#include "Logger.h"


void Logger::info(std::string message)
{
	std::cout << getTime() << message << std::endl;
}

void Logger::info2(std::string message)
{
	std::cout << "\033[34m" << getTime() << message << "\033[0m" << std::endl;
}

void Logger::warn(std::string message)
{
	std::cout << "\033[33m" << getTime() << message << "\033[0m" << std::endl;
}

void Logger::error(std::string message)
{
	std::cout << "\033[31m" << getTime() << message << "\033[0m" << std::endl;
}

void Logger::logVector(glm::vec3 vec, std::string text, int precision, std::string colorCode)
{
	std::string message = text+": (" + Helper::to_string_with_precision(vec.x, precision) + "|" + Helper::to_string_with_precision(vec.y, precision) + "|" + Helper::to_string_with_precision(vec.z, precision) + ")";
	std::cout << colorCode << getTime() << message << "\033[0m" << std::endl;
}

std::string Logger::getTime()
{
	time_t now = time(0);
	tm* ltm = localtime(&now);

	std::string time = "[";

	if (ltm->tm_hour < 10) time += "0";
	time += std::to_string(ltm->tm_hour) + ":";
	if (ltm->tm_min < 10) time += "0";
	time += std::to_string(ltm->tm_min) + ":";
	if (ltm->tm_sec < 10) time += "0";
	time += std::to_string(ltm->tm_sec) + "] ";

	return time;
}
