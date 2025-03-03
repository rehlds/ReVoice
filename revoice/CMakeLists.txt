#-------------------------------------------------------------------------------
# Project Definition
#-------------------------------------------------------------------------------

project("${CMAKE_PROJECT_NAME}")

#-------------------------------------------------------------------------------
# Target Definition
#-------------------------------------------------------------------------------

set(TARGET_NAME "revoice")
set(TARGET_ALIAS "ReVoice::${TARGET_NAME}")

add_library("${TARGET_NAME}" SHARED)
add_library("${TARGET_ALIAS}" ALIAS "${TARGET_NAME}")

if(WIN32)
  set(BIN_OUTPUT_NAME "${TARGET_NAME}_mm")
else()
  set(BIN_OUTPUT_NAME "${TARGET_NAME}_mm_i386")
endif()

#-------------------------------------------------------------------------------
# Source Files
#-------------------------------------------------------------------------------

# Generate appversion.h
set(APP_VERSION_FILE "${PROJECT_BINARY_DIR}/appversion.h")
configure_file("src/appversion.h.in" "${APP_VERSION_FILE}" @ONLY)

# Define the precompiled headers
target_precompile_headers("${TARGET_NAME}"
  PRIVATE
    "src/precompiled.h"
)

# Define the source files
target_sources("${TARGET_NAME}"
  PRIVATE
    "${APP_VERSION_FILE}"
    "src/dllapi.cpp"
    "src/dllapi.h"
    "src/engine_api.cpp"
    "src/engine_api.h"
    "src/enginecallbacks.h"
    "src/h_export.cpp"
    "src/h_export.h"
    "src/iframeencoder.h"
    "src/IVoiceCodec.h"
    "src/log_meta.h"
    "src/meta_api.cpp"
    "src/meta_api.h"
    "src/mhook.h"
    "src/mreg.h"
    "src/mutil.h"
    "src/osconf.h"
    "src/plinfo.h"
    "src/precompiled.h"
    "src/reunion_api.h"
    "src/revoice_cfg.cpp"
    "src/revoice_cfg.h"
    "src/revoice_main.cpp"
    "src/revoice_main.h"
    "src/revoice_player.cpp"
    "src/revoice_player.h"
    "src/revoice_rehlds_api.cpp"
    "src/revoice_rehlds_api.h"
    "src/revoice_reunion_api.cpp"
    "src/revoice_reunion_api.h"
    "src/revoice_shared.h"
    "src/revoice_utils.cpp"
    "src/sdk_util.cpp"
    "src/sdk_util.h"
    "src/SteamP2PCodec.cpp"
    "src/SteamP2PCodec.h"
    "src/types_meta.h"
    "src/utlbuffer.cpp"
    "src/utlbuffer.h"
    "src/utlmemory.h"
    "src/voice_codec_frame.cpp"
    "src/voice_codec_frame.h"
    "src/VoiceEncoder_Opus.cpp"
    "src/VoiceEncoder_Opus.h"
    "src/VoiceEncoder_Silk.cpp"
    "src/VoiceEncoder_Silk.h"
    "src/VoiceEncoder_Speex.cpp"
    "src/VoiceEncoder_Speex.h"
)

if(WIN32)
  set(RESOURCE_FILENAME "${BIN_OUTPUT_NAME}.dll.rc")
  set(MODULE_DEFINITION_FILENAME "${BIN_OUTPUT_NAME}.dll.def")

  configure_file(
    "platform/windows/${RESOURCE_FILENAME}.in"
    "${PROJECT_BINARY_DIR}/${RESOURCE_FILENAME}"
    @ONLY
  )

  target_sources("${TARGET_NAME}"
    PRIVATE
      "${PROJECT_BINARY_DIR}/${RESOURCE_FILENAME}"
      "platform/windows/${MODULE_DEFINITION_FILENAME}"
  )
endif()

#-------------------------------------------------------------------------------
# Include Directories
#-------------------------------------------------------------------------------

target_include_directories("${TARGET_NAME}"
  PRIVATE
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>"
)

#-------------------------------------------------------------------------------
# Target Properties
#-------------------------------------------------------------------------------

set_target_properties("${TARGET_NAME}"
  PROPERTIES
    PREFIX ""
    OUTPUT_NAME "${BIN_OUTPUT_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
    PDB_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
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

    $<$<NOT:$<PLATFORM_ID:Windows>>:
      __int16=int16_t
      _stricmp=strcasecmp
    >
)

#-------------------------------------------------------------------------------
# Compile Options
#-------------------------------------------------------------------------------

target_compile_options("${TARGET_NAME}"
  PRIVATE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
      -Wno-write-strings
    >
)

#-------------------------------------------------------------------------------
# Link Libraries
#-------------------------------------------------------------------------------

target_link_libraries("${TARGET_NAME}"
  PRIVATE
    "Speex::speex"
    "Silk::silk"
    "Opus::opus"
    "Metamod::metamod"

    $<$<NOT:$<PLATFORM_ID:Windows>>:
      "c"
      "m"
      "${CMAKE_DL_LIBS}"
    >
)

#-------------------------------------------------------------------------------
# Post-Build Actions
#-------------------------------------------------------------------------------

if(MSVC)
  set(ILK_FILE_PATH "$<TARGET_FILE_DIR:${TARGET_NAME}>/${BIN_OUTPUT_NAME}.ilk")
  set(ILK_OUTPUT_DIR "${PROJECT_BINARY_DIR}/$<CONFIG>")
  set(ILK_OUTPUT_PATH "${ILK_OUTPUT_DIR}/${BIN_OUTPUT_NAME}.ilk")

  add_custom_command(TARGET "${TARGET_NAME}" POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename "${ILK_FILE_PATH}" "${ILK_OUTPUT_PATH}" >nul 2>&1 || ${CMAKE_COMMAND} -E true
    COMMENT "Moving '${ILK_FILE_PATH}' file to '${ILK_OUTPUT_DIR}' if it exists..."
  )
endif()
