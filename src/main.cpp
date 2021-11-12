#include "common/IDebugLog.h"  // IDebugLog
#include "skse64_common/skse_version.h"  // RUNTIME_VERSION
#include "skse64/PluginAPI.h"  // SKSEInterface, PluginInfo

#include <ShlObj.h>  // CSIDL_MYDOCUMENTS

#include "version.h"  // VERSION_VERSTRING, VERSION_MAJOR
#include "skse64_common/SafeWrite.h" // SafeWriteBuf
#include "skse64_common/Relocation.h" // RelocPtr

#include "versiondb.h" //meh's magical versioning header

bool bIsAE = 0;

unsigned long long ContainerWithKeyFunc = 0;
unsigned long long LockpickActivateFunc = 0;

unsigned long long ContainerWithKeyOffset = 0;
unsigned long long LockpickActivateOffset = 0;

const unsigned long long ContainerWithKeyOffsetSE = 0x164;
const unsigned long long LockpickActivateOffsetSE = 0x1BE;

const unsigned long long ContainerWithKeyFuncAE = 0x23C1F0;
const unsigned long long LockpickActivateFuncAE = 0x8C8730;
const unsigned long long ContainerWithKeyOffsetAE = 0x2E4;
const unsigned long long LockpickActivateOffsetAE = 0x1C0;


bool InitializeOffsets()
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
	if (!db.FindOffsetById(17485, ContainerWithKeyFunc))
	{
		_FATALERROR("[FATAL ERROR] Failed to find offset for activating container with key!");
		return false;
	}
	if (!db.FindOffsetById(51088, LockpickActivateFunc))
	{
		_FATALERROR("[FATAL ERROR] Failed to find offset for activating container with lockpick!");
		return false;
	}

	// Everything was successful.
	db.Clear();
	return true;
}

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
			_FATALERROR("[FATAL ERROR] Loaded in editor, marking as incompatible!");
			return false;
		} 

		if (skse->runtimeVersion >= RUNTIME_VERSION_1_6_317)
		{
			bIsAE = 1;
		}
		return true;
	}


	bool SKSEPlugin_Load(const SKSEInterface* skse)
	{
		if (!bIsAE)
		{
			ContainerWithKeyOffset = ContainerWithKeyOffsetSE;
			LockpickActivateOffset = LockpickActivateOffsetSE;

			if (!InitializeOffsets())
			{
				_FATALERROR("[FATAL ERROR] Unable to find offsets for this version of the game!");
				_FATALERROR("[FATAL ERROR] Make sure you install meh321's Address Library for SKSE Plugins!");
				_FATALERROR("[FATAL ERROR] These can be found at: https://www.nexusmods.com/skyrimspecialedition/mods/32444");
				return false;
			}
		}
		else
		{
			ContainerWithKeyFunc = ContainerWithKeyFuncAE;
			ContainerWithKeyOffset = ContainerWithKeyOffsetAE;

			LockpickActivateFunc = LockpickActivateFuncAE;
			LockpickActivateOffset = LockpickActivateOffsetAE;

		}
		RelocPtr <uintptr_t> ContainerWithKeyAddr(ContainerWithKeyFunc + ContainerWithKeyOffset);
		RelocPtr <uintptr_t> LockpickActivateAddr(LockpickActivateFunc + LockpickActivateOffset);
		SafeWriteBuf(ContainerWithKeyAddr.GetUIntPtr(), (void*)"\x90\x90\x90\x90\x90", 5); // container with key
		SafeWriteBuf(LockpickActivateAddr.GetUIntPtr(), (void*)"\x90\x90\x90\x90\x90", 5); // lockpick activate

		_MESSAGE("[MESSAGE] No Lockpick Activate loaded successfully.");

		return true;
	}
}

