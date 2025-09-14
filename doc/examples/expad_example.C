/**
 * ---------------------------
 * Example macro for REx/ExPad
 * ---------------------------
 *
 * Note : libExPad must be loaded before executing the macro !
 *
 * > gSystem->Load("path/to/libExPad[.dll/.so]")
 * > .x expad_example.C
 */

void expad_example() {
    gSystem->mkdir("./expad_output");

    /// Create a canvas
    TCanvas* c1 = new TCanvas();
    TH1D* h = new TH1D("hist", "hist;xtitle;ytitle", 50, 0, 10);
    TF1* f = new TF1("f", "gausn", 0, 10);
    f->SetParameters(200, 5, 1);
    f->SetLineStyle(kDashed);
    h->FillRandom("f", 1000);
    h->Draw();
    f->Draw("same");
    c1->BuildLegend(0.65, 0.15, 0.88, 0.35);
    auto arrow = new TArrow(3.8, 40, 6.2, 40, 0.03, "<|>");
    arrow->SetLineColor(kRed);
    arrow->SetFillColor(kRed);
    arrow->Draw();
    auto text = new TText(5, 39, "FWHM");
    text->SetTextAlign(23); // center top
    text->SetTextColor(kRed);
    text->Draw();
    c1->Update();
    c1->SaveAs("expad_output/c1.pdf");

    /// Use ExPad clases to export pad in different languages

    // Data export (only export datasets)
    auto dem = new REx::DataExportManager();
    dem->SetVerbose(true);
    dem->ExportPad(c1, "expad_output/only_data");

    // GLE
    gSystem->mkdir("expad_output/gle");
    auto gle_man = new REx::GleExportManager();
    gle_man->SetVerbose(true);
    gle_man->ExportPad(c1, "expad_output/gle/c1.gle");

    // gnuplot
    gSystem->mkdir("expad_output/gnuplot");
    auto gnuplot_man = new REx::GnuplotExportManager();
    gnuplot_man->SetVerbose(true);
    gnuplot_man->ExportPad(c1, "expad_output/gnuplot/c1.gp");

    // python
    gSystem->mkdir("expad_output/python");
    auto pyplot_man = new REx::PyplotExportManager();
    pyplot_man->SetVerbose(true);
    pyplot_man->ExportPad(c1, "expad_output/python/c1.py");
}
