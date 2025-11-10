#ifndef ROOTTOTEXT_HH
#define ROOTTOTEXT_HH

#include "TROOT.h"
#include "TString.h"

#include "DataType.hh"

#include <map>

class TH1;
class TH2;
class THStack;
class TGraph;
class TMultiGraph;
class TGraph2D;
class TF1;
class TClass;

namespace REx {

// The user writers must have this signature
// Hence, the object should be cast *inside* the writer, and *must* inherit from TObject
// This is not the most convenient way, but it is a more straightforward approach than using templates
typedef void (*rtt_writer)(const TObject* obj, const TString& option, std::ofstream& ofs);

// using writer = std::function<void(const TObject* obj, const TString& option, std::ofstream& ofs)>; // not compatible with my old ROOT5 + MSVC 12.0 config

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
    inline bool GetVerbose() const;
    inline void SetDefaultNpFunction(int n);
    // set precision, format ?
    bool AddCustomWriter(const char* class_name, rtt_writer& func);
    bool RemoveCustomWriter(const char* class_name);
    void ClearCustomWriters();

    bool SaveObject(const TObject* obj, const char* filename = "", Option_t* opt = "") const;
    bool SaveObject(const TObject* obj, DataType dt, const char* filename = "", Option_t* opt = "") const;
    bool SaveObject(const TObject* obj, DataType dt, TString& filename, Option_t* opt = "") const;
    void PrintOptions() const;

private:
    ROOTToText();
    TString GetFilePath(const TObject* obj, const char* filename) const;

    bool SaveMultiGraph(const TMultiGraph* mg, TString& filename, Option_t* opt = "") const;
    bool SaveHistoStack(const THStack* hs, TString& filename, Option_t* opt = "") const;

    void WriteTH1(const TH1* h, const TString& option, std::ofstream& ofs) const;
    void WriteTH2(const TH2* h, const TString& option, std::ofstream& ofs) const;
    void WriteGraph(const TGraph* gr, const TString& option, std::ofstream& ofs) const;
    void WriteGraph2D(const TGraph2D* gr, const TString& option, std::ofstream& ofs) const;
    void WriteTF1(const TF1* f, const TString& option, std::ofstream& ofs) const;

private:
    bool headerTitle_ = true;
    bool headerAxis_ = true;
    TString defaultExtension_;
    TString baseDirectory_;
    static ROOTToText* instance_;
    char cc_;    // comment character
    bool verb_;  // verbose
    int npfunc_; // default number of points for functions (TF1)
    std::map<TClass*, rtt_writer> userWriters_;
};

/// @brief Setup the default header content
/// @param title Print object title in header
/// @param axis Print axis titles in header
void ROOTToText::SetHeader(bool title, bool axis) {
    headerTitle_ = title;
    headerAxis_ = axis;
}

TString ROOTToText::GetFileExtension() const {
    return defaultExtension_;
}

TString ROOTToText::GetDirectory() const {
    return baseDirectory_;
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

bool ROOTToText::GetVerbose() const {
    return verb_;
}

/// @brief Set the default number of points to use when saving a TF1
/// @param n number of points
inline void ROOTToText::SetDefaultNpFunction(int n) {
    npfunc_ = n;
}

} // namespace REx

R__EXTERN REx::ROOTToText* gRTT;

#endif
