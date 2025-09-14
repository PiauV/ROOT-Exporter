#include "GnuplotExportManager.hh"

#include "Log.hh"
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

const std::unordered_map<int, std::string> gnuplot_arrow = {
    {0, "nohead"},
    {1, "head nofilled"},
    {2, "backhead nofilled"},
    {3, "heads nofilled"},
    {10, "nohead"}, // should never happen
    {11, "head filled"},
    {12, "backhead filled"},
    {13, "heads filled"},
};

const std::unordered_map<int, std::string> gnuplot_just = {
    {0, ""},
    {1, "left"},
    {2, "center"},
    {3, "right"},
};

} // namespace

namespace REx {

GnuplotExportManager::GnuplotExportManager() {
    ext_ = ".gp";
    com_ = '#';
}

GnuplotExportManager::~GnuplotExportManager() {
}

TString GnuplotExportManager::FormatLabel(const TString& str) const {
    TString ltx = VirtualExportManager::FormatLabel(str);
    if (latex_) {
        int s = ltx.Index("\\");
        while (s >= 0) {
            ltx.Replace(s, 1, "\\\\");
            if (s + 2 >= ltx.Length())
                s = -1; // should never happen, but it's better to be safe
            else {
                s = ltx.Index("\\", s + 2);
            }
        }
    }
    return ltx;
}

void GnuplotExportManager::WriteToFile(const char* filename, const PadProperties& pp) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        LOG_ERROR("Could not open file " << filename);
        return;
    }

    TString outfile(gSystem->BaseName(filename));    // outfile : *.gp
    outfile.Replace(outfile.Index(ext_), 4, ".tex"); // outfile : *.tex

    // write default header (gnuplot configuration)
    InitFile(ofs, outfile);

    // >>> plot data
    SetTitleAndAxis(ofs, pp);

    // plot other graphical elements (must come before the plot!)
    SetDecorators(ofs, pp);

    // configure legend
    SetLegend(ofs, pp);

    // draw data from files
    SetData(ofs, pp);

    // plot data <<<

    ofs << "\nunset output"
        << "\n!pdflatex -interaction=nonstopmode " << outfile
        << std::endl;

    ofs.close();
}

void GnuplotExportManager::InitFile(std::ofstream& ofs, const TString& file) const {
    ofs << "set terminal cairolatex pdf colortext standalone size 10cm,7cm\n"
        << "set output \"" << file << "\""
        << std::endl;
}

void GnuplotExportManager::SetTitleAndAxis(std::ofstream& ofs, const PadProperties& pp) const {
    // set title
    if (pp.title.Length()) {
        ofs << "\nset title " << FormatLabel(pp.title) << std::endl;
    }

    // draw axis
    auto cx = pp.xaxis.color; // colored axis : do not change border color (tricky use of "set border" + "set arrow" will be done by user)
    ofs << "\nset xlabel " << FormatLabel(pp.xaxis.title);
    if (cx != Black)
        ofs << " textcolor rgb " << cx.hex_str();
    ofs << std::endl;
    if (cx != Black || pp.xaxis.log) {
        ofs << "set xtics";
        if (cx != Black)
            ofs << " textcolor rgb " << cx.hex_str();
        if (pp.xaxis.log)
            ofs << " logscale";
        ofs << std::endl;
    }
    ofs << "set xrange [" << pp.xaxis.min << ":" << pp.xaxis.max << "]" << std::endl;

    auto cy = pp.yaxis.color;
    ofs << "set ylabel " << FormatLabel(pp.yaxis.title);
    if (cy != Black)
        ofs << " textcolor rgb " << cy.hex_str();
    ofs << std::endl;
    if (cy != Black || pp.yaxis.log) {
        ofs << "set ytics";
        if (cy != Black)
            ofs << " textcolor rgb " << cy.hex_str();
        if (pp.yaxis.log)
            ofs << " logscale";
        ofs << std::endl;
    }
    ofs << "set yrange [" << pp.yaxis.min << ":" << pp.yaxis.max << "]" << std::endl;
}

void GnuplotExportManager::SetData(std::ofstream& ofs, const PadProperties& pp) const {
    int n = pp.datasets.size();
    ofs << "\nplot ";
    for (int i = 0; i < n; ++i) {
        auto di = pp.datasets[i];
        ofs << " \"" << di.file.first << "\"";
        auto ci = Black;     // color
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
                int dt = dashtype.at(li.style);
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
}

void GnuplotExportManager::SetLegend(std::ofstream& ofs, const PadProperties& pp) const {
    if (pp.legend) {
        ofs << "\nset key notitle box opaque"
            << "\nset key " << (pp.legend > 2 ? "bottom" : "top") << (pp.legend % 2 ? " left" : " right") // (1 -> tl ; 2 -> tr ; 3 -> bl ; 4 -> br)
            << std::endl;
    }
    else {
        ofs << "\nunset key" << std::endl;
    }
}

void GnuplotExportManager::SetDecorators(std::ofstream& ofs, const PadProperties& pp) const {
    if (pp.decorators.size()) {
        int tag = 0;
        for (const auto& d : pp.decorators) {
            ++tag;
            ofs << std::endl;
            switch (d.type) {
                case Line: {
                    if (!d.pos.isok) {
                        LOG_WARN("Uninitialized line position");
                        continue;
                    }
                    ofs << "set arrow " << tag
                        << " from " << d.pos.x1 << "," << d.pos.y1
                        << " to " << d.pos.x2 << "," << d.pos.y2;
                    // set line/arrow properties
                    // line width
                    auto line = d.properties;
                    if (line.size > 1)
                        ofs << " lw " << line.size;
                    // line style
                    if (dashtype.count(line.style)) {
                        int dt = pointtype.at(line.style);
                        if (dt) ofs << " dt " << dt;
                    }
                    // color
                    if (d.properties.color != Black)
                        ofs << " lc rgb " << d.properties.color.hex_str();
                    // arrow tip
                    int arrow = 0;
                    if (d.label.Contains('>')) arrow += 1;
                    if (d.label.Contains('<')) arrow += 2;
                    if (d.label.Contains("|>") || d.label.Contains("<|")) arrow += 10;
                    ofs << " " << gnuplot_arrow.at(arrow) << " front" << std::endl;
                    break;
                }
                case BareText: {
                    ofs << "set label " << tag
                        << " " << FormatLabel(d.label)
                        << " at " << d.pos.x1 << "," << d.pos.y1
                        << " " << gnuplot_just.at(d.properties.style / 10); // horizontal alignment
                    // now, a small trick for vertical alignement
                    // we apply a vertical offset to reproduce the top/bottom alignment wrt the text position
                    if (d.properties.style % 2) {
                        // bottom alignment is [1-3]1 --> move upwards (position is bottom of character)
                        // top alignment is [1-3]3    --> move downwards (position is top of character)
                        ofs << " offset 0, character " << ((d.properties.style % 10 == 1) ? "0.6" : "-0.6");
                    }
                    if (d.properties.color != Black)
                        ofs << " tc rgb " << d.properties.color.hex_str();
                    ofs << " front" << std::endl;
                    break;
                }
                default:
                    LOG_WARN("Decorator not implemented for gnuplot");
                    break;
            }
        }
    }
}

} // namespace REx
