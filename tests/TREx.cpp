#include "TROOT.h"

#include "DataType_test.hh"
#include "ExPad_test.hh"
#include "RTT_test.hh"

// initialization
int nfailed = 0;
int npassed = 0;
int ntest = 0;
bool pass_all_tests = true;

// Testing REx (TREx)
int main(int /*argc*/, char** /*argv*/) {
    // terminate handler
    std::set_terminate([]() {
        printf("FATAL ERROR: test was terminated");
        std::exit(EXIT_FAILURE);
    });

    // use ROOT in batch mode (no need to display canvases)
    gROOT->SetBatch();

    // perform the tests
    // (the tests are placed inside a try-catch block to avoid unhandled exceptions)
    try {
        TestDataType();
        TestRTTConfig();
        TestRTTOutput();
        TestPlotSerializer();
        TestExportManager();
    }
    catch (const std::invalid_argument& ex) {
        printf("Invalid argument: %s\n", ex.what());
        return EXIT_FAILURE;
    }
    catch (const std::runtime_error& ex) {
        printf("Runtime error: %s\n", ex.what());
        return EXIT_FAILURE;
    }
    catch (const std::exception& ex) {
        printf("Error: %s\n", ex.what());
        return EXIT_FAILURE;
    }

    return pass_all_tests ? EXIT_SUCCESS : EXIT_FAILURE;
}
