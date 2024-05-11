add_executable(kilo_tests)

find_package(GTest REQUIRED)

target_link_libraries(kilo_tests
    PRIVATE
        GTest::gtest_main
        GTest::gmock_main
)

target_include_directories(kilo_tests
    PRIVATE
        "${CMAKE_SOURCE_DIR}/includes"
)

target_sources(kilo_tests
    PUBLIC
        "${CMAKE_SOURCE_DIR}/includes/Terminal.hpp"
        "${CMAKE_SOURCE_DIR}/includes/AppendBuffer.hpp"
    PRIVATE
        AppendBuffer.test.cpp
        "${CMAKE_SOURCE_DIR}/src/Terminal.cpp"
)

target_compile_features(kilo_tests
    PRIVATE
        cxx_std_20
)

target_compile_options(kilo_tests
    PRIVATE
        -Wall -Wextra -Werror -Wpedantic
)