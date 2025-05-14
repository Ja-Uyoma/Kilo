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
        "${PROJECT_SOURCE_DIR}/src/Constants/Constants.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/Cursor/Cursor.hpp"

        "${PROJECT_SOURCE_DIR}/src/Editor/Editor.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/Editor.cpp"
        
        "${PROJECT_SOURCE_DIR}/src/Application/Application.hpp"
        "${PROJECT_SOURCE_DIR}/src/Application/Application.cpp"

        "${PROJECT_SOURCE_DIR}/src/Editor/Offset/Offset.hpp"

        "${PROJECT_SOURCE_DIR}/src/Terminal/TerminalMode/TerminalMode.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/TerminalMode/TerminalMode.cpp"

        "${PROJECT_SOURCE_DIR}/src/File/File.hpp"
        "${PROJECT_SOURCE_DIR}/src/File/File.cpp"

        "${PROJECT_SOURCE_DIR}/src/Utilities/Utilities.hpp"
        "${PROJECT_SOURCE_DIR}/src/Utilities/Utilities.cpp"
        
        "${PROJECT_SOURCE_DIR}/src/Terminal/Window/Window.hpp"
        "${PROJECT_SOURCE_DIR}/src/Terminal/Window/Window.cpp"

        "${PROJECT_SOURCE_DIR}/src/Editor/ScreenBuffer/ScreenBuffer.hpp"
        "${PROJECT_SOURCE_DIR}/src/Editor/ScreenBuffer/ScreenBuffer.cpp"

        IO/IO.hpp
        IO/IO.cpp
    
        main.cpp
)

target_compile_features(kilo
    PRIVATE
        cxx_std_20
)

target_compile_options(kilo
    PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Werror -Wpedantic>
)
