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
        "${PROJECT_SOURCE_DIR}/src/Editor/cursor/cursor.hpp"
        Cursor.test.cpp

        "${PROJECT_SOURCE_DIR}/src/Editor/Editor.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/Editor.cpp"
        Editor.test.cpp

        "${PROJECT_SOURCE_DIR}/src/Editor/config/config.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/config/config.cpp"

        "${PROJECT_SOURCE_DIR}/src/Editor/offset/offset.hpp"
        Offset.test.cpp

        "${PROJECT_SOURCE_DIR}/src/Terminal/Terminal.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/Terminal.cpp"

        "${PROJECT_SOURCE_DIR}/src/Terminal/mode/terminal_mode.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/mode/terminal_mode.cpp"
        TerminalMode.test.cpp

        "${PROJECT_SOURCE_DIR}/src/Utilities/Utilities.hpp"
        "${PROJECT_SOURCE_DIR}/src/Utilities/Utilities.cpp"

        "${PROJECT_SOURCE_DIR}/src/Editor/screen_buffer/screen_buffer.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/screen_buffer/screen_buffer.cpp"
        ScreenBuffer.test.cpp
        
        "${PROJECT_SOURCE_DIR}/src/Terminal/window/window.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/window/window.cpp"
        Window.test.cpp
)

target_compile_features(tests
    PRIVATE
        cxx_std_20
)

target_compile_options(tests
    PRIVATE
        -Wall -Wextra -Werror -Wpedantic
)

add_test(NAME tests COMMAND tests)

gtest_discover_tests(tests)
