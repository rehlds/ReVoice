#include "precompiled.h"

void SV_DropClient_hook(IRehldsHook_SV_DropClient *chain, IGameClient *cl, bool crash, const char *msg)
{
	CRevoicePlayer *plr = GetPlayerByClientPtr(cl);

	plr->OnDisconnected();

	chain->callNext(cl, crash, msg);
}

void CvarValue2_PreHook(const edict_t *pEnt, int requestID, const char *cvarName, const char *cvarValue)
{
	CRevoicePlayer *plr = GetPlayerByEdict(pEnt);
	if (plr->GetRequestId() != requestID) {
		RETURN_META(MRES_IGNORED);
	}

	const char *lastSeparator = strrchr(cvarValue, ',');
	if (lastSeparator)
	{
		int buildNumber = atoi(lastSeparator + 1);
		if (buildNumber > 4554 || buildNumber == 2017) {
			plr->SetCodecType(vct_opus);
		}
	}

	RETURN_META(MRES_IGNORED);
}

int TranscodeVoice(CRevoicePlayer *srcPlayer, const char *srcBuf, int srcBufLen, IVoiceCodec *srcCodec, IVoiceCodec *dstCodec, char *dstBuf, int dstBufSize)
{
	char decodedBuf[32768];

	int numDecodedSamples = srcCodec->Decompress(srcBuf, srcBufLen, decodedBuf, sizeof(decodedBuf));
	if (numDecodedSamples <= 0) {
		return 0;
	}

	int compressedSize = dstCodec->Compress(decodedBuf, numDecodedSamples, dstBuf, dstBufSize, false);
	if (compressedSize <= 0) {
		return 0;
	}

	/*
	int numDecodedSamples2 = dstCodec->Decompress(dstBuf, compressedSize, decodedBuf, sizeof(decodedBuf));
	if (numDecodedSamples2 <= 0) {
		return compressedSize;
	}

	FILE *rawSndFile = fopen("d:\\revoice_raw.snd", "ab");
	if (rawSndFile) {
		fwrite(decodedBuf, 2, numDecodedSamples2, rawSndFile);
		fclose(rawSndFile);
	}
	*/

	return compressedSize;
}

void SV_ParseVoiceData_emu(IGameClient *cl)
{
	char chReceived[4096];
	unsigned int nDataLength = g_RehldsFuncs->MSG_ReadShort();

	if (nDataLength > sizeof(chReceived)) {
		g_RehldsFuncs->DropClient(cl, FALSE, "Invalid voice data\n");
		return;
	}

	g_RehldsFuncs->MSG_ReadBuf(nDataLength, chReceived);

	if (g_pcv_sv_voiceenable->value == 0.0f) {
		return;
	}

	CRevoicePlayer *srcPlayer = GetPlayerByClientPtr(cl);



	//FILE *fp = fopen("recorder.snd", "ab");
	//if (fp)
	//{
	//	printf("	-> Write chunk: (%d)\n", nDataLength);
	//	fwrite(chReceived, 1, nDataLength, fp);
	//	fclose(fp);
	//}






	/*srcPlayer->SetLastVoiceTime(g_RehldsSv->GetTime());
	srcPlayer->IncreaseVoiceRate(nDataLength);

	char compressedBuf[16384];
	int compressedSize = TranscodeVoice(srcPlayer, chReceived, nDataLength, srcPlayer->GetOpusCodec(), srcPlayer->GetSilkCodec(), compressedBuf, sizeof(compressedBuf));

	uint32_t computedChecksum = crc32(compressedBuf, compressedSize - 4);
	uint32 wireChecksum = *(uint32 *)(compressedBuf + compressedSize - 4);

	FILE *fp = fopen("recorder.snd", "ab");
	if (fp)
	{
		printf("	-> Write chunk: (%d), computedChecksum: (%u), wireChecksum: (%u)\n", compressedSize, computedChecksum, wireChecksum);
		printf("	-> Write chunk: (%d), computedChecksum: (%u), wireChecksum: (%u)\n", compressedSize, computedChecksum, wireChecksum);
		fwrite(compressedBuf, 1, compressedSize, fp);
		fclose(fp);
	}*/

	//return;






	int maxclients = g_RehldsSvs->GetMaxClients();
	for (int i = 0; i < maxclients; i++)
	{
		CRevoicePlayer *dstPlayer = &g_Players[i];
		IGameClient *dstClient = dstPlayer->GetClient();

		if (!((1 << i) & cl->GetVoiceStream(0)) && dstPlayer != srcPlayer) {
			//printf("-> #0\n");
			continue;
		}

		if (!dstClient->IsActive() && !dstClient->IsConnected() && dstPlayer != srcPlayer) {
			//printf("-> #1\n");
			continue;
		}

		sizebuf_t *dstDatagram = dstClient->GetDatagram();
		if (dstDatagram->cursize + nDataLength + 6 < dstDatagram->maxsize)
		{
			//uint32_t computedChecksum = crc32(compressedBuf, compressedSize - 4);
			//uint32 wireChecksum = *(uint32 *)(compressedBuf + compressedSize - 4);

			//FILE *fp = fopen("silk_chunk.snd", "ab");
			//if (fp)
			//{
			//	printf("	-> Write chunk: (%d), computedChecksum: (%u), wireChecksum: (%u)\n", compressedSize, computedChecksum, wireChecksum);
			//	fwrite(compressedBuf, 1, compressedSize, fp);
			//	fclose(fp);
			//}

			g_RehldsFuncs->MSG_WriteByte(dstDatagram, svc_voicedata);
			g_RehldsFuncs->MSG_WriteByte(dstDatagram, cl->GetId());
			g_RehldsFuncs->MSG_WriteShort(dstDatagram, nDataLength);
			g_RehldsFuncs->MSG_WriteBuf(dstDatagram, nDataLength, chReceived);
		}
	}
}

void Rehlds_HandleNetCommand(IRehldsHook_HandleNetCommand *chain, IGameClient *cl, int8 opcode)
{
	const int clc_voicedata = 8;
	//if (opcode == clc_voicedata) {
	//	SV_ParseVoiceData_emu(cl);
	//	return;
	//}

	chain->callNext(cl, opcode);
}

qboolean ClientConnect_PreHook(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128])
{
	CRevoicePlayer *plr = GetPlayerByEdict(pEntity);
	plr->OnConnected();

	RETURN_META_VALUE(MRES_IGNORED, TRUE);
}

struct SpeexContext_t
{
	const char *filename;
	char *data;
	int chunk;
	int frame;
	bool play;
	bool load;
	bool send_order;
	double time;
	double nextsend;
	float framerate;
	int index;
	IVoiceCodec *encoder;
};

int g_NumFullSended = 0;
SpeexContext_t g_SoundLists[] =
{
	{ "akol.wav",    nullptr, 0, 0, false, false, false, 0, 0, 8000,  4, nullptr },
	//{ "akol2.wav",   nullptr, 0, 0, false, false, false, 0, 0, 8000,  5, nullptr },
	{ "apchela.wav", nullptr, 0, 0, false, false, false, 0, 0, 11025, 6, nullptr },
};

void ClientCommand(edict_t *pEdict)
{
	const char *argv1 = CMD_ARGV(0);
	if (_stricmp(argv1, "speexmulti") == 0)
	{
		g_SoundLists[0].play ^= true;
		g_SoundLists[1].play ^= true;

		if (g_SoundLists[0].play)
		{
			g_SoundLists[0].time = gpGlobals->time;
		}

		if (g_SoundLists[1].play)
		{
			g_SoundLists[1].time = gpGlobals->time;
		}

		g_SoundLists[0].frame = 0;
		g_SoundLists[1].frame = 0;

		RETURN_META(MRES_SUPERCEDE);
	}
	else if (_stricmp(argv1, "speexplay") == 0)
	{
		if (CMD_ARGC() < 2)
		{
			CLIENT_PRINTF(pEdict, print_console, UTIL_VarArgs("\nusage: speexplay [ <tracknumber> ]\n\n"));
			CLIENT_PRINTF(pEdict, print_console, UTIL_VarArgs("%4s  %-32s %-10s %-12s\n", "#", "[name]", "[chunk]", "[framerate]"));

			int nIndex = 0;
			for (auto &list : g_SoundLists)
			{
				CLIENT_PRINTF(pEdict, print_console, UTIL_VarArgs("%4d. %-32s %-10d %-12.2f\n", nIndex++, list.filename, list.chunk, list.framerate));
			}

			CLIENT_PRINTF(pEdict, print_console, "\n");
			RETURN_META(MRES_SUPERCEDE);
		}

		int trackNumber = atoi(CMD_ARGV(1));
		if (trackNumber < 0 || trackNumber >= ARRAYSIZE(g_SoundLists))
		{
			trackNumber = 0;
		}

		g_SoundLists[trackNumber].play ^= true;

		if (g_SoundLists[trackNumber].play)
		{
			g_SoundLists[trackNumber].time = gpGlobals->time;
		}

		g_SoundLists[trackNumber].frame = 0;

		CLIENT_PRINTF(pEdict, print_console, UTIL_VarArgs("	-> send voice are %s, track: (%s)\n", g_SoundLists[trackNumber].play ? "RUNNING" : "STOPPED", g_SoundLists[trackNumber].filename));
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

bool ReadWaveFile(const char *pFilename, char **pData, int &nDataBytes, int &nBitsPerSample, int &nChannels, int &nSamplesPerSec)
{
	FILE *fp = fopen(pFilename, "rb");
	if (!fp) {
		return false;
	}

	int pOutput;
	fseek(fp, 22, SEEK_SET);
	fread(&pOutput, sizeof(uint16), 1, fp);
	nChannels = (uint16)pOutput;

	fread(&pOutput, sizeof(int), 1, fp);
	nSamplesPerSec = pOutput;

	fseek(fp, 34, SEEK_SET);
	fread(&pOutput, sizeof(uint16), 1, fp);
	nBitsPerSample = (uint16)pOutput;

	fseek(fp, 40, SEEK_SET);
	fread(&pOutput, sizeof(int), 1, fp);
	nDataBytes = pOutput;

	fread(&pOutput, sizeof(int), 1, fp);

	*pData = new char[ nDataBytes ];

	if (*pData)
	{
		fread(*pData, nDataBytes, 1, fp);
		fclose(fp);
		return true;
	}

	fclose(fp);
	return false;
}

int Voice_GetCompressedData(SpeexContext_t &snd, char *pchDest, int nCount, bool bFinal)
{
	if (!snd.encoder)
	{
		snd.encoder = new VoiceCodec_Frame(new VoiceEncoder_Speex());
		snd.encoder->Init(SPEEX_VOICE_QUALITY);
	}

	if (snd.encoder)
	{
		int gotten;
		short tempData[8192];

		// If they want to get the data from a file instead of the mic, use that.
		if (snd.data)
		{
			double curtime = gpGlobals->time;
			int nShouldGet = (curtime - snd.time) * 8000;
			gotten = min((signed)sizeof(tempData) / 2, min(nShouldGet, (snd.chunk - snd.frame) / 2));
			memcpy(tempData, &snd.data[snd.frame], gotten * 2);
			snd.frame += gotten * 2;
			snd.time = curtime;

			return snd.encoder->Compress((char *)tempData, gotten, pchDest, nCount, !!bFinal);
		}
	}

	return 0;
}

#include "counter.h"
CCounter g_Timer;

#include <iostream>
#include <chrono>

void StartFrame_Post()
{
	static bool all_sound_initializes = false;
	if (!all_sound_initializes)
	{
		g_Timer.Init();

		for (auto &list : g_SoundLists)
		{
			if (list.load)
			{
				continue;
			}

			int a, b, c;
			list.load = ReadWaveFile(list.filename, &list.data, list.chunk, a, b, c);
			list.frame = 0;
			list.time = gpGlobals->time;

			if (list.load)
			{
				printf(" Load: (%s), chunk: (%d), framerate: (%0.2f)\n", list.filename, list.chunk, list.framerate);
			}
		}
	}

	all_sound_initializes = true;
	for (auto &list : g_SoundLists)
	{
		if (!list.load)
		{
			all_sound_initializes = false;
			break;
		}
	}

	static float fltime = 0.0f;
	if (fltime > gpGlobals->time)
	{
		RETURN_META(MRES_IGNORED);
	}

	fltime = gpGlobals->time + 0.02f - 0.000125;

	for (int snd = 0; snd < ARRAYSIZE(g_SoundLists); snd++)
	{
		if (!g_SoundLists[snd].play)
			continue;

		char uchVoiceData[4096];
		bool bFinal = false;
		int nDataLength = Voice_GetCompressedData(g_SoundLists[snd], uchVoiceData, sizeof(uchVoiceData), bFinal);

		if (g_SoundLists[snd].frame >= g_SoundLists[snd].chunk)// || nDataLength <= 0)
		{
			g_SoundLists[snd].play = false;
			g_SoundLists[snd].frame = 0;

			//printf("> HIT END\n");
			continue;
		}

		int maxclients = g_RehldsSvs->GetMaxClients();
		for (int i = 0; i < maxclients; i++)
		{
			CRevoicePlayer *dstPlayer = &g_Players[i];
			IGameClient *dstClient = dstPlayer->GetClient();

			if (!dstClient->IsActive() && !dstClient->IsConnected()) {
				continue;
			}

			sizebuf_t *dstDatagram = dstClient->GetDatagram();
			if (dstDatagram->cursize + nDataLength + 6 < dstDatagram->maxsize)
			{
				//CLIENT_PRINTF(dstPlayer->GetClient()->GetEdict(), print_console, UTIL_VarArgs(" -> chunk:  %4d / %4d,   chunksize:  %4d,   progress:  %3.2f%%\n", g_SoundLists[snd].frame, g_SoundLists[snd].chunk, nDataLength, ((double)g_SoundLists[snd].frame / (double)g_SoundLists[snd].chunk) * 100.0));
				printf(" -> chunk:  %4d / %4d,   chunksize:  %4d,   progress:  %3.2f%%\n", g_SoundLists[snd].frame, g_SoundLists[snd].chunk, nDataLength, ((double)g_SoundLists[snd].frame / (double)g_SoundLists[snd].chunk) * 100.0);

				g_RehldsFuncs->MSG_WriteByte(dstDatagram, svc_voicedata);
				g_RehldsFuncs->MSG_WriteByte(dstDatagram, g_SoundLists[snd].index);
				g_RehldsFuncs->MSG_WriteShort(dstDatagram, nDataLength);
				g_RehldsFuncs->MSG_WriteBuf(dstDatagram, nDataLength, uchVoiceData);
			}
		}
	}

	RETURN_META(MRES_IGNORED);
}

void ServerActivate_PostHook(edict_t *pEdictList, int edictCount, int clientMax)
{
	Revoice_Exec_Config();
	SET_META_RESULT(MRES_IGNORED);
}

void SV_WriteVoiceCodec_hooked(IRehldsHook_SV_WriteVoiceCodec *chain, sizebuf_t *sb)
{
	IGameClient *cl = g_RehldsFuncs->GetHostClient();
	CRevoicePlayer *plr = GetPlayerByClientPtr(cl);

	switch (plr->GetCodecType())
	{
		case vct_silk:
		case vct_opus:
		case vct_speex:
		{
			g_RehldsFuncs->MSG_WriteByte(sb, svc_voiceinit);
			g_RehldsFuncs->MSG_WriteString(sb, "voice_speex");	// codec id
			g_RehldsFuncs->MSG_WriteByte(sb, 5);				// quality
			break;
		}
		default:
			LCPrintf(true, "SV_WriteVoiceCodec() called on client(%d) with unknown voice codec\n", cl->GetId());
			break;
	}
}

bool Revoice_Load()
{
	if (!Revoice_Utils_Init())
		return false;

	if (!Revoice_RehldsApi_Init()) {
		LCPrintf(true, "Failed to locate REHLDS API\n");
		return false;
	}

	if (!Revoice_ReunionApi_Init())
		return false;

	Revoice_Init_Cvars();
	Revoice_Init_Config();
	Revoice_Init_Players();

	if (!Revoice_Main_Init()) {
		LCPrintf(true, "Initialization failed\n");
		return false;
	}

	return true;
}

bool Revoice_Main_Init()
{
	g_RehldsHookchains->SV_DropClient()->registerHook(&SV_DropClient_hook, HC_PRIORITY_DEFAULT + 1);
	g_RehldsHookchains->HandleNetCommand()->registerHook(&Rehlds_HandleNetCommand, HC_PRIORITY_DEFAULT + 1);
	g_RehldsHookchains->SV_WriteVoiceCodec()->registerHook(&SV_WriteVoiceCodec_hooked, HC_PRIORITY_DEFAULT + 1);

	return true;
}

void Revoice_Main_DeInit()
{
	g_RehldsHookchains->SV_DropClient()->unregisterHook(&SV_DropClient_hook);
	g_RehldsHookchains->HandleNetCommand()->unregisterHook(&Rehlds_HandleNetCommand);
	g_RehldsHookchains->SV_WriteVoiceCodec()->unregisterHook(&SV_WriteVoiceCodec_hooked);

	Revoice_DeInit_Cvars();
}
