#include "WindowSettings.h"

#include "WindowMain.h"

#include "../Mod.h"

//#include "../Vehicle.h"

//#include "../menu/Menu.h"
#include "../localization/Localization.h"

void WindowSettings::Create()
{
	auto window = Menu::AddWindow(Mod::m_Name, Localization::GetLine("main_settings"));

	auto lightIdOffset = window->AddNumberRange(Localization::GetLine("light_id_offset"), &Mod::m_LightIdOffset, 0, 999999);
	lightIdOffset->m_HoldToChange = true;

	auto back = window->AddButton(Localization::GetLine("back"));
	back->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowMain::Create();
	};
}