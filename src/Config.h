#pragma once

namespace Config
{
	using namespace DKUtil::Alias;


	class Main : 
		public DKUtil::model::Singleton<Main>
	{
	public:
		// [SpeedCasting]
		Double SpeedCastingFactor{ "SpeedCastingFactor" };
		Double SpeedCastingRequirement{ "SpeedCastingRequirement" };
		Double SpeedCastingMinimum{ "SpeedCastingMinimum" };
		Boolean EnableHook{ "EnableHook" };
		Boolean EnableTESGlobalControl{ "EnableTESGlobalControl" };
		Boolean EnableDebug{ "EnableDebug" };

		// [IncreasedMagickaCost]
		Double IncreasedMagickaCostFactor{ "IncreasedMagickaCostFactor" };
		Double IncreasedMagickaCostCombatOverride{ "IncreasedMagickaCostCombatOverride" };

		// [Player]
		Boolean EnablePlayer{ "EnablePlayer" };
		Double PlayerSCRFactor{ "PlayerSCRFactor" };
		Double PlayerSCRCombatOverride{ "PlayerSCRCombatOverride" };
		Double PlayerIMCFactorOverride{ "PlayerIMCFactorOverride" };

		// [Hostile]
		Boolean EnableHostile{ "EnableHostile" };
		Double HostileSCRFactor{ "HostileSCRFactor" };
		Double HostileSCRCombatOverride{ "HostileSCRCombatOverride" };
		Double HostileIMCFactorOverride{ "HostileIMCFactorOverride" };
		String ExcludedHostile{ "ExcludedHostile" };
		
		// [Ally]
		Boolean EnableAlly{ "EnableAlly" };
		Double AllySCRFactor{ "AllySCRFactor" };
		Double AllySCRCombatOverride{ "AllySCRCombatOverride" };
		Double AllyIMCFactorOverride{ "AllyIMCFactorOverride" };
		String ExcludedAlly{ "ExcludedAlly" };

		// [Follower]
		Boolean EnableFollower{ "EnableFollower" };
		Double FollowerSCRFactor{ "FollowerSCRFactor" };
		Double FollowerSCRCombatOverride{ "FollowerSCRCombatOverride" };
		Double FollowerIMCFactorOverride{ "FollowerIMCFactorOverride" };
		String ExcludedFollower{ "ExcludedFollower" };

		// [TESGlobal]
		String SCRFactor_Global{ "SCRFactor_Global" };
		String IMCFactor_Global{ "IMCFactor_Global" };
		RE::TESGlobal* TES_SCRFactor{ nullptr };
		RE::TESGlobal* TES_IMCFactor{ nullptr };

		// [MasterSpell]
		Boolean EnableMasterSpell{ "EnableMasterSpell" };
		Double MasterSpellSCRFactor{ "MasterSpellSCRFactor" };
		Double MasterSpellIMCFactor{ "MasterSpellIMCFactor" };

		// [Exclusion]
		String ExcludedSpell{ "ExcludedSpell" };
		String EnabledSpell{ "EnabledSpell" };

		// [Exclusion.Player]
		String PlayerExcludedSpell{ "PlayerExcludedSpell" };
		String PlayerEnabledSpell{ "PlayerEnabledSpell" };

		// [Exclusion.Hostile]
		String HostileExcludedSpell{ "HostileExcludedSpell" };
		String HostileEnabledSpell{ "HostileEnabledSpell" };

		// [Exclusion.Ally]
		String AllyExcludedSpell{ "AllyExcludedSpell" };
		String AllyEnabledSpell{ "AllyEnabledSpell" };

		// [Exclusion.Follower]
		String FollowerExcludedSpell{ "FollowerExcludedSpell" };
		String FollowerEnabledSpell{ "FollowerEnabledSpell" };
		

		std::unordered_set<RE::FormID> BaseExclusion;
		std::unordered_set<RE::FormID> PlayerExclusion;
		std::unordered_set<RE::FormID> HostileExclusion;
		std::unordered_set<RE::FormID> AllyExclusion;
		std::unordered_set<RE::FormID> FollowerExclusion;


		void Load() noexcept;

	private:

		TomlConfig config = COMPILE_PROXY("SpeedCastingRemake.toml"sv);
	};


	class MasterSpell : 
		public DKUtil::model::Singleton<MasterSpell>
	{
	public:

		void Load() noexcept;

	private:

	};
}