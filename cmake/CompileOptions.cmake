#-------------------------------------------------------------------------------
# Platform-specific compile options
#-------------------------------------------------------------------------------

set(PLATFORM_OPTIONS_FILE "CompileOptions${CMAKE_SYSTEM_NAME}.cmake")
set(PLATFORM_OPTIONS_PATH "${CMAKE_SOURCE_DIR}/cmake/${PLATFORM_OPTIONS_FILE}")

include("${PLATFORM_OPTIONS_PATH}"
  OPTIONAL
  RESULT_VARIABLE IS_PLATFORM_OPTIONS_INCLUDED
)

if(IS_PLATFORM_OPTIONS_INCLUDED)
  message(STATUS "Applied platform-specific compile options.")
else()
  message(STATUS "No platform-specific compile options found. Using defaults.")
endif()

#-------------------------------------------------------------------------------
# Compiler-specific options
#-------------------------------------------------------------------------------

get_property(ENABLED_LANG_LIST GLOBAL PROPERTY ENABLED_LANGUAGES)

if(ENABLED_LANG_LIST)
  foreach(lang IN LISTS ENABLED_LANG_LIST)
    string(TOUPPER "${lang}" lang_prefix)

    if(DEFINED CMAKE_${lang_prefix}_COMPILER_ID)
      list(APPEND COMPILER_LIST "${CMAKE_${lang_prefix}_COMPILER_ID}")
    endif()
  endforeach()

  list(REMOVE_DUPLICATES COMPILER_LIST)

  foreach(compiler_id IN LISTS COMPILER_LIST)
    if(NOT compiler_id OR "${compiler_id}" STREQUAL "")
      continue()
    endif()

    set(COMPILER_OPTIONS_FILE "CompileOptions${CMAKE_SYSTEM_NAME}${compiler_id}.cmake")
    set(COMPILER_OPTIONS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${COMPILER_OPTIONS_FILE}")

    include("${COMPILER_OPTIONS_PATH}"
      OPTIONAL
      RESULT_VARIABLE IS_COMPILER_OPTIONS_INCLUDED
    )

    if(IS_COMPILER_OPTIONS_INCLUDED)
      message(STATUS "Applied ${compiler_id} compiler options.")
    else()
      message(STATUS "No ${compiler_id} compiler options found. Using defaults.")
    endif()
  endforeach()
else()
  message(WARNING "No enabled languages found - skipping applying compiler-specific options.")
endif()
