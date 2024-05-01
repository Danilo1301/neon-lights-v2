#include "Mod.h"

#include "Vehicles.h"
#include "LightGroups.h"
#include "Patterns.h"
#include "Config.h"
#include "Keybinds.h"
#include "localization/Localization.h"

#include "windows/WindowMain.h"

bool Mod::m_EnableDebug = false;

std::string Mod::m_Version = "2.1.0";
std::string Mod::m_Name = "Neon Lights (by Danilo1301)";
bool Mod::m_IsSamp = false;
int Mod::m_LightIdOffset = 2000;

//

bool firstLoad = true;

Mod::Mod()
{
	Log::Open("NeonLights.log");
	Log::file << "Initialized v" << m_Version << std::endl;

	if (GetModuleHandle("SAMP.dll"))
	{
		m_IsSamp = true;
		//Menu::m_DefaultPosition.y += 140.0f;
	}

	Config::LoadJSON();

	if (Patterns::GetPatterns().size() == 0)
	{
		auto pattern1 = Patterns::CreatePattern("pattern1");
		pattern1->AddStep({ 255, 0, 0 }, 500);
		pattern1->AddStep({ 0, 255, 0 }, 500);
		pattern1->AddStep({ 0, 0, 255 }, 500);

		auto pattern2 = Patterns::CreatePattern("pattern2");
		pattern2->AddStep({ 255, 255, 0 }, 500);
		pattern2->AddStep({ 0, 255, 255 }, 500);
		pattern2->AddStep({ 255, 0, 255 }, 500);

		auto lightGroup1 = LightGroups::CreateLightGroup(481);
		lightGroup1->AddPoint(CVector(-1, 0, 0));
		lightGroup1->AddPoint(CVector(0, 1, 0));
		lightGroup1->AddPoint(CVector(1, 0, 0));
		lightGroup1->pattern = Patterns::GetPatterns()[0];

		auto lightGroup2 = LightGroups::CreateLightGroup(481);
		lightGroup2->AddPoint(CVector(-1, 0, 3));
		lightGroup2->AddPoint(CVector(0, 1, 3));
	}

	Config::SaveJSON();


	Events::vehicleDtorEvent.after += [](CVehicle* vehicle)
	{
		if (Vehicles::HasVehicle(vehicle)) Vehicles::RemoveVehicle(vehicle);
	};

	Events::vehicleRenderEvent.after += [](CVehicle* veh)
	{
		if (Vehicles::HasVehicle(veh)) Vehicles::GetVehicle(veh)->RegisterCoronas();
	};

	Events::drawingEvent.after += Draw;
	Events::processScriptsEvent += Update;
}

void Mod::Update()
{
	Input::Update();
	Menu::Update();

	if (Keybinds::toggleDebug.CheckKeybind())
	{
		m_EnableDebug = !m_EnableDebug;
		CMessages::AddMessageJumpQ(m_EnableDebug ? "Debug enabled" : "Debug disabled", 1000, 0, false);
	}

	if (Keybinds::openMenu.CheckKeybind())
	{
		ToggleMenu();
	}
	else {
		if (Keybinds::toggleLights.CheckKeybind())
		{
			auto veh = FindPlayerVehicle(0, false);

			if (veh > 0) {
				if (Vehicles::HasVehicle(veh)) {
					auto vehicle = Vehicles::GetVehicle(veh);
					vehicle->m_Enabled = !vehicle->m_Enabled;
				}
			}
		}
	}

	//

	if (Menu::m_Visible) {
		if (Input::GetKeyDown(18)) {
			SetPlayerControl(true);
			Menu::m_Hide = true;
		}
		if (Input::GetKeyUp(18)) {
			SetPlayerControl(false);
			Menu::m_Hide = false;
		}
	}
	else {
		Vehicle::m_DrawPoints = false;
	}

	//

	if (Keybinds::reloadConfig.CheckKeybind())
	{
		ReloadConfig();
	}

	Vehicles::TryAddAllVehicles();

	for (auto vehicle : Vehicles::GetVehicles())
	{
		vehicle->Update();
	}
}

void Mod::Draw()
{
	CVector2D screenPos(200, 400);
	char buffer[256];

	if (m_EnableDebug) {

		sprintf(buffer, "Menu windows %d", Menu::m_Windows.size());
		DrawScreenText(buffer, screenPos);

		screenPos.y += 20;

		sprintf(buffer, "Vehicles: %d", Vehicles::GetVehicles().size());
		DrawScreenText(buffer, screenPos);

		screenPos.y += 20;

		sprintf(buffer, "LightGroups:");
		DrawScreenText(buffer, screenPos);

		screenPos.y += 20;

		for (auto lightGroup : LightGroups::GetLightGroups())
		{
			sprintf(buffer, "name= %s, id= %d", lightGroup->name.c_str(), lightGroup->modelId);
			DrawScreenText(buffer, screenPos);
			screenPos.y += 20;
		}
	}

	for (auto vehicle : Vehicles::GetVehicles())
	{
		vehicle->Draw();

		if (m_EnableDebug)
		{
			vehicle->DrawDebug();
		}
	}

	Menu::Draw();
}

void Mod::ToggleMenu()
{
	if (!Menu::m_Visible) {
		auto veh = FindPlayerVehicle(0, false);

		if (!veh) {
			Localization::PrintString("error_need_vehicle", 1000);
			return;
		}

		WindowMain::m_Vehicle = veh;

		if (Vehicles::HasVehicle(veh)) Vehicles::GetVehicle(veh)->m_Enabled = true;
	}

	Menu::m_Visible = !Menu::m_Visible;
	Menu::m_Hide = false;

	if (Menu::m_Visible) {
		WindowMain::Create();
		//WindowTest::Create();

		SetPlayerControl(false);
	}
	else {
		Menu::RemoveAllWindows();

		SetPlayerControl(true);

		Config::SaveJSON();
	}
}

void Mod::SetPlayerControl(bool enabled)
{
	Command<Commands::SET_PLAYER_CONTROL>(0, enabled);
}

void Mod::ReloadConfig()
{
	if (Menu::m_Visible) {
		ToggleMenu();
	}

	CMessages::AddMessageJumpQ("Reloading config...", 1000, 0, false);

	Vehicles::RemoveAllVehicles();
	LightGroups::RemoveAllLightGroups();
	Patterns::RemoveAllPatterns();

	Config::LoadJSON();

	Vehicles::TryAddAllVehicles();

	CMessages::AddMessageJumpQ("Config reloaded", 1000, 0, false);
}

void Mod::ShowErrorMessage(std::string title, std::string content)
{
	static char buffer[256];
	snprintf(buffer, 256, "%s", content.c_str());

	Log::file << "------------------------------------" << std::endl;
	Log::file << title << std::endl;
	Log::file << content << std::endl;
	Log::file << "------------------------------------" << std::endl;

	if (firstLoad)
	{
		MessageBox(HWND_DESKTOP, content.c_str(), title.c_str(), MB_ICONERROR);
	}
	else {
		static char buffer[256];
		sprintf(buffer, "%s~n~Check NeonLights.log for more info", title.c_str());
		CMessages::AddMessageJumpQ(buffer, 3000, 0, true);
	}
}

template<class T>
inline Json::Value Mod::ValidateValue(Json::Value value, T defaultValue)
{
	if (value.empty()) return defaultValue;
	return value;
}

Mod mod;
