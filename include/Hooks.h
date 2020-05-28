#pragma once


namespace RE
{
	class SpellItem;
}


namespace Hooks
{
	float __fastcall Hook_RecalculateChargeTime(RE::SpellItem* a_spellItem);
	
	bool InstallHooks();
}
