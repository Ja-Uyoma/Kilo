cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(Kilo 
    VERSION 0.1.1
    DESCRIPTION "A minimalistic, modal, text editor"
    HOMEPAGE_URL "https://github.com/Ja-Uyoma/Kilo"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---------- Declare library and executable -------------

add_subdirectory(src)

# ---------- Install rules -------------

if (NOT CMAKE_SKIP_INSTALL_RULES)
    include(cmake/install-rules.cmake)
endif()

# ------------ Developer mode ---------------------

if (NOT Kilo_DEVELOPER_MODE)
    return()
elseif (NOT PROJECT_IS_TOP_LEVEL)
    message(AUTHOR_WARNING "Developer mode is intended for developers of Kilo")
endif()

include(cmake/dev-mode.cmake)
