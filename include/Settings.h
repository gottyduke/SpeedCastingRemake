#pragma once

#include "Json2Settings.h"
#include "RE/TESGlobal.h"


namespace J2S = Json2Settings;

class Settings
{
public:
	Settings() = delete;

	static bool LoadSettings(bool a_dumpParse = false);

	static J2S::fSetting factor;

	static J2S::bSetting useHook;
	static J2S::bSetting useGlobal;
	
	static J2S::sSetting globalName;

	static RE::TESGlobal* global;
	
private:
	static constexpr auto FILE_NAME = R"(Data\SKSE\Plugins\SpeedCastingRemake.json)";
};