cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")

project(BetterQuests VERSION 1.0.0)

file(GLOB SOURCES
    src/utils/*.cpp
    src/hooks/*.cpp
    src/ui/*.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# CPMAddPackage("gh:FireMario211/dashauth#main")
# target_link_libraries(${PROJECT_NAME} DashAuth)

setup_geode_mod(${PROJECT_NAME})