add_executable(kilo)

target_include_directories(kilo
    PRIVATE
        "${CMAKE_SOURCE_DIR}/includes"
)

target_sources(kilo
    PUBLIC
        "${CMAKE_SOURCE_DIR}/includes/TerminalSettings.hpp"
    PRIVATE
        main.cpp
        TerminalSettings.cpp
)

target_compile_features(kilo
    PRIVATE
        cxx_std_20
)

target_compile_options(kilo
    PRIVATE
        -Wall -Wextra -Werror -Wpedantic
)

target_compile_definitions(kilo
    PRIVATE
        $<IF:$<CONFIG:Debug>,DEBUG,"">
)