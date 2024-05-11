add_executable(tests)

find_package(GTest REQUIRED)

target_link_libraries(tests
    PRIVATE
        GTest::gtest_main
        GTest::gmock_main
)

target_include_directories(tests
    PRIVATE
        "${CMAKE_SOURCE_DIR}/includes"
)

target_sources(tests
    PUBLIC
        "${CMAKE_SOURCE_DIR}/includes/Terminal.hpp"
        "${CMAKE_SOURCE_DIR}/includes/AppendBuffer.hpp"
    PRIVATE
        AppendBuffer.test.cpp
        "${CMAKE_SOURCE_DIR}/src/Terminal.cpp"
)

target_compile_features(tests
    PRIVATE
        cxx_std_20
)

target_compile_options(tests
    PRIVATE
        -Wall -Wextra -Werror -Wpedantic
)