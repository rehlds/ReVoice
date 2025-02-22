#pragma once

#include "osconf.h"
#include <rehlsdk/engine/sys_shared.h>
#include <rehlsdk/engine/crc32c.h>

#include <time.h>

#include <rehlsdk/dlls/extdll.h>
#include <rehlsdk/engine/rehlds_api.h>
#include <rehlsdk/dlls/enginecallback.h>    // ALERT()
#include <metamod/osdep.h>                  // win32 vsnprintf, etc
#include <metamod/dllapi.h>
#include <metamod/meta_api.h>
#include <metamod/h_export.h>
#include <metamod/sdk_util.h>               // UTIL_LogPrintf, etc

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

#include <rehlsdk/public/interface.h>
#include "utlbuffer.h"
#include "reunion_api.h"
#include "revoice_reunion_api.h"
