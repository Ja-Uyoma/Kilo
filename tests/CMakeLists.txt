add_executable(tests)

find_package(GTest REQUIRED)
find_package(Microsoft.GSL REQUIRED)

target_link_libraries(tests
    PRIVATE
        GTest::gtest_main
        GTest::gmock_main
        Microsoft.GSL::GSL
)

target_include_directories(tests
    PRIVATE
        "${PROJECT_SOURCE_DIR}/includes"
)

target_sources(tests
    PUBLIC
        "${PROJECT_SOURCE_DIR}/includes/WriteBuffer.hpp"
        "${PROJECT_SOURCE_DIR}/includes/Cursor.hpp"
        "${PROJECT_SOURCE_DIR}/includes/Offset.hpp"
        "${PROJECT_SOURCE_DIR}/includes/Window.hpp"
        "${PROJECT_SOURCE_DIR}/includes/TerminalState.hpp"
        "${PROJECT_SOURCE_DIR}/includes/Editor.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/Terminal.hpp"
    PRIVATE
        WriteBuffer.test.cpp
        Cursor.test.cpp
        Offset.test.cpp
        Window.test.cpp
        "${PROJECT_SOURCE_DIR}/src/TerminalState.cpp"
        TerminalState.test.cpp
        Editor.test.cpp
)

target_compile_features(tests
    PRIVATE
        cxx_std_20
)

target_compile_options(tests
    PRIVATE
        -Wall -Wextra -Werror -Wpedantic
)