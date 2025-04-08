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

Expad::ROOTToText* gRTT = Expad::ROOTToText::GetInstance();

namespace Expad {

ROOTToText* ROOTToText::instance_ = 0;

ROOTToText::ROOTToText() {
    headerTitle_ = true;
    headerAxis_ = true;
    defaultExtension_ = ".txt";
    defaultDirectory_ = "./";
    cc_ = '#';
    verb_ = false;
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
    return SaveObject(obj, GetDataType(obj), filename, opt);
}

bool ROOTToText::SaveObject(const TObject* obj, DataType dt, const char* filename, Option_t* opt) const {
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

    TString path = GetFilePath(obj, filename);
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
    return true;
}

bool ROOTToText::SaveMultiGraph(const TMultiGraph* mg, const char* filename, Option_t* opt) const {
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
        res = res && SaveObject(gr, Graph1D, filename_graph.Data(), opt);
    }
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

void ROOTToText::SetDirectory(TString dir) {
    // a cleaner implementation would use std::filesystem (or std::experimental::filesystem)
    // but it it would limit compatibility with some old compiler since it is a C++17 feature (and it may require additional libs)
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

TString ROOTToText::GetFilePath(const TObject* obj, const char* filename) const {
    TString str(filename);

    // if no filename is given, use the object name
    TPRegexp anything("^.*[a-zA-Z0-9]+.*$");
    if (!anything.MatchB(str)) {
        str.Append(TString(obj->GetName()));
    }
    str.ReplaceAll(' ', '_');

    // check that filename ends with a file extension
    // if not, add the default extension
    TPRegexp fileExt("\\.[a-zA-Z0-9]+$");
    if (!fileExt.MatchB(str))
        str.Append(defaultExtension_);

    if (!gSystem->IsAbsoluteFileName(str) && !str.BeginsWith("./")) {
        gSystem->PrependPathName(defaultDirectory_, str);
    }

    return str;
}

void ROOTToText::WriteTH1(const TH1* h, const TString& option, std::ofstream& ofs) const {
    bool low_edge = option.Contains('L');
    bool with_errors = option.Contains('E');

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
}

void ROOTToText::WriteTH2(const TH2* h, const TString& option, std::ofstream& ofs) const {
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
}

void ROOTToText::WriteGraph(const TGraph* gr, const TString& option, std::ofstream& ofs) const {
    if (gr->IsA() != TGraph::Class() && gr->IsA() != TGraphErrors::Class())
        std::cerr << "Warning: only limited support for class " << gr->IsA()->GetName() << std::endl;

    bool with_errors = (gr->IsA() == TGraphErrors::Class());
    bool with_herrors = with_errors && option.Contains("H");

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
        TPRegexp pattern("N(\\d+)");
        TObjArray* strL = pattern.MatchS(option);
        if (strL->GetSize() > 1) {
            // strL contains the whole string matching the regex (0) and the captured group (1)
            // we are only interested in the captured group (i.e. the figures)
            npoints = ((TObjString*)strL->At(1))->GetString().Atoi();
        }
    }
    if (npoints <= 0) npoints = 100;

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

} // namespace Expad
