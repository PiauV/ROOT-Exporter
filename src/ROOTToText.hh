#include "TROOT.h"
#include "TString.h"

class TH1;
class TH2;
class TGraph;
class TGraph2D;

class ROOTToText {
public:
    ~ROOTToText();

    static ROOTToText *GetInstance();

    inline void SetHeader(bool title = true, bool axis = true);
    void SetFileExtension(TString ext);
    void SetDirectory(TString dir);
    inline void SetCommentChar(char c);
    inline TString GetFileExtension() const;
    inline TString GetDirectory() const;
    inline char GetCommentChar() const;
    // set precision, format ?

    bool SaveObject(const TObject *obj, char *filename = "", Option_t *opt = "") const;
    bool SaveTH1(const TH1 *h, char *filename = "", Option_t *opt = "") const;
    bool SaveTH2(const TH2 *h, char *filename = "", Option_t *opt = "") const;
    bool SaveGraph(const TGraph *gr, char *filename = "", Option_t *opt = "") const;
    bool SaveGraph2D(const TGraph2D *gr, char *filename = "", Option_t *opt = "") const;
    // TMultiGraph ?
    // TF1 ? (evaluated)

private:
    ROOTToText();
    TString GetFilePath(const TObject *obj, char *filename) const;

private:
    bool headerTitle_ = true;
    bool headerAxis_ = true;
    TString defaultExtension_;
    TString defaultDirectory_;
    static ROOTToText *instance_;
    char cc_; // comment character
};

R__EXTERN ROOTToText *gRTT;

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

void ROOTToText::SetCommentChar(char c){
    cc_ = c;
}

char ROOTToText::GetCommentChar() const{
    return cc_;
}
