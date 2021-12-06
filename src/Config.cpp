#include "Config.h"


namespace Config
{
	Double SpellCastingFactor{ "SpellCastingFactor" };
	Boolean EnableHookUsage{ "EnableHook" };
	Boolean EnableHostileNPC{ "EnableHostileNPC" };
	Boolean EnableAlliedNPC{ "EnableAlliedNPC" };
	Boolean EnableFollower{ "EnableFollower" };
	Boolean EnableGlobalUsage{ "EnableGlobalUsage" };
	String GlobalName{ "GlobalName" };
	Boolean EnableMasterSpellSeparation{ "EnableMasterSpellSeparation" };
	Double MasterSpellCastingFactor{ "MasterSpellCastingFactor" };
	Integer ExcludedSpells{ "ExcludedSpells" };
	RE::TESGlobal* Global{ nullptr };


	void Load()
	{
		auto mainConfig = COMPILE_PROXY("SpeedCastingRemake.toml"sv);

		mainConfig.Bind(SpellCastingFactor, 0.5);
		mainConfig.Bind(EnableHookUsage, true);
		mainConfig.Bind(EnableHostileNPC, false);
		mainConfig.Bind(EnableAlliedNPC, true);
		mainConfig.Bind(EnableFollower, true);
		mainConfig.Bind(EnableGlobalUsage, false);
		mainConfig.Bind(GlobalName, "SpeedCastingFactor");
		mainConfig.Bind(EnableMasterSpellSeparation, 0.8);
		mainConfig.Bind(ExcludedSpells, 0);

		mainConfig.Load();
		
		INFO("Config loaded"sv);
	}
}