#include "Config.h"
#include <filesystem>

#include "Vehicle.h"
#include "Keybinds.h"

std::string Config::GetFullPath(std::string path) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;
	return finalPath;
}

void Config::CreatePath(std::string path)
{
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	if (!std::filesystem::is_directory(finalPath) || !std::filesystem::exists(finalPath)) {
		std::filesystem::create_directory(finalPath);
	}
}

bool Config::Exists(std::string path)
{
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;
	return std::filesystem::exists(finalPath);
}

void Config::WriteToFile(std::string path, Json::Value value)
{
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	//Log::file << "[Config] WriteToFile " << finalPath << std::endl;

	Json::StyledWriter writer;
	std::string strJson = writer.write(value);

	std::ofstream file(finalPath);
	file << strJson;
	file.close();
}

Json::Value Config::ReadFile(std::string path)
{
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	//Log::file << "[Config] ReadFile " << finalPath << std::endl;

	std::ifstream file(finalPath);

	Json::Value value;
	Json::Reader reader;

	if (!reader.parse(file, value, true)) {
		MessageBox(HWND_DESKTOP, std::string("Error loading " + path).c_str(), "Vehicle Siren Lights", MB_ICONERROR);
	}

	return value;
}

void Config::DeleteAllConfig()
{
	Log::file << "[Config] DeleteAllConfig" << std::endl;
}


void Config::Save()
{
	Log::file << "[Config] Save" << std::endl;
}

void Config::Load()
{
	Log::file << "[Config] Load" << std::endl;
}