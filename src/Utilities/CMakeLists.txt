add_library(kilo_lib_utilities OBJECT)

target_sources(kilo_lib_utilities
    PUBLIC
        Constants.hpp
        Utilities.hpp
        Utilities.cpp
)

target_include_directories(kilo_lib_utilities
    PUBLIC
        "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_compile_features(kilo_lib_utilities
    PUBLIC
        cxx_std_20
)
