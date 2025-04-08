#ifndef ROOTTOTEXT_HH
#define ROOTTOTEXT_HH

#include "TROOT.h"
#include "TString.h"

#include "DataType.hh"

#include <map>

class TH1;
class TH2;
class TGraph;
class TMultiGraph;
class TGraph2D;
class TF1;
class TClass;

// The user writers must have this signature
// Hence, the object should be cast *inside* the writer, and *must* inherit from TObject
// This is not the most convenient way, but it is a more straightforward approach than using templates
// using writer = std::function<void(const TObject* obj, const TString& option, std::ofstream& ofs)>; // not compatible with my old ROOT5 + MSVC 12.0 config
typedef void (*writer)(const TObject* obj, const TString& option, std::ofstream& ofs);

namespace Expad {

// struct dummyWriter {
//     void operator()(const TObject* obj, const TString& option, std::ofstream& ofs) {
//         return;
//     }
// };

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
    bool AddCustomWriter(const char* class_name, writer& func);
    bool RemoveCustomWriter(const char* class_name);

    bool SaveObject(const TObject* obj, const char* filename = "", Option_t* opt = "") const;
    bool SaveObject(const TObject* obj, DataType dt, const char* filename = "", Option_t* opt = "") const;

private:
    ROOTToText();
    TString GetFilePath(const TObject* obj, const char* filename) const;

    bool SaveMultiGraph(const TMultiGraph* mg, const char* filename = "", Option_t* opt = "") const;

    void WriteTH1(const TH1* h, const TString& option, std::ofstream& ofs) const;
    void WriteTH2(const TH2* h, const TString& option, std::ofstream& ofs) const;
    void WriteGraph(const TGraph* gr, const TString& option, std::ofstream& ofs) const;
    void WriteGraph2D(const TGraph2D* gr, const TString& option, std::ofstream& ofs) const;
    void WriteTF1(const TF1* f, const TString& option, std::ofstream& ofs) const;

private:
    bool headerTitle_ = true;
    bool headerAxis_ = true;
    TString defaultExtension_;
    TString defaultDirectory_;
    static ROOTToText* instance_;
    char cc_;   // comment character
    bool verb_; // verbose
    std::map<TClass*, writer> userWriters_;
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
