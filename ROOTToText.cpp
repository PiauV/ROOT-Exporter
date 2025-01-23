#include "ROOTToText.h"

#include "TGraph2D.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TPRegexp.h"
#include "TSystem.h"

#include <iostream>
#include <fstream>

ROOTToText* ROOTToText::instance_ = 0;

ROOTToText* gRTT = ROOTToText::getInstance();

ROOTToText::ROOTToText(){
    headerTitle_ = true;
    headerAxis_ = true;
    defaultExtension_ = ".txt";
    defaultDirectory_ = "./";
}

ROOTToText::~ROOTToText(){

}

ROOTToText* ROOTToText::getInstance(){
    if (!instance_)
        instance_ = new ROOTToText();
    return instance_;
}

void ROOTToText::SetDefaultExtension(TString ext){
    if (ext.Length() < 1 || (ext[0] == '.' && ext.Length() < 2))
        throw std::invalid_argument("this is not a valid file extension");
    defaultExtension_ = ext;
    if (!defaultExtension_.BeginsWith('.'))
        defaultExtension_.Prepend('.');
}

void ROOTToText::SetDirectory(TString dir){
    TString path(dir);
    gSystem->ExpandPathName(path); // remove symbols
    if (!gSystem->IsAbsoluteFileName(path)){
        // relative directory... to what ?
        // let's assume its relative to current dir
        gSystem->PrependPathName(defaultDirectory_, path);
    }

    if (!gSystem->AccessPathName(path)){
        // this path can be accessed (! do not check if this is a directory!)
        defaultDirectory_ = path;
        return;
    }

    if (!gSystem->AccessPathName(gSystem->DirName(path))){
        // parent directory can be accessed
        // we try to make the desired directory
        if (gSystem->mkdir(path) == 0){
            defaultDirectory_ = path;
            return;
        }
    }

    // failed -> do not change anything
    std::cerr << "Error: path " << path << " cannot be accessed." << std::endl;
    std::cerr << "Keeping the previous directory: " << defaultDirectory_ << std::endl;
}

TString ROOTToText::getFilePath(const TObject *obj, char *filename) const{
    TString str(filename);

    // if no filename is given, use the histogram name
    TPRegexp anything("^.*[a-zA-Z0-9]+.*$");
    if (!anything.MatchB(str)){
        str.Append(TString(obj->GetName()));
    }
    str.ReplaceAll(' ', '_');
    std::cout << str << std::endl;

    // check that filename ends with a file extension
    // if not, add the default ".txt" extension
    TPRegexp fileExt("\\.[a-z]+$");
    if (!fileExt.MatchB(str))
        str.Append(".txt");

    std::cout << str << std::endl;

    if (!gSystem->IsAbsoluteFileName(str)){
        gSystem->PrependPathName(defaultDirectory_, str);
    }

    std::cout << str << std::endl;
    return str;
}

bool ROOTToText::SaveObject(const TObject *obj, char *filename, Option_t *opt) const{
    if (obj->InheritsFrom("TH2")){
        return SaveTH2((TH2 *)obj, filename, opt);
    }
    else if (obj->InheritsFrom("TH1") || !obj->InheritsFrom("TH3")){
        return SaveTH1((TH1 *)obj, filename, opt);
    }
    else if (obj->InheritsFrom("TGraph")){
        return SaveGraph((TGraph *)obj, filename, opt);
    }
    else{
        TString error_message = TString::Format("This kind of object (%s) is not supported.", obj->Class_Name());
        throw std::invalid_argument(error_message.Data());
    }
    return false;
}

bool ROOTToText::SaveTH1(const TH1 *h, char *filename, Option_t *opt) const{
    TString option(opt);
    option.ToUpper();
    bool low_edge = option.Contains('L');
    bool with_errors = option.Contains('E');

    TString path = getFilePath(h, filename);
    std::ofstream ofs(path);
    if (!ofs.is_open()){
        std::cerr << "Error: could not open file " << path << std::endl;
        return false;
    }

    if (headerTitle_)
        ofs << "# " << h->GetTitle() << std::endl;

    if (headerAxis_){
        ofs << "# 1:X";
        TString xaxis = h->GetXaxis()->GetTitle();
        if (xaxis.Length() > 0)
            ofs << " - " << xaxis;
        ofs << std::endl;
        ofs << "# 2:Y";
        TString yaxis = h->GetYaxis()->GetTitle();
        if (yaxis.Length() > 0)
            ofs << " - " << yaxis;
        ofs << std::endl;
        if (with_errors)
            ofs << "# 3:EY" << std::endl;
    }

    for (int i = 1; i <= h->GetNbinsX(); i++){
        if (low_edge)
            ofs << h->GetBinLowEdge(i);
        else
            ofs << h->GetBinCenter(i);
        ofs << " " << h->GetBinContent(i);
        if (with_errors)
            ofs << " " << h->GetBinError(i);
        ofs << std::endl;
    }
    return true;
}

bool ROOTToText::SaveTH2(const TH2 *h, char *filename, Option_t *opt) const{
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

    TString path = getFilePath(h, filename);
    std::ofstream ofs(path);
    if (!ofs.is_open()){
        std::cerr << "Error: could not open file " << path << std::endl;
        return false;
    }

    if (headerTitle_)
        ofs << "# " << h->GetTitle() << std::endl;

    if (headerAxis_){
        ofs << "# 1:X";
        TString xaxis = h->GetXaxis()->GetTitle();
        if (xaxis.Length() > 0)
            ofs << " - " << xaxis;
        ofs << std::endl;
        if (!in_columns){
            int nx = h->GetNbinsX();
            ofs << "# nx " << nx
                << " xmin " << h->GetXaxis()->GetBinLowEdge(1)
                << " xmax " << h->GetXaxis()->GetBinLowEdge(nx)
                << std::endl;
        }
        ofs << "# 2:Y";
        TString yaxis = h->GetYaxis()->GetTitle();
        if (yaxis.Length() > 0)
            ofs << " - " << yaxis;
        ofs << std::endl;
        if (!in_columns){
            int ny = h->GetNbinsY();
            ofs << "# ny " << ny
                << " ymin " << h->GetYaxis()->GetBinLowEdge(1)
                << " ymax " << h->GetYaxis()->GetBinLowEdge(ny)
                << std::endl;
        }
        ofs << "# 3:Z";
        TString zaxis = h->GetZaxis()->GetTitle();
        if (zaxis.Length() > 0)
            ofs << " - " << zaxis;
        ofs << std::endl;
    }

    if (in_columns){
        for (int i = 1; i <= h->GetNbinsX(); i++){
            for (int j = 1; j <= h->GetNbinsY(); j++){
                ofs << h->GetXaxis()->GetBinCenter(i) << " "
                    << h->GetYaxis()->GetBinCenter(j) << " "
                    << h->GetBinContent(i, j)
                    << std::endl;
            }
            ofs << std::endl;
        }
    }
    else{
        for (int j = 1; j <= h->GetNbinsY(); j++){
            for (int i = 1; i <= h->GetNbinsX(); i++){
                if (i > 1)
                    ofs << " ";
                ofs << h->GetBinContent(i, j);
            }
            ofs << std::endl;
        }
    }

    return true;
}

bool ROOTToText::SaveGraph(const TGraph *gr, char *filename, Option_t *opt) const{
    if (gr->IsA() != TGraph::Class() && gr->IsA() != TGraphErrors::Class() && gr->IsA() != TGraph2D::Class())
        std::cout << "Warning: only limited support for class " << gr->Class_Name() << std::endl;

    TString option(opt);
    option.ToUpper();
    bool with_errors = (gr->IsA() == TGraphErrors::Class());
    bool with_herrors = with_errors && option.Contains("H");
    bool with_zdata = (gr->IsA() == TGraph2D::Class());

    TString path = getFilePath(gr, filename);
    std::ofstream ofs(path);
    if (!ofs.is_open()){
        std::cerr << "Error: could not open file " << path << std::endl;
        return false;
    }

    if (headerTitle_)
        ofs << "# " << gr->GetTitle() << std::endl;

    if (headerAxis_){
        ofs << "# 1:X";
        TString xaxis = gr->GetXaxis()->GetTitle();
        if (xaxis.Length() > 0)
            ofs << " - " << xaxis;
        ofs << std::endl;
        ofs << "# 2:Y";
        TString yaxis = gr->GetYaxis()->GetTitle();
        if (yaxis.Length() > 0)
            ofs << " - " << yaxis;
        ofs << std::endl;
        if (with_errors){
            if (with_herrors)
                ofs << "# 3:EY\n# 4:EX" << std::endl;
            else
                ofs << "# 3:EY" << std::endl;
        }
        else if (with_zdata){
            ofs << "# 3:Z";
            TString yaxis = gr->GetYaxis()->GetTitle();
            if (yaxis.Length() > 0)
                ofs << " - " << yaxis;
            ofs << std::endl;
        }
    }

    Double_t *XX = gr->GetX();
    Double_t *YY = gr->GetY();
    Double_t *EX = gr->GetEX();
    Double_t *EY = gr->GetEY();
    Int_t *idx = new Int_t[gr->GetN()];
    TMath::Sort(gr->GetN(), XX, idx);
    Double_t *ZZ = 0;
    if (with_zdata)
        ZZ = ((TGraph2D *)gr)->GetZ();

    for (int i = 1; i < gr->GetN(); i++){
        Int_t k = idx[i];
        ofs << XX[k] << " " << YY[k];
        if (with_errors){
            if (with_herrors)
                ofs << " " << EX[k] << " " << EY[k];
            else
                ofs << " " << EY[k];
        }
        else if (with_zdata){
            ofs << " " << ZZ[k];
        }
        ofs << std::endl;
    }
    delete[] idx;
    return true;
}