add_library(dawn_core STATIC)
add_library(dawn::core ALIAS dawn_core)

add_dependencies(dawn_core generate_resources)

target_include_directories(dawn_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src
        ${CMAKE_BINARY_DIR}/generated
)

target_sources(dawn_core PRIVATE
        logger.h
)

add_subdirectory(resource)

target_link_libraries(dawn_core PUBLIC juce::juce_gui_basics juce::juce_core)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(dawn_core PRIVATE -Wno-c23-extensions)
endif ()