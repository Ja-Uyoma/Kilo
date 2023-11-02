add_executable(unit_tests)

find_package(GTest REQUIRED)

target_link_libraries(unit_tests
    PRIVATE
        gtest::gtest
)

target_include_directories(unit_tests
    PRIVATE
        "${CMAKE_SOURCE_DIR}/includes"
)

target_sources(unit_tests
    PUBLIC
        "${CMAKE_SOURCE_DIR}/includes/TerminalSettings.hpp"
    PRIVATE
        main.cpp
        "${CMAKE_SOURCE_DIR}/src/TerminalSettings.cpp"
)

target_compile_features(unit_tests
    PRIVATE
        cxx_std_20
)

target_compile_options(unit_tests
    PRIVATE
        -Wall -Wextra -Werror -Wpedantic
)