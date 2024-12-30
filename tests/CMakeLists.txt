add_executable(tests)

find_package(GTest REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(fmt REQUIRED)

include(GoogleTest)

target_link_libraries(tests
    PRIVATE
        GTest::gtest_main
        GTest::gmock_main
        Microsoft.GSL::GSL
        fmt::fmt
)

target_include_directories(tests
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)

target_sources(tests
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Editor/Editor.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/Editor.cpp"
        Editor/Editor.test.cpp

        "${PROJECT_SOURCE_DIR}/src/Terminal/Terminal.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/Terminal.cpp"

        "${PROJECT_SOURCE_DIR}/src/Terminal/mode/terminal_mode.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/mode/terminal_mode.cpp"
        TerminalMode/TerminalMode.test.cpp

        "${PROJECT_SOURCE_DIR}/src/Utilities/Utilities.hpp"
        "${PROJECT_SOURCE_DIR}/src/Utilities/Utilities.cpp"

        "${PROJECT_SOURCE_DIR}/src/Terminal/File.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/File.cpp"

        "${PROJECT_SOURCE_DIR}/src/ScreenBuffer/ScreenBuffer.hpp"
        "${PROJECT_SOURCE_DIR}/src/ScreenBuffer/ScreenBuffer.cpp"
        ScreenBuffer/ScreenBuffer.test.cpp
        
        "${PROJECT_SOURCE_DIR}/src/Terminal/window/window.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/window/window.cpp"
        Window/Window.test.cpp

        "${PROJECT_SOURCE_DIR}/src/Termios/Termios.hpp"
        "${PROJECT_SOURCE_DIR}/src/Termios/Termios.cpp"
        Termios/Termios.test.cpp
)

target_compile_features(tests
    PRIVATE
        cxx_std_20
)

target_compile_options(tests
    PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Werror -Wpedantic>
)

add_test(NAME tests COMMAND tests)

gtest_discover_tests(tests)
