#include "Config.h"


namespace Config
{
	void Main::Load() noexcept
	{
		static std::once_flag ConfigInit;
		std::call_once(ConfigInit, [&]() {
			// [[SpeedCasting]]
			config.Bind(SpeedCastingFactor, 1.0);
			config.Bind(SpeedCastingRequirement, 0.5);
			config.Bind(SpeedCastingMinimum, 0.1);
			config.Bind(EnableHook, true);
			config.Bind(EnableTESGlobalControl, false);
			config.Bind(EnableDebug, false);

			// [[IncreasedMagickaCost]]
			config.Bind(IncreasedMagickaCostFactor, 0.0);
			config.Bind(IncreasedMagickaCostCombatOverride, 0.0);

			// [[Player]]
			config.Bind(EnablePlayer, true);
			config.Bind(PlayerSCRFactor, 0.5);
			config.Bind(PlayerSCRCombatOverride, 0.0);
			config.Bind(PlayerIMCFactorOverride, 0.0);

			// [[Hostile]]
			config.Bind(EnableHostile, true);
			config.Bind(HostileSCRFactor, 0.5);
			config.Bind(HostileSCRCombatOverride, 0.0);
			config.Bind(HostileIMCFactorOverride, 0.0);
			config.Bind(ExcludedHostile, "");

			// [[Ally]]
			config.Bind(EnableAlly, true);
			config.Bind(AllySCRFactor, 0.5);
			config.Bind(AllySCRCombatOverride, 0.0);
			config.Bind(AllyIMCFactorOverride, 0.0);
			config.Bind(ExcludedAlly, "");

			// [[Follower]]
			config.Bind(EnableFollower, true);
			config.Bind(FollowerSCRFactor, 0.5);
			config.Bind(FollowerSCRCombatOverride, 0.0);
			config.Bind(FollowerIMCFactorOverride, 0.0);
			config.Bind(ExcludedFollower, "");

			// [[TESGlobal]]
			config.Bind(SCRFactor_Global, "SCR_CastingFactor");
			config.Bind(IMCFactor_Global, "SCR_MagickaFactor");

			// [[MasterSpell]]
			config.Bind(EnableMasterSpell, true);
			config.Bind(MasterSpellSCRFactor, 1.0);
			config.Bind(MasterSpellIMCFactor, 0.0);

			// [[Exclusion]]
			config.Bind(ExcludedSpell, "");
			config.Bind(EnabledSpell, "");

			// [[Exclusion.Player]]
			config.Bind(PlayerExcludedSpell, "");
			config.Bind(PlayerEnabledSpell, "");

			// [[Exclusion.Hostile]]
			config.Bind(HostileExcludedSpell, "");
			config.Bind(HostileEnabledSpell, "");

			// [[Exclusion.Ally]]
			config.Bind(AllyExcludedSpell, "");
			config.Bind(AllyEnabledSpell, "");

			// [[Exclusion.Follower]]
			config.Bind(FollowerExcludedSpell, "");
			config.Bind(FollowerEnabledSpell, "");
		});


		config.Load();
		
		if (*EnableDebug) {
			ENABLE_DEBUG
		} else {
			DISABLE_DEBUG
		}



		INFO("Config loaded"sv);
	}
}
