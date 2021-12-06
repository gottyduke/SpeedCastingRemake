#include "Hooks.h"
#include "Config.h"


namespace Hooks
{
#if ANNIVERSARY_EDITION

	constexpr std::uint64_t FUNC_ID = 0;
	constexpr std::ptrdiff_t OFFSET_START = 0;
	constexpr std::ptrdiff_t OFFSET_END = 0;

	// post patch:
	// movss xmm6, xmm0
	constexpr DKUtil::Hook::BranchInstruction INSTRUCTION = {
		nullptr,
		0,
		nullptr,
		0,
		true
	};

#else

	constexpr std::uint64_t FUNC_ID = 11358;
	constexpr std::ptrdiff_t OFFSET_START = 0x11;
	constexpr std::ptrdiff_t OFFSET_END = 0x19;

	// post patch:
	// movss xmm6, xmm0
	constexpr DKUtil::Hook::BranchInstruction INSTRUCTION = {
		nullptr,
		0,
		"\xF3\x0F\x10\xF0",
		4,
		true
	};

#endif


	// rcx == rdx == r8 == r9 == RE::SpellItem*
	// but I only need one
	float __cdecl Hook_RecalculateChargeTime(RE::SpellItem* a_spellItem)
	{
		if (!a_spellItem) {
			return 0.0f;
		}
		
		return a_spellItem->data.chargeTime * (*Config::EnableGlobalUsage && Config::Global
											   ? Config::Global->value
											   : static_cast<float>(*Config::SpellCastingFactor));
	}


	void Install()
	{
		
		DKUtil::Hook::BranchToID<FUNC_ID, OFFSET_START, OFFSET_END>(
			&Hook_RecalculateChargeTime,
			INSTRUCTION
		);
		
		INFO("Hooks installed"sv);
	}
}
