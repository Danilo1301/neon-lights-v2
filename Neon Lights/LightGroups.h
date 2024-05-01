#pragma once

#include "pch.h"
#include "LightGroup.h"

class LightGroups {
private:
	static std::map<int, std::vector<LightGroup*>> m_Groups;
public:
	static LightGroup* CreateLightGroup(int modelId);
	static std::vector<LightGroup*> GetLightGroups();
	static std::vector<LightGroup*> GetLightGroups(int modelId);
	static void RemoveLightGroup(LightGroup* lightGroup);
	static void RemovePatternReferences(Pattern* pattern);
	static void RemoveAllLightGroups();
};