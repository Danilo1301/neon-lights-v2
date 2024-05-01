#include "WindowLightGroup.h"

#include "WindowMain.h"
#include "WindowPoint.h"

#include "../Mod.h"
#include "../Patterns.h"
#include "../Vehicles.h"
#include "../menu/Menu.h"
#include "../menu/PositionEditor.h"
#include "../menu/TextEditor.h"

#include "../localization/Localization.h"

LightGroup* WindowLightGroup::m_LightGroup = nullptr;

void WindowLightGroup::Create() {
	auto veh = WindowMain::m_Vehicle;
	auto window = Menu::AddWindow(Mod::m_Name, "LightGroups");

	auto buttonAddLightGroupSiren = window->AddButton(Localization::GetLine("add_light_group"));
	buttonAddLightGroupSiren->m_OnClick = [veh, window]() mutable {

		auto lightGroup = LightGroups::CreateLightGroup(veh->m_nModelIndex);
		lightGroup->position = CVector(0, 0, 2);

		lightGroup->AddPoint(CVector(-1, 0, 0));
		lightGroup->AddPoint(CVector(1, 0, 0));

		auto patterns = Patterns::GetPatterns();

		if (patterns.size() > 0) lightGroup->pattern = patterns[0];

		Menu::RemoveWindow(window);
		Create();
	};

	auto lightGroups = LightGroups::GetLightGroups(veh->m_nModelIndex);

	window->AddItem("---------- " + std::to_string(lightGroups.size()) + " LightGroups ----------");

	for (auto lightGroup : lightGroups)
	{
		auto button1 = window->AddButton(Localization::GetLineFormatted("edit_light_group", lightGroup->name));
		button1->m_OnClick = [window, lightGroup]() mutable {
			Menu::RemoveWindow(window);

			m_LightGroup = lightGroup;
			CreateEditLightGroup();
		};
	}

	window->AddItem("--------------------------------------");

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowMain::Create();
	};
}

void WindowLightGroup::CreateEditLightGroup() {
	auto lightGroup = m_LightGroup;
	CVehicle* veh = WindowMain::m_Vehicle;

	auto window = Menu::AddWindow(Mod::m_Name, "LightGroups > " + lightGroup->name);

	auto buttonName = window->AddButton(Localization::GetLine("edit_name"));
	buttonName->AddTextStr(&lightGroup->name, CVector2D(10, 0));
	buttonName->m_OnClick = [lightGroup]() {
		TextEditor::Open(Localization::GetLine("edit_name"), true, &lightGroup->name);
	};

	auto optionsPattern = window->AddButton(Localization::GetLine("edit_lightgroup_pattern"));

	if(lightGroup->pattern) optionsPattern->AddTextStr(&lightGroup->pattern->name, CVector2D(10, 0));

	optionsPattern->m_OnClick = [window, lightGroup]() {
		Menu::SaveIndex();

		auto window2 = Menu::AddWindow("", Localization::GetLine("select_pattern"));
		window2->m_Position.x += window2->m_Size.x + 5.0f;

		for (auto pattern : Patterns::GetPatterns()) {
			auto button = window2->AddButton(pattern->name);
			button->m_OnClick = [window, window2, pattern, lightGroup] () {

				lightGroup->pattern = pattern;

				Menu::RemoveWindow(window2);
				Menu::RemoveWindow(window);
				CreateEditLightGroup();

				Menu::RestoreIndex();
			};
		}
	};


	auto buttonEditPoints = window->AddButton(Localization::GetLineFormatted("edit_points", lightGroup->points.size()));
	buttonEditPoints->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowPoint::Create();
	};

	auto positionButton = window->AddButton(Localization::GetLine("edit_position_points"));
	positionButton->m_OnClick = [lightGroup]() {
		PositionEditor::Toggle(&lightGroup->position);
	};

	auto amountLights = window->AddNumberRange(Localization::GetLine("lights_amount"), &lightGroup->amountLights, 1, 9999);
	amountLights->m_HoldToChange = true;

	auto offsetBy = window->AddNumberRange(Localization::GetLine("offset_by"), &lightGroup->offsetBy, -999999, 999999);
	offsetBy->m_HoldToChange = true;

	auto lerpColor = window->AddCheckBox(Localization::GetLine("lerp_color"), &lightGroup->lerpColor);

	auto size = window->AddNumberRange(Localization::GetLine("size"), &lightGroup->size, 0.0f, 100.0f);
	size->m_HoldToChange = true;

	auto buttonDelete = window->AddButton(Localization::GetLine("delete"));
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, lightGroup]() {
		Vehicles::RemoveAllVehicles();
		LightGroups::RemoveLightGroup(lightGroup);
		Vehicles::TryAddAllVehicles();

		Menu::RemoveWindow(window);
		Create();
	};

	auto buttonClone = window->AddButton(Localization::GetLine("clone"));
	buttonClone->m_OnClick = [window, lightGroup]() {

		auto newLightGroup = LightGroups::CreateLightGroup(lightGroup->modelId);
		newLightGroup->FromJSON(lightGroup->ToJSON());

		Menu::RemoveWindow(window);
		Create();
	};

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		Create();
	};
}