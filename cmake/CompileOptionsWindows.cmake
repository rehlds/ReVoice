#-------------------------------------------------------------------------------
# C/CXX Flags
#-------------------------------------------------------------------------------

set(CMAKE_C_FLAGS
  "/DWIN32 /D_WINDOWS"
)

set(CMAKE_C_FLAGS_DEBUG
  "/Ob0 /Oi- /Od /Oy- /RTC1 /D_DEBUG"
)

set(CMAKE_C_FLAGS_RELEASE
  "/O2 /Oi /Ob2 /Ot /Oy /DNDEBUG"
)

set(CMAKE_C_FLAGS_MINSIZEREL
  "/O1 /Oi- /Ob1 /Os /Oy /DNDEBUG"
)

set(CMAKE_C_FLAGS_RELWITHDEBINFO
  "/O2 /Oi /Ob1 /Ot /DNDEBUG"
)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")

#-------------------------------------------------------------------------------
# Compile Definitions
#-------------------------------------------------------------------------------

add_compile_definitions(
  # Shared library definitions
  $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:_USRDLL>
  $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:_WINDLL>
  $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:_WINDOWS>

  # Static library definitions
  $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>:_LIB>

  # Enable/disable exception support
  $<$<BOOL:${ENABLE_EXCEPTIONS}>:_HAS_EXCEPTIONS=1>
  $<$<NOT:$<BOOL:${ENABLE_EXCEPTIONS}>>:_HAS_EXCEPTIONS=0>
)

#-------------------------------------------------------------------------------
# Compile Options
#-------------------------------------------------------------------------------

# Code generation
add_compile_options(
  /QIntel-jcc-erratum # Mitigate performance impact of the Intel JCC erratum microcode update
  /Gy                 # Enable function-level linking

  # Non-Debug: Enable string pooling
  $<IF:$<CONFIG:Debug>,/GF-,/GF>

  # Non-Debug: Enable whole-program global data optimization
  $<IF:$<CONFIG:Debug>,/Gw-,/Gw>

  # Non-Debug executables: Optimize for Windows applications
  $<$<AND:$<NOT:$<CONFIG:Debug>>,$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>>:/GA>

  # Enable/disable standard exception handling
  $<$<BOOL:${ENABLE_EXCEPTIONS}>:/EHsc>
  $<$<NOT:$<BOOL:${ENABLE_EXCEPTIONS}>>:/EHs-c->

  # Enable/disable Run-Time Type Information
  $<$<COMPILE_LANGUAGE:CXX>:$<IF:$<BOOL:${ENABLE_RTTI}>,/GR,/GR->>

  # Enable AddressSanitizer instrumentation
  $<$<BOOL:${ENABLE_ASAN}>:/fsanitize=address>
)

# Language
add_compile_options(
  # Non-Debug: Strip unreferenced COMDAT
  $<$<NOT:$<CONFIG:Debug>>:/Zc:inline>
)

# Miscellaneous
add_compile_options(
  /cgthreads${NUM_CORES}  # Thread count for code generation
  /utf-8                  # Set source and execution character sets to UTF-8
  /validate-charset       # Validate the source file charset

  # Debug and RelWithDebInfo: Enable concurrent PDB writes for shared access
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/FS>
)

# Diagnostics
add_compile_options(
  /diagnostics:caret      # Display error diagnostics with caret indicators
  /external:W0            # Suppress warnings from external headers
)

# Debug: Enable Just My Code debugging in Visual Studio
set(CMAKE_VS_JUST_MY_CODE_DEBUGGING $<CONFIG:Debug>)

#-------------------------------------------------------------------------------
# Link Options
#-------------------------------------------------------------------------------

set(CMAKE_EXE_LINKER_FLAGS_DEBUG "/INCREMENTAL")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/INCREMENTAL:NO")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/INCREMENTAL:NO")

set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}")
set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")

set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL}")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")

add_link_options(
  # Debug and RelWithDebInfo: Generate debug information in PDB files
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/DEBUG>

  # Release and MinSizeRel: Disable debug information generation
  $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:/DEBUG:NONE>

  # Non-Debug: Enable Link Time Code Generation
  $<$<NOT:$<CONFIG:Debug>>:/LTCG>

  # Non-Debug: Apply link-time optimizations
  $<$<NOT:$<CONFIG:Debug>>:/OPT:REF,ICF,LBR>

  # Non-Debug: Treat warnings as errors
  $<$<NOT:$<CONFIG:Debug>>:/WX>

  # Enable/disable Safe Exception Handlers
  $<$<BOOL:${ENABLE_EXCEPTIONS}>:/SAFESEH>
  $<$<NOT:$<BOOL:${ENABLE_EXCEPTIONS}>>:/SAFESEH:NO>

  # Enable AddressSanitizer instrumentation
  $<$<BOOL:${ENABLE_ASAN}>:/fsanitize=address>
  $<$<BOOL:${ENABLE_ASAN}>:/INFERASANLIBS>

  # Enable/disable Output detailed linking info
  $<$<BOOL:${ENABLE_LINK_TRACE}>:/VERBOSE:LIB>
)

#-------------------------------------------------------------------------------
# Link Libraries
#-------------------------------------------------------------------------------

# Set the MSVC runtime library variant (debug/release, static/dynamic)
set(CMAKE_MSVC_RUNTIME_LIBRARY
  "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<NOT:$<BOOL:${LINK_STATIC_MSVC_RT}>>:DLL>"
)
