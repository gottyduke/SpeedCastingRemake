#pragma once


namespace Hooks
{
	using namespace DKUtil::Alias;

	
	float __cdecl Hook_RecalculateChargeTime(RE::SpellItem* a_spellItem);
	extern HookHandle _Hook_RCT;


	void Install();
	void Uninstall();
}
