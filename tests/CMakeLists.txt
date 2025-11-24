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
        "${PROJECT_SOURCE_DIR}/src/editor/Editor.hpp"
        "${PROJECT_SOURCE_DIR}/src/editor/Editor.cpp"
        Editor/Editor.test.cpp

        "${PROJECT_SOURCE_DIR}/src/terminal/TerminalMode/TerminalMode.hpp"
        "${PROJECT_SOURCE_DIR}/src/terminal/TerminalMode/TerminalMode.cpp"
        TerminalMode/TerminalMode.test.cpp

        "${PROJECT_SOURCE_DIR}/src/utilities/Utilities.hpp"
        "${PROJECT_SOURCE_DIR}/src/utilities/Utilities.cpp"

        "${PROJECT_SOURCE_DIR}/src/io/File.hpp"
        "${PROJECT_SOURCE_DIR}/src/io/File.cpp"

        "${PROJECT_SOURCE_DIR}/src/editor/ScreenBuffer/ScreenBuffer.hpp"
        "${PROJECT_SOURCE_DIR}/src/editor/ScreenBuffer/ScreenBuffer.cpp"
        ScreenBuffer/ScreenBuffer.test.cpp
        
        "${PROJECT_SOURCE_DIR}/src/terminal/Window/Window.hpp"
        "${PROJECT_SOURCE_DIR}/src/terminal/Window/Window.cpp"
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
