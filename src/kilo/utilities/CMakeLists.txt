add_library(Kilo_lib_utilities OBJECT)

target_sources(Kilo_lib_utilities
    PUBLIC
        Constants.hpp
        Utilities.hpp
        Utilities.cpp
)

target_include_directories(Kilo_lib_utilities
    ${warning_guard}
    PUBLIC
        "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_compile_features(Kilo_lib_utilities
    PUBLIC
        cxx_std_20
)
