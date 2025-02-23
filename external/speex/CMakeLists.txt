#-------------------------------------------------------------------------------
# Project Definition
#-------------------------------------------------------------------------------

project("Speex")

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
    "include/speex/speex_header.h"
    "include/speex/speex_stereo.h"
    "include/speex/speex.h"

  PRIVATE
    "src/bits.c"
    "src/cb_search.c"
    "src/cb_search.h"
    "src/exc_10_16_table.c"
    "src/exc_10_32_table.c"
    "src/exc_20_32_table.c"
    "src/exc_5_256_table.c"
    "src/exc_5_64_table.c"
    "src/exc_8_128_table.c"
    "src/filters_sse.h"
    "src/filters.c"
    "src/filters.h"
    "src/gain_table_lbr.c"
    "src/gain_table.c"
    "src/hexc_10_32_table.c"
    "src/hexc_table.c"
    "src/high_lsp_tables.c"
    "src/lpc.c"
    "src/lpc.h"
    "src/lsp_tables_nb.c"
    "src/lsp.c"
    "src/lsp.h"
    "src/ltp_sse.h"
    "src/ltp.c"
    "src/ltp.h"
    "src/math_approx.c"
    "src/math_approx.h"
    "src/misc.c"
    "src/misc.h"
    "src/modes.c"
    "src/modes.h"
    "src/nb_celp.c"
    "src/nb_celp.h"
    "src/quant_lsp.c"
    "src/quant_lsp.h"
    "src/sb_celp.c"
    "src/sb_celp.h"
    "src/speex_callbacks.c"
    "src/speex_header.c"
    "src/stack_alloc.h"
    "src/stereo.c"
    "src/vbr.c"
    "src/vbr.h"
    "src/vq.c"
    "src/vq.h"
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
# Compile Options
#-------------------------------------------------------------------------------

target_compile_options("${TARGET_NAME}"
  PRIVATE
    $<IF:$<CXX_COMPILER_ID:MSVC>,/W0,-w>
)
