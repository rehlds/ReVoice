#-------------------------------------------------------------------------------
# Compile Options
#-------------------------------------------------------------------------------

add_compile_options(
  # Debug: Optimize for debugging experience
  $<$<CONFIG:Debug>:-Og>

  # Debug and RelWithDebInfo: Generate debug information for GDB
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:-ggdb>
)

# Check if the GCC compiler version is greater than or equal to 12
if(CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 12)
  set(CMAKE_C_COMPILE_OPTIONS_IPO
    -flto=auto            # Enable Link Time Optimization with automatic parallelization
    -fno-fat-lto-objects  # Disable generation of fat LTO objects
  )
endif()

# Check if the G++ compiler version is greater than or equal to 12
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12)
  set(CMAKE_CXX_COMPILE_OPTIONS_IPO
    -flto=auto
    -fno-fat-lto-objects
  )
endif()

#-------------------------------------------------------------------------------
# Link Options
#-------------------------------------------------------------------------------

if(USE_LINKER_GOLD)
  add_link_options(
    -fuse-ld=gold                       # Use the Gold linker
    -Wl,--icf=safe                      # Enable safe identical code folding
    -Wl,--icf-iterations=5              # Set identical code folding iterations
    -Wl,--no-whole-archive              # Do not force inclusion of entire static libraries
    -Wl,--unresolved-symbols=report-all # Report all unresolved symbols
    -Wl,--warn-execstack                # Warn if an executable stack is detected
    -Wl,--warn-search-mismatch          # Warn about mismatches in library search paths
    -Wl,--warn-shared-textrel           # Warn about text relocations in shared objects
    #-Wl,--warn-drop-version            # Warn when version information is dropped

    #-Wl,--print-gc-sections            # Print removed unused sections
    #-Wl,--print-icf-sections           # Print folded identical sections
    #-Wl,--stats                        # Print statistics about the linking process

    # Non-Debug: Set linker optimization level
    $<$<NOT:$<CONFIG:Debug>>:-Wl,-O3>

    # Non-Debug: Enable linker relaxation optimizations
    $<$<NOT:$<CONFIG:Debug>>:-Wl,--relax>

    # Non-Debug: Disable incremental linking
    $<$<NOT:$<CONFIG:Debug>>:-Wl,--no-incremental>

    # ASAN is disabled: Detect One Definition Rule violations
    $<$<NOT:$<BOOL:${ENABLE_ASAN}>>:-Wl,--detect-odr-violations>

    # Release and MinSizeRel: Disable generation of unwind info
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:-Wl,--no-ld-generated-unwind-info>
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
