if(NOT PROJECT_IS_TOP_LEVEL)
    option(MyProject_ENABLE_CACHE "Enable ccache" OFF)
else()
    option(MyProject_ENABLE_CACHE "Enable ccache" ON)
endif()

if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(MyProject_ENABLE_CACHE)
endif()

macro(MyProjectLocalOptions)
    if(PROJECT_IS_TOP_LEVEL)
        include(cmake/StandardProjectSettings.cmake)
    endif()

    if(MyProject_ENABLE_CACHE)
        include(cmake/Cache.cmake)
        EnableCache()
    endif()
    
endmacro()
