add_executable(tests)

find_package(GTest REQUIRED)

target_link_libraries(tests
    PRIVATE
        GTest::gtest_main
        GTest::gmock_main
)

target_include_directories(tests
    PRIVATE
        "${PROJECT_SOURCE_DIR}/includes"
)

target_sources(tests
    PUBLIC
        "${PROJECT_SOURCE_DIR}/includes/Terminal.hpp"
        "${PROJECT_SOURCE_DIR}/includes/WriteBuffer.hpp"
        "${PROJECT_SOURCE_DIR}/includes/Cursor.hpp"
    PRIVATE
        WriteBuffer.test.cpp
        Terminal.test.cpp
        "${PROJECT_SOURCE_DIR}/src/Terminal.cpp"
        Cursor.test.cpp
)

target_compile_features(tests
    PRIVATE
        cxx_std_20
)

target_compile_options(tests
    PRIVATE
        -Wall -Wextra -Werror -Wpedantic
)