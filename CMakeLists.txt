cmake_minimum_required(VERSION 3.14)
project(arduino_network VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch cpp_logger (only dependency)
FetchContent_Declare(
    cpp_logger
    GIT_REPOSITORY https://github.com/compilerNayan/cpp_logger.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(cpp_logger)

# Header-only INTERFACE library (no source files); just includes cpp_logger
add_library(arduino_network INTERFACE)

set_target_properties(arduino_network PROPERTIES
    VERSION ${PROJECT_VERSION}
)

target_include_directories(arduino_network INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(TARGET cpp_logger::cpp_logger)
    target_link_libraries(arduino_network INTERFACE cpp_logger::cpp_logger)
elseif(TARGET cpp_logger)
    target_link_libraries(arduino_network INTERFACE cpp_logger)
else()
    FetchContent_GetProperties(cpp_logger)
    if(cpp_logger_SOURCE_DIR)
        target_include_directories(arduino_network INTERFACE
            $<BUILD_INTERFACE:${cpp_logger_SOURCE_DIR}/src>
        )
    endif()
endif()
