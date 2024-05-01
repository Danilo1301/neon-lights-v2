#include "Vehicle.h"

#include "Mod.h"
#include "LightGroups.h"

bool Vehicle::m_DrawPoints = true;

struct CoronaGroup
{

};

Vehicle::Vehicle(CVehicle* vehicle)
{
	m_Vehicle = vehicle;
	m_PrevTime = CTimer::m_snTimeInMilliseconds;
}

void Vehicle::Update()
{
	auto dt = CTimer::m_snTimeInMilliseconds - m_PrevTime;
	if (dt > 500) dt = 500;

	CheckForLightGroups();

	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		auto lightGroupData = pair.second;

		if (!lightGroup->pattern) continue;

		lightGroupData->progress += dt;
		if (lightGroupData->progress >= GetPatternMaxTime(lightGroupData->lightGroup->pattern)) {
			lightGroupData->progress = 0;
		}
	}

	m_PrevTime = CTimer::m_snTimeInMilliseconds;
}

void Vehicle::CheckForLightGroups()
{
	auto lightGroups = LightGroups::GetLightGroups(m_Vehicle->m_nModelIndex);

	for (auto lightGroup : lightGroups)
	{
		if (m_LightGroupData.find(lightGroup) != m_LightGroupData.end()) continue;

		auto lightGroupData = new LightGroupData(lightGroup);
		m_LightGroupData[lightGroup] = lightGroupData;
	}
}

void Vehicle::Draw()
{
	if (Vehicle::m_DrawPoints)
	{
		DrawPoints();
	}
}

void Vehicle::DrawDebug()
{
	char buffer[256];

	CVector vehPosition = m_Vehicle->GetPosition();
	RwV3d rwp = { vehPosition.x, vehPosition.y, vehPosition.z };
	RwV3d screenCoord; float w, h;

	CSprite::CalcScreenCoors(rwp, &screenCoord, &w, &h, true, true);

	CVector2D screenPos({ screenCoord.x, screenCoord.y });

	//

	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		auto lightGroupData = pair.second;

		sprintf(buffer, "[data] id= %d, points=%d progress=%d", lightGroup->modelId, lightGroup->points.size(), lightGroupData->progress);
		DrawScreenText(buffer, screenPos);

		screenPos.y += 20;
	}
}

void Vehicle::DrawPoints()
{
	char buffer[256];

	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		//auto lightGroupData = pair.second;

		int i = 0;
		for (auto point : lightGroup->points)
		{
			auto position = lightGroup->GetPointPosition(point, m_Vehicle);
			sprintf(buffer, "[%d]", i + 1);
			DrawWorldText(buffer, position);

			i++;
		}
	}
}

void Vehicle::RegisterCoronas()
{
	//check C:\Users\danil\Desktop\PC\Jogos\GTA San Andreas\lightId offset mods.txt
	m_LightId = reinterpret_cast<unsigned int>(m_Vehicle) + Mod::m_LightIdOffset;

	if (Vehicle::m_DrawPoints)
	{
		RegisterPointCoronas();
	}

	if (m_Enabled) {
		for (auto pair : m_LightGroupData)
		{
			auto lightGroup = pair.first;
			auto lightGroupData = pair.second;

			if (!lightGroup->pattern) continue;

			RegisterLightGroupCoronas(lightGroupData);
		}
	}
}

void Vehicle::RegisterPointCoronas()
{
	for (auto pair : m_LightGroupData)
	{
		auto lightGroup = pair.first;
		//auto lightGroupData = pair.second;

		for (auto point : lightGroup->points)
		{
			auto position = lightGroup->GetPointPosition(point, m_Vehicle);

			CCoronas::RegisterCorona(
				m_LightId++,
				NULL,
				255,
				0,
				0,
				255,
				position,
				0.2f,
				800.0f,
				eCoronaType::CORONATYPE_SHINYSTAR,
				eCoronaFlareType::FLARETYPE_NONE,
				false,
				false,
				0,
				0.0f,
				false,
				0.01f,
				0,
				100.0f,
				false,
				false
			);
		}
	}
}

void Vehicle::RegisterLightGroupCoronas(LightGroupData* lightGroupData)
{
	auto lightGroup = lightGroupData->lightGroup;
	auto pattern = lightGroup->pattern;

	std::vector<CVector> pointsPos;
	for (auto point : lightGroup->points) pointsPos.push_back(lightGroup->GetPointPosition(point, m_Vehicle));
	float totalDistance = TotalDistanceBetweenPoints(pointsPos);

	for (int i = 0; i < lightGroup->amountLights; i++)
	{
		float linePosition = totalDistance / (lightGroup->amountLights - 1) * i;

		CVector position = PointInLines(pointsPos, linePosition);

		int t;
		int stepIndex;
		//int offset = (i + (coronaGroup->offsetId * lightGroup->amount)) * lightGroup->offsetBy;
		int offset = i * lightGroup->offsetBy;

		//Log::file << "coronaGroup " << cn << ", i " << i << ", offsetId " << coronaGroup->offsetId << ", offset " << offset << std::endl;


		GetPatternStepAndTime(
			lightGroup->pattern,
			lightGroupData->progress + offset,
			t,
			stepIndex
		);
		

		PatternStep* step = pattern->steps[stepIndex];
		PatternStep* nextStep = pattern->steps[(stepIndex + 1) % pattern->steps.size()];

		CRGBA color = step->color;
		
		if (lightGroup->lerpColor) {
			float lerpProgress = (float)t / (float)step->time;

			color = CRGBALerp(
				step->color,
				nextStep->color,
				lerpProgress
			);
		}

		CCoronas::RegisterCorona(
			m_LightId++,
			NULL,
			color.r,
			color.g,
			color.b,
			color.a,
			position,
			lightGroup->size,
			lightGroup->farClip,
			eCoronaType::CORONATYPE_SHINYSTAR,
			eCoronaFlareType::FLARETYPE_NONE,
			false,
			false,
			0,
			0.0f,
			false,
			lightGroup->nearClip,
			0,
			400.0f,
			false,
			false
		);
	}
}

void Vehicle::Destroy()
{
	
};
