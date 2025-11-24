add_library(Kilo_lib_editor OBJECT)

target_sources(Kilo_lib_editor
    PUBLIC
        Cursor/Cursor.hpp
        Offset/Offset.hpp
        ScreenBuffer/ScreenBuffer.hpp
        ScreenBuffer/ScreenBuffer.cpp
        Editor.hpp
        Editor.cpp
)

target_link_libraries(Kilo_lib_editor
    PUBLIC
        kilo_lib_io
        kilo_lib_terminal
        kilo_lib_utilities
        Microsoft.GSL::GSL
)

target_include_directories(Kilo_lib_editor
    ${warning_guard}
    PUBLIC
        "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    PRIVATE
        "\$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
)

target_compile_features(Kilo_lib_editor
    PUBLIC
        cxx_std_20
)
