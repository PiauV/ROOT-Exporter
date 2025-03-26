add_executable(TestExPaD 
        TestExPaD.cpp 
        macros.hh
        DataType_test.cpp 
        DataType_test.hh 
        RTT_test.cpp
        RTT_test.hh
        ExPlot_test.cpp
        ExPlot_test.hh
)

target_link_libraries(TestExPaD PUBLIC libROOTToText libExPlot libGraf libGpad)

if (MSVC)
        # in Windows, it is necessary to copy the shared library (dll) to the same folder as the executable
        add_custom_command(
                TARGET TestExPaD
                COMMAND ${CMAKE_COMMAND} -E copy
                        $<TARGET_FILE:libROOTToText>
                        $<TARGET_FILE:libExPlot>
                        $<TARGET_FILE_DIR:TestExPaD>
                DEPENDS libROOTToText libExPlot TestExPaD
        )
endif()
