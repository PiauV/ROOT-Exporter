#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TString.h"

class ROOTToText {
public:
    ~ROOTToText();

    static ROOTToText* getInstance();

    inline void SetHeader(bool title = true, bool axis = true);
    void SetDefaultExtension(TString ext);
    void SetDirectory(TString dir);
    inline TString GetDefaultExtension() const;
    inline TString GetDirectory() const;
    // set precision, format ?

    bool SaveObject(const TObject *obj, char *filename = "", Option_t *opt = "") const;
    bool SaveTH1(const TH1 *h, char *filename = "", Option_t *opt = "") const;
    bool SaveTH2(const TH2 *h, char *filename = "", Option_t *opt = "") const;
    bool SaveGraph(const TGraph *gr, char *filename = "", Option_t *opt = "") const;
    // TMultiGraph ?
    // TF1 ? (evaluated)

private:
    ROOTToText();
    TString getFilePath(const TObject *obj, char *filename) const;

private:
    bool headerTitle_ = true;
    bool headerAxis_ = true;
    TString defaultExtension_;
    TString defaultDirectory_;
    static ROOTToText* instance_;
};

R__EXTERN ROOTToText* gRTT;

void ROOTToText::SetHeader(bool title, bool axis){
    headerTitle_ = title;
    headerAxis_ = axis;
}

TString ROOTToText::GetDefaultExtension() const{
    return defaultExtension_;
}

TString ROOTToText::GetDirectory() const{
    return defaultDirectory_;
}