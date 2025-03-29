#include "TROOT.h"

#include "DataType_test.hh"
#include "ExPlot_test.hh"
#include "RTT_test.hh"

// initialization
int nfailed = 0;
int npassed = 0;
int ntest = 0;
bool pass_all_tests = true;

int main(int argc, char** argv) {
    // terminate handler (e.g., in case of unhandled exception)
    std::set_terminate([]() {
        // I would prefer to rethrow unhandled exception (if any), but there is a MVSC bug (fixed in Visual Studio 2019)
        // see https://developercommunity.visualstudio.com/t/msconnect-3133821-stdterminate-and-stdcurrent-exce/246253
        // so for now let's just print minimal information
        printf("FATAL ERROR: test was terminated (there could be an uncaught exception)");
        std::exit(EXIT_FAILURE);
    });

    // use ROOT in batch mode (no need to display canvases)
    gROOT->SetBatch();

    // perform the tests
    TestDataType();
    TestRTTConfig();
    TestRTTOutput();
    TestPlotSerializer();

    return pass_all_tests ? EXIT_SUCCESS : EXIT_FAILURE;
}
