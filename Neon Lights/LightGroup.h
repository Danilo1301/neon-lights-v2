#pragma once

#include "pch.h"
#include "Point.h"
#include "Pattern.h"
#include "Patterns.h"

class LightGroup {
public:
	int modelId;
	std::string name = "Light Group";

	CVector position = CVector(0, 0, 0);
	int amountLights = 30;
	Pattern* pattern = NULL;
	float size = 0.2f;
	bool lerpColor = true;
	float nearClip = 0.01f;
	float farClip = 800.0f;
	int offsetBy = 0;

	std::vector<Point*> points;

	LightGroup(int modelId)
	{
		this->modelId = modelId;
	}

	Point* AddPoint(CVector offset)
	{
		Point* point = new Point();
		point->offset = offset;
		points.push_back(point);
		return point;
	}

	void RemovePoint(Point* point)
	{
		auto it = std::find(points.begin(), points.end(), point);
		if (it == points.end()) return;
		points.erase(it);
		delete point;
	}

	CVector GetPointPosition(Point* point, CVehicle* vehicle)
	{
		auto offset = this->position + point->GetOffset();
		auto position = VehicleDummy::GetTransformedPosition(vehicle, offset);

		return position;
	}

	void Destroy()
	{
		while (points.size() > 0) RemovePoint(points[0]);
	}

	Json::Value ToJSON() {
		Json::Value value = Json::objectValue;

		//value["modelId"] = modelId;
		value["name"] = name;

		value["position"] = CVectorToJSON(position);
		value["amountLights"] = amountLights;

		value["pattern"] = pattern ? Patterns::FindPatternIndex(pattern) : -1;
		
		value["size"] = size;
		value["lerpColor"] = lerpColor;
		value["nearClip"] = nearClip;
		value["farClip"] = farClip;
		value["offsetBy"] = offsetBy;
		
		value["points"] = Json::arrayValue;
		for (auto point : points) {
			value["points"].append(point->ToJSON());
		}

		return value;
	}

	void FromJSON(Json::Value value) {
		//modelId = value["modelId"].asInt();
		name = value["name"].asString();

		position = CVectorFromJSON(value["position"]);
		amountLights = value["amountLights"].asInt();

		pattern = Patterns::FindPatternByIndex(value["pattern"].asInt());

		size = value["size"].asFloat();
		lerpColor = value["lerpColor"].asBool();
		nearClip = value["nearClip"].asFloat();
		farClip = value["farClip"].asFloat();
		offsetBy = value["offsetBy"].asInt();

		for (size_t i = 0; i < value["points"].size(); i++)
		{
			Json::Value pointValue = value["points"][i];

			Point* point = new Point();
			point->FromJSON(pointValue);
			points.push_back(point);
		}
	}

	void FlipPoints(bool horizontal, bool vertical)
	{
		for (auto point : points)
		{
			if (horizontal) point->offset.x *= -1;
			if (vertical) point->offset.y *= -1;
		}
	}
};