#ifndef __CINT__

#include "TApplication.h"
#include "TROOT.h"

#include "DataType_test.hh"
#include "ExPlot_test.hh"
#include "RTT_test.hh"

int nfailed = 0;
int npassed = 0;
int ntest = 0;
bool pass_all_tests = true;

void start_tests();

int main(int argc, char** argv) {
    gROOT->SetBatch();
    TApplication theApp("App", &argc, argv);
    start_tests();
    return pass_all_tests ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif

void start_tests() {
    TestDataType();
    TestRTTDefault();
    TestRTTOutput();
    TestPlotSerializer();
}
