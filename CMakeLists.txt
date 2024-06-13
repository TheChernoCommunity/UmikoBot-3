cmake_minimum_required(VERSION 3.15)

#
# General.
#

project(UmikoBot-3 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


#
# Files.
#

set(HPP_CPP_PAIRS
    umiko_bot
)

set(HEADERS
    common

    ${HPP_CPP_PAIRS}
)
set(SOURCES
    main

    commands/general

    ${HPP_CPP_PAIRS}
)

list(TRANSFORM HEADERS PREPEND src/)
list(TRANSFORM HEADERS APPEND .hpp)
list(TRANSFORM SOURCES PREPEND src/)
list(TRANSFORM SOURCES APPEND .cpp)


#
# Dependencies.
#

add_subdirectory(dep/DPP)
add_subdirectory(dep/spdlog)

#
# The executable.
#

add_executable(UmikoBot-3 ${HEADERS} ${SOURCES})
target_include_directories(UmikoBot-3 PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(UmikoBot-3 PRIVATE dpp)
target_link_libraries(UmikoBot-3 PRIVATE spdlog)

#
# Compiler specific configuration.
#

if (MSVC)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT UmikoBot-3)
    set_target_properties(UmikoBot-3 PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    target_compile_options(UmikoBot-3 PRIVATE /W4 /WX) # Extra warnings, treated as errors.
    target_compile_definitions(UmikoBot-3 PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(UmikoBot-3 PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
