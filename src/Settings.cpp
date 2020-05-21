#include "Settings.h"


bool Settings::LoadSettings(const bool a_dumpParse)
{
	auto [log, success] = J2S::load_settings(FILE_NAME, a_dumpParse);

	if (!log.empty()) {
		_ERROR("%s", log.c_str());
	}
	
	return success;
}


decltype(Settings::factor) Settings::factor("SpeedCastFactor", 0.0f);
decltype(Settings::useGlobal) Settings::useGlobal("UseGlobal", false);
decltype(Settings::globalName) Settings::globalName("GlobalName", "SpeedCastGlobal");
decltype(Settings::global) Settings::global(nullptr);