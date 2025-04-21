#include "GnuplotExportManager.hh"

#include "PlotSerializer.hh"

#include "TPad.h"
#include "TSystem.h"

#include <fstream>
#include <iostream>
#include <unordered_map>

namespace {
const std::unordered_map<int, int> pointtype = {
    {1, 0},
    {2, 1},
    {3, 3},
    {4, 6},
    {5, 2},
    {6, 0},
    {7, 0},
    {8, 0},
    {20, 7},
    {21, 5},
    {22, 9},
    {23, 11},
    {24, 6},
    {25, 4},
    {26, 8},
    {27, 12},
    {29, 15}, // replace star with pentagon
    {30, 14}, // replace star with pentagon
    {31, 3},
    {33, 13},
}; // ROOT marker style -> gnuplot pointtype

const std::unordered_map<int, int> dashtype = {
    {1, 1},
    {2, 2},
    {3, 3},
    {4, 4},
    {8, 5},
    {9, 2},
    {10, 4},
}; // ROOT line style -> gnuplot (native) dashtype
} // namespace

namespace Expad {

GnuplotExportManager::GnuplotExportManager() {
    ext_ = ".gp";
    com_ = '#';
}

GnuplotExportManager::~GnuplotExportManager() {
}

void GnuplotExportManager::WriteToFile(const char* filename, const PadProperties& pp) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return;
    }

    // write default header (gnuplot configuration)
    TString outfile(gSystem->BaseName(filename)); // outfile : *.gp
    WriteHeader(ofs, outfile);                    // outfile : *.tex

    const PadProperties::Color black(0, 0, 0);

    // >>> plot data
    if (pp.title.Length()) {
        ofs << "\nset title " << FormatLabel(pp.title) << std::endl;
    }
    // draw axis
    auto cx = pp.xaxis.color; // colored axis : do not change border color (tricky use of "set border" + "set arrow" will be done by user)
    ofs << "\nset xlabel " << FormatLabel(pp.xaxis.title);
    if (cx != black)
        ofs << " textcolor rgb " << cx.hex_str();
    ofs << std::endl;
    if (cx != black || pp.xaxis.log) {
        ofs << "set xtics";
        if (cx != black)
            ofs << " textcolor rgb " << cx.hex_str();
        if (pp.xaxis.log)
            ofs << " logscale";
        ofs << std::endl;
    }
    ofs << "set xrange [" << pp.xaxis.min << ":" << pp.xaxis.max << "]" << std::endl;

    auto cy = pp.yaxis.color;
    ofs << "set ylabel " << FormatLabel(pp.yaxis.title);
    if (cy != black)
        ofs << " textcolor rgb " << cy.hex_str();
    ofs << std::endl;
    if (cy != black || pp.yaxis.log) {
        ofs << "set ytics";
        if (cy != black)
            ofs << " textcolor rgb " << cy.hex_str();
        if (pp.yaxis.log)
            ofs << " logscale";
        ofs << std::endl;
    }
    ofs << "set yrange [" << pp.yaxis.min << ":" << pp.yaxis.max << "]" << std::endl;

    // configure legend
    if (pp.legend) {
        ofs << "\nset key notitle box opaque"
            << "\nset key " << (pp.legend > 2 ? "bottom" : "top") << (pp.legend % 2 ? " left" : " right") // (1 -> tl ; 2 -> tr ; 3 -> bl ; 4 -> br)
            << std::endl;
    }
    else {
        ofs << "\nunset key" << std::endl;
    }

    // draw data from files
    int n = pp.datasets.size();
    ofs << "\nplot ";
    for (int i = 0; i < n; ++i) {
        auto di = pp.datasets[i];
        ofs << " \"" << di.file.first << "\"";
        auto ci = black;     // color
        auto mi = di.marker; // marker
        auto li = di.line;   // line
        // pointtype style
        if (mi.style) {
            ofs << " ps " << 0.05 * mi.size;
            ofs << " pt " << (pointtype.count(mi.style) ? pointtype.at(mi.style) : 1);
            ci = mi.color;
        }
        // line style
        if (li.style) {
            if (li.size > 1)
                ofs << " lw " << li.size;
            if (dashtype.count(li.style)) {
                int dt = pointtype.at(li.style);
                if (dt) ofs << " dt " << dt;
            }
            ci = li.color;
        }
        // color
        ofs << " lc rgb " << ci.hex_str();

        // plotting style : points / line / error bars
        int ncol = di.file.second; // get error bars
        if (ncol == 2) {
            // no error bars
            if (li.style == 0)
                ofs << " with points";
            else {
                // with line
                if (di.type == Histo1D)
                    ofs << " with histeps";
                else {
                    if (mi.style)
                        ofs << " with linespoints";
                    else
                        ofs << " with lines";
                }
            }
        }
        else {
            // error bars
            ofs << " with "
                << (ncol > 3 ? "xyerror" : "yerror")
                << (li.style ? "lines" : "bars");
        }

        // title (key)
        if (pp.legend) {
            ofs << " t " << FormatLabel(di.label);
        }
        // end of line
        if (i < n - 1)
            ofs << ", \\\n     "; // go to next line and align
        else
            ofs << std::endl;
    }
    // plot data <<<

    ofs << "\nunset output"
        << "\n!pdflatex " << outfile
        << std::endl;

    ofs.close();
}

TString GnuplotExportManager::FormatLabel(const TString& str) const {
    TString ltx = ExportManager::FormatLabel(str);
    if (latex_) {
        int s = ltx.Index("$\\");
        while (s >= 0) {
            ltx.Replace(s, 2, "$\\\\");
            if (s + 2 >= ltx.Length())
                s = -1; // should never happen, but it's better to be safe
            else {
                s = ltx.Index("$\\", s + 2);
            }
        }
    }
    return ltx;
}

void GnuplotExportManager::WriteHeader(std::ofstream& ofs, TString& file) const {
    // filename.gp --> filename.tex
    file.Replace(file.Index(ext_), 4, ".tex");
    ofs << "set terminal cairolatex pdf standalone size 10cm,7cm\n"
        << "set output \"" << file << "\""
        << std::endl;
}

} // namespace Expad
