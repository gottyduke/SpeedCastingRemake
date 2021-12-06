#pragma once


namespace Config
{
	using namespace DKUtil::Alias;

	extern Double SpellCastingFactor;
	extern Boolean EnableHookUsage;
	extern Boolean EnableHostileNPC;
	extern Boolean EnableAlliedNPC;
	extern Boolean EnableFollower;
	extern Boolean EnableGlobalUsage;
	extern String GlobalName;
	extern Boolean EnableMasterSpellSeparation;
	extern Double MasterSpellCastingFactor;
	extern Integer ExcludedSpells;
	extern RE::TESGlobal* Global;


	void Load();
}