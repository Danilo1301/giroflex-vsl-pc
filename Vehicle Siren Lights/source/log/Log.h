#pragma once

#include "plugin.h"
#include <filesystem>

class Log {
public:
	static std::fstream file;

	static void Open(std::string fileName);
};

