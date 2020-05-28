#include "Hooks.h"

#include "Settings.h"

// reduced trampoline load from 23B to 21B
#define DKUTIL_HOOK_SMART_ALLOC
//#define DKUTIL_HOOK_VERBOSE
#include "DKUtil/Hook.hpp"

#include "RE/SpellItem.h"


namespace Hooks
{
	namespace
	{
		constexpr std::uint64_t FUNC_ID = 11358;
		constexpr std::uintptr_t OFFSET_START = 0x11;
		constexpr std::uintptr_t OFFSET_END = 0x19;

		// post patch:
		// movss xmm6, xmm0
		constexpr BranchInstruction FUNC_INSTRUCTION = {
			nullptr,
			0,
			"\xF3\x0F\x10\xF0",
			4,
			true
		};
	}

	
	/*
	 * maybe I could just alter the chargeTime directly
	 * but to keep it decoupled, I guess it should be left untouched
	 * only use this temporary value as new chargeTime
	 */
	
	// rcx == rdx == r8 == r9 == RE::SpellItem*
	// but I only need one
	float __fastcall Hook_RecalculateChargeTime(RE::SpellItem* a_spellItem)
	{
		if (!a_spellItem) {
			return 0.0f;
		}
		
		return a_spellItem->data.chargeTime * (*Settings::useGlobal && Settings::global 
											   ? Settings::global->value
											   : *Settings::factor);
	}


	bool InstallHooks()
	{
		auto success = true;
		const auto funcAddr = std::uintptr_t(&Hook_RecalculateChargeTime);
		
		success &= DKUtil::Hook::BranchToFunction<FUNC_ID, OFFSET_START, OFFSET_END>(
			funcAddr,
			FUNC_INSTRUCTION
		);

		return success;
	}
}
