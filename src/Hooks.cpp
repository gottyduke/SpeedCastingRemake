#include "Hooks.h"
#include "Config.h"


namespace Hooks
{
#if ANNIVERSARY_EDITION

	constexpr std::uint64_t FUNC_ID = 0;
	constexpr std::ptrdiff_t OFFSET_START = 0;
	constexpr std::ptrdiff_t OFFSET_END = 0;

	constexpr DKUtil::Hook::Patch Prolog = {
		nullptr,
		0
	};

	constexpr DKUtil::Hook::Patch Epilog = {
		nullptr,
		0
	};

#else
	
	constexpr std::uint64_t FuncID = 11358;
	constexpr std::ptrdiff_t OffsetLow = 0x11;
	constexpr std::ptrdiff_t OffsetHigh = 0x19;

	// Epilog to unwind the stack because of MSVC 14.3 ... why did you touch my stack?!
	constexpr DKUtil::Hook::Patch Epilog = {
		"\x0F\x10\xF0\x48\x8B\x03\x48\x89\xD9",
		9
	};
	
#endif

	
	// rcx -> RE::SpellItem*
	float __cdecl Hook_RecalculateChargeTime(RE::SpellItem* a_spellItem)
	{
		if (!a_spellItem) {
			return 0.0f;
		}
		
		return a_spellItem->data.chargeTime * (*Config::EnableGlobalUsage && Config::Global
											   ? Config::Global->value
											   : static_cast<float>(*Config::SpellCastingFactor));
	}


	HookHandle _Hook_RCT;


	void Install()
	{
		static std::once_flag HookInit;
		std::call_once(HookInit, [&]()
			{
				const auto address = DKUtil::Hook::RVA2Abs(FuncID);
				_Hook_RCT = DKUtil::Hook::AddCaveHook<OffsetLow, OffsetHigh>(address, FUNC_INFO(Hook_RecalculateChargeTime), nullptr, &Epilog);
			}
		);

		_Hook_RCT->Enable();

		INFO("Hooks installed"sv);
	}

	
	void Uninstall()
	{
		_Hook_RCT->Disable();

		INFO("Hooks uninstalled"sv);
	}
}
