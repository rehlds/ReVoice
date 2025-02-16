#pragma once

#include "revoice_shared.h"

extern IRehldsApi* g_RehldsApi;
extern const RehldsFuncs_t* g_RehldsFuncs;
extern IRehldsHookchains* g_RehldsHookchains;
extern IRehldsServerStatic* g_RehldsSvs;
extern IRehldsServerData* g_RehldsSv;

extern bool Revoice_RehldsApi_Init();
