#ifndef __CINT__

#include "ROOTToText.hh"

#include "TApplication.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraph2D.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

void RTT_test();
bool open_file(const char *filename, int _col, int _lin, double _sum = 0, int _idx_col = -1);

int main(int argc, char **argv){
    gROOT->SetBatch();
    TApplication theApp("App", &argc, argv);
    RTT_test();
    return EXIT_SUCCESS;
}

#endif

#include "macros.h"

void TestRTTDefault(){
    BEGIN_TEST();
    // file extention - default extension
    const TString default = gRTT->GetFileExtension();
    SIMPLE_TEST(default.Length() > 1);
    SIMPLE_TEST(default[0] == '.');
    SIMPLE_TEST(TString(default(1, default.Length())).IsAlnum());

    // file extention - correct argument
    gRTT->SetFileExtension(".t");
    COMPARE_TSTRING(gRTT->GetFileExtension(), ".t");
    gRTT->SetFileExtension("dat");
    COMPARE_TSTRING(gRTT->GetFileExtension(), ".dat");

    // file extention - invalid argument
    EXPECTED_EXCEPTION(gRTT->SetFileExtension(""), std::invalid_argument);
    EXPECTED_EXCEPTION(gRTT->SetFileExtension("."), std::invalid_argument);
    EXPECTED_EXCEPTION(gRTT->SetFileExtension("/"), std::invalid_argument);
    EXPECTED_EXCEPTION(gRTT->SetFileExtension(" "), std::invalid_argument);

    // default directory
    const TString dir_name1 = "./temp_for_test_only";
    const TString dir_name2 = "temp_for_test_only";
    gSystem->mkdir(dir_name1);
    gRTT->SetDirectory(dir_name1);
    COMPARE_TSTRING(gRTT->GetDirectory(), dir_name1);
    gRTT->SetDirectory("");
    COMPARE_TSTRING(gRTT->GetDirectory(), "./");
    gRTT->SetDirectory(dir_name2);
    TString out_dir(gSystem->UnixPathName(gRTT->GetDirectory())); // small trick otherwise the test fails in Windows
    COMPARE_TSTRING(out_dir, dir_name1);
    EXPECTED_EXCEPTION(gRTT->SetDirectory(__FILE__), std::invalid_argument);
    END_TEST();
}

void TestRTTOutput(){

}

void RTT_test(){
    TestRTTDefault();
}
