#pragma once

#include "pch.h"
#include "LightGroup.h"
#include "LightGroupData.h"

class Vehicle {
private:
	std::map<LightGroup*, LightGroupData*> m_LightGroupData;
	int m_LightId = 0;
public:
	static bool m_DrawPoints;

	unsigned int m_PrevTime= 0;
	bool m_Enabled = true;

	CVehicle* m_Vehicle;

	Vehicle(CVehicle* vehicle);

	void Update();
	void CheckForLightGroups();

	void Draw();
	void DrawDebug();
	void DrawPoints();

	void RegisterCoronas();
	void RegisterPointCoronas();

	void RegisterLightGroupCoronas(LightGroupData* lightGroupData);

	void Destroy();

	static int GetPatternMaxTime(Pattern* pattern) {
		int time = 0;
		for (PatternStep* step : pattern->steps) time += step->time;
		return time;
	}

	static void GetPatternStepAndTime(Pattern* pattern, int time, int& outTime, int& outStep) {
		int maxTime = GetPatternMaxTime(pattern);

		static char buffer[512];

		int t = (int)((float)time - (floor((float)time / (float)maxTime) * (float)maxTime));

		//sprintf(buffer, "time %d; t %d; max %d", time, t, maxTime);
		//MessageBox(HWND_DESKTOP, buffer, "", MB_ICONERROR);

		outTime = 0;

		int findTime = 0;
		int stepIndex = -1;
		while (findTime < t) //&& stepIndex < pattern->steps.size()
		{
			stepIndex++;
			outTime = t - findTime;


			sprintf(buffer, "time %d; t %d; findTime %d; stepIndex %d", time, t, findTime, stepIndex);
			//MessageBox(HWND_DESKTOP, buffer, "", MB_ICONERROR);

			findTime += pattern->steps[stepIndex]->time;

		}

		if (stepIndex == -1) stepIndex = 0;

		outStep = stepIndex;
	}

};