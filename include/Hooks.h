#pragma once


namespace RE
{
	class SpellItem;
}


namespace Hooks
{
	float Hook_RecalculateChargeTime(RE::SpellItem* a_spellItem);
	
	bool InstallHooks();
}
