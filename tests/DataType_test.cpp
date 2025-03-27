#include "DataType_test.hh"
#include "DataType.hh"
#include "macros.hh"

#include "TF1.h"
#include "TF12.h"
#include "TF2.h"
#include "TFrame.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TPaveText.h"
#include "TProfile.h"

#include <iostream>

void TestDataType() {
    BEGIN_TEST();
    TH1S* h1 = new TH1S();
    TH2D* h2 = new TH2D();
    TH3F* h3 = new TH3F();
    TProfile* pf = new TProfile();
    TGraph* gr = new TGraph();
    TGraph* gre = new TGraphErrors();
    TGraph2D* gr2D = new TGraph2D();
    TMultiGraph* mg = new TMultiGraph();
    TF1* f1 = new TF1();
    TF1* f12 = new TF12();
    TF2* f2 = new TF2();
    TLegend* leg = new TLegend();
    TLatex* ltx = new TLatex();
    TPaveText* txt = new TPaveText();
    TFrame* frm = new TFrame();

    // histograms
    SIMPLE_TEST(Expad::GetDataType(h1) == Expad::Histo1D);
    SIMPLE_TEST(Expad::GetDataType(h2) == Expad::Histo2D);
    SIMPLE_TEST(Expad::GetDataType(h3) == Expad::Histo3D);
    SIMPLE_TEST(Expad::GetDataType(pf) == Expad::Histo1D);

    // graphs
    SIMPLE_TEST(Expad::GetDataType(gr) == Expad::Graph1D);
    SIMPLE_TEST(Expad::GetDataType(gre) == Expad::Graph1D);
    SIMPLE_TEST(Expad::GetDataType(gr2D) == Expad::Graph2D);
    SIMPLE_TEST(Expad::GetDataType(mg) == Expad::MultiGraph1D);

    // functions
    SIMPLE_TEST(Expad::GetDataType(f1) == Expad::Function1D);
    SIMPLE_TEST(Expad::GetDataType(f12) == Expad::Function1D);
    SIMPLE_TEST(Expad::GetDataType(f2) == Expad::Function2D);

    // others (not supported for now)
    SIMPLE_TEST(Expad::GetDataType(leg) == Expad::Undefined);
    SIMPLE_TEST(Expad::GetDataType(txt) == Expad::Undefined);
    SIMPLE_TEST(Expad::GetDataType(ltx) == Expad::Undefined);
    SIMPLE_TEST(Expad::GetDataType(frm) == Expad::Undefined);

    // check DataType numbering scheme (undef/1D/2D/3D)
    SIMPLE_TEST(Expad::Undefined == 0)
    SIMPLE_TEST(Expad::Function1D < 10)
    SIMPLE_TEST(Expad::Graph1D < 10)
    SIMPLE_TEST(Expad::MultiGraph1D < 10)
    SIMPLE_TEST(Expad::Histo2D < 20)
    SIMPLE_TEST(Expad::Function2D < 20)
    SIMPLE_TEST(Expad::Graph2D < 20)
    SIMPLE_TEST(Expad::Histo2D < 20)
    SIMPLE_TEST(Expad::Histo3D < 30)

    END_TEST();
}
