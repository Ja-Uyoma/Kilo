install(
    TARGETS Kilo_exe
    RUNTIME COMPONENT Kilo_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
