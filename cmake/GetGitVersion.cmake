find_package(Git QUIET)

set(GIT_VERSION "0.0.0") # unknown version

if(GIT_FOUND)
  execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  OUTPUT_VARIABLE GIT_TAG
                  OUTPUT_STRIP_TRAILING_WHITESPACE
                  RESULT_VARIABLE GIT_RESULT
  )

  if(GIT_RESULT EQUAL 0)
    # GIT_TAG format should be : v0.0.0
    string(SUBSTRING ${GIT_TAG} 1 -1 GIT_VERSION) # remove leading 'v'
  endif()
endif()
