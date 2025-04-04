#include "PlotSerializer.hh"

#include "TColor.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TList.h"
#include "TMultiGraph.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TROOT.h"
#include "TString.h"
#include "TText.h"

namespace Expad {

PlotSerializer::PlotSerializer(TVirtualPad* pad) : pad_(pad) {
    ExtractPadProperties();
}

PlotSerializer::~PlotSerializer() {
}

void PlotSerializer::Restart() {
    pp_.data.clear();
    pp_ = PadProperties();
    ExtractPadProperties();
}

int PlotSerializer::GetNumberOfDatasets() const {
    return pp_.data.size();
}

DataProperties1D PlotSerializer::GetDatasetProperties(int i) const {
    return pp_.data.at(i);
}

TString PlotSerializer::GetPlotTitle() const {
    return TString(pp_.title);
}

TString PlotSerializer::GetXaxisTitle() const {
    // return TString::Format("%s from %.1f to %.1f", pp_.xaxis.title, pp_.xaxis.min, pp_.xaxis.max);
    return TString(pp_.xaxis.title);
}

TString PlotSerializer::GetYaxisTitle() const {
    return TString(pp_.yaxis.title);
}

int PlotSerializer::GetLegendPosition() const {
    return pp_.legend;
}

void PlotSerializer::ExtractPadProperties() {
    bool axis_needed = true;
    const TLegend* legend = nullptr;
    for (const TObject* obj_p : *(pad_->GetListOfPrimitives())) {
        DataType data = GetDataType(obj_p);
        if (data != Undefined) {
            if (data < 10) {
                // 1D data
                StoreDataWithAxis(obj_p, data, axis_needed);
                if (legend) {
                    // Update this object's label (legend has already been read)
                    for (const TObject* obj_l : *legend->GetListOfPrimitives()) {
                        const TLegendEntry* entry = static_cast<const TLegendEntry*>(obj_l);
                        const TObject* entry_obj = entry->GetObject();
                        if (entry_obj != obj_l) continue;
                        pp_.data.back().label = entry->GetLabel();
                        break; // obj_l found : end the loop
                    }
                }
            }
            else if (data < 100) {
                throw std::invalid_argument("2D/3D plots are not supported yet.");
            }
            else {
                // other graphics entities (text, legend, ...)
                if (data == TextBox) {
                    if (strcmp(obj_p->GetName(), "title") == 0) {
                        // title of the plot
                        pp_.title = ((TPaveText*)obj_p)->GetLine(0)->GetTitle();
                    }
                }
                else if (data == Legend) {
                    if (legend)
                        throw std::runtime_error("A legend has already been registered.");
                    legend = dynamic_cast<const TLegend*>(obj_p);
                    GetLegend(legend);
                }
            }
        }
    }
    if (!pp_.data.size())
        throw std::runtime_error("ExPaD failed to export this plot (no compatible data was found).");
}

void PlotSerializer::StoreData(const TObject* obj, DataType data_type) {
    if (data_type == MultiGraph1D) {
        for (const TObject* gr : *(((TMultiGraph*)obj)->GetListOfGraphs())) {
            StoreData(gr, Graph1D);
        }
    }
    else {
        DataProperties1D prop;
        prop.obj = obj;
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
            prop.marker.size = 10 * marker->GetMarkerSize();
            prop.marker.style = marker->GetMarkerStyle();
        }
        pp_.data.push_back(prop);
    }
}

void PlotSerializer::StoreDataWithAxis(const TObject* obj, DataType data_type, Bool_t& get_axis) {
    StoreData(obj, data_type);
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
            case MultiGraph1D:
                h = ((TMultiGraph*)obj)->GetHistogram();
                break;
            default:
                throw std::invalid_argument("Unexpected data type (" + std::to_string(data_type) + ")");
        }

        if (GetAxis(h))
            get_axis = false;
        else
            std::cout << "Warning : could not get axis from " << obj->GetName() << std::endl;
    }
}

bool PlotSerializer::GetAxis(const TH1* h) {
    if (!h) return false;

    const TAxis* xx = h->GetXaxis();
    pp_.xaxis.title = xx->GetTitle();
    pp_.xaxis.color = Color(xx->GetAxisColor());
    pp_.xaxis.min = h->GetBinLowEdge(xx->GetFirst());
    pp_.xaxis.max = h->GetBinLowEdge(xx->GetLast() + 1);
    pp_.xaxis.log = (pad_->GetLogx() == 1);

    const TAxis* yy = h->GetYaxis();
    pp_.yaxis.title = yy->GetTitle();
    pp_.yaxis.color = Color(yy->GetAxisColor());
    pp_.yaxis.min = h->GetMinimum();
    pp_.yaxis.max = h->GetMaximum();
    pp_.yaxis.log = (pad_->GetLogy() == 1);

    return true;
}

bool PlotSerializer::GetLegend(const TLegend* leg) {
    if (!leg) return false;
    // approximate position of the legend
    // (1 -> top left ; 2 -> top right ; 3 -> bottom left ; 4 -> bottom right)
    pp_.legend = 1;
    if (leg->GetX1NDC() + leg->GetX2NDC() > 1.) pp_.legend += 1;
    if (leg->GetY1NDC() + leg->GetY2NDC() < 1.) pp_.legend += 2;
    // Update labels for objects that have already been registered
    for (const TObject* obj : *leg->GetListOfPrimitives()) {
        const TLegendEntry* entry = static_cast<const TLegendEntry*>(obj);
        const TObject* entry_obj = entry->GetObject();
        TString label = entry->GetLabel();
        if (label.CompareTo(entry_obj->GetTitle()) == 0) continue; // the label is the object title, no need to change anything
        for (DataProperties1D& dp : pp_.data) {
            if (dp.obj == entry_obj) {
                dp.label = label;
                break; // obj found : end the loop
            }
        }
    }
    return true;
}

RGBAcolor PlotSerializer::Color(Color_t ci) const {
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
