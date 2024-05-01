#pragma once

#include <iomanip>
#include <iostream>
#include <map>
#include <math.h>
#include <fstream>
#include <filesystem>

#include "plugin.h"

#include "CPed.h"
#include "RenderWare.h"
#include "CSprite.h"
#include "CFont.h"
#include "common.h"
#include "CRadar.h"
#include "CMenuManager.h"
#include "CFont.h"
#include "CSprite.h"
#include "CTimer.h"
#include "CMessages.h"
#include "CCoronas.h"

#include "extensions/ScriptCommands.h"

#include "json/json.h"

#include "TestHelper.h"

#include "log/Log.h"
#include "input/Input.h"
#include "menu/Menu.h"


#define M_PI 3.14159265358979323846

using namespace plugin;

static float lerp(float a, float b, float f) {
	return a + f * (b - a);
}

static unsigned char ucharIntensity(unsigned char uc, float intensity) {
	return (unsigned char)std::clamp((int)round(((float)uc) * intensity), 0, 255);
}

static void DrawWorldText(char* text, CVector position, CRGBA color = CRGBA(255, 255, 255, 255)) {
	CFont::SetOrientation(ALIGN_CENTER);
	CFont::SetColor(color);
	CFont::SetDropShadowPosition(1);
	CFont::SetBackground(false, false);
	CFont::SetWrapx(500.0);
	CFont::SetScale(0.5 * 0.7f, 1.0 * 0.7f);
	CFont::SetFontStyle(FONT_SUBTITLES);
	CFont::SetProportional(true);

	RwV3d rwp = { position.x, position.y, position.z };
	RwV3d screenCoors; float w, h;

	CSprite::CalcScreenCoors(rwp, &screenCoors, &w, &h, true, true);
	CFont::PrintString(screenCoors.x, screenCoors.y, text);
}

static void DrawScreenText(char* text, CVector2D position, CRGBA color = CRGBA(0, 255, 0, 255)) {
	CFont::SetOrientation(ALIGN_CENTER);
	CFont::SetColor(color);
	CFont::SetDropShadowPosition(1);
	CFont::SetBackground(false, false);
	CFont::SetWrapx(500.0);
	CFont::SetScale(0.5, 1.0);
	CFont::SetFontStyle(FONT_SUBTITLES);
	CFont::SetProportional(true);

	CFont::PrintString(position.x, position.y, text);
}

static float distance3d(float x1, float y1, float z1, float x2, float y2, float z2) {
	float d = (float)sqrt(pow(x2 - x1, 2) +
		pow(y2 - y1, 2) +
		pow(z2 - z1, 2) * 1.0);
	std::cout << std::fixed;
	std::cout << std::setprecision(2);
	return d;
}

static char* (*GetFrameNodeName)(RwFrame* frame) = (char* (*)(RwFrame*))0x72FB30;

static CVector GetFrameNodePosition(RwFrame* frame) {
	return { frame->modelling.pos.x, frame->modelling.pos.y, frame->modelling.pos.z };
}

static CVector CVectorLerp(CVector A, CVector B, float t) {
	return A * t + B * (1.f - t);
}

static CRGBA CRGBALerp(CRGBA color1, CRGBA color2, float t) {
	CRGBA color;
	color.r = (unsigned char)(color1.r + ((color2.r - color1.r) * t));
	color.g = (unsigned char)(color1.g + ((color2.g - color1.g) * t));
	color.b = (unsigned char)(color1.b + ((color2.b - color1.b) * t));
	color.a = 255;
	return color;
}

static std::string FormatCVector(CVector vec) {
	return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
}

static std::string ToUpper(std::string data) {
	std::for_each(data.begin(), data.end(), [](char& c) {
		c = ::toupper(c);
	});
	return data;
}

static std::string ToLower(std::string data) {
	std::for_each(data.begin(), data.end(), [](char& c) {
		c = ::tolower(c);
	});
	return data;
}

static float TotalDistanceBetweenPoints(std::vector<CVector> points) {
	float totalDistance = 0.0f;
	for (size_t i = 0; i < points.size() - 1; i++) totalDistance += DistanceBetweenPoints(points[i], points[i + 1]);
	return totalDistance;
}


static CVector PointInLines(std::vector<CVector> points, float position) {
	//static char buffer[512];

	float totalDistance = TotalDistanceBetweenPoints(points);

	float distance = 0.0f;
	float toLerp = 0.0f;

	int point = -1;

	while (distance < position && point < (int)points.size())
	{
		point++;

		float len = DistanceBetweenPoints(points[point], points[point + 1]);

		toLerp = (position - distance) / len;

		//sprintf(buffer, "point %d; position %.2f; distance %.2f; len %.2f; toLerp %.2f", point, position, distance, len, toLerp);
		//MessageBox(HWND_DESKTOP, buffer, "", MB_ICONERROR);

		distance += len;
	}

	if (point == -1) return points[0];

	return CVectorLerp(points[point + 1], points[point], toLerp);
}


static Json::Value ColorToJSON(CRGBA color) {
	Json::Value json;
	json["r"] = color.r;
	json["g"] = color.g;
	json["b"] = color.b;
	json["a"] = color.a;
	return json;
}

static CRGBA ColorFromJSON(Json::Value json) {
	CRGBA color;
	color.r = json["r"].asInt();
	color.g = json["g"].asInt();
	color.b = json["b"].asInt();
	color.a = json["a"].asInt();
	return color;
}

static Json::Value CVectorToJSON(CVector vec) {
	Json::Value json;
	json["x"] = vec.x;
	json["y"] = vec.y;
	json["z"] = vec.z;
	return json;
}

static CVector CVectorFromJSON(Json::Value json) {
	CVector vec;
	vec.x = json["x"].asFloat();
	vec.y = json["y"].asFloat();
	vec.z = json["z"].asFloat();
	return vec;
}

template <typename T>
static std::string join(const T& v, const std::string& delim) {
	std::ostringstream s;
	for (const auto& i : v) {
		if (&i != &v[0]) {
			s << delim;
		}
		s << i;
	}
	return s.str();
}