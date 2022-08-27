#include "Hooks.h"
#include "Config.h"


namespace Hooks
{
	auto* config = Config::Main::GetSingleton();


	// TODO...
	constexpr std::uint64_t FuncIDAE = 11496;
	constexpr std::ptrdiff_t OffsetLowAE = 0x00;
	constexpr std::ptrdiff_t OffsetHighAE = 0x00;

	constexpr DKUtil::Hook::Patch EpilogAE = {
		"",
		1
	};


	constexpr std::uint64_t FuncIDSE = 11358;
	constexpr std::ptrdiff_t OffsetLowSE = 0x11;
	constexpr std::ptrdiff_t OffsetHighSE = 0x19;

	// Epilog to unwind the stack because of MSVC 14.3 ... why did you touch my stack?!
	constexpr DKUtil::Hook::Patch EpilogSE = {
		// movups xmm6, xmm0
		"\x0F\x10\xF0"
		// mov rax, qword ptr [rbx]
		"\x48\x8B\x03"
		// mov rcx, rbx
		"\x48\x89\xD9",
		9
	};

	
	// rcx -> RE::SpellItem*
	float __cdecl Hook_RecalculateChargeTime(RE::SpellItem* a_spellItem)
	{
		if (!a_spellItem) {
			return 0.0f;
		}

		// TODO
		return 0.0f;
	}


	HookHandle _Hook_RCT;


	void Install()
	{
		static std::once_flag HookInit;
		std::call_once(HookInit, [&]()
			{
			PAGE_ALLOC(1 << 6);

			_Hook_RCT = DKUtil::Hook::AddCaveHook(
				DKUtil::Hook::IDToAbs(FuncIDAE, FuncIDSE), 
				DKUtil::Hook::RuntimeOffset(OffsetLowAE, OffsetHighAE, OffsetLowSE, OffsetHighSE), 
				FUNC_INFO(Hook_RecalculateChargeTime), 
				nullptr, DKUtil::Hook::RuntimePatch(&EpilogAE, &EpilogSE));
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
