#pragma once

#include "LightGroup.h"

struct LightGroupData
{
	LightGroup* lightGroup;
	int progress = 0;

	LightGroupData(LightGroup* lightGroup)
	{
		this->lightGroup = lightGroup;
	}
};