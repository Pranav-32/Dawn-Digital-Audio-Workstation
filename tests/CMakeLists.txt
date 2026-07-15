add_executable(dawn_tests)

add_dependencies(dawn_tests generate_resources)

target_sources(dawn_tests PRIVATE
        ${GENERATED_DIR}/resources.h
)

add_subdirectory(core)
add_subdirectory(ui)

target_link_libraries(dawn_tests
        PRIVATE
        dawn::core
        dawn::ui
        Catch2::Catch2WithMain
)

include(Catch)

catch_discover_tests(dawn_tests)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(dawn_tests PRIVATE -Wno-c23-extensions)
endif ()