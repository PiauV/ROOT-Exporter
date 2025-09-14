#include "RTT_test.hh"
#include "ROOTToText.hh"
#include "macros.hh"

#include "TF1.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TMultiGraph.h"
#include "TString.h"
#include "TSystem.h"
#include "TText.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>

void TestRTTConfig() {
    BEGIN_TEST();
    // file extention - default extension
    const TString ext = gRTT->GetFileExtension();
    SIMPLE_TEST(ext.Length() > 1);
    SIMPLE_TEST(ext[0] == '.');
    SIMPLE_TEST(TString(ext(1, ext.Length())).IsAlnum());

    // file extention - correct argument
    COMPARE_TSTRING(gRTT->GetFileExtension(), ".txt");
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
    const TString dir_name1 = "./output";
    const TString dir_name2 = "output";
    gSystem->mkdir(dir_name1);
    gRTT->SetDirectory(dir_name1);
    COMPARE_TSTRING(gRTT->GetDirectory(), dir_name1);
    gRTT->SetDirectory("");
    COMPARE_TSTRING(gRTT->GetDirectory(), "./");
    gRTT->SetDirectory(dir_name2);
    TString rtt_dir(gSystem->UnixPathName(gRTT->GetDirectory())); // small trick otherwise the test fails in Windows
    COMPARE_TSTRING(rtt_dir, dir_name1);
    EXPECTED_EXCEPTION(gRTT->SetDirectory(__FILE__), std::invalid_argument);
    END_TEST();
}

void TestRTTOutput() {
    BEGIN_TEST();
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

    // TH1(D)
    TH1D* h = new TH1D("h", "histo;x;y", N, 0.5, N + 0.5);
    for (int k = 1; k <= N; k++) {
        h->SetBinContent(k, yy[k - 1]);
        h->SetBinError(k, ey[k - 1]);
    }

    // TH2(D)
    TH2D* h2 = new TH2D("h2", "histo2D;x;y;z", N, 0.5, N + 0.5, N, 0.5, N + 0.5);
    for (int k = 1; k <= N; k++) {
        for (int p = 1; p <= N; p++) {
            h2->SetBinContent(k, p, zz[k - 1 + (p - 1) * N]);
        }
    }

    // TH3(D)
    TH3D* h3 = new TH3D("h3", "histo2D;x;y;z", N, 0.5, N + 0.5, N, 0.5, N + 0.5, 2, 0, 1);

    // TGraph
    TGraph* gr = new TGraph(N, xx, yy);
    gr->SetName("gr");
    gr->SetTitle("graph_no_error");

    // TGraphErrors
    TGraphErrors* gre = new TGraphErrors(N, xx, yy, ex, ey);
    gre->SetName("gre");
    gre->SetTitle("graph_with_errors");

    // TMultiGraph
    TMultiGraph* mg = new TMultiGraph("mg", "multigraph");
    mg->Add(gr);
    mg->Add(gre);

    // TGraph2D
    TGraph2D* gr2d = new TGraph2D(N * N);
    gr2d->SetName("gr2d");
    gr2d->SetTitle("2d_graph");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int k = i + j * N;
            gr2d->SetPoint(k, xx[i], xx[j], zz[k]);
        }
    }

    // TF1
    TF1* f = new TF1("func", "x+1", 1, 3);

    // TText
    TText* text = new TText(0, 0, "100");

    gRTT->SetDirectory("./output/test_rtt");
    gRTT->SetFileExtension(".txt");

    // simple saves
    SIMPLE_TEST(gRTT->SaveObject(h));
    SIMPLE_TEST(gRTT->SaveObject(h2));
    SIMPLE_TEST(gRTT->SaveObject(gr));
    SIMPLE_TEST(gRTT->SaveObject(gre));
    SIMPLE_TEST(gRTT->SaveObject(mg));
    SIMPLE_TEST(gRTT->SaveObject(gr2d));
    EXPECTED_EXCEPTION(gRTT->SaveObject(h3), std::invalid_argument);

    // saving with options and filenames
    SIMPLE_TEST(gRTT->SaveObject(h, "h_with_errors", "E"));
    SIMPLE_TEST(gRTT->SaveObject(h, "h_lowedge_and_errors.dat", "EL"));
    SIMPLE_TEST(gRTT->SaveObject(gre, "gre_horizontal_errors.txt", "H"));
    SIMPLE_TEST(gRTT->SaveObject(h2, "h2_columns", "C"));

    // Use a custom writer
    REx::rtt_writer fw = [](const TObject* obj, const TString& /*opt*/, std::ofstream& ofs) {
        auto ff = dynamic_cast<const TF1*>(obj);
        if (ff) {
            ofs << ff->Eval(100) << std::endl;
        }
        else {
            ofs << obj->GetTitle() << std::endl;
        }
    };
    // we use the same dummy function for TF1 and TText objects
    gRTT->AddCustomWriter("TF1", fw);
    gRTT->AddCustomWriter("TText", fw);

    SIMPLE_TEST(gRTT->SaveObject(f, "", "D N3"));    // save f with default writer
    SIMPLE_TEST(gRTT->SaveObject(f, "func_custom")); // use custom writer
    SIMPLE_TEST(gRTT->SaveObject(text, "text"));     // use custom writer (TText is not supported by default)

    gRTT->ClearCustomWriters();

    // read files
    double sum_y = 0;
    double sum_ey = 0;
    double sum_ex = 0;
    double sum_z = 0;
    for (int i = 0; i < N; i++) {
        sum_y += yy[i];
        sum_ey += ey[i];
        sum_ex += ex[i];
        for (int j = 0; j < N; j++) {
            sum_z += zz[i + j * N];
        }
    }

    // gRTT->SetVerbose(true);
    SIMPLE_TEST(check_file_content("./output/test_rtt/h.txt", 2, N, sum_y, 2));
    SIMPLE_TEST(check_file_content("./output/test_rtt/h2.txt", N, N, sum_z, 0));
    SIMPLE_TEST(check_file_content("./output/test_rtt/gr.txt", 2, N, sum_y, 2));
    SIMPLE_TEST(check_file_content("./output/test_rtt/gre.txt", 3, N, sum_y, 2));
    SIMPLE_TEST(check_file_content("./output/test_rtt/gre.txt", 3, N, sum_ey, 3));
    SIMPLE_TEST(check_file_content("./output/test_rtt/mg_gr.txt", 2, N, sum_y, 2));
    SIMPLE_TEST(check_file_content("./output/test_rtt/mg_gre.txt", 3, N, sum_y, 2));
    SIMPLE_TEST(check_file_content("./output/test_rtt/gr2d.txt", 3, N * N, sum_z, 3));
    SIMPLE_TEST(check_file_content("./output/test_rtt/func.txt", 2, 3, 2 + 3 + 4, 2));
    SIMPLE_TEST(check_file_content("./output/test_rtt/h_with_errors.txt", 3, N, sum_y, 2));
    SIMPLE_TEST(check_file_content("./output/test_rtt/h_lowedge_and_errors.dat", 3, N, sum_ey, 3));
    SIMPLE_TEST(check_file_content("./output/test_rtt/gre_horizontal_errors.txt", 4, N, sum_y, 2));
    SIMPLE_TEST(check_file_content("./output/test_rtt/gre_horizontal_errors.txt", 4, N, sum_ex, 3));
    SIMPLE_TEST(check_file_content("./output/test_rtt/gre_horizontal_errors.txt", 4, N, sum_ey, 4));
    SIMPLE_TEST(check_file_content("./output/test_rtt/h2_columns.txt", 3, N * N, sum_z, 3));
    SIMPLE_TEST(check_file_content("./output/test_rtt/text.txt", 1, 1, 100, 0));
    SIMPLE_TEST(check_file_content("./output/test_rtt/func_custom.txt", 1, 1, 101, 0));

    delete h;
    delete h2;
    delete h3;
    delete mg;
    delete gr2d;
    delete f;
    delete text;

    END_TEST();
}

bool check_file_content(const char* filename, int _col, int _lin, double _sum, int _idx_col) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Could not open file" << std::endl;
        return false;
    }
    std::vector<std::vector<double>> in;
    std::string line;
    while (line.size() == 0 || line[0] == '#') {
        getline(ifs, line);
    }
    std::stringstream ss;
    ss.str(line);
    double val;
    while (ss >> val)
        in.push_back({val});
    int ncol = in.size();
    int nline = 1;
    while (getline(ifs, line)) {
        if (line.size() == 0 || line[0] == '#') continue;
        ss.clear();
        ss.str(line);
        int icol = 0;
        while (ss >> val) {
            in[icol++].push_back(val);
        }
        if (icol != ncol) {
            std::cerr << "something is wrong!" << std::endl;
            return false;
        }
        nline++;
    }
    ifs.close();

    if (_col != ncol) {
        std::cerr << "Unexpected number of columns" << std::endl;
        std::cerr << "Expected " << _col << ", found " << ncol << std::endl;
        return false;
    }
    if (_lin != nline) {
        std::cerr << "Unexpected number of lines" << std::endl;
        std::cerr << "Expected " << _lin << ", found " << nline << std::endl;
        return false;
    }

    if (_idx_col >= 0) {
        double sum_col = 0;
        if (_idx_col == 0) {
            // 0 --> sum all contents (=> for 2D matrix)
            for (const auto& vec : in) {
                for (const auto& x : vec) {
                    sum_col += x;
                }
            }
        }
        else {
            if (_idx_col > ncol) {
                std::cerr << "bad sum index" << std::endl;
                return false;
            }
            for (const auto& x : in[_idx_col - 1]) {
                sum_col += x;
            }
        }
        if (fabs(_sum - sum_col) > 1e-6) {
            std::cerr << "Unexpected sum" << std::endl;
            std::cerr << "Expected " << _sum << ", found " << sum_col << std::endl;
            return false;
        }
    }

    return true;
}
