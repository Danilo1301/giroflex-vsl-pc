#pragma once

#include "plugin.h"
#include <filesystem>

class Log {
public:
	static std::fstream file;

	static void Open() {
		file.open(PLUGIN_PATH("\\Vehicle Siren Lights.log"), std::fstream::out | std::fstream::trunc);
	}
};

