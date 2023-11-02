cmake_minimum_required(VERSION 3.25)

project(Kilo 
    VERSION 0.1.1
    LANGUAGES CXX
    DESCRIPTION ""
)

enable_testing()

# Only set the C++ standard if it has not been set by someone else
if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(src)
