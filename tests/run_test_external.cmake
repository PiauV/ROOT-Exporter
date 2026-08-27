macro(gle_test CMD_GLE)
    file(STRINGS "gle.out" files)
    foreach(fin ${files})
        string(REPLACE ".gle" ".pdf" fout ${fin})
        execute_process(COMMAND ${CMD_GLE} -d pdf -o ${fout} ${fin} 
                        RESULT_VARIABLE res
                        ERROR_VARIABLE gle_err
                        )
        # for some reason all gle output goes to stderr
        string(FIND ${gle_err} "error" found_error)
        if((NOT res EQUAL 0) OR (${found_error} GREATER_EQUAL 0))
            # there is an error
            file (WRITE "trex-gle.log" ${gle_err}) # write it to file
            message(${gle_err}) # and print it
            message(FATAL_ERROR "Error running gle (${fin})")
        else()
            message(STATUS "File ${fin} was successfully processed.") 
        endif()
    endforeach()
endmacro()

macro(gnuplot_test CMD_GNUPLOT)
    file(STRINGS "gnuplot.out" files)
    foreach(fin ${files}) 
        # we need to call gnuplot in the directory where the scripts are
        string(REPLACE "gnuplot/" "" filename ${fin}) # remove gnuplot/ dir from file name
        execute_process(COMMAND ${CMD_GNUPLOT} ${filename} WORKING_DIRECTORY "gnuplot" 
                        RESULT_VARIABLE res 
                        ERROR_FILE "trex-gnuplot.log" 
                        ECHO_ERROR_VARIABLE
                        OUTPUT_QUIET
                        )
        if(NOT res EQUAL 0)
            message(FATAL_ERROR "Error running gnuplot (${fin})")
        else()
            message(STATUS "File ${fin} was successfully processed.")
        endif()
    endforeach()
endmacro()

macro(python_test CMD_PYTHON)
    file(STRINGS "python.out" files)
    foreach(fin ${files})
        # we need to call python in the directory where the scripts are
        string(REPLACE "python/" "" filename ${fin}) # remove python/ dir from file name
        execute_process(COMMAND ${CMD_PYTHON} ${filename} WORKING_DIRECTORY "python" 
                        RESULT_VARIABLE res 
                        ERROR_FILE "trex-python.log"
                        ECHO_ERROR_VARIABLE
                        OUTPUT_QUIET
                        )
        if(NOT res EQUAL 0)
            message(FATAL_ERROR "Error running python (${fin})")
        else()
            message(STATUS "File ${fin} was successfully processed.")
        endif()
    endforeach()
endmacro()

if(${TEST} STREQUAL "GLE")
    gle_test(${PROG})
elseif(${TEST} STREQUAL "GNUPLOT")
    gnuplot_test(${PROG})
elseif(${TEST} STREQUAL "PYTHON")
    python_test(${PROG})
else()
    message(FATAL_ERROR "Unknown test : ${TEST}")
endif()
