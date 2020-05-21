#include "version.h"

#define USE_HOOK
#ifdef USE_HOOK
#include "Hooks.h"
#endif

#include "Settings.h"

#include "SKSE/API.h"
#include "RE/Skyrim.h"

namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		switch (a_msg->type) {
		case SKSE::MessagingInterface::kDataLoaded:
			{
				auto* data = RE::TESDataHandler::GetSingleton();
				
				if (*Settings::useGlobal) {
					auto& globals = data->GetFormArray(RE::FormType::Global);

					for (auto global : globals) {
						if (global && global->GetName() == *Settings::globalName) {
							Settings::global = static_cast<RE::TESGlobal*>(global);
						}
					}
				}

#ifndef USE_HOOK
				auto& spells = data->GetFormArray(RE::FormType::Spell);

				for (auto spell : spells) {
					auto* spellRef = static_cast<RE::SpellItem*>(spell);
					spellRef->data.chargeTime *= *Settings::factor;
				}

				_MESSAGE("Finished modifying");
#endif
				
				break;
			}
		default:;
		}
	}
}


extern "C"
{
	bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		SKSE::Logger::OpenRelative(FOLDERID_Documents, L"\\My Games\\Skyrim Special Edition\\SKSE\\SpeedCastingRemake.log");
		SKSE::Logger::UseLogStamp(true);
		SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::TrackTrampolineStats(true);

		_MESSAGE("SPCR v%s", SPCR_VERSION_VERSTRING);

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "SPCR";
		a_info->version = SPCR_VERSION_MAJOR;

		if (a_skse->IsEditor()) {
			_FATALERROR("Loaded in editor, marking as incompatible!\n");
			return false;
		}

		const auto ver = a_skse->RuntimeVersion();
		if (ver <= SKSE::RUNTIME_1_5_39) {
			_FATALERROR("Unsupported runtime version %s!", ver.GetString().c_str());
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
	{
		_MESSAGE("SpeedCastingRemake loaded");

		if (!Init(a_skse)) {
			return false;
		}

		if (Settings::LoadSettings()) {
			_MESSAGE("Settings loaded successfully");
		} else {
			_FATALERROR("Failed to load json settings!\n");
			return false;
		}

		const auto* const messaging = SKSE::GetMessagingInterface();
		if (messaging->RegisterListener("SKSE", MessageHandler)) {
			_MESSAGE("Messaging interface registration successful");
		} else {
			_FATALERROR("Messaging interface registration failed!\n");
			return false;
		}

#ifdef USE_HOOK
		if (!SKSE::AllocTrampoline(1 << 6)) {
			_FATALERROR("Failed to allocate trampoline");
			return false;
		}

		if (Hooks::InstallHooks()) {
			_MESSAGE("Hooks installed successfully");
		} else {
			_FATALERROR("Failed to install hooks!\n");
			return false;
		}
#endif
		
		return true;
	}
};
