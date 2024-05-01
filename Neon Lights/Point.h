#pragma once

#include "pch.h"
#include "VehicleDummy.h"

class Point {
public:
	CVector offset = CVector(0, 0, 0);

	CVector GetOffset()
	{
		return offset;
	}

	Json::Value ToJSON()
	{
		Json::Value value = Json::objectValue;

		value["offset"] = CVectorToJSON(offset);
		
		return value;
	}

	void FromJSON(Json::Value value)
	{
		offset = CVectorFromJSON(value["offset"]);
	}
};