#-------------------------------------------------------------------------------
# C/CXX Flags
#-------------------------------------------------------------------------------

set(CMAKE_C_FLAGS_DEBUG
  "-g -fno-omit-frame-pointer"
)

set(CMAKE_C_FLAGS_RELEASE
  "-O3 -fno-stack-protector -fomit-frame-pointer -DNDEBUG"
)

set(CMAKE_C_FLAGS_MINSIZEREL
  "-Os -fno-stack-protector -fomit-frame-pointer -DNDEBUG"
)

set(CMAKE_C_FLAGS_RELWITHDEBINFO
  "-O2 -g -DNDEBUG"
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")

#-------------------------------------------------------------------------------
# Machine Architecture Flag
#-------------------------------------------------------------------------------

function(set_march_flag lang compiler_id compiler_version)
  if(NOT compiler_id OR "${compiler_id}" STREQUAL "")
    return()
  endif()

  if(OPTIMIZE_FOR_CURRENT_CPU)
    set(march_flag "-march=native")
  elseif("${compiler_id}" STREQUAL "GNU" AND "${compiler_version}" VERSION_LESS "11")
    set(march_flag "-march=nehalem") # nehalem is close to x86-64-v2
  elseif("${compiler_id}" STREQUAL "Clang" AND "${compiler_version}" VERSION_LESS "12")
    set(march_flag "-march=nehalem")
  else()
    set(march_flag "-march=x86-64-v2")
  endif()

  string(STRIP "${CMAKE_${lang}_FLAGS} ${march_flag}" march_flag)
  set(CMAKE_${lang}_FLAGS "${march_flag}" PARENT_SCOPE)
endfunction()

set_march_flag("C" "${CMAKE_C_COMPILER_ID}" "${CMAKE_C_COMPILER_VERSION}")
set_march_flag("CXX" "${CMAKE_CXX_COMPILER_ID}" "${CMAKE_CXX_COMPILER_VERSION}")

#-------------------------------------------------------------------------------
# Compile Options
#-------------------------------------------------------------------------------

add_compile_options(
  -m32                # Generate 32-bit code
  -mmmx               # Enable MMX instruction set
  -msse               # Enable SSE instruction set
  -msse2              # Enable SSE2 instruction set
  -msse3              # Enable SSE3 instruction set
  -mssse3             # Enable SSSE3 instruction set
  -msse4              # Enable SSE4 instruction set
  -msse4.1            # Enable SSE4.1 instruction set
  -msse4.2            # Enable SSE4.2 instruction set
  -mfpmath=sse        # Use SSE for floating-point math
  -pipe               # Use pipes rather than intermediate files
  -fdata-sections     # Place data items in separate sections
  -ffunction-sections # Place each function in its own section

  # Link libc++ as the C++ standard library
  $<$<BOOL:${LINK_LIBCPP}>:-stdlib=libc++>

  # Enable AddressSanitizer instrumentation
  $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>

  # Enable UndefinedBehaviorSanitizer instrumentation
  $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>

  # Enable/disable Run-Time Type Information
  $<$<COMPILE_LANGUAGE:CXX>:$<IF:$<BOOL:${ENABLE_RTTI}>,-frtti,-fno-rtti>>

  # Enable/disable exception handling
  $<$<COMPILE_LANGUAGE:CXX>:$<IF:$<BOOL:${ENABLE_EXCEPTIONS}>,-fexceptions,-fno-exceptions>>
)

#-------------------------------------------------------------------------------
# Link Options
#-------------------------------------------------------------------------------

add_link_options(
  -m32                # Generate 32-bit code
  -Wl,--as-needed     # Only link libraries that are actually used
  -Wl,--gc-sections   # Remove unused code sections during linking
  -Wl,--no-undefined  # Report undefined symbols as errors

  # ASAN is disabled: Warn about common symbols
  $<$<NOT:$<BOOL:${ENABLE_ASAN}>>:-Wl,--warn-common>

  # RelWithDebInfo: Compress debug sections using zlib
  # $<$<CONFIG:RelWithDebInfo>:-Wl,--compress-debug-sections=zlib>

  # Release and MinSizeRel: Strip all symbols
  $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:-Wl,--strip-all>

  # Non-Debug: Discard local symbols
  $<$<NOT:$<CONFIG:Debug>>:-Wl,--discard-all>

  # Link libc++ as the C++ standard library
  $<$<BOOL:${LINK_LIBCPP}>:-stdlib=libc++>

  # Enable AddressSanitizer instrumentation
  $<$<BOOL:${ENABLE_ASAN}>:-fsanitize=address>

  # Enable UndefinedBehaviorSanitizer instrumentation
  $<$<BOOL:${ENABLE_UBSAN}>:-fsanitize=undefined>

  # Link libgcc statically
  $<$<BOOL:${LINK_STATIC_GCC}>:-static-libgcc>

  # Link libstdc++ statically
  $<$<BOOL:${LINK_STATIC_STDCPP}>:-static-libstdc++>

  # Enable linker trace output
  $<$<BOOL:${ENABLE_LINK_TRACE}>:-Wl,--trace>
)
