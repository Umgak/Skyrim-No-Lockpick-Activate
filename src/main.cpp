#include "common/IDebugLog.h"  // IDebugLog
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION
#include "skse64/PluginAPI.h"  // SKSEInterface, PluginInfo

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "version.h"  // VERSION_VERSTRING, VERSION_MAJOR
#include "skse64_common/SafeWrite.h" // SafeWriteBuf
#include "skse64_common/Relocation.h" // RelocPtr

RelocPtr <uintptr_t> ContainerWithKeyAddr(0x22B834);
RelocPtr <uintptr_t> LockpickActivateAddr(0x897FCE);
extern "C" {
	bool SKSEPlugin_Query(const SKSEInterface* skse, PluginInfo* info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\No Lockpick Activate.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("No Lockpick Activate v%s", NLA_VERSION_VERSTRING);

		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "No Lockpick Activate";
		info->version = NLA_VERSION_MAJOR;
		
		if (skse->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!\n");
			return false;
		} 
		
		switch (skse->runtimeVersion) {
		case RUNTIME_VERSION_1_5_73:
		case RUNTIME_VERSION_1_5_80:
			break;
		default:
			_MESSAGE("This plugin is not compatible with this version of game.");
			return false;
		}

		return true;
	}


	bool SKSEPlugin_Load(const SKSEInterface* skse)
	{
		SafeWriteBuf(ContainerWithKeyAddr.GetUIntPtr(), (void*)"\x90\x90\x90\x90\x90", 5); // container with key
		SafeWriteBuf(LockpickActivateAddr.GetUIntPtr(), (void*)"\x90\x90\x90\x90\x90", 5); // lockpick activate

		_MESSAGE("[MESSAGE] No Lockpick Activate loaded");

		return true;
	}

}