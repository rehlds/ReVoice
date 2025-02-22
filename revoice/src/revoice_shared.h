#pragma once

#include <rehlsdk/engine/rehlds_api.h>

#define MAX_PLAYERS					32

#define LOG_PREFIX					"[REVOICE]: "

#define MAX_SILK_DATA_LEN			650
#define MAX_OPUS_DATA_LEN			960
#define MAX_SPEEX_DATA_LEN			228

#define MAX_SILK_VOICE_RATE			3800
#define MAX_OPUS_VOICE_RATE			3100
#define MAX_SPEEX_VOICE_RATE		2014

#define SILK_VOICE_QUALITY			5
#define OPUS_VOICE_QUALITY			5
#define SPEEX_VOICE_QUALITY			5

enum revoice_log_mode {
	rl_none = 0,
	rl_console = 1,
	rl_logfile = 2,
};

enum CodecType {
	vct_none,
	vct_silk,
	vct_opus,
	vct_speex,
};

enum svc_messages {
	svc_voiceinit = 52,
	svc_voicedata = 53
};

extern char* trimbuf(char *str);
extern void NormalizePath(char *path);
extern bool IsFileExists(const char *path);
extern void LCPrintf(bool critical, const char *fmt, ...);
extern uint32 crc32(const void *buf, unsigned int bufLen);

extern bool Revoice_Load();
extern bool Revoice_Utils_Init();
extern void util_syserror(const char *fmt, ...);
