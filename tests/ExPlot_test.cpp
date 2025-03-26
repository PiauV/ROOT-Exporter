#include "ExPlot_test.hh"
#include "PlotSerializer.hh"
#include "macros.hh"

#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"
#include "TPad.h"
#include "TROOT.h"

#include <iostream>

void TestPlotSerializer() {
    BEGIN_TEST();
    gROOT->SetBatch();
    TH1D* h = new TH1D("h", "h;xtitle;ytitle", 50, 0, 10);
    TF1* f = new TF1("f", "gaus", 0, 10);
    f->SetParameters(1, 5, 1);
    h->FillRandom("f", 100);
    TCanvas* c = new TCanvas();
    h->Draw();
    f->Draw("same");
    Expad::PlotSerializer* ps = nullptr;
    try {
        ps = new Expad::PlotSerializer(gPad);
        SIMPLE_TEST(ps->GetNumberOfDatasets() == 2);
        COMPARE_TSTRING(ps->GetDatasetLabel(0), "h");
        COMPARE_TSTRING(ps->GetXaxisTitle(), "xtitle");
        COMPARE_TSTRING(ps->GetYaxisTitle(), "ytitle");
    }
    catch (const std::exception& e) {
        EXCEPTION_CAUGHT(e);
    }

    delete ps;
    delete h;
    delete f;

    END_TEST();
}
