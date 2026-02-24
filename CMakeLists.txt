cmake_minimum_required(VERSION 3.13)
project(rustifyC)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. FIND LLVM
execute_process(COMMAND brew --prefix llvm@15 OUTPUT_VARIABLE LLVM_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_PREFIX_PATH "${LLVM_PREFIX}/lib/cmake/llvm")

find_package(LLVM 15 REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. CREATE THE PLUGIN (Using add_library MODULE instead of the wrapper)
# This gives us more control over the Mac linker settings
add_library(rustifyC MODULE src/rustifyC.cpp)

# 3. MAC-SPECIFIC FIXES
if(APPLE)
    # Allow undefined symbols (they are provided by the executable 'opt' at runtime)
    target_link_options(rustifyC PRIVATE -Wl,-undefined,dynamic_lookup)
    
    # Don't put "lib" prefix on the file (makes it rustifyC.dylib, not librustifyC.dylib)
    set_target_properties(rustifyC PROPERTIES PREFIX "")
endif()