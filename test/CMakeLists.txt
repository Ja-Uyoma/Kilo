project(KiloTests LANGUAGES CXX)

# --------------- Dependencies ---------------

find_package(GTest REQUIRED)
include(GoogleTest)

# --------------- Tests ---------------

add_executable(Kilo_test)

target_sources(Kilo_test
    PRIVATE
        src/kilo/editor/editor_config/editor_config.test.cpp
        src/kilo/editor/append_buffer/append_buffer.test.cpp
        src/kilo/terminal/terminal_mode/terminal_mode.test.cpp
        src/kilo/terminal/window_size/window_size.test.cpp
        src/kilo/editor/erow/erow.test.cpp
)

target_link_libraries(Kilo_test
    PRIVATE
        Kilo_lib
        GTest::gtest_main
        GTest::gmock_main
)

target_compile_features(Kilo_test
    PRIVATE
        cxx_std_20
)

add_test(NAME Kilo_test COMMAND Kilo_test)

gtest_discover_tests(Kilo_test)

# --------------- End-of-file commands ---------------

add_folders(Test)

