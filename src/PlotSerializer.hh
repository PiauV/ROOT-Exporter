#ifndef PLOTSERIALIZER_HH
#define PLOTSERIALIZER_HH

#include <vector>

#include "Rtypes.h"
#include "TString.h"

#include "DataType.hh"

class TH1;
class TVirtualPad;
class TObject;

namespace Expad {

struct RGBAcolor {
    double red = 0.0;
    double blue = 0.0;
    double green = 0.0;
    double alpha = 0.0;
};

struct AxisProperties {
    TString title;
    bool log = false;
    double min = 0.0;
    double max = 0.0;
    RGBAcolor color;
};

struct DrawingStyle {
    unsigned short style = 0;
    unsigned short size = 1;
    RGBAcolor color;
};

struct DataProperties1D {
    const TObject* obj;
    DataType type;
    TString label;
    DrawingStyle marker;
    DrawingStyle line;
};

struct PadProperties {
    TString title;
    AxisProperties xaxis;
    AxisProperties yaxis;
    std::vector<DataProperties1D> data;
};

class PlotSerializer {
public:
    PlotSerializer(TVirtualPad* pad);
    ~PlotSerializer();

    void Restart();

    // public member functions for checking serialization output
    int GetNumberOfDatasets() const;
    DataProperties1D GetDatasetProperties(int i) const;
    TString GetPlotTitle() const;
    TString GetXaxisTitle() const;
    TString GetYaxisTitle() const;

private:
    void ExtractPadProperties();
    void StoreData(const TObject* obj, DataType data_type);
    void StoreDataWithAxis(const TObject* obj, DataType data_type, Bool_t& get_axis);
    bool GetAxis(const TH1* h);
    RGBAcolor Color(Color_t color) const;

private:
    TVirtualPad* pad_;
    PadProperties pp_;
};

} // namespace Expad

#endif
