add_library(kilo_lib_terminal OBJECT)

target_sources(kilo_lib_terminal
    PUBLIC
        TerminalMode/TerminalMode.hpp
        TerminalMode/TerminalMode.cpp
        Window/Window.hpp
        Window/Window.cpp
)

target_link_libraries(kilo_lib_terminal
    PUBLIC
        kilo_lib_io
        Microsoft.GSL::GSL
)

target_include_directories(kilo_lib_terminal
    ${warning_guard}
    PUBLIC
        "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_compile_features(kilo_lib_terminal
    PUBLIC
        cxx_std_20
)
