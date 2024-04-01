add_executable(kilo_tests)

find_package(Catch2 REQUIRED)

target_link_libraries(kilo_tests
    PRIVATE
        Catch2::Catch2WithMain
)

target_include_directories(kilo_tests
    PRIVATE
        "${CMAKE_SOURCE_DIR}/includes"
)

target_sources(kilo_tests
    PUBLIC
        "${CMAKE_SOURCE_DIR}/includes/Terminal.hpp"
    PRIVATE
        main.cpp
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