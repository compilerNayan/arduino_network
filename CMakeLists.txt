cmake_minimum_required(VERSION 3.14)
project(arduino_network VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch springbootplusplus
FetchContent_Declare(
    springbootplusplus
    GIT_REPOSITORY https://github.com/compilerNayan/springbootplusplus.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(springbootplusplus)

# Header-only INTERFACE library (no source files); just includes cpp_logger
add_library(arduino_network INTERFACE)

set_target_properties(arduino_network PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_include_directories(arduino_network INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Link to springbootplusplus
# Try the namespaced target first, fallback to non-namespaced
if(TARGET springbootplusplus::springbootplusplus)
    target_link_libraries(arduino_network INTERFACE springbootplusplus::springbootplusplus)
elseif(TARGET springbootplusplus)
    target_link_libraries(arduino_network INTERFACE springbootplusplus)
else()
    # Fallback: manually add include directory if target doesn't exist
    FetchContent_GetProperties(springbootplusplus)
    if(springbootplusplus_SOURCE_DIR)
        target_include_directories(arduino_network INTERFACE
            $<BUILD_INTERFACE:${springbootplusplus_SOURCE_DIR}/src>
        )
    endif()
endif()

# Note: For INTERFACE libraries, we don't need to add header files to target_sources
# The include directories are sufficient for consumers to find the headers