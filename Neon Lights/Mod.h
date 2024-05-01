#include "pch.h"

class Mod {
public:
    static bool m_EnableDebug;
    static std::string m_Version;
    static std::string m_Name;
    static bool m_IsSamp;
    static int m_LightIdOffset;

    Mod();

    static void Update();
    static void Draw();

    static void ToggleMenu();

    static void SetPlayerControl(bool enabled);

    static void ReloadConfig();

    static void ShowErrorMessage(std::string title, std::string content);

    template<class T>
    static Json::Value ValidateValue(Json::Value value, T defaultValue);
};