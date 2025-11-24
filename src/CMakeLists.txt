find_package(Microsoft.GSL REQUIRED)
find_package(fmt REQUIRED)

add_library(kilo_lib OBJECT)

add_subdirectory(Utilities)

target_sources(kilo_lib PRIVATE
    "${PROJECT_SOURCE_DIR}/src/Editor/Cursor/Cursor.hpp"

    "${PROJECT_SOURCE_DIR}/src/Editor/Editor.hpp"
    "${PROJECT_SOURCE_DIR}/src/Editor/Editor.cpp"

    "${PROJECT_SOURCE_DIR}/src/Application/Application.hpp"
    "${PROJECT_SOURCE_DIR}/src/Application/Application.cpp"

    "${PROJECT_SOURCE_DIR}/src/Editor/Offset/Offset.hpp"

    "${PROJECT_SOURCE_DIR}/src/Terminal/TerminalMode/TerminalMode.hpp"
    "${PROJECT_SOURCE_DIR}/src/Terminal/TerminalMode/TerminalMode.cpp"

    "${PROJECT_SOURCE_DIR}/src/IO/File.hpp"
    "${PROJECT_SOURCE_DIR}/src/IO/File.cpp"

    "${PROJECT_SOURCE_DIR}/src/Terminal/Window/Window.hpp"
    "${PROJECT_SOURCE_DIR}/src/Terminal/Window/Window.cpp"

    "${PROJECT_SOURCE_DIR}/src/Editor/ScreenBuffer/ScreenBuffer.hpp"
    "${PROJECT_SOURCE_DIR}/src/Editor/ScreenBuffer/ScreenBuffer.cpp"

    "${PROJECT_SOURCE_DIR}/src/IO/IO.hpp"
    "${PROJECT_SOURCE_DIR}/src/IO/IO.cpp"
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
)

target_compile_features(kilo_lib PUBLIC
    cxx_std_20
)

add_executable(kilo_exe)

target_sources(kilo_exe PRIVATE
    main.cpp
)

add_executable(kilo::exe ALIAS kilo_exe)

target_link_libraries(kilo_exe PRIVATE
    kilo_lib
)

target_compile_features(kilo_exe PRIVATE
    cxx_std_20
)

target_compile_options(kilo_exe PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Werror -Wpedantic>
)
