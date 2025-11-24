find_package(Microsoft.GSL REQUIRED)
find_package(fmt REQUIRED)

add_library(kilo_lib OBJECT)

add_subdirectory(kilo/utilities)
add_subdirectory(kilo/io)
add_subdirectory(kilo/terminal)
add_subdirectory(kilo/editor)

target_sources(kilo_lib PRIVATE
    "${PROJECT_SOURCE_DIR}/src/kilo/Application/Application.hpp"
    "${PROJECT_SOURCE_DIR}/src/kilo/Application/Application.cpp"
)

target_include_directories(kilo_lib
    ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)

target_link_libraries(kilo_lib PRIVATE
    Microsoft.GSL::GSL
    fmt::fmt
    kilo_lib_utilities
    kilo_lib_io
    kilo_lib_terminal
    kilo_lib_editor
)

target_compile_features(kilo_lib PUBLIC
    cxx_std_20
)

add_executable(kilo_exe)

target_sources(kilo_exe PRIVATE
    main.cpp
)

add_executable(kilo::exe ALIAS kilo_exe)

set_property(TARGET kilo_exe PROPERTY OUTPUT_NAME kilo)

target_link_libraries(kilo_exe PRIVATE
    kilo_lib
)

target_compile_features(kilo_exe PRIVATE
    cxx_std_20
)

target_compile_options(kilo_exe PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Werror -Wpedantic>
)
