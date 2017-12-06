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
			continue;
		}

		if (!dstClient->IsActive() && !dstClient->IsConnected() && dstPlayer != srcPlayer) {
			continue;
		}

		sizebuf_t *dstDatagram = dstClient->GetDatagram();
		if (dstDatagram->cursize + nDataLength + 6 < dstDatagram->maxsize)
		{
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

enum VoiceCodec_e
{
	VC_SPEEX = 0,
	VC_SILK,
	VC_MAX
};

struct SpeexContext_t
{
	VoiceCodec_e type;
	const char *filename;
	char *data;
	int nDataBytes;
	int wBitsPerSample;
	int frame;
	bool play;
	bool load;
	bool failed;
	bool send_order;
	double time;
	double nextsend;
	float framerate;
	int index;
	int startindex;
	uint64_t steamid;
	IVoiceCodec *encoder;
};

int g_tracks_silk = 0;
int g_tracks_speex = 0;
int g_tracksNumbers[VC_MAX][16] = {0};
uint64_t g_ulSteamIdCurrent = 0ULL;

SpeexContext_t g_SoundLists[] =
{
	{ VC_SPEEX, "kolshik.wav",        nullptr, 0, 0, 0, false, false, false, false, 0, 0, 8000,  2, 0,      76561198051972183ULL, nullptr },
	{ VC_SPEEX, "pchela.wav",         nullptr, 0, 0, 0, false, false, false, false, 0, 0, 11025, 3, 0,      76561198051972185ULL, nullptr },
	{ VC_SILK,  "c2a2_ba_launch.wav", nullptr, 0, 0, 0, false, false, false, false, 0, 0, 8000,  7, 0,      76561198051972186ULL, nullptr },
	{ VC_SILK,  "cslig_nuages.wav",   nullptr, 0, 0, 0, false, false, false, false, 0, 0, 8000,  7, 0,      76561198051972182ULL, nullptr },
	{ VC_SILK,  "kolshik.wav",        nullptr, 0, 0, 0, false, false, false, false, 0, 0, 8000,  4, 0,      76561198051972187ULL, nullptr },
	{ VC_SILK,  "dance.wav",          nullptr, 0, 0, 0, false, false, false, false, 0, 0, 8000,  5, 102832, 76561198051972188ULL, nullptr },
};

void ClientCommand(edict_t *pEdict)
{
	const char *argv1 = CMD_ARGV(0);
	if (_stricmp(argv1, "speexmulti") == 0 || _stricmp(argv1, "silkmulti") == 0)
	{
		VoiceCodec_e type = _stricmp(argv1, "speexmulti") == 0 ? VC_SPEEX : VC_SILK;

		if (type == VC_SILK)
		{
			g_SoundLists[2].play ^= true;
			g_SoundLists[3].play ^= true;
			g_SoundLists[4].play ^= true;
			g_SoundLists[5].play ^= true;

			if (g_SoundLists[2].play)
				g_SoundLists[2].time = gpGlobals->time;

			if (g_SoundLists[3].play)
				g_SoundLists[3].time = gpGlobals->time;

			if (g_SoundLists[4].play)
				g_SoundLists[4].time = gpGlobals->time;

			if (g_SoundLists[5].play)
				g_SoundLists[5].time = gpGlobals->time;

			g_SoundLists[2].frame = g_SoundLists[2].startindex;
			g_SoundLists[3].frame = g_SoundLists[3].startindex;
			g_SoundLists[4].frame = g_SoundLists[4].startindex;
			g_SoundLists[5].frame = g_SoundLists[5].startindex;
		}
		else
		{
/*
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
*/
		}

		RETURN_META(MRES_SUPERCEDE);
	}
	else if (_stricmp(argv1, "speexplay") == 0 || _stricmp(argv1, "silkplay") == 0)
	{
		VoiceCodec_e type = _stricmp(argv1, "speexplay") == 0 ? VC_SPEEX : VC_SILK;

		if (CMD_ARGC() < 2)
		{
			int nIndex = 0;

			bool header_once = false;
			for (auto &list : g_SoundLists)
			{
				if (list.type != type)
					continue;

				if (!header_once)
				{
					header_once = true;

					CLIENT_PRINTF(pEdict, print_console, UTIL_VarArgs("\nusage: silkplay [ <tracknumber> ]\n\n"));
					CLIENT_PRINTF(pEdict, print_console, UTIL_VarArgs("%4s  %-32s %-10s %-12s\n", "#", "[name]", "[chunk]", "[framerate]"));
				}

				CLIENT_PRINTF(pEdict, print_console, UTIL_VarArgs("%4d. %-32s %-10d %-12.2f\n", nIndex++, list.filename, list.nDataBytes, list.framerate));
			}

			CLIENT_PRINTF(pEdict, print_console, "\n");
			RETURN_META(MRES_SUPERCEDE);
		}

		int trackNumber = atoi(CMD_ARGV(1));
		if (trackNumber < 0 || trackNumber >= (type == VC_SPEEX ? g_tracks_speex : g_tracks_silk))
		{
			trackNumber = 0;
		}

		trackNumber = g_tracksNumbers[type][trackNumber];

		g_SoundLists[trackNumber].play ^= true;

		if (g_SoundLists[trackNumber].play)
		{
			g_SoundLists[trackNumber].time = gpGlobals->time;
		}

		g_SoundLists[trackNumber].frame = g_SoundLists[trackNumber].startindex;
		g_SoundLists[trackNumber].type = type;

		CLIENT_PRINTF(pEdict, print_console, UTIL_VarArgs("	-> send voice are %s, track: (%s)\n", g_SoundLists[trackNumber].play ? "RUNNING" : "STOPPED", g_SoundLists[trackNumber].filename));
		RETURN_META(MRES_SUPERCEDE);
	}

	RETURN_META(MRES_IGNORED);
}

unsigned long ReadDWord(FILE *fp)
{
	unsigned long ret;
	fread(&ret, 4, 1, fp);
	return ret;
}

unsigned short ReadWord(FILE *fp)
{
	unsigned short ret;
	fread(&ret, 2, 1, fp);
	return ret;
}

bool ReadWaveFile(const char *pFilename, char *&pData, int &nDataBytes, int &wBitsPerSample, int &nChannels, int &nSamplesPerSec)
{
	FILE *fp = fopen(pFilename, "rb");
	if(!fp)
		return false;

	fseek(fp, 22, SEEK_SET);

	nChannels = ReadWord(fp);
	nSamplesPerSec = ReadDWord(fp);

	fseek(fp, 34, SEEK_SET);
	wBitsPerSample = ReadWord(fp);

	fseek(fp, 40, SEEK_SET);
	nDataBytes = ReadDWord(fp);
	ReadDWord(fp);
	pData = new char[nDataBytes];
	if(!pData)
	{
		fclose(fp);
		return false;
	}
	fread(pData, nDataBytes, 1, fp);
	fclose(fp);
	return true;
}

int Voice_GetSpeexCompressed(SpeexContext_t &snd, char *pchDest, int nCount, bool bFinal)
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
			gotten = min((signed)sizeof(tempData) / 2, min(nShouldGet, (snd.nDataBytes - snd.frame) / 2));
			memcpy(tempData, &snd.data[snd.frame], gotten * 2);
			snd.frame += gotten * 2;
			snd.time = curtime;

			return snd.encoder->Compress((char *)tempData, gotten, pchDest, nCount, !!bFinal);
		}
	}

	return 0;
}

char uncompressedBuf[12002];

int Voice_GetSilkCompressed(SpeexContext_t &snd, char *pchDest, int nCount, bool bFinal)
{
	if (!snd.encoder)
	{
		snd.encoder = new CSteamP2PCodec(new VoiceEncoder_Silk());
		snd.encoder->Init(SILK_VOICE_QUALITY);
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
			gotten = min((signed)sizeof(tempData) / 2, min(nShouldGet, (snd.nDataBytes - snd.frame) / 2));
			memcpy(tempData, &snd.data[snd.frame], gotten * 2);
			snd.frame += gotten * 2;
			snd.time = curtime;

			g_ulSteamIdCurrent = snd.steamid;
			int res = snd.encoder->Compress((char *)tempData, gotten, pchDest, nCount, !!bFinal);
			g_ulSteamIdCurrent = 0ULL;
			return res;
		}
	}

	return 0;
}

int Voice_GetCompressedData(SpeexContext_t &snd, char *pchDest, int nCount, bool bFinal)
{
	switch (snd.type)
	{
	case VC_SPEEX:
		return Voice_GetSpeexCompressed(snd, pchDest, nCount, bFinal);
	case VC_SILK:
		return Voice_GetSilkCompressed(snd, pchDest, nCount, bFinal);
	default:
		break;
	}

	return 0;
}

void StartFrame_Post()
{
	static bool all_sound_initializes = false;
	if (!all_sound_initializes)
	{
		for (auto &list : g_SoundLists)
		{
			if (list.load || list.failed)
			{
				continue;
			}

			int nDataBytes, wBitsPerSample, nChannels, nSamplesPerSec;
			list.load = ReadWaveFile(list.filename, list.data, nDataBytes, wBitsPerSample, nChannels, nSamplesPerSec);

			list.frame = 0;
			list.nDataBytes = nDataBytes;
			list.wBitsPerSample = wBitsPerSample;
			list.time = gpGlobals->time;

			//if (list.type == VC_SILK)
			//{
			//	if (wBitsPerSample != 16 || nChannels != 1 || nSamplesPerSec != 24000)
			//	{
			//		printf(" > %s Wave file mismatch was got %d bits, %d channels, "
			//			"%d sample rate was expecting %d bits, %d channels, %d sample rate\n",
			//			list.filename, wBitsPerSample, nChannels, nSamplesPerSec, 16, 1, 24000);
			//
			//		delete [] list.data;
			//		list.load = false;
			//		list.failed = true;
			//		continue;
			//	}
			//}

			if (list.load)
			{
				g_tracksNumbers[list.type][list.type == VC_SPEEX ? g_tracks_speex++ : g_tracks_silk++] = &list - g_SoundLists;
				printf(" '%s' trackid: (%d), Load: (%s), chunk: (%d), framerate: (%0.2f), wBitsPerSample: (%d), nDataBytes: (%d), nSamplesPerSec: (%d)\n",
					list.type == VC_SPEEX ? "SPEEX" : "SILK", g_tracksNumbers[list.type][(list.type == VC_SPEEX ? g_tracks_speex++ : g_tracks_silk) - 1],
					list.filename, list.nDataBytes, list.framerate, list.wBitsPerSample, nDataBytes, nSamplesPerSec);
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

	fltime = gpGlobals->time + 0.02f;// - 0.000125;

	for (int snd = 0; snd < ARRAYSIZE(g_SoundLists); snd++)
	{
		if (!g_SoundLists[snd].play)
			continue;

		char uchVoiceData[4096];
		bool bFinal = false;
		int nDataLength = Voice_GetCompressedData(g_SoundLists[snd], uchVoiceData, sizeof(uchVoiceData), bFinal);
		if (nDataLength <= 0)
			continue;

		if (g_SoundLists[snd].frame >= g_SoundLists[snd].nDataBytes)// || nDataLength <= 0)
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
				//CLIENT_PRINTF(dstPlayer->GetClient()->GetEdict(), print_console, UTIL_VarArgs(" -> chunk:  %4d / %4d,   chunksize:  %4d,   progress:  %3.2f%%\n", g_SoundLists[snd].frame, g_SoundLists[snd].nDataBytes, nDataLength, ((double)g_SoundLists[snd].frame / (double)g_SoundLists[snd].nDataBytes) * 100.0));
				printf(" -> chunk:  %4d / %4d,   chunksize:  %4d,   progress:  %3.2f%%\n", g_SoundLists[snd].frame, g_SoundLists[snd].nDataBytes, nDataLength, ((double)g_SoundLists[snd].frame / (double)g_SoundLists[snd].nDataBytes) * 100.0);

				g_RehldsFuncs->MSG_WriteByte(dstDatagram, svc_voicedata);
				g_RehldsFuncs->MSG_WriteByte(dstDatagram, g_SoundLists[snd].index);
				g_RehldsFuncs->MSG_WriteShort(dstDatagram, nDataLength);
				g_RehldsFuncs->MSG_WriteBuf(dstDatagram, nDataLength, uchVoiceData);
			}
		}
	}

	RETURN_META(MRES_IGNORED);
}

static const char *steamids[]
{
	"76561198051972183",
	"76561198051972184",
	"76561198051972185",
	"76561198051972186",
	"76561198051972187",
	"76561198051972188",
	"76561198051972189",
	"76561198051972190",
	"76561198051972191",
	"76561198051972192",
	"76561198051972193",
	"76561198051972194",
	"76561198051972195",
	"76561198051972196",
	"76561198051972197",
};

void ClientPutInServer_PostHook(edict_t *pEntity)
{
	IGameClient* client = g_RehldsSvs->GetClient(ENTINDEX(pEntity) - 1);
	sizebuf_t *buf = client->GetNetChan()->GetMessageBuf();

	unsigned char digest[] =
	{
		0xcd, 0xff, 0xcc, 0x70, 0xda, 0x4a, 0x79, 0x1c, 0xea, 0x66, 0xba, 0xaa, 0xad, 0x2b, 0x40, 0x01
	};

	//memset(digest, 0, sizeof(digest));

	for (int i = 1; i < 8; i++)
	{
		char name[64];
		_snprintf(name, sizeof(name), "play-voice-%i", i);

		char userinfo[256];
		_snprintf(userinfo, sizeof(userinfo), "\\bottomcolor\\0\\topcolor\\0\\name\\%s\\model\\urban\\*sid\\%s", name, steamids[i]);

		g_RehldsFuncs->MSG_WriteByte(buf, 13); // svc_updateuserinfo
		g_RehldsFuncs->MSG_WriteByte(buf, i);
		g_RehldsFuncs->MSG_WriteLong(buf, i - 1);
		g_RehldsFuncs->MSG_WriteString(buf, userinfo);
		g_RehldsFuncs->MSG_WriteBuf(buf, sizeof(digest), digest);
	}

	SET_META_RESULT(MRES_IGNORED);
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
