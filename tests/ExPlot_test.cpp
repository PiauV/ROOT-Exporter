#include "ExPlot_test.hh"
#include "PlotSerializer.hh"
#include "macros.hh"

#include "TCanvas.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TMultigraph.h"
#include "TPad.h"
#include "TROOT.h"

#include <iostream>

void TestPlotSerializer() {
    BEGIN_TEST();

    Expad::PlotSerializer* ps = nullptr;

    // CASE 0 - empty canvas
    TCanvas* c0 = new TCanvas();
    c0->Draw();
    EXPECTED_EXCEPTION(ps = new Expad::PlotSerializer(gPad), std::runtime_error);
    delete ps;
    ps = nullptr;

    // CASE 1 - histo & func
    TCanvas* c1 = new TCanvas();
    TH1D* h = new TH1D("h", "h;xtitle;ytitle", 50, 0, 10);
    TF1* f = new TF1("f", "gaus", 0, 10);
    f->SetParameters(1, 5, 1);
    h->FillRandom("f", 100);
    h->Draw();
    f->Draw("same");
    c1->Update(); // necessary to get the title in the list of primitives
    try {
        ps = new Expad::PlotSerializer(gPad);
        SIMPLE_TEST(ps->GetNumberOfDatasets() == 2);
        COMPARE_TSTRING(ps->GetDatasetProperties(0).label, "h");
        COMPARE_TSTRING(ps->GetDatasetProperties(1).label, "gaus");
        SIMPLE_TEST(ps->GetLegendPosition() == 0);
        COMPARE_TSTRING(ps->GetPlotTitle(), "h");
        COMPARE_TSTRING(ps->GetXaxisTitle(), "xtitle");
        COMPARE_TSTRING(ps->GetYaxisTitle(), "ytitle");
    }
    catch (const std::exception& e) {
        EXCEPTION_CAUGHT(e);
    }
    gPad->BuildLegend();
    h->SetTitle("title");
    c1->Update();
    try {
        ps->Restart();
        SIMPLE_TEST(ps->GetNumberOfDatasets() == 2);
        SIMPLE_TEST(ps->GetLegendPosition() > 0);
        COMPARE_TSTRING(ps->GetDatasetProperties(0).label, "h"); // should still be 'h' (legend built before the title was changed)
        COMPARE_TSTRING(ps->GetDatasetProperties(1).label, "gaus");
        COMPARE_TSTRING(ps->GetPlotTitle(), "title"); // title has changed
    }
    catch (const std::exception& e) {
        EXCEPTION_CAUGHT(e);
    }

    delete ps;
    ps = nullptr;
    delete c1;

    // CASE 2 - multigraph
    TCanvas* c2 = new TCanvas();
    Double_t x[2] = {1, 2};
    Double_t y1[2] = {3, 4};
    Double_t y2[2] = {2, 5};
    Double_t ey[2] = {1, 2};
    TGraph* gr = new TGraph(2, x, y1);
    gr->SetLineWidth(2);
    TGraphErrors* gre = new TGraphErrors(2, x, y2, 0, ey);
    gre->SetLineWidth(1);
    TMultiGraph* mg = new TMultiGraph();
    mg->SetTitle("title;x;y");
    mg->Add(gr);
    mg->Add(gre);
    mg->Draw("ap");
    TLegend* legend = new TLegend(0.5, 0.6, 0.9, 0.9);
    legend->AddEntry(gr, "graph1");
    legend->AddEntry(gre, "graph2");
    legend->AddEntry(f, "function");
    f->Draw("same");
    legend->Draw();
    c2->Update(); // necessary to get the title in the list of primitives
    try {
        ps = new Expad::PlotSerializer(gPad);
        SIMPLE_TEST(ps->GetNumberOfDatasets() == 3);
        SIMPLE_TEST(ps->GetDatasetProperties(0).line.size == 2);
        SIMPLE_TEST(ps->GetDatasetProperties(1).line.size == 1);
        COMPARE_TSTRING(ps->GetPlotTitle(), "title");
        COMPARE_TSTRING(ps->GetXaxisTitle(), "x");
        COMPARE_TSTRING(ps->GetYaxisTitle(), "y");
        SIMPLE_TEST(ps->GetLegendPosition() > 0);
        COMPARE_TSTRING(ps->GetDatasetProperties(0).label, "graph1");
        COMPARE_TSTRING(ps->GetDatasetProperties(1).label, "graph2");
        COMPARE_TSTRING(ps->GetDatasetProperties(2).label, "function");
    }
    catch (const std::exception& e) {
        EXCEPTION_CAUGHT(e);
    }

    delete ps;
    delete c2;

    delete mg; // gr and gre are owned by the multigraph (they should not be deleted !)
    delete h;
    delete f;
    delete legend;
    END_TEST();
}
