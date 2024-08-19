add_executable(tests)

find_package(GTest REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(fmt REQUIRED)

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
        "${PROJECT_SOURCE_DIR}/src/Cursor/Cursor.hpp"
        "${PROJECT_SOURCE_DIR}/src/Cursor/Cursor.cpp"
        Cursor.test.cpp

        "${PROJECT_SOURCE_DIR}/src/Editor/Editor.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/Editor.cpp"
        Editor.test.cpp

        "${PROJECT_SOURCE_DIR}/src/EditorConfig/EditorConfig.hpp"
        "${PROJECT_SOURCE_DIR}/src/EditorConfig/EditorConfig.cpp"

        "${PROJECT_SOURCE_DIR}/src/Editor/Offset.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/Offset.cpp"
        Offset.test.cpp

        "${PROJECT_SOURCE_DIR}/src/Terminal/Terminal.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/Terminal.cpp"

        "${PROJECT_SOURCE_DIR}/src/TerminalState/TerminalState.hpp"
        "${PROJECT_SOURCE_DIR}/src/TerminalState/TerminalState.cpp"
        TerminalState.test.cpp

        "${PROJECT_SOURCE_DIR}/src/Utilities/Utilities.hpp"
        "${PROJECT_SOURCE_DIR}/src/Utilities/Utilities.cpp"

        "${PROJECT_SOURCE_DIR}/src/ScreenBuffer/ScreenBuffer.hpp"
        "${PROJECT_SOURCE_DIR}/src/ScreenBuffer/ScreenBuffer.cpp"
        ScreenBuffer.test.cpp
        
        "${PROJECT_SOURCE_DIR}/src/Window/Window.hpp"
        "${PROJECT_SOURCE_DIR}/src/Window/Window.cpp"
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