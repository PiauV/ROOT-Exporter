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

    // text
    SIMPLE_TEST(Expad::GetDataType(pvtxt) == Expad::TextBox);
    SIMPLE_TEST(Expad::GetDataType(txt) == Expad::BareText);
    SIMPLE_TEST(Expad::GetDataType(ltx) == Expad::BareText);

    // legend
    SIMPLE_TEST(Expad::GetDataType(leg) == Expad::Legend);

    // others (not supported)
    SIMPLE_TEST(Expad::GetDataType(frm) == Expad::Undefined);
    SIMPLE_TEST(Expad::GetDataType(c) == Expad::Undefined);

    // check DataType numbering scheme (undef/1D/2D/3D)
    SIMPLE_TEST(Expad::GetDataDimension(Expad::Undefined) == -1)
    SIMPLE_TEST(Expad::GetDataDimension(Expad::Function1D) == 1)
    SIMPLE_TEST(Expad::GetDataDimension(Expad::Graph1D) == 1)
    SIMPLE_TEST(Expad::GetDataDimension(Expad::MultiGraph1D) == 1)
    SIMPLE_TEST(Expad::GetDataDimension(Expad::Histo2D) == 2)
    SIMPLE_TEST(Expad::GetDataDimension(Expad::Function2D) == 2)
    SIMPLE_TEST(Expad::GetDataDimension(Expad::Graph2D) == 2)
    SIMPLE_TEST(Expad::GetDataDimension(Expad::Histo2D) == 2)
    SIMPLE_TEST(Expad::GetDataDimension(Expad::Histo3D) == 3)

    END_TEST();
}
