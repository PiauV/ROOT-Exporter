#include "ExPlot_test.hh"
#include "GleExportManager.hh"
#include "GnuplotExportManager.hh"
#include "PlotSerializer.hh"
#include "PyplotExportManager.hh"
#include "macros.hh"

#include "TArrow.h"
#include "TCanvas.h"
#include "TError.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TPad.h"
#include "TROOT.h"
#include "TText.h"

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

    gErrorIgnoreLevel = 1001; // reduce ROOT verbosity (Info) when saving canvas

    TCanvas* c1 = new TCanvas();
    TH1D* h = new TH1D("h", "h;xtitle;ytitle", 50, 0, 10);
    TF1* f = new TF1("f", "gausn", 0, 10);
    f->SetParameters(200, 5, 1);
    h->FillRandom("f", 1000);
    h->Draw();
    f->Draw("same");
    auto arrow = new TArrow(3.8, 40, 6.2, 40, 0.03, "<|>");
    arrow->SetLineColor(kRed);
    arrow->SetFillColor(kRed);
    arrow->Draw();
    auto text = new TText(5, 39, "FWHM");
    text->SetTextAlign(23); // center top
    text->SetTextColor(kRed);
    text->Draw();
    c1->Update();

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
    gr->SetTitle("#Gamma");
    gr->SetLineStyle(2);
    gr->SetLineColor(kRed);
    gr->SetFillColor(kWhite);
    TGraphErrors* gre1 = new TGraphErrors(3, x2, y2, 0, ey2);
    gre1->SetName("gre1_c2");
    gre1->SetTitle("graph1");
    gre1->SetMarkerStyle(20);
    gre1->SetMarkerColor(kBlue);
    gre1->SetLineColor(kBlue);
    gre1->SetFillColor(kWhite);
    TGraphErrors* gre2 = new TGraphErrors(2, x3, y3, ex3, ey3);
    gre2->SetName("gre2_c2");
    gre2->SetTitle("graph2");
    gre2->SetMarkerStyle(25);
    gre2->SetFillColor(kWhite);
    TMultiGraph* mg = new TMultiGraph();
    mg->Add(gr, "l");
    mg->Add(gre1, "p");
    mg->Add(gre2, "pl");
    mg->Draw("a");
    mg->GetXaxis()->SetTitle("#alpha");
    mg->GetYaxis()->SetTitle("#beta");
    c2->BuildLegend(0.65, 0.15, 0.88, 0.35);
    auto ltx = new TLatex(1.2, 4.8, "#alpha+#beta+#gamma");
    ltx->Draw();
    c2->Update();

    // Data export
    try {
        auto dem = std::make_unique<Expad::DataExportManager>();
        dem->ExportPad(c1, "output/test_export");
        // RTT was tested previously --> if files exist, their content should be ok
        SIMPLE_TEST(!gSystem->AccessPathName("output/test_export/h.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/test_export/f.txt"));

        gSystem->mkdir("output/gle");
        auto gle_man = std::make_unique<Expad::GleExportManager>();
        gle_man->SaveInFolder(true);
        gle_man->ExportPad(c1, "output/gle/c1");
        SIMPLE_TEST(!gSystem->AccessPathName("output/gle/c1/h.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/gle/c1/f.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/gle/c1/c1.gle"));
        gle_man->SaveInFolder(false);
        gle_man->SetDataDirectory("data_c2");
        gle_man->ExportPad(c2, "output/gle/c2.gle");
        SIMPLE_TEST(!gSystem->AccessPathName("output/gle/data_c2/gr_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/gle/data_c2/gre1_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/gle/data_c2/gre2_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/gle/c2.gle"));

        gSystem->mkdir("output/gnuplot");
        auto gnuplot_man = std::make_unique<Expad::GnuplotExportManager>();
        gnuplot_man->SaveInFolder(false);
        gnuplot_man->ExportPad(c1, "output/gnuplot/c1");
        SIMPLE_TEST(!gSystem->AccessPathName("output/gnuplot/h.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/gnuplot/f.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/gnuplot/c1.gp"));
        gnuplot_man->ExportPad(c2, "output/gnuplot/c2");
        SIMPLE_TEST(!gSystem->AccessPathName("output/gnuplot/gr_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/gnuplot/gre1_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/gnuplot/gre2_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/gnuplot/c2.gp"));

        gSystem->mkdir("output/python");
        auto pyplot_man = std::make_unique<Expad::PyplotExportManager>();
        pyplot_man->SaveInFolder(false);
        pyplot_man->ExportPad(c1, "output/python/c1");
        SIMPLE_TEST(!gSystem->AccessPathName("output/python/h.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/python/f.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/python/c1.py"));
        pyplot_man->ExportPad(c2, "output/python/c2");
        SIMPLE_TEST(!gSystem->AccessPathName("output/python/gr_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/python/gre1_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/python/gre2_c2.txt"));
        SIMPLE_TEST(!gSystem->AccessPathName("output/python/c2.py"));

        // Prepare next test --> using external tools to render the plots
        // - save plots as PDF using ROOT internal method (for comparison)
        c1->SaveAs("output/c1.pdf");
        c2->SaveAs("output/c2.pdf");
        // - save name of sources in dedicated file
        std::ofstream ofs;
        ofs.open("output/gle.out");
        ofs << "gle/c1/c1.gle\n";
        ofs << "gle/c2.gle\n";
        ofs.close();
        ofs.open("output/gnuplot.out");
        ofs << "gnuplot/c1.gp\n";
        ofs << "gnuplot/c2.gp\n";
        ofs.close();
        ofs.open("output/python.out");
        ofs << "python/c1.py\n";
        ofs << "python/c2.py\n";
        ofs.close();
    }
    catch (const std::exception& e) {
        EXCEPTION_CAUGHT(e);
    }

    delete c1;
    delete c2;

    delete f;
    delete h;
    delete mg;

    END_TEST();
}
