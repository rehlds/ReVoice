#pragma once

#include "osconf.h"
#include "sys_shared.h"
#include "crc32c.h"

#include <time.h>

#include <extdll.h>
#include "rehlds_api.h"
#include <enginecallback.h>		// ALERT()
#include "osdep.h"				// win32 vsnprintf, etc
#include <dllapi.h>
#include <meta_api.h>
#include <h_export.h>
#include "sdk_util.h"			// UTIL_LogPrintf, etc

#include "revoice_shared.h"
#include "revoice_cfg.h"
#include "revoice_rehlds_api.h"
#include "VoiceEncoder_Silk.h"
#include "VoiceEncoder_Speex.h"
#include "VoiceEncoder_Opus.h"
#include "voice_codec_frame.h"
#include "SteamP2PCodec.h"
#include "revoice_player.h"
#include "revoice_main.h"

#include "interface.h"
#include "utlbuffer.h"
#include "reunion_api.h"
#include "revoice_reunion_api.h"
