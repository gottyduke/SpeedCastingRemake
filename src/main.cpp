#include "Hooks.h"
#include "Config.h"


namespace
{
	auto* config = Config::Main::GetSingleton();


	void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
	{
		if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			auto* data = RE::TESDataHandler::GetSingleton();

			auto& spells = data->GetFormArray(RE::FormType::Spell);
			auto& npcs = data->GetFormArray(RE::FormType::NPC);

			for (auto*& form : spells) {
				if (auto* spell = static_cast<RE::SpellItem*>(form); spell) {
					spell->data.flags.set(RE::SpellItem::SpellFlag::kCostOverride);

					// TODO
				}
			}

			if (*config->EnableTESGlobalControl) {
				INFO("TESGlobal variable control enabled"sv);

				auto& forms = data->GetFormArray(RE::FormType::Global);

				for (auto& form : forms) {
					if (!form) {
						continue;
					}

					if (auto* global = static_cast<RE::TESGlobal*>(form); global) {
						if (global->formEditorID == *config->SCRFactor_Global) {
							config->TES_SCRFactor = global;

							INFO("SCRFactor_Global found, initial value: {}", global->value);
							break;
						} else if (global->formEditorID == *config->IMCFactor_Global) {
							config->TES_IMCFactor = global;

							INFO("IMCFactor_Global found, initial value: {}", global->value);
							break;
						}
					}
				}

				if (!config->TES_SCRFactor && !config->TES_IMCFactor) {
					INFO("TESGlobal variable control enabled but failed to find any matching global! Disabled and will use configuration file instead"sv);
					*config->EnableTESGlobalControl = false;
				}
			}
		}
	}
}


DLLEXPORT constinit auto SKSEPlugin_Version = []() noexcept
{
	SKSE::PluginVersionData data{};

	data.PluginVersion(Plugin::Version);
	data.PluginName(Plugin::NAME);
	data.AuthorName(Plugin::AUTHOR);
	data.UsesAddressLibrary(true);

	return data;
}();


DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* pluginInfo)
{
    pluginInfo->name = SKSEPlugin_Version.pluginName;
    pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
    pluginInfo->version = SKSEPlugin_Version.pluginVersion;

    return true;
}


DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{

	DKUtil::Logger::Init(Plugin::NAME, REL::Module::get().version().string());

	REL::Module::reset();
	SKSE::Init(a_skse);
	
	INFO("{} v{} loaded", Plugin::NAME, Plugin::Version);

	// do stuff
	Config::Main::GetSingleton()->Load();

	const auto* const messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener(MessageHandler)) {
		return false;
	}

	Hooks::Install();

	return true;
}
