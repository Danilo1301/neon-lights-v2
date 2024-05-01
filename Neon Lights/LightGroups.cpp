#include "LightGroups.h"

std::map<int, std::vector<LightGroup*>> LightGroups::m_Groups;

LightGroup* LightGroups::CreateLightGroup(int modelId)
{
	LightGroup* lightGroup = new LightGroup(modelId);
	m_Groups[modelId].push_back(lightGroup);
	return lightGroup;
}

std::vector<LightGroup*> LightGroups::GetLightGroups()
{
	std::vector<LightGroup*> groups;
	for (auto p : m_Groups)	for (auto v : p.second) groups.push_back(v);
	return groups;
}

std::vector<LightGroup*> LightGroups::GetLightGroups(int modelId)
{
	auto lightGroups = GetLightGroups();
	std::vector<LightGroup*> groups;
	for (auto group : lightGroups)
	{
		if (group->modelId == modelId) {
			groups.push_back(group);
		}
	}
	return groups;
}


void LightGroups::RemoveLightGroup(LightGroup* lightGroup)
{
	Log::file << "[LightGroups] RemoveLightGroup" << lightGroup->modelId << std::endl;

	auto &vec = m_Groups[lightGroup->modelId];

	for (auto pair : vec) {
		auto it = std::find(vec.begin(), vec.end(), lightGroup);
		if (it == vec.end()) continue;
		vec.erase(it);
		lightGroup->Destroy();
		delete lightGroup;
	}
}

void LightGroups::RemovePatternReferences(Pattern* pattern)
{
	Log::file << "[LightGroups] Trying to remove pattern '" << pattern->name << "' references" << std::endl;

	for (auto pair : m_Groups) {
		auto lightGroups = pair.second;

		for (auto lightGroup : lightGroups) {
			if (lightGroup->pattern == pattern)
			{
				lightGroup->pattern = NULL;
			}
		}
	}
}


void LightGroups::RemoveAllLightGroups()
{
	Log::file << "[LightGroups] RemoveAllLightGroups" << std::endl;

	std::vector<LightGroup*> toremove;
	for (auto pair : m_Groups) {
		while (m_Groups[pair.first].size() > 0) {
			RemoveLightGroup(m_Groups[pair.first][0]);
		}
	}
}