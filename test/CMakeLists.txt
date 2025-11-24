project(KiloTests LANGUAGES CXX)

# --------------- Dependencies ---------------

find_package(GTest REQUIRED)
include(GoogleTest)

# --------------- Tests ---------------

add_executable(Kilo_test)

target_sources(Kilo_test
    PRIVATE
        Editor/Editor.test.cpp
        ScreenBuffer/ScreenBuffer.test.cpp
        TerminalMode/TerminalMode.test.cpp       
)

target_link_libraries(Kilo_test
    PRIVATE
        kilo_lib
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

