#include "ROOTToText.hh"

#include "TClass.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TGraphErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TMultiGraph.h"
#include "TObjString.h"
#include "TPRegexp.h"
#include "TSystem.h"

#include <fstream>
#include <iostream>

REx::ROOTToText* gRTT = REx::ROOTToText::GetInstance();

namespace REx {

ROOTToText* ROOTToText::instance_ = 0;

ROOTToText::ROOTToText() {
    headerTitle_ = true;
    headerAxis_ = true;
    defaultExtension_ = ".txt";
    baseDirectory_ = "./";
    cc_ = '#';
    verb_ = false;
    npfunc_ = 100;
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

bool ROOTToText::SaveObject(const TObject* obj, const char* filename, Option_t* opt) const {
    TString str(filename);
    return SaveObject(obj, GetDataType(obj), str, opt);
}

bool ROOTToText::SaveObject(const TObject* obj, DataType dt, const char* filename, Option_t* opt) const {
    TString str(filename);
    return SaveObject(obj, dt, str, opt);
}

bool ROOTToText::SaveObject(const TObject* obj, DataType dt, TString& filename, Option_t* opt) const {
    if (!obj) {
        std::cerr << "Error: null pointer" << std::endl;
        return false;
    }

    TString option(opt);
    option.ToUpper();

    if (dt == MultiGraph1D) {
        if (userWriters_.count(obj->IsA()) == 0 || option.Contains("D")) {
            return SaveMultiGraph(dynamic_cast<const TMultiGraph*>(obj), filename, opt);
        }
    }

    TString path = GetFilePath(obj, filename.Data());
    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        std::cerr << "Error: could not open file " << path << std::endl;
        return false;
    }

    bool written = false;
    // "D" -> force Default writer
    if (!option.Contains("D")) {
        auto cl = obj->IsA();
        if (userWriters_.count(cl)) {
            try {
                userWriters_.at(cl)(obj, option, ofs);
            }
            catch (const std::exception& e) {
                std::cerr << "Error when using custom writer for class " << cl->GetName() << std::endl;
                std::cerr << e.what() << '\n';
                ofs.close();
                return false;
            }
            written = true;
        }
    }

    if (!written) {
        switch (dt) {
            case Histo1D:
                WriteTH1(dynamic_cast<const TH1*>(obj), option, ofs);
                break;
            case Histo2D:
                WriteTH2(dynamic_cast<const TH2*>(obj), option, ofs);
                break;
            case Graph1D:
                WriteGraph(dynamic_cast<const TGraph*>(obj), option, ofs);
                break;
            case Graph2D:
                WriteGraph2D(dynamic_cast<const TGraph2D*>(obj), option, ofs);
                break;
            case Function1D:
                WriteTF1(dynamic_cast<const TF1*>(obj), option, ofs);
                break;
            default:
                TString error_message = TString::Format("This kind of object (%s) is not supported, but you could use a custom writer.", obj->IsA()->GetName());
                throw std::invalid_argument(error_message.Data());
                // std::cerr << "This kind of object (" << obj->Class_Name() << ") is not supported" << std::endl;
        }
    }

    ofs.close();
    if (verb_) std::cout << "Saved " << obj->GetName() << " in " << path << std::endl;
    filename = path;
    return true;
}

void ROOTToText::PrintOptions() const {
    std::cout << "Available options :\n"
              << "\tD      [all] - Use default RTT writer\n"
              << "\tL      [TH1] - Use bin low edge instead of bin center\n"
              << "\tE      [TH1] - Save bin errors\n"
              << "\tC      [TH2] - Save 2D data in columns : X Y Z\n"
              << "\tG      [TH2] - Save 2D data in GLE format\n"
              << "\tR  [TH1/TH2] - Save only data in the bin range\n"
              << "\tH   [TGraph] - Save horizontal errors\n"
              << "\tN<n>   [TF1] - Use n points to save function\n"
              << std::endl;
}

bool ROOTToText::SaveMultiGraph(const TMultiGraph* mg, TString& filename, Option_t* opt) const {
    if (!mg) {
        std::cerr << "Error: null pointer" << std::endl;
        return false;
    }

    // TString option(opt);
    // option.ToUpper();

    // the title of the graphs will be basename_graphname
    TString basename(filename);
    if (basename.IsWhitespace()) {
        basename = mg->GetName();
        if (basename.IsNull()) basename = "Graph";
    }
    Ssiz_t s = basename.Length();
    TPRegexp has_ext("\\.[a-zA-Z0-9]+$");
    if (has_ext.MatchB(basename)) {
        s = basename.Last('.');
    }

    bool res = true;
    int idx = 0;
    // loop over all graphs stored in the multigraph, and save them
    for (const TObject* gr : *mg->GetListOfGraphs()) {
        idx++;
        TString gr_name = gr->GetName();
        if (gr_name.CompareTo("Graph") == 0) {
            // default name --> replace it with an index
            // because if all graphs have the same name it will not work !!
            gr_name = TString::Itoa(++idx, 10);
        }
        // set the filename corresponding to this graph
        TString filename_graph(basename);
        filename_graph.Insert(s, "_" + gr_name);
        // save it
        res = res && SaveObject(gr, Graph1D, filename_graph, opt);
    }
    filename = basename;
    return res;
}

bool ROOTToText::AddCustomWriter(const char* class_name, writer& func) {
    auto cl = TClass::GetClass(class_name);
    if (!cl) {
        std::cerr << "Error: class " << class_name << " not found in ROOT." << std::endl;
        return false;
    }
    if (!cl->InheritsFrom("TObject")) {
        std::cerr << "Error: class " << class_name << " does not inherit from TObject." << std::endl;
        return false;
    }
    if (userWriters_.count(cl))
        std::cout << "Warning: overwritting writer function for class " << class_name << std::endl;
    userWriters_[cl] = func;
    return true;
}

bool ROOTToText::RemoveCustomWriter(const char* class_name) {
    auto cl = TClass::GetClass(class_name);
    if (userWriters_.count(cl)) {
        userWriters_.erase(cl);
        return true;
    }
    else {
        std::cout << "Warning: did not find user writer function for class " << class_name << std::endl;
        return false;
    }
}

void ROOTToText::ClearCustomWriters() {
    userWriters_.clear();
}

void ROOTToText::SetDirectory(TString dir) {
    // a cleaner implementation would use std::filesystem (or std::experimental::filesystem)
    // but it it would limit compatibility with some old compiler since it is a C++17 feature (and it may require additional libs)
    TString path(dir);
    if (dir.Length() == 0) {
        // empty string -> current directory
        baseDirectory_ = "./";
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
        gSystem->PrependPathName("./", path);
    }

    if (!gSystem->AccessPathName(path)) {
        // this path exists
        baseDirectory_ = path;
        return;
    }

    if (!gSystem->AccessPathName(gSystem->DirName(path))) {
        // parent directory exists
        // we try to make the desired directory
        if (gSystem->mkdir(path) == 0) {
            baseDirectory_ = path;
            return;
        }
    }

    // what to do in case of failure ?? -> exception, error message, return value ?
    TString error_msg = "path " + path + " cannot be accessed.";
    throw std::runtime_error(error_msg);
    // std::cerr << "Error: path " << path << " cannot be accessed." << std::endl;
    // std::cerr << "Keeping the previous directory: " << baseDirectory_ << std::endl;
}

TString ROOTToText::GetFilePath(const TObject* obj, const char* filename) const {
    TString str(filename);

    // if no filename is given, use the object name
    if (str.IsWhitespace()) {
        str = obj->GetName();
        str.ReplaceAll(' ', '_');
    }

    // check that filename ends with a file extension
    // if not, add the default extension
    TPRegexp fileExt("\\.[a-zA-Z0-9]+$");
    if (!fileExt.MatchB(str))
        str.Append(defaultExtension_);

    gSystem->ExpandPathName(str);
    if (!gSystem->IsAbsoluteFileName(str) && !str.BeginsWith("./")) {
        gSystem->PrependPathName(baseDirectory_, str);
    }

    return str;
}

void ROOTToText::WriteTH1(const TH1* h, const TString& option, std::ofstream& ofs) const {
    bool low_edge = option.Contains('L');
    bool with_errors = option.Contains('E');
    bool use_range = option.Contains('R');

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

    int imin = 1, imax = h->GetNbinsX();
    if (use_range) {
        imin = h->GetXaxis()->GetFirst();
        imax = h->GetXaxis()->GetLast();
    }
    for (int i = imin; i <= imax; i++) {
        if (low_edge)
            ofs << h->GetBinLowEdge(i);
        else
            ofs << h->GetBinCenter(i);
        ofs << " " << h->GetBinContent(i);
        if (with_errors)
            ofs << " " << h->GetBinError(i);
        ofs << std::endl;
    }
}

void ROOTToText::WriteTH2(const TH2* h, const TString& option, std::ofstream& ofs) const {
    bool use_range = option.Contains('R');
    bool in_columns = option.Contains("C");
    // x1, y1, z11
    // x2, y1, z21
    // ...
    // else : matrix
    // z11 z21 z31 ...
    // z12 z22 z32 ...
    // ...
    bool glefile = option.Contains("G");
    if (in_columns && glefile) {
        std::cerr << "Warning: Changing 2D GLE file format from \'columns\' to \'matrix\'." << std::endl;
        in_columns = false;
    }

    if (glefile) {
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

    int imin = 1, imax = h->GetNbinsX();
    if (use_range) {
        imin = h->GetXaxis()->GetFirst();
        imax = h->GetXaxis()->GetLast();
    }
    int jmin = 1, jmax = h->GetNbinsY();
    if (use_range) {
        jmin = h->GetYaxis()->GetFirst();
        jmax = h->GetYaxis()->GetLast();
    }

    if (in_columns) {
        for (int i = imin; i <= imax; i++) {
            for (int j = jmin; j <= jmax; j++) {
                ofs << h->GetXaxis()->GetBinCenter(i) << " "
                    << h->GetYaxis()->GetBinCenter(j) << " "
                    << h->GetBinContent(i, j)
                    << std::endl;
            }
            ofs << std::endl;
        }
    }
    else {
        for (int j = jmin; j <= jmax; j++) {
            for (int i = imin; i <= imax; i++) {
                if (i > 1)
                    ofs << " ";
                ofs << h->GetBinContent(i, j);
            }
            ofs << std::endl;
        }
    }
}

void ROOTToText::WriteGraph(const TGraph* gr, const TString& option, std::ofstream& ofs) const {
    if (gr->IsA() != TGraph::Class() && gr->IsA() != TGraphErrors::Class())
        std::cerr << "Warning: only limited support for class " << gr->IsA()->GetName() << std::endl;

    bool with_errors = true;
    bool with_herrors = option.Contains("H");

    Double_t* XX = gr->GetX();
    Double_t* YY = gr->GetY();
    Double_t* EX = gr->GetEX();
    Double_t* EY = gr->GetEY();
    if (!EY) with_errors = with_herrors = false;
    if (!EX) with_herrors = false;

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

    Int_t np = gr->GetN();
    Int_t* idx = new Int_t[np];
    TMath::Sort(np, XX, idx, false);

    for (int i = 0; i < np; i++) {
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
    delete[] idx;
}

void ROOTToText::WriteGraph2D(const TGraph2D* gr, const TString& /*option*/, std::ofstream& ofs) const {
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
    delete[] idx;
}

void ROOTToText::WriteTF1(const TF1* f, const TString& option, std::ofstream& ofs) const {
    // number of points for evaluation
    int npoints = -1;
    if (option.Contains("N")) {
        auto s1 = option.First('N');
        auto s2 = s1;
        while (isdigit(option[++s2])) // string ends with '\0' --> no need to perform out-of-bounds check
            continue;
        TString sub = TString(option(s1 + 1, s2 - s1 - 1));
        if (!sub.IsNull())
            npoints = sub.Atoi();
    }
    if (npoints <= 0) npoints = npfunc_;

    // function range
    Double_t xmin, xmax;
    f->GetRange(xmin, xmax);

    if (headerTitle_)
        ofs << cc_ << " Function " << f->GetName() << " : x -> " << f->GetExpFormula("P") << std::endl;

    if (headerAxis_) {
        ofs << cc_ << " 1:X - from " << xmin << " to " << xmax << std::endl;
        ofs << cc_ << " 2:Y=" << f->GetName() << "(X)" << std::endl;
    }

    double dx = (xmax - xmin) / ((double)npoints - 1);
    for (int i = 0; i < npoints; i++) {
        double xi = xmin + dx * i;
        ofs << xi << " " << f->Eval(xi) << std::endl;
    }
}

} // namespace REx
