#ifndef PLOTSERIALIZER_HH
#define PLOTSERIALIZER_HH

#include "DataType.hh"
#include "PadProperties.hh"

#include "Rtypes.h"
#include "TString.h"

class TH1;
class TVirtualPad;
class TLegend;
class TObject;

namespace REx {

class PlotSerializer {
public:
    PlotSerializer(TVirtualPad* pad);
    ~PlotSerializer();

    void Restart();

    // public member functions for checking serialization output
    int GetNumberOfDatasets() const;
    TString GetDatasetTitle(int i) const;
    TString GetPlotTitle() const;
    TString GetXaxisTitle() const;
    TString GetYaxisTitle() const;
    int GetLegendPosition() const;

private:
    void ExtractPadProperties();
    void StoreData(const TObject* obj, DataType data_type, const TString& extra_opts = "");
    void StoreDataWithAxis(const TObject* obj, DataType data_type, Bool_t& get_axis);
    void StoreDecorator(const TObject* obj, DataType data_type);
    bool GetAxis(const TH1* h);
    bool GetLegend(const TLegend* leg);
    PadProperties::Color GetColor(Color_t color) const;

private:
    TVirtualPad* pad_;
    PadProperties pp_;
    std::vector<const TObject*> dataObjects_;

    friend class BaseExportManager;
};

} // namespace REx

#endif
