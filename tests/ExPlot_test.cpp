#include "ExPlot_test.hh"
#include "DataExportManager.hh"
#include "GleExportManager.hh"
#include "PlotSerializer.hh"
#include "macros.hh"

#include "TCanvas.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TPad.h"
#include "TROOT.h"

#include <iostream>
#include <memory>

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
        COMPARE_TSTRING(ps->GetDatasetTitle(0), "h");
        COMPARE_TSTRING(ps->GetDatasetTitle(1), "gaus");
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
        COMPARE_TSTRING(ps->GetDatasetTitle(0), "h"); // should still be 'h' (legend built before the title was changed)
        COMPARE_TSTRING(ps->GetDatasetTitle(1), "gaus");
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
    TGraphErrors* gre = new TGraphErrors(2, x, y2, 0, ey);
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
        COMPARE_TSTRING(ps->GetPlotTitle(), "title");
        COMPARE_TSTRING(ps->GetXaxisTitle(), "x");
        COMPARE_TSTRING(ps->GetYaxisTitle(), "y");
        SIMPLE_TEST(ps->GetLegendPosition() > 0);
        COMPARE_TSTRING(ps->GetDatasetTitle(0), "graph1");
        COMPARE_TSTRING(ps->GetDatasetTitle(1), "graph2");
        COMPARE_TSTRING(ps->GetDatasetTitle(2), "function");
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

void TestExportManager() {
    BEGIN_TEST();
    TCanvas* c1 = new TCanvas();
    TH1D* h = new TH1D("h", "h;xtitle;ytitle", 50, 0, 10);
    TF1* f = new TF1("f", "gausn", 0, 10);
    f->SetParameters(200, 5, 1);
    h->FillRandom("f", 1000);
    h->Draw();
    f->Draw("same");

    TCanvas* c2 = new TCanvas();
    double x1[5] = {1, 2, 3, 4, 5};
    double y1[5] = {1, 2, 3, 4, 5};
    double x2[5] = {1, 2, 5};
    double y2[3] = {1, 2, 4};
    double ey2[3] = {1, 1, 1};
    double x3[2] = {3, 4};
    double y3[2] = {2.5, 3};
    double ey3[2] = {0.5, 1.5};
    double ex3[2] = {0.5, 0.5};
    TGraph* gr = new TGraph(5, x1, y1);
    gr->SetName("gr_c2");
    gr->SetMarkerStyle(2);
    gr->SetLineColor(kRed);
    TGraphErrors* gre1 = new TGraphErrors(3, x2, y2, 0, ey2);
    gre1->SetName("gre1_c2");
    gre1->SetMarkerStyle(20);
    gre1->SetMarkerColor(kBlue);
    TGraphErrors* gre2 = new TGraphErrors(2, x3, y3, ex3, ey3);
    gre2->SetName("gre2_c2");
    gre2->SetMarkerStyle(25);
    gr->Draw("al");
    gr->GetXaxis()->SetTitle("axis with #Delta symbol");
    gre1->Draw("p same");
    gre2->Draw("pl same");
    c2->Update();

    // Data export
    try {
        auto dem = std::make_unique<Expad::DataExportManager>();
        dem->ExportPad(c1, "output/test_export");
        // RTT was tested previously --> if files exist, their content should be ok
        SIMPLE_TEST(!gSystem->AccessPathName("output/test_export/h.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/test_export/f.txt"));

        auto gle_man = std::make_unique<Expad::GleExportManager>();
        gle_man->SaveInFolder(true);
        gle_man->ExportPad(c1, "output/c1");
        SIMPLE_TEST(!gSystem->AccessPathName("output/c1/h.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/c1/f.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/c1/c1.gle"));
        gle_man->SaveInFolder(false);
        gle_man->SetDataDirectory("data_c2");
        gle_man->ExportPad(c2, "output/c2.gle");
        SIMPLE_TEST(!gSystem->AccessPathName("output/data_c2/gr_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/data_c2/gre1_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/data_c2/gre2_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/c2.gle"));
    }
    catch (const std::exception& e) {
        EXCEPTION_CAUGHT(e);
    }

    delete c1;
    delete c2;

    delete f;
    delete h;
    delete gr;
    delete gre1;
    delete gre2;

    END_TEST();
}
