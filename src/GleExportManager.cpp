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
    latex_ = true;
}

GleExportManager::~GleExportManager() {
}

void GleExportManager::EnableLatex(bool flag) {
    latex_ = flag;
}

void GleExportManager::WriteToFile(const char* filename, const PadProperties& pp) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return;
    }

    // write default header for configuration (size, font, etc...)
    InitFile(ofs);

    const PadProperties::Color black(0, 0, 0);

    // >>> plot data
    ofs << "begin graph\n\tscale auto" << std::endl;
    if (pp.title.Length()) {
        ofs << "\ttitle \"" << ProcessLatex(pp.title) << "\"" << std::endl;
    }
    // draw axis
    ofs << "\txtitle \"" << ProcessLatex(pp.xaxis.title) << "\"" << std::endl;
    ofs << "\txaxis min " << pp.xaxis.min << " max " << pp.xaxis.max;
    if (pp.xaxis.log) ofs << " log";
    ofs << std::endl;
    if (pp.xaxis.color != black) {
        auto c = pp.xaxis.color;
        ofs << "\txaxis color rgb(" << c.red << "," << c.green << "," << c.blue << ")" << std::endl;
    }
    ofs << "\tytitle \"" << ProcessLatex(pp.yaxis.title) << "\"" << std::endl;
    ofs << "\tyaxis min " << pp.yaxis.min << " max " << pp.yaxis.max;
    if (pp.yaxis.log) ofs << " log";
    ofs << std::endl;
    if (pp.yaxis.color != black) {
        auto c = pp.yaxis.color;
        ofs << "\tyaxis color rgb(" << c.red << "," << c.green << "," << c.blue << ")" << std::endl;
    }
    ofs << std::endl;

    // read data files
    for (const auto& d : pp.datasets) {
        ofs << "\tdata \"" << d.file.first << "\"" << std::endl;
    }

    // draw data points
    int n = pp.datasets.size();
    int idx_data = 1;
    for (int i = 0; i < n; i++) {
        // first line : marker, line & color
        ofs << "\n\td" << idx_data;
        auto ci = black;
        auto mi = pp.datasets[i].marker;
        if (mi.style) {
            ofs << " marker " << (GLE_marker.count(mi.style) ? GLE_marker.at(mi.style) : "circle");
            ofs << " msize " << 0.02 * mi.size; // 0.2 / 10
            ci = mi.color;
        }
        auto li = pp.datasets[i].line;
        if (li.style) {
            if (pp.datasets[i].type == Histo1D)
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
        if (ci != black)
            ofs << " color rgb(" << ci.red << "," << ci.green << "," << ci.blue << ")";
        ofs << std::endl;

        // next line : label
        if (pp.legend)
            ofs << "\td" << idx_data << " key \"" << ProcessLatex(pp.datasets[i].label) << "\"" << std::endl;

        // last line : errors (if any)
        int ncol = pp.datasets[i].file.second;
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

    // configure legend
    if (pp.legend) {
        ofs << "\nkey compact pos ";
        ofs << (pp.legend > 2 ? "b" : "t") << (pp.legend / 2 ? "l" : "r"); // (1 -> tl ; 2 -> tr ; 3 -> bl ; 4 -> br)
        ofs << " hei 0.3 offset 0.2 0.2" << std::endl;
    }

    ofs << "\nend graph" << std::endl;
    // plot data <<<

    ofs.close();
}

void GleExportManager::InitFile(std::ofstream& ofs) const {
    ofs << "size 8 6\n"
        << "set hei 0.353\n"
        << "set lwidth 0.015\n"
        << "set texlabels 1\n"
        << std::endl;
}

TString GleExportManager::ProcessLatex(const TString& str) const {
    if (!latex_) return str;
    TString ltx(str);
    int s = ltx.Index('#');
    while (s >= 0) {
        ltx.Replace(s, 1, "\\");
        ltx.Insert(s, '$');
        int s0 = s;
        s = ltx.Index(' ', s0);
        if (s >= 0)
            ltx.Insert(s, '$');
        else
            ltx.Append('$');
        s0 = ltx.Index(s, '$');
        s = ltx.Index('#', s0);
    }
    return ltx;
}

} // namespace Expad
