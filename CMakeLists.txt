cmake_minimum_required(VERSION 3.25)

# Only set the C++ standard if it has not been set by someone else
if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

# Set globally to avoid conflicts between -Wpedantic being enabled and
# -std=c++20 and -std=gnu++20, for example when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS OFF)

project(Kilo 
    VERSION 0.1.1
    LANGUAGES CXX
    DESCRIPTION "A minimalistic, modal, text editor"
)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
