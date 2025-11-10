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
#include "THStack.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TPaveText.h"
#include "TProfile.h"
#include "TText.h"

#include <iostream>

void TestDataType() {
    BEGIN_TEST();
    auto h1 = new TH1S();
    auto h2 = new TH2D();
    auto h3 = new TH3F();
    auto pf = new TProfile();
    auto hs = new THStack();
    auto gr = new TGraph();
    auto gre = new TGraphErrors();
    auto gr2D = new TGraph2D();
    auto mg = new TMultiGraph();
    auto f1 = new TF1();
    auto f12 = new TF12();
    auto f2 = new TF2();
    auto leg = new TLegend();
    auto txt = new TText();
    auto ltx = new TLatex();
    auto pvtxt = new TPaveText();
    auto frm = new TFrame();
    auto c = new TCanvas();

    // histograms
    SIMPLE_TEST(REx::GetDataType(h1) == REx::Histo1D);
    SIMPLE_TEST(REx::GetDataType(h2) == REx::Histo2D);
    SIMPLE_TEST(REx::GetDataType(h3) == REx::Histo3D);
    SIMPLE_TEST(REx::GetDataType(pf) == REx::Histo1D);
    SIMPLE_TEST(REx::GetDataType(hs) == REx::MultiHisto1D);

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
