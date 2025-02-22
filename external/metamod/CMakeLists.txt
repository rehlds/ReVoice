#-------------------------------------------------------------------------------
# Project Definition
#-------------------------------------------------------------------------------

project("MetamodSDK")

#-------------------------------------------------------------------------------
# Target Definition
#-------------------------------------------------------------------------------

set(TARGET_NAME "metamod")
set(TARGET_ALIAS "Metamod::${TARGET_NAME}")

add_library("${TARGET_NAME}" INTERFACE)
add_library("${TARGET_ALIAS}" ALIAS "${TARGET_NAME}")

#-------------------------------------------------------------------------------
# Source Files
#-------------------------------------------------------------------------------

target_sources("${TARGET_NAME}"
  INTERFACE
    "include/metamod/dllapi.h"
    "include/metamod/engine_api.h"
    "include/metamod/enginecallbacks.h"
    "include/metamod/h_export.h"
    "include/metamod/log_meta.h"
    "include/metamod/meta_api.h"
    "include/metamod/mhook.h"
    "include/metamod/mreg.h"
    "include/metamod/mutil.h"
    "include/metamod/osdep.h"
    "include/metamod/plinfo.h"
    "include/metamod/sdk_util.h"
    "include/metamod/types_meta.h"
)

#-------------------------------------------------------------------------------
# Include Directories
#-------------------------------------------------------------------------------

target_include_directories("${TARGET_NAME}"
  INTERFACE
    "${PROJECT_SOURCE_DIR}/include"
)

#-------------------------------------------------------------------------------
# Link Libraries
#-------------------------------------------------------------------------------

target_link_libraries("${TARGET_NAME}"
  INTERFACE
    "ReHLSDK::rehlsdk"
)
