#include "WindowPattern.h"

#include "WindowMain.h"

#include "../Vehicles.h"
#include "../Mod.h"

#include "../menu/TextEditor.h"
#include "../localization/Localization.h"

Pattern* WindowPattern::m_Pattern = nullptr;
PatternStep* WindowPattern::m_PatternStep = nullptr;

static CRGBA disabledColor = CRGBA(0, 0, 0, 255);

void WindowPattern::Create() {
	auto window = Menu::AddWindow(Mod::m_Name, "Patterns");

	auto patterns = Patterns::GetPatterns();

	window->AddItem("---------- " + std::to_string(patterns.size()) + " Patterns ----------");

	for (auto pattern : patterns)
	{
		auto button = window->AddButton(Localization::GetLineFormatted("edit_pattern", pattern->name));
		button->m_OnClick = [window, pattern]() {
			Menu::RemoveWindow(window);
			m_Pattern = pattern;
			CreateEditPattern();
		};
	}

	window->AddItem("--------------------------------------");


	auto addPattern = window->AddButton(Localization::GetLine("add_pattern"));
	addPattern->m_OnClick = [window]() mutable {
		auto pattern = Patterns::CreatePattern("New pattern");
		pattern->AddStep(CRGBA(255, 0, 0), 500);
		pattern->AddStep(CRGBA(0, 255, 0), 500);
		pattern->AddStep(CRGBA(0, 0, 255), 500);

		Menu::RemoveWindow(window);
		Create();
	};

	auto back = window->AddButton(Localization::GetLine("back"));
	back->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		WindowMain::Create();
	};
}

void WindowPattern::CreateEditPattern() {
	auto pattern = m_Pattern;
	auto window = Menu::AddWindow(Mod::m_Name, "Patterns > " + pattern->name);

	int i = 0;
	for (auto step : pattern->steps)
	{
		auto button = window->AddButton(Localization::GetLineFormatted("edit_pattern_step", step->time));
		button->AddColorIndicator(&step->color, CVector2D(30, 0));
		
		button->m_OnClick = [window, step]() mutable {
			m_PatternStep = step;
			Menu::RemoveWindow(window);
			CreateEditStep();
		};
		i++;
	}

	auto buttonAddStep = window->AddButton(Localization::GetLine("add_step"));
	buttonAddStep->m_OnClick = [window, pattern]() {
		Menu::RemoveWindow(window);
		pattern->AddStep(CRGBA(255, 0, 0), 500);
		CreateEditPattern();
	};

	auto buttonName = window->AddButton(Localization::GetLine("edit_name"));
	buttonName->AddTextStr(&pattern->name, CVector2D(10, 0));
	buttonName->m_OnClick = [pattern]() {
		TextEditor::Open("Edit name", true, &pattern->name);
	};

	/*
	auto buttonClone = window->AddButton(Localization::GetLine("clone"));
	buttonClone->m_OnClick = [window, pattern]() {

		auto newPattern = Patterns::CreatePattern("");
		newPattern->FromJSON(pattern->ToJSON());
		newPattern->name += " CLONE";

		Menu::RemoveWindow(window);
		CreatePatterns();
	};
	*/

	auto buttonDelete = window->AddButton(Localization::GetLine("delete"));
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, pattern]() {
		if (Patterns::GetPatterns().size() == 1) {
			Localization::PrintString("error_need_at_least_one", 1000);
			return;
		}

		
		Menu::RemoveWindow(window);

		Vehicles::RemoveAllVehicles();
		LightGroups::RemovePatternReferences(pattern);
		Patterns::RemovePattern(pattern);
		Vehicles::TryAddAllVehicles();

		m_Pattern = nullptr;

		Create();
		
	};

	auto back = window->AddButton(Localization::GetLine("back"));
	back->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		Create();
	};
}

void WindowPattern::CreateEditStep() {
	auto pattern = m_Pattern;
	auto step = m_PatternStep;
	auto window = Menu::AddWindow(Mod::m_Name, "Edit step");

	auto color = window->AddButton("Color");
	color->AddColorIndicator(&step->color, CVector2D(20, 0));
	color->m_OnClick = [step, window]() {
		Menu::SaveIndex();
		Menu::CreateColorPickerWindow(&step->color, [window]() {
			Menu::RemoveWindow(window);
			CreateEditStep();
			Menu::RestoreIndex();
		});
	};

	auto time = window->AddNumberRange(Localization::GetLine("time"), &step->time, 1, 999999);
	time->m_OnClick = [step]() mutable {
		TextEditor::Open(Localization::GetLine("time"), true, &step->time);
	};
	time->m_HoldToChange = true;
	time->m_AddBy = 5;

	auto buttonDelete = window->AddButton(Localization::GetLine("delete"));
	buttonDelete->m_BackgroundColor = CRGBA(255, 0, 0, 80);
	buttonDelete->m_BackgroundColorSelected = CRGBA(255, 0, 0, 172);
	buttonDelete->m_OnClick = [window, pattern, step]() {
		if (pattern->steps.size() == 1) {
			Localization::PrintString("error_need_at_least_one", 1000);
			return;
		}

		
		Menu::RemoveWindow(window);

		Vehicles::RemoveAllVehicles();
		pattern->RemoveStep(step);
		Vehicles::TryAddAllVehicles();

		CreateEditPattern();
		
	};

	auto back = window->AddButton(Localization::GetLine("back"));
	back->m_OnClick = [window]() {
		Menu::RemoveWindow(window);
		CreateEditPattern();
	};
}