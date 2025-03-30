#ifndef ROOTTOTEXT_HH
#define ROOTTOTEXT_HH

#include "TROOT.h"
#include "TString.h"

#include "DataType.hh"

class TH1;
class TH2;
class TGraph;
class TMultiGraph;
class TGraph2D;
class TF1;

namespace Expad {

class ROOTToText {
public:
    ~ROOTToText();

    static ROOTToText* GetInstance();

    inline void SetHeader(bool title = true, bool axis = true);
    void SetFileExtension(TString ext);
    void SetDirectory(TString dir);
    inline void SetCommentChar(char c);
    inline TString GetFileExtension() const;
    inline TString GetDirectory() const;
    inline char GetCommentChar() const;
    inline void SetVerbose(bool v);
    // set precision, format ?

    bool SaveObject(const TObject* obj, const char* filename = "", Option_t* opt = "") const;
    bool SaveObject(const TObject* obj, DataType dt, const char* filename = "", Option_t* opt = "") const;
    bool SaveTH1(const TH1* h, const char* filename = "", Option_t* opt = "") const;
    bool SaveTH2(const TH2* h, const char* filename = "", Option_t* opt = "") const;
    bool SaveGraph(const TGraph* gr, const char* filename = "", Option_t* opt = "") const;
    bool SaveMultiGraph(const TMultiGraph* mg, const char* filename = "", Option_t* opt = "") const;
    bool SaveGraph2D(const TGraph2D* gr, const char* filename = "", Option_t* opt = "") const;
    bool SaveTF1(const TF1* f, const char* filename = "", Option_t* opt = "") const;

private:
    ROOTToText();
    TString GetFilePath(const TObject* obj, const char* filename) const;

private:
    bool headerTitle_ = true;
    bool headerAxis_ = true;
    TString defaultExtension_;
    TString defaultDirectory_;
    static ROOTToText* instance_;
    char cc_;   // comment character
    bool verb_; // verbose
};

void ROOTToText::SetHeader(bool title, bool axis) {
    headerTitle_ = title;
    headerAxis_ = axis;
}

TString ROOTToText::GetFileExtension() const {
    return defaultExtension_;
}

TString ROOTToText::GetDirectory() const {
    return defaultDirectory_;
}

void ROOTToText::SetCommentChar(char c) {
    cc_ = c;
}

char ROOTToText::GetCommentChar() const {
    return cc_;
}

void ROOTToText::SetVerbose(bool v) {
    verb_ = v;
}

} // namespace Expad

R__EXTERN Expad::ROOTToText* gRTT;

#endif
