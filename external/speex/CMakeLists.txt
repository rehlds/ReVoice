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
    "include/speex/cb_search.h"
    "include/speex/filters_sse.h"
    "include/speex/filters.h"
    "include/speex/lpc.h"
    "include/speex/lsp.h"
    "include/speex/ltp_sse.h"
    "include/speex/ltp.h"
    "include/speex/math_approx.h"
    "include/speex/misc.h"
    "include/speex/modes.h"
    "include/speex/nb_celp.h"
    "include/speex/quant_lsp.h"
    "include/speex/sb_celp.h"
    "include/speex/speex_bits.h"
    "include/speex/speex_callbacks.h"
    "include/speex/speex_header.h"
    "include/speex/speex_stereo.h"
    "include/speex/speex.h"
    "include/speex/stack_alloc.h"
    "include/speex/vbr.h"
    "include/speex/vq.h"

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
