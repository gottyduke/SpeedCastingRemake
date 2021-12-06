#include "Hooks.h"
#include "Config.h"


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

				if (!*Config::EnableHookUsage) {
					spell->data.chargeTime *= static_cast<float>(*Config::SpellCastingFactor);
				}
			}

			if (*Config::EnableHookUsage && *Config::EnableGlobalUsage) {
				INFO("Global usage enabled"sv);

				auto& forms = data->GetFormArray(RE::FormType::Global);

				for (auto& form : forms) {
					if (!form) {
						continue;
					}

					auto* global = static_cast<RE::TESGlobal*>(form);

					if (global && global->formEditorID == *Config::GlobalName) {
						Config::Global = global;

						INFO("Global found, initial value: {}", global->value);
						break;
					}
				}

				if (!Config::Global) {
					INFO("Failed to find global"sv);
				}
			}
		}
	}
}


#if ANNIVERSARY_EDITION

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
{
	SKSE::PluginVersionData data{};

	data.PluginVersion(Version::MAJOR);
	data.PluginName(Version::NAME);
	data.AuthorName("Dropkicker"sv);

	data.CompatibleVersions({ SKSE::RUNTIME_LATEST });
	data.UsesAddressLibrary(true);

	return data;
}();

#else

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	DKUtil::Logger::Init(Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		ERROR("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		ERROR("Unable to load this plugin, incompatible runtime version!\nExpected: Newer than 1-5-39-0 (A.K.A Special Edition)\nDetected: {}", ver.string());
		return false;
	}

	return true;
}

#endif


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#if ANNIVERSARY_EDITION

	DKUtil::Logger::Init(Version::PROJECT, Version::NAME);

	if (REL::Module::get().version() < SKSE::RUNTIME_1_6_317) {
		ERROR("Unable to load this plugin, incompatible runtime version!\nExpected: Newer than 1-6-317-0 (A.K.A Anniversary Edition)\nDetected: {}", REL::Module::get().version().string());
		return false;
	}

#endif

	INFO("{} v{} loaded", Version::PROJECT, Version::NAME);

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(1 << 6);

	Config::Load();

	const auto* const messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener(MessageHandler)) {
		return false;
	}

	Hooks::Install();

	return true;
}
