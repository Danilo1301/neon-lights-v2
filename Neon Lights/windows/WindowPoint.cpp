#include "WindowPoint.h"

#include "WindowLightGroup.h"
#include "WindowMain.h"

#include "../VehicleDummy.h"
#include "../Vehicles.h"
#include "../Mod.h"

#include "../menu/Menu.h"
#include "../menu/PositionEditor.h"
#include "../menu/TextEditor.h"
#include "../localization/Localization.h"

Point* WindowPoint::m_Point = nullptr;

void WindowPoint::Create() {
	Vehicle::m_DrawPoints = true;

	CVehicle* veh = WindowMain::m_Vehicle;
	auto lightGroup = WindowLightGroup::m_LightGroup;

	auto window = Menu::AddWindow(Mod::m_Name, "LightGroups > " + lightGroup->name + " > Points");

	auto flipHorizontal = window->AddButton(Localization::GetLine("points_flip_horizontal"));
	flipHorizontal->m_OnClick = [window, lightGroup]() {
		lightGroup->FlipPoints(true, false);
	};

	auto flipVertical = window->AddButton(Localization::GetLine("points_flip_vertical"));
	flipVertical->m_OnClick = [window, lightGroup]() {
		lightGroup->FlipPoints(false, true);
	};

	window->AddItem("----------------");

	int i = 0;
	for (auto point : lightGroup->points)
	{
		auto buttonEditPoint = window->AddButton(Localization::GetLineFormatted("edit_point", (i + 1)));
		buttonEditPoint->m_OnClick = [window, point]() {
			m_Point = point;

			Menu::RemoveWindow(window);
			CreateEditPoint();
		};

		i++;
	}

	auto buttonAddPoint = window->AddButton(Localization::GetLine("add_point"));
	buttonAddPoint->m_OnClick = [window, lightGroup]() {

		auto point = lightGroup->AddPoint(CVector(0, 0, 0));

		Menu::RemoveWindow(window);
		Create();
	};

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowLightGroup::CreateEditLightGroup();

		Vehicle::m_DrawPoints = false;
	};
}

void WindowPoint::CreateEditPoint() {
	CVehicle* veh = WindowMain::m_Vehicle;
	auto lightGroup = WindowLightGroup::m_LightGroup;
	auto point = m_Point;

	auto window = Menu::AddWindow(Mod::m_Name, "LightGroups > " + lightGroup->name + " > Points");

	auto positionButton = window->AddButton(Localization::GetLine("edit_position"));
	positionButton->m_OnClick = [point]() {
		PositionEditor::Toggle(&point->offset);
	};

	auto buttonDelete = window->AddButton(Localization::GetLine("delete"));
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, lightGroup, point]() {
		if (lightGroup->points.size() < 2) return;

		lightGroup->RemovePoint(point);

		Menu::RemoveWindow(window);
		Create();
	};

	auto buttonClone = window->AddButton(Localization::GetLine("clone"));
	buttonClone->m_OnClick = [window, lightGroup, point]() {
		auto newPoint = lightGroup->AddPoint(CVector(0, 0, 0));
		newPoint->FromJSON(point->ToJSON());

		Menu::RemoveWindow(window);
		Create();
	};

	auto buttonBack = window->AddButton(Localization::GetLine("back"));
	buttonBack->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		Create();
	};
}
