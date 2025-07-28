#include "DataType_test.hh"
#include "DataType.hh"
#include "macros.hh"

#include "TCanvas.h"
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
#include "TText.h"

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
    TText* txt = new TText();
    TLatex* ltx = new TLatex();
    TPaveText* pvtxt = new TPaveText();
    TFrame* frm = new TFrame();
    TCanvas* c = new TCanvas();

    // histograms
    SIMPLE_TEST(REx::GetDataType(h1) == REx::Histo1D);
    SIMPLE_TEST(REx::GetDataType(h2) == REx::Histo2D);
    SIMPLE_TEST(REx::GetDataType(h3) == REx::Histo3D);
    SIMPLE_TEST(REx::GetDataType(pf) == REx::Histo1D);

    // graphs
    SIMPLE_TEST(REx::GetDataType(gr) == REx::Graph1D);
    SIMPLE_TEST(REx::GetDataType(gre) == REx::Graph1D);
    SIMPLE_TEST(REx::GetDataType(gr2D) == REx::Graph2D);
    SIMPLE_TEST(REx::GetDataType(mg) == REx::MultiGraph1D);

    // functions
    SIMPLE_TEST(REx::GetDataType(f1) == REx::Function1D);
    SIMPLE_TEST(REx::GetDataType(f12) == REx::Function1D);
    SIMPLE_TEST(REx::GetDataType(f2) == REx::Function2D);

    // text
    SIMPLE_TEST(REx::GetDataType(pvtxt) == REx::TextBox);
    SIMPLE_TEST(REx::GetDataType(txt) == REx::BareText);
    SIMPLE_TEST(REx::GetDataType(ltx) == REx::BareText);

    // legend
    SIMPLE_TEST(REx::GetDataType(leg) == REx::Legend);

    // others (not supported)
    SIMPLE_TEST(REx::GetDataType(frm) == REx::Undefined);
    SIMPLE_TEST(REx::GetDataType(c) == REx::Undefined);

    // check DataType numbering scheme (undef/1D/2D/3D)
    SIMPLE_TEST(REx::GetDataDimension(REx::Undefined) == -1)
    SIMPLE_TEST(REx::GetDataDimension(REx::Function1D) == 1)
    SIMPLE_TEST(REx::GetDataDimension(REx::Graph1D) == 1)
    SIMPLE_TEST(REx::GetDataDimension(REx::MultiGraph1D) == 1)
    SIMPLE_TEST(REx::GetDataDimension(REx::Histo2D) == 2)
    SIMPLE_TEST(REx::GetDataDimension(REx::Function2D) == 2)
    SIMPLE_TEST(REx::GetDataDimension(REx::Graph2D) == 2)
    SIMPLE_TEST(REx::GetDataDimension(REx::Histo2D) == 2)
    SIMPLE_TEST(REx::GetDataDimension(REx::Histo3D) == 3)

    END_TEST();
}
