#-------------------------------------------------------------------------------
# Project Definition
#-------------------------------------------------------------------------------

project(        "Speex"
  VERSION       "1.2.1"
  DESCRIPTION   "Speex voice codec"
  HOMEPAGE_URL  "https://gitlab.xiph.org/xiph/speex"
  LANGUAGES     "C"
)

#-------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------

option(SPEEX_USE_KISS_FFT "Use KISS Fast Fourier Transform (otherwise OggVorbis)" OFF)

#-------------------------------------------------------------------------------
# Target Definition
#-------------------------------------------------------------------------------

set(TARGET_NAME "speex")
set(TARGET_ALIAS "Speex::${TARGET_NAME}")

add_library("${TARGET_NAME}")
add_library("${TARGET_ALIAS}" ALIAS "${TARGET_NAME}")

#-------------------------------------------------------------------------------
# Source Files
#-------------------------------------------------------------------------------

target_sources("${TARGET_NAME}"
  PUBLIC
    "include/speex/speex_bits.h"
    "include/speex/speex_callbacks.h"
    "include/speex/speex_config_types.h"
    "include/speex/speex_header.h"
    "include/speex/speex_stereo.h"
    "include/speex/speex_types.h"
    "include/speex/speex.h"

  PRIVATE
    "src/arch.h"
    "src/bfin.h"
    "src/bits.c"
    "src/cb_search_bfin.h"
    "src/cb_search_sse.h"
    "src/cb_search.c"
    "src/cb_search.h"
    "src/exc_10_16_table.c"
    "src/exc_10_32_table.c"
    "src/exc_20_32_table.c"
    "src/exc_5_256_table.c"
    "src/exc_5_64_table.c"
    "src/exc_8_128_table.c"
    "src/fftwrap.h"
    "src/filters_bfin.h"
    "src/filters_sse.h"
    "src/filters.c"
    "src/filters.h"
    "src/fixed_bfin.h"
    "src/fixed_debug.h"
    "src/fixed_generic.h"
    "src/gain_table_lbr.c"
    "src/gain_table.c"
    "src/hexc_10_32_table.c"
    "src/hexc_table.c"
    "src/high_lsp_tables.c"
    "src/lpc_bfin.h"
    "src/lpc.c"
    "src/lpc.h"
    "src/lsp_bfin.h"
    "src/lsp_tables_nb.c"
    "src/lsp.c"
    "src/lsp.h"
    "src/ltp_bfin.h"
    "src/ltp_sse.h"
    "src/ltp.c"
    "src/ltp.h"
    "src/math_approx.h"
    "src/misc_bfin.h"
    "src/modes_wb.c"
    "src/modes.c"
    "src/modes.h"
    "src/nb_celp.c"
    "src/nb_celp.h"
    "src/os_support.h"
    "src/quant_lsp_bfin.h"
    "src/quant_lsp.c"
    "src/quant_lsp.h"
    "src/sb_celp.c"
    "src/sb_celp.h"
    "src/speex_callbacks.c"
    "src/speex_header.c"
    "src/speex.c"
    "src/stack_alloc.h"
    "src/stereo.c"
    "src/vbr.c"
    "src/vbr.h"
    "src/vq_bfin.h"
    "src/vq_sse.h"
    "src/vq.c"
    "src/vq.h"
    "src/window.c"

    $<$<BOOL:${SPEEX_USE_KISS_FFT}>:
      "src/_kiss_fft_guts.h"
      "src/kiss_fft.c"
      "src/kiss_fft.h"
      "src/kiss_fftr.c"
      "src/kiss_fftr.h"
    >

    $<$<NOT:$<BOOL:${SPEEX_USE_KISS_FFT}>>:
      "src/smallft.c"
      "src/smallft.h"
    >

    $<$<BOOL:${ENABLE_SPEEX_VORBIS_PSY}>:
      "src/vorbis_psy.c"
      "src/vorbis_psy.h"
    >
)

#-------------------------------------------------------------------------------
# Include Directories
#-------------------------------------------------------------------------------

target_include_directories("${TARGET_NAME}"
  PUBLIC
    "${PROJECT_SOURCE_DIR}/include"

  PRIVATE
    "${PROJECT_SOURCE_DIR}/include/speex"
)

#-------------------------------------------------------------------------------
# Compile Definitions
#-------------------------------------------------------------------------------

target_compile_definitions("${TARGET_NAME}"
  PRIVATE
    _USE_SSE
    FLOATING_POINT

    SPEEX_MAJOR_VERSION=${PROJECT_VERSION_MAJOR}
    SPEEX_MINOR_VERSION=${PROJECT_VERSION_MINOR}
    SPEEX_MICRO_VERSION=${PROJECT_VERSION_PATCH}
    SPEEX_VERSION="${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
    SPEEX_EXTRA_VERSION=""

    $<$<PLATFORM_ID:Windows>:
      _USE_SSE2
      _USE_MATH_DEFINES
      alloca=_alloca
      inline=__inline
      EXPORT=
    >

    $<$<NOT:$<PLATFORM_ID:Windows>>:
      HAVE_ALLOCA_H
      HAVE_GETOPT_H
      USE_ALLOCA
      VAR_ARRAYS
      EXPORT=__attribute__\(\(visibility\(\"default\"\)\)\)
    >

    $<$<BOOL:${ENABLE_SPEEX_VORBIS_PSY}>:VORBIS_PSYCHO>
    $<IF:$<BOOL:${SPEEX_USE_KISS_FFT}>,USE_KISS_FFT,USE_SMALLFT>
)

#-------------------------------------------------------------------------------
# Compile Options
#-------------------------------------------------------------------------------

target_compile_options("${TARGET_NAME}"
  PRIVATE
    -UARM4_ASM
    -UARM5E_ASM
    -UBFIN_ASM
    -UDISABLE_FLOAT_API
    -UDISABLE_VBR
    -UENABLE_VALGRIND
    -UFIXED_POINT_DEBUG

    $<$<PLATFORM_ID:Windows>:
      -UHAVE_GETOPT_H
      -UUSE_ALLOCA
    >

    $<IF:$<C_COMPILER_ID:MSVC>,/W0,-w>
    $<$<AND:$<C_COMPILER_ID:MSVC>,$<NOT:$<CONFIG:Debug>>>:/fp:fast>
    $<$<AND:$<NOT:$<C_COMPILER_ID:MSVC>>,$<NOT:$<CONFIG:Debug>>>:-ffast-math>
)

#-------------------------------------------------------------------------------
# Link Libraries
#-------------------------------------------------------------------------------

target_link_libraries("${TARGET_NAME}"
  PUBLIC
    $<$<NOT:$<PLATFORM_ID:Windows>>:
      "m"
    >
)
