#include "PlotSerializer.hh"
#include "Log.hh"

#include "TColor.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TLine.h"
#include "TList.h"
#include "TMultiGraph.h"
#include "TPad.h"
#include "TPaveText.h"
#include "TROOT.h"
#include "TString.h"
#include "TText.h"

namespace REx {

PlotSerializer::PlotSerializer(TVirtualPad* pad) : pad_(pad) {
    ExtractPadProperties();
}

PlotSerializer::~PlotSerializer() {
}

void PlotSerializer::Restart() {
    pp_.datasets.clear();
    pp_.decorators.clear();
    dataObjects_.clear();
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

/// @brief Extract the properties of all objects drawn in the current pad
void PlotSerializer::ExtractPadProperties() {
    bool axis_needed = true;
    const TLegend* legend = nullptr;
    for (const TObject* obj_p : *(pad_->GetListOfPrimitives())) {
        DataType data = GetDataType(obj_p);
        int dim = GetDataDimension(data);
        if (dim == 1) {
            // 1D data
            StoreDataWithAxis(obj_p, data, axis_needed);
        }
        else if (dim == 2 || dim == 3) {
            LOG_WARN("2D/3D plots are not supported yet.");
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
        throw std::runtime_error("REx failed to export this plot (no compatible data was found).");
    // update object labels if there is a legend
    if (legend) {
        for (const auto* legObj : *legend->GetListOfPrimitives()) {
            auto entry = static_cast<const TLegendEntry*>(legObj);
            const TObject* entry_obj = entry->GetObject();
            auto legend_label = entry->GetLabel();
            bool found = false;
            for (int i = 0; i < dataObjects_.size(); i++) {
                if (entry_obj == dataObjects_[i]) {
                    pp_.datasets[i].label = legend_label;
                    found = true;
                    break; // object found : go to next legend entry
                }
            }
            if (!found) {
                // object not found... may be it is a clone (different address)
                // --> so we will use the object name this time
                auto entryName = entry_obj->GetName();
                for (int i = 0; i < dataObjects_.size(); i++) {
                    if (strcmp(entryName, dataObjects_[i]->GetName()) == 0) {
                        pp_.datasets[i].label = legend_label;
                        break; // object found : go to next legend entry
                    }
                }
            }
        }
    }
}

/// @brief Extract properties of a data object
void PlotSerializer::StoreData(const TObject* obj, DataType data_type, const TString& extra_opts) {
    if (data_type == MultiGraph1D) {
        TString opt(obj->GetDrawOption());
        TListIter next(((TMultiGraph*)obj)->GetListOfGraphs());
        while (next()) {
            StoreData(*next, Graph1D, opt);
        }
    }
    else if (data_type == MultiHisto1D) {
        TString opt(obj->GetDrawOption());
        opt.ToUpper();
        TCollection* hlist = nullptr;
        Bool_t dir = kIterForward;
        if (opt.Contains("NOSTACK")) {
            hlist = ((THStack*)obj)->GetHists();
        }
        else {
            hlist = ((THStack*)obj->Clone())->GetStack();
            dir = kIterBackward; // in case of stacked histogram, the last one is plotted first
        }
        // remove drawing options specific to THStack (not relevant for TH1)
        opt.ReplaceAll("NOSTACKB", 0x0);
        opt.ReplaceAll("NOSTACK", 0x0);
        opt.ReplaceAll("PADS", 0x0);
        opt.ReplaceAll("NOCLEAR", 0x0);
        TIter next(hlist, dir);
        while (next()) {
            StoreData(*next, Histo1D, opt);
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
        if (!line) {
            LOG_WARN("Could not get line attributes from " << obj->GetName());
        }
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
        if (!marker) {
            LOG_WARN("Could not get marker attributes from " << obj->GetName());
        }
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
        if ((data_type == Histo1D || data_type == Histo2D) && !opt.Contains("HIST")) {
            // also add functions from the list of functions associated to the histogram
            // (not drawn if the histo is plotted with 'HIST' option)
            TListIter iter(((TH1*)obj)->GetListOfFunctions());
            while (auto func = iter.Next()) {
                // ignore stat box for now
                if (strcmp(func->GetName(), "stats") == 0)
                    continue;
                auto dt = GetDataType(func); // not always a function !
                // unknown objects are ignored
                if (dt == DataType::Undefined)
                    continue;
                // some functions from the list might not be plotted
                if (!func->TestBit(TF1::kNotDraw))
                    StoreData(func, dt);
            }
        }
    }
}

/// @brief Extract properties of a data object, and its associated axis
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
            case MultiHisto1D:
                h = ((THStack*)obj)->GetHistogram();
                break;
            default:
                throw std::invalid_argument("Unexpected data type (" + std::to_string(data_type) + ")");
        }

        if (GetAxis(h))
            get_axis = false;
        else
            LOG_WARN("Could not get axis from " << obj->GetName());
    }
}

/// @brief Extract properties of a decorator object
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
            deco.properties.style = tt->GetTextAlign(); // align/just
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

/// @brief Extract axis information from an histogram
bool PlotSerializer::GetAxis(const TH1* h) {
    if (!h) return false;

    const TAxis* xx = h->GetXaxis();
    pp_.xaxis.title = xx->GetTitle();
    pp_.xaxis.color = GetColor(xx->GetAxisColor());
    if (pad_->GetLogx() == 1) {
        pp_.xaxis.min = pow(10, pad_->GetUxmin());
        pp_.xaxis.max = pow(10, pad_->GetUxmax());
        pp_.xaxis.log = true;
    }
    else {
        pp_.xaxis.min = pad_->GetUxmin();
        pp_.xaxis.max = pad_->GetUxmax();
        pp_.xaxis.log = false;
    }

    const TAxis* yy = h->GetYaxis();
    pp_.yaxis.title = yy->GetTitle();
    pp_.yaxis.color = GetColor(yy->GetAxisColor());
    if (pad_->GetLogy() == 1) {
        pp_.yaxis.min = pow(10, pad_->GetUymin());
        pp_.yaxis.max = pow(10, pad_->GetUymax());
        pp_.yaxis.log = true;
    }
    else {
        pp_.yaxis.min = pad_->GetUymin();
        pp_.yaxis.max = pad_->GetUymax();
        pp_.yaxis.log = false;
    }

    return true;
}

/// @brief Store the legend position and labels
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
        for (int i = 0; i < (int)dataObjects_.size(); i++) {
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

} // namespace REx
