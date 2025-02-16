#include "precompiled.h"

IRehldsApi *g_RehldsApi;
const RehldsFuncs_t *g_RehldsFuncs;
IRehldsHookchains *g_RehldsHookchains;
IRehldsServerStatic *g_RehldsSvs;
IRehldsServerData *g_RehldsSv;

bool Revoice_RehldsApi_TryInit(CSysModule *engineModule, char *failureReason)
{
	if (!engineModule) {
		LCPrintf(true, "Failed to locate engine module\n");
		return false;
	}

	CreateInterfaceFn ifaceFactory = Sys_GetFactory(engineModule);

	if (!ifaceFactory) {
		sprintf(failureReason, "Failed to locate interface factory in engine module\n");
		return false;
	}

	int retCode = 0;
	g_RehldsApi = (IRehldsApi*)ifaceFactory(VREHLDS_HLDS_API_VERSION, &retCode);

	if (!g_RehldsApi) {
		sprintf(failureReason, "Failed to locate retrieve rehlds api interface from engine module, return code is %d\n", retCode);
		return false;
	}

	int majorVersion = g_RehldsApi->GetMajorVersion();
	int minorVersion = g_RehldsApi->GetMinorVersion();

	if (majorVersion != REHLDS_API_VERSION_MAJOR) {
		sprintf(failureReason, "REHLDS Api major version mismatch; expected %d, real %d\n", REHLDS_API_VERSION_MAJOR, majorVersion);
		return false;
	}

	if (minorVersion < REHLDS_API_VERSION_MINOR) {
		sprintf(failureReason, "REHLDS Api minor version mismatch; expected at least %d, real %d\n", REHLDS_API_VERSION_MINOR, minorVersion);
		return false;
	}

	g_RehldsFuncs = g_RehldsApi->GetFuncs();
	g_RehldsHookchains = g_RehldsApi->GetHookchains();
	g_RehldsSvs = g_RehldsApi->GetServerStatic();
	g_RehldsSv = g_RehldsApi->GetServerData();

	return true;
}

bool Revoice_RehldsApi_Init() {
	char failReason[2048];

#ifdef WIN32
	CSysModule *engineModule = Sys_LoadModule("swds.dll");
	if (!Revoice_RehldsApi_TryInit(engineModule, failReason)) {
		LCPrintf(true, "%s", failReason);
		return false;
	}
#else
	CSysModule *engineModule = Sys_LoadModule("engine_i486.so");
	if (!Revoice_RehldsApi_TryInit(engineModule, failReason)) {
		LCPrintf(true, "%s", failReason);
		return false;
	}
#endif

	return true;
}
