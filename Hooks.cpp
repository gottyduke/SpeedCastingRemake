#include "Hooks.h"

#include "Settings.h"

#include "DKUtil/Hook.h"
#include "RE/SpellItem.h"


namespace Hooks
{
	namespace
	{
		constexpr std::uint64_t FUNC_ID = 11358;
		constexpr std::uintptr_t OFFSET_START = 0x21;
		constexpr std::uintptr_t OFFSET_END = 0x26;

		// post patch:
		// movss xmm6, ptr [ rcx + 0xCC ]
		constexpr BranchInstruction FUNC_INSTRUCTION = {
			nullptr,
			0,
			"\xF3\x0F\x10\xB1\xCC\x00\x00\x00",
			8,
			true
		};
	}


	void __fastcall Hook_RecalculateChargeTime(RE::SpellItem* a_spellItem)
	{
		if (*Settings::useGlobal && Settings::global) {
			a_spellItem->data.chargeTime *= Settings::global->value;
		} else {
			a_spellItem->data.chargeTime *= *Settings::factor;
		}
	}
	
	
	bool InstallHooks()
	{
		auto success = true;

		/**
		success &= DKUtil::Hook::BranchToFunction<FUNC_ID, OFFSET_START, OFFSET_END>(
			&Hook_RecalculateChargeTime,
			FUNC_INSTRUCTION
		);
		/**/


		return success;
	}
}
