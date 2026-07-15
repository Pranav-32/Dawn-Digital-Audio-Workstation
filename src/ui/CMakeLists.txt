add_library(dawn_ui STATIC)
add_library(dawn::ui ALIAS dawn_ui)

add_dependencies(dawn_ui generate_resources)

target_include_directories(dawn_ui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src
        ${CMAKE_BINARY_DIR}/generated
)

target_sources(dawn_ui PRIVATE
        dawn_window.h
        dawn_window.cpp
)

add_subdirectory(components)
add_subdirectory(controls)
add_subdirectory(theme)

target_link_libraries(dawn_ui PRIVATE dawn::core)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(dawn_ui PRIVATE -Wno-c23-extensions)
endif ()
