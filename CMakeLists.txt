cmake_minimum_required(VERSION 3.14)
project(03_device_connectivity VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch 01_osal (osal/Core.h, osal/WiFi.h)
FetchContent_Declare(
    01_osal
    GIT_REPOSITORY https://github.com/compilerNayan/01_osal.git
    GIT_TAG        main
)

# Fetch 02_cpp_utils (helper utilities)
FetchContent_Declare(
    02_cpp_utils
    GIT_REPOSITORY https://github.com/compilerNayan/02_cpp_utils.git
    GIT_TAG        main
)

# Fetch springbootplusplus
FetchContent_Declare(
    springbootplusplus
    GIT_REPOSITORY https://github.com/compilerNayan/springbootplusplus.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(01_osal 02_cpp_utils springbootplusplus)

# Header-only INTERFACE library (no source files); just includes cpp_logger
add_library(03_device_connectivity INTERFACE)

set_target_properties(03_device_connectivity PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_include_directories(03_device_connectivity INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Link to 01_osal (provides osal/OSAL_Core.h, osal/OSAL_WiFi.h)
if(TARGET 01_osal::01_osal)
    target_link_libraries(03_device_connectivity INTERFACE 01_osal::01_osal)
elseif(TARGET 01_osal)
    target_link_libraries(03_device_connectivity INTERFACE 01_osal)
endif()

# Link to 02_cpp_utils (provides cpp_utils/StringUtils.h etc.)
if(TARGET 02_cpp_utils::02_cpp_utils)
    target_link_libraries(03_device_connectivity INTERFACE 02_cpp_utils::02_cpp_utils)
elseif(TARGET 02_cpp_utils)
    target_link_libraries(03_device_connectivity INTERFACE 02_cpp_utils)
endif()

# Link to springbootplusplus
# Try the namespaced target first, fallback to non-namespaced
if(TARGET springbootplusplus::springbootplusplus)
    target_link_libraries(03_device_connectivity INTERFACE springbootplusplus::springbootplusplus)
elseif(TARGET springbootplusplus)
    target_link_libraries(03_device_connectivity INTERFACE springbootplusplus)
else()
    # Fallback: manually add include directory if target doesn't exist
    FetchContent_GetProperties(springbootplusplus)
    if(springbootplusplus_SOURCE_DIR)
        target_include_directories(03_device_connectivity INTERFACE
            $<BUILD_INTERFACE:${springbootplusplus_SOURCE_DIR}/src>
        )
    endif()
endif()

# Note: For INTERFACE libraries, we don't need to add header files to target_sources
# The include directories are sufficient for consumers to find the headers