#-------------------------------------------------------------------------------
# Compile Options
#-------------------------------------------------------------------------------

add_compile_options(
  -mmmx                 # Enable MMX extended instruction set
  -msse                 # Enable SSE extended instruction set
  -msse2                # Enable SSE2 extended instruction set
  -msse3                # Enable SSE3 extended instruction set
  -mssse3               # Enable SSSE3 extended instruction set
  -msse4.1              # Enable SSE4.1 extended instruction set
  -msse4.2              # Enable SSE4.2 extended instruction set
  -fcf-protection=none  # Instrument control-flow architecture protection

  # Generator toolset ClangCL: Enable multi-processor compilation
  $<$<STREQUAL:${CMAKE_GENERATOR_TOOLSET},ClangCL>:/MP>

  # Debug and RelWithDebInfo: Generate complete debug information in PDB files
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/Zi>

  # Non-Debug: Enable automatic parallelization of loops
  $<$<NOT:$<CONFIG:Debug>>:/Qvec>

  # Generate instructions for a specified machine type
  $<IF:$<BOOL:${OPTIMIZE_FOR_CURRENT_CPU}>,-march=native,-march=x86-64-v2>
)

# Set the MSVC debug information format (/Zi (ProgramDatabase))
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:ProgramDatabase>"
)
