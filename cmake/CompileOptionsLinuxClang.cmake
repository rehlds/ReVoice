#-------------------------------------------------------------------------------
# Compile Options
#-------------------------------------------------------------------------------

add_compile_options(
  # Debug: Optimize for debugging experience
  $<$<CONFIG:Debug>:-Og>

  # Debug and RelWithDebInfo: Generate debug information for GDB
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:-ggdb>
)

# Enable full Link-Time Optimization for C
set(CMAKE_C_COMPILE_OPTIONS_IPO -flto=full)

# Enable full Link-Time Optimization for C++
set(CMAKE_CXX_COMPILE_OPTIONS_IPO -flto=full)

#-------------------------------------------------------------------------------
# Link Options
#-------------------------------------------------------------------------------

if(USE_LINKER_LLD)
  add_link_options(
    -fuse-ld=lld                # Use the LLVM LLD linker
    -Wl,--check-sections        # Check section address conflicts
    -Wl,--icf=safe              # Enable safe identical code folding
    -Wl,--warn-backrefs         # Warn about backward references in the symbol table
    -Wl,--warn-ifunc-textrel    # Warn about text relocations in indirect functions
    -Wl,--warn-symbol-ordering  # Warn when symbol ordering is not as expected

    #-Wl,--print-gc-sections    # Print removed unused sections
    #-Wl,--print-icf-sections   # Print folded identical sections

    # Non-Debug: Set linker optimization level
    $<$<NOT:$<CONFIG:Debug>>:-Wl,-O2>

    # Release and MinSizeRel: Set LTO optimization level
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:-Wl,--lto-O3>
  )
else()
  add_link_options(
    -Wl,--check-sections            # Check section address conflicts
    -Wl,--no-allow-shlib-undefined  # Do not allow undefined symbols in shared libraries
    -Wl,--warn-alternate-em         # Warn about ELF machine type mismatches

    # Non-Debug: Set linker optimization level
    $<$<NOT:$<CONFIG:Debug>>:-Wl,-O3>

    # Non-Debug: Enable linker relaxation optimizations
    $<$<NOT:$<CONFIG:Debug>>:-Wl,--relax>
  )
endif()
