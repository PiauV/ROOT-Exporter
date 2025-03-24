#include "ROOTToText.hh"

#include "TClass.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TPRegexp.h"
#include "TSystem.h"

#include <fstream>
#include <iostream>

Expad::ROOTToText* gRTT = Expad::ROOTToText::GetInstance();

namespace Expad {

ROOTToText* ROOTToText::instance_ = 0;

ROOTToText::ROOTToText() {
    headerTitle_ = true;
    headerAxis_ = true;
    defaultExtension_ = ".txt";
    defaultDirectory_ = "./";
    cc_ = '#';
}

ROOTToText::~ROOTToText() {
}

ROOTToText* ROOTToText::GetInstance() {
    if (!instance_)
        instance_ = new ROOTToText();
    return instance_;
}

void ROOTToText::SetFileExtension(TString ext) {
    TPRegexp is_ext("^\\.?[a-zA-Z0-9]+$");
    if (!is_ext.MatchB(ext))
        throw std::invalid_argument("this is not a valid file extension");
    defaultExtension_ = ext;
    if (!defaultExtension_.BeginsWith('.'))
        defaultExtension_.Prepend('.');
}

void ROOTToText::SetDirectory(TString dir) {
    // a cleaner implementation would use std::filesystem (or std::experimental::filesystem)
    // but it it would limit compatibility with some old compiler since it is a C++17 feature
    TString path(dir);
    if (dir.Length() == 0) {
        // empty string -> current directory
        defaultDirectory_ = "./";
        return;
    }

    TPRegexp is_file("[a-zA-Z0-9]+\\.[a-zA-Z0-9]+$");
    if (is_file.MatchB(path)) {
        // this path is a file !
        throw std::invalid_argument("this is not a directory");
    }

    gSystem->ExpandPathName(path); // remove symbols
    if (!gSystem->IsAbsoluteFileName(path) && !path.BeginsWith("./")) {
        // relative directory... to what ?
        // let's assume its relative to current dir
        // --> prepend path name
        gSystem->PrependPathName(defaultDirectory_, path);
    }

    if (!gSystem->AccessPathName(path)) {
        // this path can be accessed
        defaultDirectory_ = path;
        return;
    }

    if (!gSystem->AccessPathName(gSystem->DirName(path))) {
        // parent directory can be accessed
        // we try to make the desired directory
        if (gSystem->mkdir(path) == 0) {
            defaultDirectory_ = path;
            return;
        }
    }

    // what to do in case of failure ?? -> exception, error message, return value ?
    TString error_msg = "path " + path + " cannot be accessed.";
    throw std::runtime_error(error_msg);
    // std::cerr << "Error: path " << path << " cannot be accessed." << std::endl;
    // std::cerr << "Keeping the previous directory: " << defaultDirectory_ << std::endl;
}

TString ROOTToText::GetFilePath(const TObject* obj, char* filename) const {
    TString str(filename);

    // if no filename is given, use the histogram name
    TPRegexp anything("^.*[a-zA-Z0-9]+.*$");
    if (!anything.MatchB(str)) {
        str.Append(TString(obj->GetName()));
    }
    str.ReplaceAll(' ', '_');

    // check that filename ends with a file extension
    // if not, add the default ".txt" extension
    TPRegexp fileExt("\\.[a-z]+$");
    if (!fileExt.MatchB(str))
        str.Append(".txt");

    if (!gSystem->IsAbsoluteFileName(str) && !str.BeginsWith("./")) {
        gSystem->PrependPathName(defaultDirectory_, str);
    }

    return str;
}

bool ROOTToText::SaveObject(const TObject* obj, char* filename, Option_t* opt) const {
    return SaveObject(obj, GetDataType(obj), filename, opt);
}

bool ROOTToText::SaveObject(const TObject* obj, DataType dt, char* filename, Option_t* opt) const {
    switch (dt) {
        case Histo1D:
            return SaveTH1(dynamic_cast<const TH1*>(obj), filename, opt);
        case Histo2D:
            return SaveTH2(dynamic_cast<const TH2*>(obj), filename, opt);
        case Graph1D:
            return SaveGraph(dynamic_cast<const TGraph*>(obj), filename, opt);
        case Graph2D:
            return SaveGraph2D(dynamic_cast<const TGraph2D*>(obj), filename, opt);
        default:
            TString error_message = TString::Format("This kind of object (%s) is not supported.", obj->Class_Name());
            throw std::invalid_argument(error_message.Data());
            // std::cerr << "This kind of object (" << obj->Class_Name() << ") is not supported" << std::endl;
    }
    return false;
}

bool ROOTToText::SaveTH1(const TH1* h, char* filename, Option_t* opt) const {
    if (!h) {
        std::cerr << "Error: null pointer (probably coming from SaveObject)" << std::endl;
        return false;
    }

    TString option(opt);
    option.ToUpper();
    bool low_edge = option.Contains('L');
    bool with_errors = option.Contains('E');

    TString path = GetFilePath(h, filename);
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "Error: could not open file " << path << std::endl;
        return false;
    }

    if (headerTitle_)
        ofs << cc_ << " " << h->GetTitle() << std::endl;

    if (headerAxis_) {
        ofs << cc_ << " 1:X";
        TString xaxis = h->GetXaxis()->GetTitle();
        if (xaxis.Length() > 0)
            ofs << " - " << xaxis;
        ofs << std::endl;
        ofs << cc_ << " 2:Y";
        TString yaxis = h->GetYaxis()->GetTitle();
        if (yaxis.Length() > 0)
            ofs << " - " << yaxis;
        ofs << std::endl;
        if (with_errors)
            ofs << cc_ << " 3:EY" << std::endl;
    }

    for (int i = 1; i <= h->GetNbinsX(); i++) {
        if (low_edge)
            ofs << h->GetBinLowEdge(i);
        else
            ofs << h->GetBinCenter(i);
        ofs << " " << h->GetBinContent(i);
        if (with_errors)
            ofs << " " << h->GetBinError(i);
        ofs << std::endl;
    }

    ofs.close();
    return true;
}

bool ROOTToText::SaveTH2(const TH2* h, char* filename, Option_t* opt) const {
    if (!h) {
        std::cerr << "Error: null pointer (probably coming from SaveObject)" << std::endl;
        return false;
    }

    TString option(opt);
    option.ToUpper();
    bool in_columns = option.Contains("C");
    // x1, y1, z11
    // x2, y1, z21
    // ...
    // else : matrix
    // z11 z21 z31 ...
    // z12 z22 z32 ...
    // ...

    TString path = GetFilePath(h, filename);
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "Error: could not open file " << path << std::endl;
        return false;
    }

    if (!in_columns && path.EndsWith(".z") && cc_ == '!') {
        // GLE mandatory header
        int nx = h->GetNbinsX();
        int ny = h->GetNbinsY();
        ofs << "! nx " << nx
            << " xmin " << h->GetXaxis()->GetBinLowEdge(1)
            << " xmax " << h->GetXaxis()->GetBinLowEdge(nx + 1)
            << " ny " << ny
            << " ymin " << h->GetYaxis()->GetBinLowEdge(1)
            << " ymax " << h->GetYaxis()->GetBinLowEdge(ny + 1)
            << std::endl;
    }

    if (headerTitle_)
        ofs << cc_ << " " << h->GetTitle() << std::endl;

    if (headerAxis_) {
        ofs << cc_ << " 1:X";
        TString xaxis = h->GetXaxis()->GetTitle();
        if (xaxis.Length() > 0)
            ofs << " - " << xaxis;
        ofs << std::endl;
        ofs << cc_ << " 2:Y";
        TString yaxis = h->GetYaxis()->GetTitle();
        if (yaxis.Length() > 0)
            ofs << " - " << yaxis;
        ofs << std::endl;
        ofs << cc_ << " 3:Z";
        TString zaxis = h->GetZaxis()->GetTitle();
        if (zaxis.Length() > 0)
            ofs << " - " << zaxis;
        ofs << std::endl;
    }

    if (in_columns) {
        for (int i = 1; i <= h->GetNbinsX(); i++) {
            for (int j = 1; j <= h->GetNbinsY(); j++) {
                ofs << h->GetXaxis()->GetBinCenter(i) << " "
                    << h->GetYaxis()->GetBinCenter(j) << " "
                    << h->GetBinContent(i, j)
                    << std::endl;
            }
            ofs << std::endl;
        }
    }
    else {
        for (int j = 1; j <= h->GetNbinsY(); j++) {
            for (int i = 1; i <= h->GetNbinsX(); i++) {
                if (i > 1)
                    ofs << " ";
                ofs << h->GetBinContent(i, j);
            }
            ofs << std::endl;
        }
    }

    ofs.close();
    return true;
}

bool ROOTToText::SaveGraph(const TGraph* gr, char* filename, Option_t* opt) const {
    if (!gr) {
        std::cerr << "Error: null pointer (probably coming from SaveObject)" << std::endl;
        return false;
    }

    if (gr->IsA() != TGraph::Class() && gr->IsA() != TGraphErrors::Class())
        std::cerr << "Warning: only limited support for class " << gr->Class_Name() << std::endl;

    TString option(opt);
    option.ToUpper();
    bool with_errors = (gr->IsA() == TGraphErrors::Class());
    bool with_herrors = with_errors && option.Contains("H");

    TString path = GetFilePath(gr, filename);
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "Error: could not open file " << path << std::endl;
        return false;
    }

    if (headerTitle_)
        ofs << cc_ << " " << gr->GetTitle() << std::endl;

    if (headerAxis_) {
        ofs << cc_ << " 1:X";
        TString xaxis = gr->GetXaxis()->GetTitle();
        if (xaxis.Length() > 0)
            ofs << " - " << xaxis;
        ofs << std::endl;
        ofs << cc_ << " 2:Y";
        TString yaxis = gr->GetYaxis()->GetTitle();
        if (yaxis.Length() > 0)
            ofs << " - " << yaxis;
        ofs << std::endl;
        if (with_errors) {
            if (with_herrors)
                ofs << cc_ << " 3:EX\n"
                    << cc_ << " 4:EY" << std::endl;
            else
                ofs << cc_ << " 3:EY" << std::endl;
        }
    }

    Double_t* XX = gr->GetX();
    Double_t* YY = gr->GetY();
    Double_t* EX = gr->GetEX();
    Double_t* EY = gr->GetEY();
    Int_t* idx = new Int_t[gr->GetN()];
    TMath::Sort(gr->GetN(), XX, idx, false);

    for (int i = 0; i < gr->GetN(); i++) {
        Int_t k = idx[i];
        ofs << XX[k] << " " << YY[k];
        if (with_errors) {
            if (with_herrors)
                ofs << " " << EX[k] << " " << EY[k];
            else
                ofs << " " << EY[k];
        }
        ofs << std::endl;
    }
    ofs.close();
    delete[] idx;
    return true;
}

bool ROOTToText::SaveGraph2D(const TGraph2D* gr, char* filename, Option_t* opt) const {
    if (!gr) {
        std::cerr << "Error: null pointer (probably coming from SaveObject)" << std::endl;
        return false;
    }

    // TString option(opt);
    // option.ToUpper();

    TString path = GetFilePath(gr, filename);
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "Error: could not open file " << path << std::endl;
        return false;
    }

    if (headerTitle_)
        ofs << cc_ << " " << gr->GetTitle() << std::endl;

    if (headerAxis_) {
        ofs << cc_ << " 1:X";
        TString xaxis = gr->GetXaxis()->GetTitle();
        if (xaxis.Length() > 0)
            ofs << " - " << xaxis;
        ofs << std::endl;
        ofs << cc_ << " 2:Y";
        TString yaxis = gr->GetYaxis()->GetTitle();
        if (yaxis.Length() > 0)
            ofs << " - " << yaxis;
        ofs << std::endl;
        ofs << cc_ << " 3:Z";
        TString zaxis = gr->GetZaxis()->GetTitle();
        if (zaxis.Length() > 0)
            ofs << " - " << zaxis;
        ofs << std::endl;
    }

    Double_t* XX = gr->GetX();
    Double_t* YY = gr->GetY();
    Double_t* ZZ = gr->GetZ();
    Int_t* idx = new Int_t[gr->GetN()];
    TMath::Sort(gr->GetN(), XX, idx, false);

    double xprev = 0;
    for (int i = 0; i < gr->GetN(); i++) {
        Int_t k = idx[i];
        if (i > 0 && xprev != XX[k]) ofs << std::endl;
        ofs << XX[k] << " " << YY[k] << " " << ZZ[k] << std::endl;
        xprev = XX[k];
    }
    ofs.close();
    delete[] idx;
    return true;
}

} // namespace Expad
