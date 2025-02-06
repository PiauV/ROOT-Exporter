add_executable(TestRTT RTT_test.cpp macros.h)

target_link_libraries(TestRTT PUBLIC libROOTToText)

if (MSVC)
        # in Windows, it is necessary to copy the shared library (dll) to the same folder as the executable
        add_custom_command(
                TARGET TestRTT
                COMMAND ${CMAKE_COMMAND} -E copy
                        $<TARGET_FILE:libROOTToText>
                        $<TARGET_FILE_DIR:TestRTT>
                DEPENDS libROOTToText TestRTT
        )
endif()
