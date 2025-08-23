#include "GleExportManager.hh"

#include "Log.hh"
#include "PlotSerializer.hh"

#include "TPad.h"
#include "TSystem.h"

#include <fstream>
#include <iostream>
#include <unordered_map>

namespace {
const std::unordered_map<int, std::string> GLE_marker = {
    {1, "dot"},
    {2, "plus"},
    {3, "asterisk"},
    {4, "circle"},
    {5, "cross"},
    {6, "dot"},
    {7, "dot"},
    {20, "fcircle"},
    {21, "fsquare"},
    {22, "ftriangle"},
    {23, "ftriangle"},
    {24, "circle"},
    {25, "square"},
    {26, "triangle"},
    {27, "diamond"},
    {29, "star"},
    {30, "star"},
    {33, "fdiamond"},
};

const std::unordered_map<int, int> GLE_line = {
    {1, 1},
    {2, 2},
    {3, 9},
    {10, 6},
};

const std::unordered_map<int, std::string> GLE_arrow = {
    {0, ""},
    {1, "arrow end"},
    {2, "arrow start"},
    {3, "arrow both"},
};

const std::unordered_map<int, std::string> GLE_just = {
    {0, ""},
    {11, "bl"},
    {12, "cl"},
    {13, "tl"},
    {21, "bc"},
    {22, "cc"},
    {23, "tc"},
    {31, "br"},
    {32, "cr"},
    {33, "tr"},
};

} // namespace

namespace REx {

GleExportManager::GleExportManager() {
    ext_ = ".gle";
    com_ = '!';
}

GleExportManager::~GleExportManager() {
}

void GleExportManager::WriteToFile(const char* filename, const PadProperties& pp) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        LOG_ERROR("Could not open file " << filename);
        return;
    }

    // write default header (gle configuration : size, font, etc...)
    InitFile(ofs);

    // >>> plot data

    // start graph
    ofs << "begin graph\n"
        << "\tscale auto\n"
        << std::endl;

    SetTitleAndAxis(ofs, pp);

    // draw data points from file
    SetData(ofs, pp);

    // configure legend
    SetLegend(ofs, pp);

    // close graph
    ofs << "\nend graph" << std::endl;

    // plot data <<<

    // plot other graphical elements
    SetDecorators(ofs, pp);

    ofs.close();
}

void GleExportManager::InitFile(std::ofstream& ofs) const {
    ofs << "size 10 7\n"
        << "set hei 0.353\n"
        << "set lwidth 0.015\n"
        << "set texlabels 1\n"
        << std::endl;
}

void GleExportManager::SetTitleAndAxis(std::ofstream& ofs, const PadProperties& pp) const {
    // set title
    if (pp.title.Length()) {
        ofs << "\ttitle " << FormatLabel(pp.title) << std::endl;
    }
    // draw axis
    ofs << "\txtitle " << FormatLabel(pp.xaxis.title) << std::endl;
    ofs << "\txaxis min " << pp.xaxis.min << " max " << pp.xaxis.max;
    if (pp.xaxis.log) ofs << " log";
    ofs << std::endl;
    auto xc = pp.xaxis.color;
    if (xc != Black) {
        ofs << "\txaxis color " << xc.rgb_str() << std::endl;
    }
    ofs << "\tytitle " << FormatLabel(pp.yaxis.title) << std::endl;
    ofs << "\tyaxis min " << pp.yaxis.min << " max " << pp.yaxis.max;
    if (pp.yaxis.log) ofs << " log";
    ofs << std::endl;
    auto yc = pp.yaxis.color;
    if (yc != Black) {
        ofs << "\tyaxis color " << yc.rgb_str() << std::endl;
    }
    ofs << std::endl;
}

void GleExportManager::SetData(std::ofstream& ofs, const PadProperties& pp) const {
    int n = pp.datasets.size();
    int idx_data = 1;
    for (int i = 0; i < n; i++) {
        const auto di = pp.datasets[i];
        // read data file
        ofs << "\tdata \"" << di.file.first << "\"" << std::endl;

        // marker, line & color
        ofs << "\n\td" << idx_data;
        auto ci = Black;
        auto mi = di.marker;
        if (mi.style) {
            ofs << " marker " << (GLE_marker.count(mi.style) ? GLE_marker.at(mi.style) : "circle");
            ofs << " msize " << 0.02 * mi.size; // 0.2 / 10
            ci = mi.color;
        }
        auto li = di.line;
        if (li.style) {
            if (di.type == Histo1D)
                ofs << " line hist";
            else {
                if (li.style != 1 && GLE_line.count(li.style))
                    ofs << " lstyle " << GLE_line.at(li.style);
                else
                    ofs << " line";
            }
            ofs << " lwidth " << 0.015 * li.size;
            ci = li.color;
        }
        if (ci != Black)
            ofs << " color " << ci.rgb_str();
        ofs << std::endl;

        // key (legend)
        if (pp.legend)
            ofs << "\td" << idx_data << " key " << FormatLabel(di.label) << std::endl;

        // errors (if any)
        int ncol = di.file.second;
        if (ncol == 3) {
            ofs << "\td" << idx_data
                << " err d" << idx_data + 1 << " errwidth 0.05"
                << std::endl;
        }
        else if (ncol == 4) {
            ofs << "\td" << idx_data
                << " herr d" << idx_data + 1 << " herrwidth 0.05"
                << " err d" << idx_data + 2 << " errwidth 0.05"
                << std::endl;
        }
        idx_data += (ncol - 1); // first column is x
    }
}

void GleExportManager::SetLegend(std::ofstream& ofs, const PadProperties& pp) const {
    if (pp.legend) {
        ofs << "\n\tkey compact pos ";
        ofs << (pp.legend > 2 ? "b" : "t") << (pp.legend % 2 ? "l" : "r"); // (1 -> tl ; 2 -> tr ; 3 -> bl ; 4 -> br)
        ofs << " hei 0.3 offset 0.2 0.2" << std::endl;
    }
}

void GleExportManager::SetDecorators(std::ofstream& ofs, const PadProperties& pp) const {
    if (pp.decorators.size()) {
        auto current_color = Black;
        unsigned short current_alignment = 0;
        bool filled_arrow = false;
        for (const auto& d : pp.decorators) {
            ofs << std::endl;
            if (d.properties.color != current_color) {
                ofs << "set color " << d.properties.color.rgb_str() << std::endl;
                current_color = d.properties.color;
            }
            switch (d.type) {
                case Line: {
                    if (!d.pos.isok) {
                        LOG_WARN("Uninitialized line position");
                        continue;
                    }
                    // line style
                    auto line = d.properties;
                    if (line.size > 1)
                        ofs << "set lwidth " << 0.015 * line.size << std::endl;
                    if (line.style) ofs << "set lstyle " << GLE_line.at(line.style) << std::endl;
                    // arrow tip
                    int arrow = 0;
                    if (d.label.Contains('>')) arrow += 1;
                    if (d.label.Contains('<')) arrow += 2;
                    if ((d.label.Contains("|>") || d.label.Contains("<|"))) {
                        if (!filled_arrow)
                            ofs << "set arrowstyle filled" << std::endl;
                        filled_arrow = true;
                    }
                    else if (filled_arrow) {
                        ofs << "set arrowstyle simple" << std::endl;
                        filled_arrow = false;
                    }
                    // draw the line
                    ofs << "amove xg(" << d.pos.x1 << ") yg(" << d.pos.y1 << ")" << std::endl;
                    ofs << "aline xg(" << d.pos.x2 << ") yg(" << d.pos.y2 << ") " << GLE_arrow.at(arrow) << std::endl;
                    break;
                }
                case BareText: {
                    if (d.properties.style != current_alignment) {
                        current_alignment = d.properties.style;
                        ofs << "set just " << GLE_just.at(current_alignment) << std::endl;
                    }
                    ofs << "amove xg(" << d.pos.x1 << ") yg(" << d.pos.y1 << ")" << std::endl;
                    ofs << "tex " << FormatLabel(d.label) << std::endl;
                    break;
                }
                default:
                    LOG_WARN("Decorator not implemented for GLE");
                    break;
            }
        }
    }
}

} // namespace REx
