#pragma once


namespace RE
{
	class SpellItem;
}


namespace Hooks
{
	void Hook_RecalculateChargeTime(RE::SpellItem* a_spellItem);
	
	bool InstallHooks();
}
