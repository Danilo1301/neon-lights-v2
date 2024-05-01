#pragma once

#include "pch.h"

struct PatternStep {
	CRGBA color;
	int time;
};

class Pattern {
public:
	std::string name;
	std::vector<PatternStep*> steps;

	Pattern(std::string name)
	{
		this->name = name;
	}

	void AddStep(CRGBA color, int time)
	{
		PatternStep* step = new PatternStep();
		step->color = color;
		step->time = time;
		steps.push_back(step);
	}

	void RemoveStep(PatternStep* step)
	{
		steps.erase(std::find(steps.begin(), steps.end(), step));
		delete step;
	}

	Json::Value ToJSON()
	{
		Json::Value value = Json::objectValue;
		value["name"] = name;

		value["steps"] = Json::arrayValue;
		for (auto step : steps) {
			Json::Value stepValue;
			stepValue["color"] = ColorToJSON(step->color);
			stepValue["time"] = step->time;

			value["steps"].append(stepValue);
		}

		return value;
	}

	void FromJSON(Json::Value value)
	{
		name = value["name"].asString();

		for (size_t i = 0; i < value["steps"].size(); i++)
		{
			Json::Value stepValue = value["steps"][i];

			AddStep(
				ColorFromJSON(stepValue["color"]),
				stepValue["time"].asInt()
			);
		}
	}
};