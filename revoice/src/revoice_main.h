#pragma once

#include "revoice_shared.h"
#include "revoice_player.h"

bool Revoice_Load();
void Revoice_Main_DeInit();
bool Revoice_Main_Init();

void CvarValue2_PreHook(const edict_t *pEnt, int requestID, const char *cvarName, const char *cvarValue);
qboolean ClientConnect_PreHook(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
void ClientPutInServer_PostHook(edict_t *pEntity);
void ServerActivate_PostHook(edict_t *pEdictList, int edictCount, int clientMax);
void StartFrame_Post();
void ClientCommand(edict_t *pEdict);
