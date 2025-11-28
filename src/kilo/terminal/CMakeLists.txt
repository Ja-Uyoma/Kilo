add_library(Kilo_lib_terminal OBJECT)

target_sources(Kilo_lib_terminal
    PUBLIC
        TerminalMode/TerminalMode.hpp
        TerminalMode/TerminalMode.cpp
        Window/Window.hpp
        Window/Window.cpp
)

target_link_libraries(Kilo_lib_terminal
    PRIVATE
        Kilo_lib_io
        Microsoft.GSL::GSL
)

target_include_directories(Kilo_lib_terminal
    ${warning_guard}
    PUBLIC
        "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_compile_features(Kilo_lib_terminal
    PUBLIC
        cxx_std_20
)
