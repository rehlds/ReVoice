#-------------------------------------------------------------------------------
# Project Definition
#-------------------------------------------------------------------------------

project("ReHLSDK")

#-------------------------------------------------------------------------------
# Target Definition
#-------------------------------------------------------------------------------

set(TARGET_NAME "rehlsdk")
set(TARGET_ALIAS "ReHLSDK::${TARGET_NAME}")

add_library("${TARGET_NAME}")
add_library("${TARGET_ALIAS}" ALIAS "${TARGET_NAME}")

#-------------------------------------------------------------------------------
# Source Files
#-------------------------------------------------------------------------------

target_sources("${TARGET_NAME}"
  PUBLIC
    "include/rehlsdk/common/BaseSystemModule.h"
    "include/rehlsdk/common/beamdef.h"
    "include/rehlsdk/common/cl_entity.h"
    "include/rehlsdk/common/com_model.h"
    "include/rehlsdk/common/con_nprint.h"
    "include/rehlsdk/common/const.h"
    "include/rehlsdk/common/crc.h"
    "include/rehlsdk/common/cvardef.h"
    "include/rehlsdk/common/demo_api.h"
    "include/rehlsdk/common/director_cmds.h"
    "include/rehlsdk/common/dlight.h"
    "include/rehlsdk/common/dll_state.h"
    "include/rehlsdk/common/entity_state.h"
    "include/rehlsdk/common/entity_types.h"
    "include/rehlsdk/common/enums.h"
    "include/rehlsdk/common/event_api.h"
    "include/rehlsdk/common/event_args.h"
    "include/rehlsdk/common/event_flags.h"
    "include/rehlsdk/common/hltv.h"
    "include/rehlsdk/common/IAdminServer.h"
    "include/rehlsdk/common/IBaseSystem.h"
    "include/rehlsdk/common/IDemoPlayer.h"
    "include/rehlsdk/common/IEngineWrapper.h"
    "include/rehlsdk/common/IGameServerData.h"
    "include/rehlsdk/common/in_buttons.h"
    "include/rehlsdk/common/IObjectContainer.h"
    "include/rehlsdk/common/ISystemModule.h"
    "include/rehlsdk/common/IVGuiModule.h"
    "include/rehlsdk/common/ivoicetweak.h"
    "include/rehlsdk/common/kbutton.h"
    "include/rehlsdk/common/mathlib.h"
    "include/rehlsdk/common/md5.h"
    "include/rehlsdk/common/net_api.h"
    "include/rehlsdk/common/netadr.h"
    "include/rehlsdk/common/netapi.h"
    "include/rehlsdk/common/nowin.h"
    "include/rehlsdk/common/ObjectDictionary.h"
    "include/rehlsdk/common/ObjectList.h"
    "include/rehlsdk/common/parsemsg.h"
    "include/rehlsdk/common/particledef.h"
    "include/rehlsdk/common/pmtrace.h"
    "include/rehlsdk/common/port.h"
    "include/rehlsdk/common/qfont.h"
    "include/rehlsdk/common/qlimits.h"
    "include/rehlsdk/common/quakedef.h"
    "include/rehlsdk/common/r_efx.h"
    "include/rehlsdk/common/r_studioint.h"
    "include/rehlsdk/common/ref_params.h"
    "include/rehlsdk/common/screenfade.h"
    "include/rehlsdk/common/Sequence.h"
    "include/rehlsdk/common/SteamAppStartUp.h"
    "include/rehlsdk/common/SteamCommon.h"
    "include/rehlsdk/common/studio_event.h"
    "include/rehlsdk/common/textconsole.h"
    "include/rehlsdk/common/TextConsoleUnix.h"
    "include/rehlsdk/common/TextConsoleWin32.h"
    "include/rehlsdk/common/TokenLine.h"
    "include/rehlsdk/common/triangleapi.h"
    "include/rehlsdk/common/usercmd.h"
    "include/rehlsdk/common/vmodes.h"
    "include/rehlsdk/common/weaponinfo.h"
    "include/rehlsdk/common/winsani_in.h"
    "include/rehlsdk/common/winsani_out.h"

    "include/rehlsdk/dlls/activity.h"
    "include/rehlsdk/dlls/activitymap.h"
    "include/rehlsdk/dlls/animation.h"
    "include/rehlsdk/dlls/basemonster.h"
    "include/rehlsdk/dlls/cbase.h"
    "include/rehlsdk/dlls/cdll_dll.h"
    "include/rehlsdk/dlls/client.h"
    "include/rehlsdk/dlls/decals.h"
    "include/rehlsdk/dlls/doors.h"
    "include/rehlsdk/dlls/effects.h"
    "include/rehlsdk/dlls/enginecallback.h"
    "include/rehlsdk/dlls/explode.h"
    "include/rehlsdk/dlls/extdll.h"
    "include/rehlsdk/dlls/func_break.h"
    "include/rehlsdk/dlls/game.h"
    "include/rehlsdk/dlls/gamerules.h"
    "include/rehlsdk/dlls/hornet.h"
    "include/rehlsdk/dlls/items.h"
    "include/rehlsdk/dlls/maprules.h"
    "include/rehlsdk/dlls/monsterevent.h"
    "include/rehlsdk/dlls/monsters.h"
    "include/rehlsdk/dlls/nodes.h"
    "include/rehlsdk/dlls/plane.h"
    "include/rehlsdk/dlls/player.h"
    "include/rehlsdk/dlls/saverestore.h"
    "include/rehlsdk/dlls/schedule.h"
    "include/rehlsdk/dlls/scriptevent.h"
    "include/rehlsdk/dlls/skill.h"
    "include/rehlsdk/dlls/soundent.h"
    "include/rehlsdk/dlls/spectator.h"
    "include/rehlsdk/dlls/talkmonster.h"
    "include/rehlsdk/dlls/teamplay_gamerules.h"
    "include/rehlsdk/dlls/trains.h"
    "include/rehlsdk/dlls/util.h"
    "include/rehlsdk/dlls/vector.h"
    "include/rehlsdk/dlls/weapons.h"

    "include/rehlsdk/engine/archtypes.h"
    "include/rehlsdk/engine/bspfile.h"
    "include/rehlsdk/engine/cmd_rehlds.h"
    "include/rehlsdk/engine/common_rehlds.h"
    "include/rehlsdk/engine/crc32c.h"
    "include/rehlsdk/engine/custom.h"
    "include/rehlsdk/engine/customentity.h"
    "include/rehlsdk/engine/d_local.h"
    "include/rehlsdk/engine/edict.h"
    "include/rehlsdk/engine/eiface.h"
    "include/rehlsdk/engine/FlightRecorder.h"
    "include/rehlsdk/engine/hookchains.h"
    "include/rehlsdk/engine/keydefs.h"
    "include/rehlsdk/engine/maintypes.h"
    "include/rehlsdk/engine/model.h"
    "include/rehlsdk/engine/modelgen.h"
    "include/rehlsdk/engine/osconfig.h"
    "include/rehlsdk/engine/pr_dlls.h"
    "include/rehlsdk/engine/progdefs.h"
    "include/rehlsdk/engine/progs.h"
    "include/rehlsdk/engine/rehlds_api.h"
    "include/rehlsdk/engine/rehlds_interfaces.h"
    "include/rehlsdk/engine/Sequence.h"
    "include/rehlsdk/engine/shake.h"
    "include/rehlsdk/engine/spritegn.h"
    "include/rehlsdk/engine/static_map.h"
    "include/rehlsdk/engine/studio.h"
    "include/rehlsdk/engine/sys_shared.h"
    "include/rehlsdk/engine/userid_rehlds.h"

    "include/rehlsdk/pm_shared/pm_debug.h"
    "include/rehlsdk/pm_shared/pm_defs.h"
    "include/rehlsdk/pm_shared/pm_info.h"
    "include/rehlsdk/pm_shared/pm_materials.h"
    "include/rehlsdk/pm_shared/pm_movevars.h"
    "include/rehlsdk/pm_shared/pm_shared.h"

    "include/rehlsdk/public/commonmacros.h"
    "include/rehlsdk/public/FileSystem.h"
    "include/rehlsdk/public/interface.h"

  PRIVATE
    "src/engine/crc32c.cpp"
    "src/engine/sys_shared.cpp"
    "src/public/interface.cpp"
)

#-------------------------------------------------------------------------------
# Include Directories
#-------------------------------------------------------------------------------

target_include_directories("${TARGET_NAME}"
  PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)

#-------------------------------------------------------------------------------
# Compile Definitions
#-------------------------------------------------------------------------------

target_compile_definitions("${TARGET_NAME}"
  PRIVATE
    $<$<PLATFORM_ID:Windows>:
      _MBCS
      _CRT_SECURE_NO_WARNINGS
    >
)
