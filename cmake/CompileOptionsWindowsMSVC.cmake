#-------------------------------------------------------------------------------
# Compile Options
#-------------------------------------------------------------------------------

if(ENABLE_ASAN)
  set(DEBUG_FORMAT_FLAG "/Zi")
  set(MSVC_DEBUG_INFORMATION_FORMAT "$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:ProgramDatabase>")
else()
  set(DEBUG_FORMAT_FLAG "/ZI")
  set(MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug>:EditAndContinue>$<$<CONFIG:RelWithDebInfo>:ProgramDatabase>")
endif()

add_compile_options(
  /MP                     # Multi-processor compilation
  /arch:SSE2              # Minimum required instruction set
  /fpcvt:BC               # Floating-point to integer conversion compatibility
  /Qspectre-              # Disable Spectre variant 1 mitigations
  /external:anglebrackets # Treat headers included with angle brackets < > as external

  # Debug: Ensure distinct function addresses
  $<$<CONFIG:Debug>:/Gu>

  # Debug: Zero initialize padding for stack based class types
  $<$<CONFIG:Debug>:/presetPadding>

  # Debug: Generate enhanced debug info with 'Edit and Continue' support
  $<$<CONFIG:Debug>:${DEBUG_FORMAT_FLAG}>

  # RelWithDebInfo: Generate complete debug information in PDB files
  $<$<CONFIG:RelWithDebInfo>:/Zi>

  # Debug and RelWithDebInfo: Enable faster PDB generation
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/Zf>

  # Non-Debug: Enable whole program optimization
  $<IF:$<CONFIG:Debug>,/GL-,/GL>

  # Non-Debug: Enable automatic parallelization of loops
  $<$<NOT:$<CONFIG:Debug>>:/Qpar>
)

# Set the MSVC debug information format (/ZI (EditAndContinue), /Zi (ProgramDatabase))
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "${MSVC_DEBUG_INFORMATION_FORMAT}")

#-------------------------------------------------------------------------------
# Link Options
#-------------------------------------------------------------------------------

add_link_options(
  /CGTHREADS:${NUM_CORES} # Thread count for parallel linking
)
