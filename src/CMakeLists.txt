add_executable(kilo)

find_package(Microsoft.GSL REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(kilo Microsoft.GSL::GSL fmt::fmt)

target_include_directories(kilo
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)

target_sources(kilo
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Editor/cursor/cursor.hpp"

        "${PROJECT_SOURCE_DIR}/src/Editor/Editor.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/Editor.cpp"
        
        "${PROJECT_SOURCE_DIR}/src/EditorConfig/EditorConfig.hpp"
        "${PROJECT_SOURCE_DIR}/src/EditorConfig/EditorConfig.cpp"

        "${PROJECT_SOURCE_DIR}/src/Editor/Offset.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/Offset.cpp"

        "${PROJECT_SOURCE_DIR}/src/Terminal/Terminal.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/Terminal.cpp"

        "${PROJECT_SOURCE_DIR}/src/TerminalState/TerminalState.hpp"
        "${PROJECT_SOURCE_DIR}/src/TerminalState/TerminalState.cpp"

        "${PROJECT_SOURCE_DIR}/src/Utilities/Utilities.hpp"
        "${PROJECT_SOURCE_DIR}/src/Utilities/Utilities.cpp"
        
        "${PROJECT_SOURCE_DIR}/src/Window/Window.hpp"
        "${PROJECT_SOURCE_DIR}/src/Window/Window.cpp"

        "${PROJECT_SOURCE_DIR}/src/ScreenBuffer/ScreenBuffer.hpp"
        "${PROJECT_SOURCE_DIR}/src/ScreenBuffer/ScreenBuffer.cpp"
    
        main.cpp
)

target_compile_features(kilo
    PRIVATE
        cxx_std_20
)

target_compile_options(kilo
    PRIVATE
        -Wall -Wextra -Werror -Wpedantic
)

target_compile_definitions(kilo
    PRIVATE
        $<IF:$<CONFIG:Debug>,DEBUG,>
)