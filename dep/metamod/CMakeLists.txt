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
    "dllapi.h"
    "engine_api.h"
    "enginecallbacks.h"
    "h_export.h"
    "log_meta.h"
    "meta_api.h"
    "mhook.h"
    "mreg.h"
    "mutil.h"
    "osdep.h"
    "plinfo.h"
    "sdk_util.h"
    "types_meta.h"
)

#-------------------------------------------------------------------------------
# Include Directories
#-------------------------------------------------------------------------------

target_include_directories("${TARGET_NAME}"
  INTERFACE
    "${PROJECT_SOURCE_DIR}"
)

#-------------------------------------------------------------------------------
# Link Libraries
#-------------------------------------------------------------------------------

target_link_libraries("${TARGET_NAME}"
  INTERFACE
    "ReHLSDK::rehlsdk"
)
