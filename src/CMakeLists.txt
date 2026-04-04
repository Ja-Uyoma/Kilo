find_package(Microsoft.GSL REQUIRED)
find_package(fmt REQUIRED)

add_library(Kilo_lib OBJECT)

add_subdirectory(kilo/utilities)
add_subdirectory(kilo/io)
add_subdirectory(kilo/terminal)
add_subdirectory(kilo/editor)

target_sources(Kilo_lib PRIVATE
    "${PROJECT_SOURCE_DIR}/src/kilo/application/application.cpp"
)

target_include_directories(Kilo_lib
    ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_link_libraries(Kilo_lib
    PRIVATE
        Microsoft.GSL::GSL
        fmt::fmt
        Kilo_lib_utilities
        Kilo_lib_io
        Kilo_lib_terminal
        Kilo_lib_editor
)

target_compile_features(Kilo_lib PUBLIC
    cxx_std_20
)

add_executable(Kilo_exe)

target_sources(Kilo_exe PRIVATE
    main.cpp
)

add_executable(Kilo::exe ALIAS Kilo_exe)

set_property(TARGET Kilo_exe PROPERTY OUTPUT_NAME kilo)

target_link_libraries(Kilo_exe PRIVATE
    Kilo_lib
)

target_compile_features(Kilo_exe PRIVATE
    cxx_std_20
)
