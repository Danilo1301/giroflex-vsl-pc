#include "Log.h"

std::fstream Log::file;

void Log::Open(std::string fileName) {
	char path[256];
	sprintf(path, "\\%s", fileName.c_str());
	file.open(PLUGIN_PATH(path), std::fstream::out | std::fstream::trunc);
}