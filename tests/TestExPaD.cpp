#ifndef __CINT__

#include "TApplication.h"
#include "TROOT.h"

#include "DataType_test.hh"
#include "ExPlot_test.hh"
#include "RTT_test.hh"

int nfailed = 0;
int npassed = 0;
int ntest = 0;

void start_tests();

int main(int argc, char** argv) {
    gROOT->SetBatch();
    TApplication theApp("App", &argc, argv);
    start_tests();
    return EXIT_SUCCESS;
}

#endif

void start_tests() {
    TestDataType();
    TestRTTDefault();
    TestRTTOutput();
    TestPlotSerializer();
}
