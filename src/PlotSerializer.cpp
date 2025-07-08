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
    pp_.datasets.clear();
    pp_ = PadProperties();
    ExtractPadProperties();
}

int PlotSerializer::GetNumberOfDatasets() const {
    return pp_.datasets.size();
}

TString PlotSerializer::GetDatasetTitle(int i) const {
    return pp_.datasets.at(i).label;
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
        int dim = GetDataDimension(data);
        if (dim == 1) {
            // 1D data
            StoreDataWithAxis(obj_p, data, axis_needed);
            if (legend) {
                // Update this object's label (legend has already been read)
                for (const TObject* obj_l : *legend->GetListOfPrimitives()) {
                    auto entry = static_cast<const TLegendEntry*>(obj_l);
                    const TObject* entry_obj = entry->GetObject();
                    if (entry_obj != obj_l) continue;
                    pp_.datasets.back().label = entry->GetLabel();
                    break; // obj_l found : end the loop
                }
            }
        }
        else if (dim == 2 || dim == 3) {
            std::cerr << "Warning: 2D/3D plots are not supported yet." << std::endl;
            // throw std::invalid_argument("2D/3D plots are not supported yet.");
        }
        else if (dim == 0) {
            // other graphics entities (text, legend, ...)
            switch (data) {
                case TextBox:
                    if (strcmp(obj_p->GetName(), "title") == 0) {
                        // title of the plot
                        pp_.title = ((TPaveText*)obj_p)->GetLine(0)->GetTitle();
                    }
                    break;
                case Legend:
                    if (legend)
                        throw std::runtime_error("A legend has already been registered.");
                    legend = dynamic_cast<const TLegend*>(obj_p);
                    GetLegend(legend);
                    break;
                case BareText:
                case Line:
                    StoreDecorator(obj_p, data);
                    break;
                default:
                    break;
            }
        }
    }
    if (!pp_.datasets.size())
        throw std::runtime_error("ExPaD failed to export this plot (no compatible data was found).");
}

void PlotSerializer::StoreData(const TObject* obj, DataType data_type, const TString& extra_opts) {
    if (data_type == MultiGraph1D) {
        TListIter next(((TMultiGraph*)obj)->GetListOfGraphs());
        while (next()) {
            TString opt(next.GetOption());
            opt.Append(obj->GetDrawOption());
            StoreData(*next, Graph1D, opt);
        }
    }
    else {
        PadProperties::Data prop;
        prop.type = data_type;
        prop.label = obj->GetTitle();
        TString opt(obj->GetDrawOption());
        opt.Append(obj->GetOption());
        if (extra_opts.Length()) opt.Append(extra_opts);
        opt.ToUpper();
        auto line = dynamic_cast<const TAttLine*>(obj);
        if (!line)
            std::cout << "Warning : could not get line attributes from " << obj->GetName() << std::endl;
        else {
            prop.line.color = GetColor(line->GetLineColor());
            prop.line.size = line->GetLineWidth();
            prop.line.style = line->GetLineStyle();
            if (data_type == Graph1D) {
                if (!opt.Contains("L")) prop.line.style = 0;
            }
            else if (data_type == Histo1D) {
                if (opt.Contains("P")) prop.line.style = 0;
            }
        }
        auto marker = dynamic_cast<const TAttMarker*>(obj);
        if (!marker)
            std::cout << "Warning : could not get marker attributes from " << obj->GetName() << std::endl;
        else {
            prop.marker.color = GetColor(marker->GetMarkerColor());
            prop.marker.size = 10 * marker->GetMarkerSize();
            prop.marker.style = marker->GetMarkerStyle();
            if (!opt.Contains("P")) prop.marker.style = 0;
            if (data_type == Function1D) {
                prop.marker.style = 0;
            }
        }
        pp_.datasets.push_back(prop);
        dataObjects_.push_back(obj);
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

void PlotSerializer::StoreDecorator(const TObject* obj, DataType data_type) {
    PadProperties::Decorator deco;
    deco.type = data_type;
    auto line = dynamic_cast<const TAttLine*>(obj);
    if (line) {
        deco.properties.color = GetColor(line->GetLineColor());
        deco.properties.size = line->GetLineWidth();
        deco.properties.style = line->GetLineStyle();
    }
    auto txt = dynamic_cast<const TAttText*>(obj);
    if (txt) {
        deco.properties.color = GetColor(txt->GetTextColor());
    }
    switch (data_type) {
        case BareText: {
            deco.label = obj->GetTitle();
            auto tt = dynamic_cast<const TText*>(obj);
            deco.pos.set(tt->GetX(), tt->GetY());
            break;
        }
        case Line: {
            deco.label = obj->GetOption(); // arrow direction (if any)
            auto tl = dynamic_cast<const TLine*>(obj);
            deco.pos.set(tl->GetX1(), tl->GetY1(), tl->GetX2(), tl->GetY2());
            break;
        }
        default:
            break;
    }
    pp_.decorators.push_back(deco);
}

bool PlotSerializer::GetAxis(const TH1* h) {
    if (!h) return false;

    const TAxis* xx = h->GetXaxis();
    pp_.xaxis.title = xx->GetTitle();
    pp_.xaxis.color = GetColor(xx->GetAxisColor());
    pp_.xaxis.min = pad_->GetUxmin();
    pp_.xaxis.max = pad_->GetUxmax();
    pp_.xaxis.log = (pad_->GetLogx() == 1);

    const TAxis* yy = h->GetYaxis();
    pp_.yaxis.title = yy->GetTitle();
    pp_.yaxis.color = GetColor(yy->GetAxisColor());
    pp_.yaxis.min = pad_->GetUymin();
    pp_.yaxis.max = pad_->GetUymax();
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
        for (int i = 0; i < dataObjects_.size(); i++) {
            if (dataObjects_[i] == entry_obj) {
                pp_.datasets.at(i).label = label;
                break; // obj found : end the loop
            }
        }
    }
    return true;
}

PadProperties::Color PlotSerializer::GetColor(Color_t ci) const {
    TColor* color = gROOT->GetColor(ci);
    PadProperties::Color c;
    if (color) {
        c.red = color->GetRed();
        c.green = color->GetGreen();
        c.blue = color->GetBlue();
        c.alpha = color->GetAlpha();
    }
    return c;
}

} // namespace Expad
