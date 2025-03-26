#include "PlotSerializer.hh"

#include "TColor.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1.h"
#include "TList.h"
#include "TPad.h"
#include "TROOT.h"

namespace Expad {

PlotSerializer::PlotSerializer(TVirtualPad* pad) : pad_(pad) {
    ExtractPadProperties();
}

PlotSerializer::~PlotSerializer() {
}

int PlotSerializer::GetNumberOfDatasets() const {
    return pp_.data.size();
}

TString PlotSerializer::GetDatasetLabel(int i) const {
    return TString(pp_.data.at(i).label);
}

TString PlotSerializer::GetXaxisTitle() const {
    // return TString::Format("%s from %.1f to %.1f", pp_.xaxis.title, pp_.xaxis.min, pp_.xaxis.max);
    return TString(pp_.xaxis.title);
}

TString PlotSerializer::GetYaxisTitle() const {
    return TString(pp_.yaxis.title);
}

void PlotSerializer::ExtractPadProperties() {
    bool setupAxis = true;
    for (const TObject* obj : *(pad_->GetListOfPrimitives())) {
        DataType data = GetDataType(obj);
        if (data != Undefined) {
            if (data < 10) {
                // 1D data
                StoreData(obj, data, setupAxis);
            }
            else {
                throw std::domain_error("2D/3D plots are not supported yet.");
            }
        }
    }
    if (!pp_.data.size())
        throw std::runtime_error("ExPaD failed to export this plot (no compatible data was found).");
}

void PlotSerializer::StoreData(const TObject* obj, DataType data_type, Bool_t& get_axis) {
    DataProperties1D prop;
    prop.data = obj;
    prop.type = data_type;
    prop.label = obj->GetTitle();
    const TAttLine* line = dynamic_cast<const TAttLine*>(obj);
    if (!line)
        std::cout << "Warning : could not get line attributes from " << obj->GetName() << std::endl;
    else {
        prop.line.color = Color(line->GetLineColor());
        prop.line.size = line->GetLineWidth();
        prop.line.style = line->GetLineStyle();
    }
    const TAttMarker* marker = dynamic_cast<const TAttMarker*>(obj);
    if (!marker)
        std::cout << "Warning : could not get marker attributes from " << obj->GetName() << std::endl;
    else {
        prop.marker.color = Color(marker->GetMarkerColor());
        prop.marker.size = marker->GetMarkerSize();
        prop.marker.style = marker->GetMarkerStyle();
    }
    pp_.data.push_back(prop);
    if (get_axis) {
        const TH1* h = nullptr;
        switch (data_type) {
            case Histo1D:
                h = (TH1*)obj;
                break;
            case Function1D:
                h = ((TF1*)obj)->GetHistogram();
                break;
            case Graph1D:
                h = ((TGraph*)obj)->GetHistogram();
                break;
            default:
                throw std::domain_error("Unexpected data type (" + std::to_string(data_type) + ")");
        }

        if (GetAxis(h))
            get_axis = false;
        else
            std::cout << "Warning : could not get axis from " << obj->GetName() << std::endl;
    }
}

bool PlotSerializer::GetAxis(const TH1* h) {
    if (!h) return false;

    TAxis* xx = h->GetXaxis();
    pp_.xaxis.title = xx->GetTitle();
    pp_.xaxis.color = Color(xx->GetAxisColor());
    pp_.xaxis.min = h->GetBinLowEdge(xx->GetFirst());
    pp_.xaxis.max = h->GetBinLowEdge(xx->GetLast() + 1);
    pp_.xaxis.log = (pad_->GetLogx() == 1);

    TAxis* yy = h->GetYaxis();
    pp_.yaxis.title = yy->GetTitle();
    pp_.yaxis.color = Color(yy->GetAxisColor());
    pp_.yaxis.min = h->GetMinimum();
    pp_.yaxis.max = h->GetMaximum();
    pp_.yaxis.log = (pad_->GetLogy() == 1);

    return true;
}

PlotSerializer::RGBAcolor PlotSerializer::Color(Color_t ci) const {
    TColor* color = gROOT->GetColor(ci);
    RGBAcolor c;
    if (color) {
        c.red = color->GetRed();
        c.green = color->GetGreen();
        c.blue = color->GetBlue();
        c.alpha = color->GetAlpha();
    }
    return c;
}

} // namespace Expad
