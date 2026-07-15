cmake_minimum_required(VERSION 3.30)
project(DAWN_Audio)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)

add_custom_command(

        OUTPUT ${GENERATED_DIR}/resources.h

        COMMAND
        ${Python3_EXECUTABLE}
        ${PROJECT_SOURCE_DIR}/scripts/generate_resources_header.py
        ${PROJECT_SOURCE_DIR}/resources
        ${GENERATED_DIR}/resources.h

        DEPENDS
        ${PROJECT_SOURCE_DIR}/scripts/generate_resources_header.py

        COMMENT "Generating resources.h"

        VERBATIM
)

add_custom_target(generate_resources
        DEPENDS ${GENERATED_DIR}/resources.h
)

set(BUILD_NAME "DAWN_Studio")
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(juce CONFIG REQUIRED)
find_package(Catch2 3 REQUIRED)

add_subdirectory(src/core)
add_subdirectory(src/ui)

add_subdirectory(tests)

juce_add_gui_app(${BUILD_NAME}
        PRODUCT_NAME "DAWN Audio"
        VERSION "0.1.0"
)

add_dependencies(${BUILD_NAME} generate_resources)

target_sources(${BUILD_NAME} PRIVATE
        src/dawn_main.cpp
        ${GENERATED_DIR}/resources.h
)

target_link_libraries(${BUILD_NAME} PRIVATE
        dawn::ui
        dawn::core

        juce::juce_audio_devices
        juce::juce_events
)

if (MSVC)
    target_compile_options(${BUILD_NAME} PRIVATE /W4)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${BUILD_NAME} PRIVATE -Wno-c23-extensions)
endif ()
