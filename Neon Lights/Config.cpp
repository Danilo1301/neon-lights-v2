#include "Config.h"

#include "Mod.h"
#include "Patterns.h"
#include "LightGroups.h"
#include "Keybinds.h"
#include "localization/Localization.h"

std::string Config::m_DataPath = "\\data\\";
std::string Config::m_VehiclesPath = "\\vehicles\\";
std::string Config::m_PatternsPath = "\\patterns\\";
std::string Config::m_LocalizationPath = "\\localization\\";

void Config::SaveJSON() {
	Log::file << "[Config] Saving config" << std::endl;

	CreatePath(m_DataPath);
	CreatePath(m_DataPath + m_PatternsPath);
	CreatePath(m_DataPath + m_VehiclesPath);
	CreatePath(m_DataPath + m_LocalizationPath);

	DeleteAllConfig();

	SaveSettings();
	
	//

	auto patterns = Patterns::GetPatterns();

	for (size_t i = 0; i < patterns.size(); i++)
	{
		auto pattern = patterns[i];

		Json::Value value = pattern->ToJSON();

		WriteToFile(m_DataPath + m_PatternsPath + std::to_string(i) + ".json", value);
	}

	//
	SaveLightGroups();

	

	Log::file << "[Config] Config saved" << std::endl;
}


void Config::LoadJSON() {
	Log::file << "[Config] Loading config" << std::endl;

	CreatePath(m_DataPath);
	CreatePath(m_DataPath + m_PatternsPath);
	CreatePath(m_DataPath + m_VehiclesPath);
	CreatePath(m_DataPath + m_LocalizationPath);

	LoadSettings();

	std::string pathPattenrs = m_DataPath + m_PatternsPath;

	for (const auto& entry : std::filesystem::directory_iterator(GetFullPath(pathPattenrs))) {
		int index = std::stoi(entry.path().filename().replace_extension());

		Log::file << "[Config] Loading Pattern " << index << "..." << std::endl;

		Json::Value value = ReadFile(pathPattenrs + std::to_string(index) + ".json");

		Pattern* pattern = Patterns::CreatePattern(value["name"].asString());
		pattern->FromJSON(value);
	}

	LoadLightGroups();

	Localization::RemoveAllLines();
	Localization::m_AvailableLanguages.clear();

	std::string pathLocalization = GetFullPath(m_DataPath + m_LocalizationPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathLocalization)) {
		std::string language = entry.path().filename().replace_extension().string();

		Log::file << "[Config] Localization " << language << std::endl;

		Localization::m_AvailableLanguages.push_back(language);

		Json::Value value = ReadFile(m_DataPath + m_LocalizationPath + language + ".json");

		for (auto member : value.getMemberNames()) {
			Localization::RegisterLine(member, language, value[member].asString());
		}
	}

	Log::file << "[Config] Config loaded" << std::endl;
}


void Config::DeleteAllConfig() {
	Log::file << "[Config] DeleteAllConfig" << std::endl;

	std::filesystem::remove_all(m_DataPath + "settings.json");

	//

	std::string pathPatterns = GetFullPath(m_DataPath + m_PatternsPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathPatterns)) {
		std::filesystem::remove_all(entry.path());
	}

	//

	std::string pathVehicles = GetFullPath(m_DataPath + m_VehiclesPath);

	for (const auto& entry : std::filesystem::directory_iterator(pathVehicles)) {
		std::filesystem::remove_all(entry.path());
	}
}

void Config::SaveSettings()
{
	auto path = m_DataPath + "settings.json";

	Json::Value value = Json::objectValue;

	value["light_id_offset"] = Mod::m_LightIdOffset;
	value["keybinds"] = Keybinds::ToJSON();
	value["language"] = Localization::m_CurrentLanguage;

	WriteToFile(path, value);
}

void Config::LoadSettings()
{
	Log::file << "[Config] Loading settings.json" << std::endl;

	auto path = m_DataPath + "settings.json";

	if (!Exists(path)) {
		Log::file << "[Config] settings.json not found";
		return;
	}

	Json::Value value = ReadFile(path);

	Mod::m_LightIdOffset = value["light_id_offset"].asInt();
	Keybinds::FromJSON(value["keybinds"]);
	if (!value["language"].isNull()) Localization::m_CurrentLanguage = value["language"].asString();

	Log::file << "[Config] lightIdOffset: " << Mod::m_LightIdOffset << std::endl;
	Log::file << "[Config] language: " << Localization::m_CurrentLanguage << std::endl;
}

void Config::SaveLightGroups()
{
	auto lightGroups = LightGroups::GetLightGroups();

	for (size_t i = 0; i < lightGroups.size(); i++)
	{
		auto lightGroup = lightGroups[i];

		CreatePath(m_DataPath);
		CreatePath(m_DataPath + m_VehiclesPath);
		CreatePath(m_DataPath + m_VehiclesPath + std::to_string(lightGroup->modelId));

		Json::Value value = lightGroup->ToJSON();

		WriteToFile(m_DataPath + m_VehiclesPath + std::to_string(lightGroup->modelId) + "\\" + std::to_string(i) + ".json", value);
	}
}

void Config::LoadLightGroups()
{
	Log::file << "[Config] LoadLightGroups" << std::endl;

	for (const auto& entry : std::filesystem::directory_iterator(GetFullPath(m_DataPath + m_VehiclesPath))) {
		int modelId = std::stoi(entry.path().filename().replace_extension());

		Log::file << "[Config] Loading vehicle " << modelId << "..." << std::endl;

		std::string pathModelId = m_DataPath + m_VehiclesPath + "\\" + std::to_string(modelId) + "\\";

		for (const auto& entry2 : std::filesystem::directory_iterator(GetFullPath(pathModelId))) {
			int index = std::stoi(entry2.path().filename().replace_extension());

			Log::file << "[Config] Loading lightGroup " << index << "..." << std::endl;

			Json::Value value = ReadFile(pathModelId + std::to_string(index) + ".json");

			LightGroup* lightGroup = LightGroups::CreateLightGroup(modelId);
			lightGroup->FromJSON(value);
		}
	}
}

std::string Config::GetFullPath(std::string path) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;
	return finalPath;
}

void Config::CreatePath(std::string path) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	if (!std::filesystem::is_directory(finalPath) || !std::filesystem::exists(finalPath)) {
		std::filesystem::create_directory(finalPath);
	}
}

bool Config::Exists(std::string path) {
	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;
	return std::filesystem::exists(finalPath);
}


void Config::WriteToFile(std::string path, Json::Value value) {
	Log::file << "[Config] WriteToFile " << path << std::endl;

	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	Json::StyledWriter writer;
	std::string strJson = writer.write(value);

	std::ofstream file(finalPath);
	file << strJson;
	file.close();
}

Json::Value Config::ReadFile(std::string path) {
	Log::file << "[Config] ReadFile " << path << std::endl;

	std::string pluginPath = PLUGIN_PATH("\\");
	std::string finalPath = pluginPath + path;

	std::ifstream file(finalPath);

	Json::Value value;
	Json::Reader reader;

	if (!reader.parse(file, value, true)) {
		MessageBox(HWND_DESKTOP, std::string("Error loading " + path).c_str(), Mod::m_Name.c_str(), MB_ICONERROR);
	}

	return value;
}
