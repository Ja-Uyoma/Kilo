add_library(kilo_lib_io OBJECT)

target_sources(kilo_lib_io
    PUBLIC
        File.hpp
        File.cpp
        IO.hpp
        IO.cpp
)

target_link_libraries(kilo_lib_io
    PUBLIC
        kilo_lib_utilities
)

target_include_directories(kilo_lib_io
    PUBLIC
        "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"        
)

target_compile_features(kilo_lib_io
    PUBLIC
        cxx_std_20
)
