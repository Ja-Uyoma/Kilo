# This function enforces out-of-source builds
function(EnforceOutOfSourceBuilds)
    # Make sure the user doesn't play dirty with symlinks
    get_filename_component(srcDir "${CMAKE_SOURCE_DIR}" REALPATH)
    get_filename_component(binaryDir "${CMAKE_BINARY_DIR}" REALPATH)

    # Prevent in-source builds
    if("${srcDir}" STREQUAL "${binaryDir}")
        message("##################################################################")
        message("WARNING: in-source builds are not allowed!")
        message("Please create a separate build directory and run cmake from there.")
        message("##################################################################")
        message(FATAL_ERROR "Quitting configuration")
    endif()
endfunction()

EnforceOutOfSourceBuilds()
