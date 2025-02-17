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
    "include/cb_search.h"
    "include/filters_sse.h"
    "include/filters.h"
    "include/lpc.h"
    "include/lsp.h"
    "include/ltp_sse.h"
    "include/ltp.h"
    "include/math_approx.h"
    "include/misc.h"
    "include/modes.h"
    "include/nb_celp.h"
    "include/quant_lsp.h"
    "include/sb_celp.h"
    "include/speex_bits.h"
    "include/speex_callbacks.h"
    "include/speex_header.h"
    "include/speex_stereo.h"
    "include/speex.h"
    "include/stack_alloc.h"
    "include/vbr.h"
    "include/vq.h"

  PRIVATE
    "src/bits.c"
    "src/cb_search.c"
    "src/exc_10_16_table.c"
    "src/exc_10_32_table.c"
    "src/exc_20_32_table.c"
    "src/exc_5_256_table.c"
    "src/exc_5_64_table.c"
    "src/exc_8_128_table.c"
    "src/filters.c"
    "src/gain_table_lbr.c"
    "src/gain_table.c"
    "src/hexc_10_32_table.c"
    "src/hexc_table.c"
    "src/high_lsp_tables.c"
    "src/lpc.c"
    "src/lsp_tables_nb.c"
    "src/lsp.c"
    "src/ltp.c"
    "src/math_approx.c"
    "src/misc.c"
    "src/modes.c"
    "src/nb_celp.c"
    "src/quant_lsp.c"
    "src/sb_celp.c"
    "src/speex_callbacks.c"
    "src/speex_header.c"
    "src/stereo.c"
    "src/vbr.c"
    "src/vq.c"
)

#-------------------------------------------------------------------------------
# Include Directories
#-------------------------------------------------------------------------------

target_include_directories("${TARGET_NAME}"
  PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
)

#-------------------------------------------------------------------------------
# Compile Options
#-------------------------------------------------------------------------------

target_compile_options("${TARGET_NAME}"
  PRIVATE
    $<IF:$<CXX_COMPILER_ID:MSVC>,/W0,-w>
)
