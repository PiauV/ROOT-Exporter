#include "GleExportManager.hh"

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
} // namespace

namespace Expad {

GleExportManager::GleExportManager() {
    ext_ = ".gle";
    com_ = '!';
}

GleExportManager::~GleExportManager() {
}

void GleExportManager::WriteToFile(const char* filename, const PadProperties& pp) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
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

} // namespace Expad
