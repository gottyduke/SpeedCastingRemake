#pragma once


namespace Hooks
{
	float __cdecl Hook_RecalculateChargeTime(RE::SpellItem* a_spellItem);
	
	void Install();
}
