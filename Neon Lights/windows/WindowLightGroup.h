#pragma once

#include "../LightGroups.h"

class WindowLightGroup {
public:
	static LightGroup* m_LightGroup;

	static void Create();
	static void CreateEditLightGroup();
};