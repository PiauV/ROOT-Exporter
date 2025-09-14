/**
 * -------------------------
 * Example macro for REx/RTT
 * -------------------------
 *
 * Note : libRTT must be loaded before executing the macro !
 *
 * > gSystem->Load("path/to/libRTT[.dll/.so]")
 * > .x rtt_example.C
 */

void rtt_example() {

    // '.dat' is automatically added to ouput files (default is '.txt')
    gRTT->SetFileExtension("dat");

    // directory where the ouptut files will be written (unless specified otherwise - default is './')
    const TString outdir = "./rtt_output";
    gSystem->mkdir(outdir);
    gRTT->SetDirectory(outdir);

    // add verbosity
    gRTT->SetVerbose(true);

    const int N = 8;
    const double xx[N] = {1, 2, 3, 4, 5, 6, 7, 8};
    const double yy[N] = {1, 1.1, 2.2, 3.3, 5.4, 8.5, 13.6, 21.7};
    const double ey[N] = {0.5, 1, 1.5, 2, 5, 6, 8, 8.5};
    const double ex[N] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8};
    double zz[N * N];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            zz[i + j * N] = xx[i] * yy[j];
        }
    }

    // TH1D
    TH1D* h = new TH1D("h", "histo;x;y", N, 0.5, N + 0.5);
    for (int k = 1; k <= N; k++) {
        h->SetBinContent(k, yy[k - 1]);
        h->SetBinError(k, ey[k - 1]);
    }

    // TH2D
    TH2D* h2 = new TH2D("h2", "histo2D;x;y;z", N, 0.5, N + 0.5, N, 0.5, N + 0.5);
    for (int k = 1; k <= N; k++) {
        for (int p = 1; p <= N; p++) {
            h2->SetBinContent(k, p, zz[k - 1 + (p - 1) * N]);
        }
    }

    // TGraphErrors
    TGraphErrors* gre = new TGraphErrors(N, xx, yy, ex, ey);
    gre->SetName("gre");
    gre->SetTitle("graph_with_errors");

    // simple saves (no option given)
    gRTT->SaveObject(h);
    gRTT->SaveObject(h2);
    gRTT->SaveObject(gre);

    gRTT->PrintOptions();

    // saving with options and filenames
    gRTT->SaveObject(h, "h_with_errors", "E");
    gRTT->SaveObject(h, "h_lowedge_and_errors", "EL");
    gRTT->SaveObject(gre, "gre_horizontal_errors.txt", "H");
    gRTT->SaveObject(h2, "h2_columns", "C");

    delete h;
    delete gre;
    delete h2;
}
