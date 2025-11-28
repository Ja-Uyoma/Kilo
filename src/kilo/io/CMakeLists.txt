add_library(Kilo_lib_io OBJECT)

target_sources(Kilo_lib_io
    PUBLIC
        File.hpp
        File.cpp
        IO.hpp
        IO.cpp
)

target_link_libraries(Kilo_lib_io
    PRIVATE
        Kilo_lib_utilities
)

target_include_directories(Kilo_lib_io
    ${warning_guard}
    PUBLIC
        "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_compile_features(Kilo_lib_io
    PUBLIC
        cxx_std_20
)
