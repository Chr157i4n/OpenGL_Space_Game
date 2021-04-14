#pragma once
#include "defines.h"

#include "Helper.h"

#include <chrono>
#include <ctime> 
#include <iostream>
#include <string>


static class Logger
{
public:

	static void info(std::string message);

	static void info2(std::string message);

	static void warn(std::string message);

	static void error(std::string message);

	static void logVector(glm::vec3 vec, std::string text="vec", int precision=2, std::string colorCode = "");

	static std::string getTime();

};

