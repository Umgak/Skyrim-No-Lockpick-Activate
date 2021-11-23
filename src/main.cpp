#include "common/IDebugLog.h"  // IDebugLog
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION
#include "skse64/PluginAPI.h"  // SKSEInterface, PluginInfo

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "version.h"  // VERSION_VERSTRING, VERSION_MAJOR
#include "skse64_common/SafeWrite.h" // SafeWriteBuf
#include "skse64_common/Relocation.h" // RelocPtr

#include "versiondb.h" //meh's magical versioning header

unsigned long long ContainerWithKeyFunc = 0;
unsigned long long LockpickActivateFunc = 0;

unsigned long long ContainerWithKeyOffset = 0;
unsigned long long LockpickActivateOffset = 0;

const unsigned long long ContainerWithKeyOffsetSE = 0x164;
const unsigned long long LockpickActivateOffsetSE = 0x1BE;

const unsigned long long ContainerWithKeyOffsetAE = 0x2E4;
const unsigned long long LockpickActivateOffsetAE = 0x1C0;


bool InitializeOffsets(const SKSEInterface* skse)
{
	// Allocate on stack so it will be unloaded when we exit this function.
	// No need to have the whole database loaded and using up memory for no reason.
	VersionDb db;

	// Load database with current executable version.
	if (!db.Load())
	{
		_FATALERROR("[FATAL ERROR] Failed to load version database for current executable!");
		return false;
	}
	else
	{
		_MESSAGE("[MESSAGE] Loaded database for %s version %s.", db.GetModuleName().c_str(), db.GetLoadedVersionString().c_str());
	}

	// This offset does not include base address. Actual address would be ModuleBase + MyOffset.
	if (skse->runtimeVersion >= RUNTIME_VERSION_1_6_317)
	{
		if (!db.FindOffsetById(17922, ContainerWithKeyFunc)) //ae offsets
		{
			_FATALERROR("[FATAL ERROR] Failed to find offset for activating container with key!");
			db.Clear();
			return false;
		}
		if (!db.FindOffsetById(51968, LockpickActivateFunc))
		{
			_FATALERROR("[FATAL ERROR] Failed to find offset for activating container with lockpick!");
			db.Clear();
			return false;
		}
	}
	else
	{
		if (!db.FindOffsetById(17485, ContainerWithKeyFunc)) //se offsets
		{
			_FATALERROR("[FATAL ERROR] Failed to find offset for activating container with key!");
			db.Clear();
			return false;
		}
		if (!db.FindOffsetById(51088, LockpickActivateFunc))
		{
			_FATALERROR("[FATAL ERROR] Failed to find offset for activating container with lockpick!");
			db.Clear();
			return false;
		}
	}

	// Everything was successful.
	db.Clear();
	return true;
}

extern "C" {
	__declspec(dllexport) SKSEPluginVersionData SKSEPlugin_Version =
	{
		SKSEPluginVersionData::kVersion,
		NLA_VERSION_MAJOR,
		"No Lockpick Activate",
		"Umgak",
		"",
		SKSEPluginVersionData::kVersionIndependent_AddressLibraryPostAE,
		0,
		0,
	};

	bool SKSEPlugin_Query(const SKSEInterface* skse, PluginInfo* info)
	//SKSEPlugin_Query is used on pre-AE versions but is not actually called on AE SKSE, so this code can remain unchanged from 1.5.97's implementation.
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\No Lockpick Activate.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("No Lockpick Activate v%s", NLA_VERSION_VERSTRING);

		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "No Lockpick Activate";
		info->version = NLA_VERSION_MAJOR;
		
		if (skse->isEditor) {
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!");
			return false;
		} 

		return true;
	}


	bool SKSEPlugin_Load(const SKSEInterface* skse)
	{
		if (skse->runtimeVersion >= RUNTIME_VERSION_1_6_317) //1_6_317 was the first AE version
		{
			//logging is not set up in SKSEPlugin_Query on AE, so we set it up here
			gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\No Lockpick Activate.log");
			gLog.SetPrintLevel(IDebugLog::kLevel_DebugMessage);
			gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
			_MESSAGE("No Lockpick Activate v%s", NLA_VERSION_VERSTRING);

			ContainerWithKeyOffset = ContainerWithKeyOffsetAE;
			LockpickActivateOffset = LockpickActivateOffsetAE;
		}
		else
		{
			ContainerWithKeyOffset = ContainerWithKeyOffsetSE;
			LockpickActivateOffset = LockpickActivateOffsetSE;
		}
		if (!InitializeOffsets(skse))
		{
				_FATALERROR("[FATAL ERROR] Unable to find offsets for this version of the game!");
				_FATALERROR("[FATAL ERROR] Make sure you install meh321's Address Library for SKSE Plugins!");
				_FATALERROR("[FATAL ERROR] These can be found at: https://www.nexusmods.com/skyrimspecialedition/mods/32444");
				return false;
		}

		
		RelocPtr <uintptr_t> ContainerWithKeyAddr(ContainerWithKeyFunc + ContainerWithKeyOffset);
		RelocPtr <uintptr_t> LockpickActivateAddr(LockpickActivateFunc + LockpickActivateOffset);
		SafeWriteBuf(ContainerWithKeyAddr.GetUIntPtr(), (void*)"\x90\x90\x90\x90\x90", 5); // container with key
		SafeWriteBuf(LockpickActivateAddr.GetUIntPtr(), (void*)"\x90\x90\x90\x90\x90", 5); // lockpick activate

		_MESSAGE("[MESSAGE] No Lockpick Activate loaded successfully.");

		return true;
	}
}

