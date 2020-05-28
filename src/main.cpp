#include "version.h"

#include "Hooks.h"

#include "Settings.h"

#include "SKSE/API.h"
#include "RE/Skyrim.h"


namespace
{
	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			auto* data = RE::TESDataHandler::GetSingleton();

			auto& spellforms = data->GetFormArray(RE::FormType::Spell);

			for (auto& form : spellforms) {
				if (!form) {
					continue;
				}

				auto* spell = static_cast<RE::SpellItem*>(form);

				if ((spell->data.flags & RE::SpellItem::SpellFlag::kCostOverride) == RE::SpellItem::SpellFlag::kNone) {
					spell->data.flags = spell->data.flags ^ static_cast<RE::SpellItem::SpellFlag>(1 << 0);
				}

				if (!*Settings::useHook) {
					spell->data.chargeTime *= *Settings::factor;
				}
			}

			if (*Settings::useHook && *Settings::useGlobal) {
				_MESSAGE("Global usage enabled");

				auto& forms = data->GetFormArray(RE::FormType::Global);

				for (auto& form : forms) {
					if (!form) {
						continue;
					}

					auto* global = static_cast<RE::TESGlobal*>(form);

					if (global && global->formEditorID.c_str() == *Settings::globalName) {
						Settings::global = global;

						_MESSAGE("Global found, initial value: %f", global->value);
						break;
					}
				}

				if (!Settings::global) {
					_MESSAGE("Failed to find global");
				}
			}
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

	_MESSAGE("SpeedCastingRemake v%s", SPCR_VERSION_VERSTRING);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "SpeedCastingRemake";
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

	if (*Settings::useHook) {
		_MESSAGE("Using hook");
		
		if (!SKSE::AllocTrampoline(21)) {
			_FATALERROR("Failed to allocate trampoline");
			return false;
		}

		if (Hooks::InstallHooks()) {
			_MESSAGE("Hooks installed successfully");
		} else {
			_FATALERROR("Failed to install hooks!\n");
			return false;
		}
	} else {
		_MESSAGE("Using postload scan");
	}
	
	return true;
}
};
