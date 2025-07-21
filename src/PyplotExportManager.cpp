#include "PyplotExportManager.hh"

#include "PlotSerializer.hh"

#include "TPad.h"
#include "TSystem.h"

#include <fstream>
#include <iostream>
#include <unordered_map>

namespace {
const std::unordered_map<int, std::string> py_marker = {
    {1, "\',\'"},
    {2, "\'+\'"},
    {4, "\'o\'"},
    {5, "\'x\'"},
    {6, "\',\'"},
    {7, "\'.\'"},
    {7, "\'.\'"},
    {20, "\'o\'"},
    {21, "\'s\'"},
    {22, "\'^\'"},
    {23, "\'v\'"},
    {24, "\'o\'"},
    {25, "\'s\'"},
    {26, "\'^\'"},
    {27, "\'d\'"},
    {28, "\'P\'"},
    {29, "\'*\'"},
    {30, "\'*\'"},
    {32, "\'v\'"},
    {33, "\'d\'"},
    {34, "\'P\'"},
    {46, "\'X\'"},
    {47, "\'X\'"},
};

const std::unordered_map<int, std::string> py_line = {
    {1, "\'-\'"},
    {2, "\'--\'"},
    {3, "\'-.\'"},
    {4, "\'-:\'"},
};

const std::unordered_map<int, std::string> py_arrow = {
    {0, "\'-\'"},
    {1, "\'->\'"},
    {2, "\'<-\'"},
    {3, "\'<->\'"},
    {10, "\'-\'"}, // should not exist... this is just to avoid critical errors
    {11, "\'-|>\'"},
    {12, "\'<|-\'"},
    {13, "\'<|-|>\'"},
};

const std::unordered_map<int, std::string> py_align = {
    {0, ""},
    {11, "ha=\'left\', va=\'bottom\'"},
    {12, "ha=\'left\', va=\'center\'"},
    {13, "ha=\'left\', va=\'top\'"},
    {21, "ha=\'center\', va=\'bottom\'"},
    {22, "ha=\'center\', va=\'center\'"},
    {23, "ha=\'center\', va=\'top\'"},
    {31, "ha=\'right\', va=\'bottom\'"},
    {32, "ha=\'right\', va=\'center\'"},
    {33, "ha=\'right\', va=\'top\'"},
};

} // namespace

namespace Expad {

PyplotExportManager::PyplotExportManager() {
    ext_ = ".py";
    com_ = '#';
}

PyplotExportManager::~PyplotExportManager() {
}

TString PyplotExportManager::FormatLabel(const TString& str) const {
    TString ltx = ExportManager::FormatLabel(str);
    if (latex_) {
        ltx.Prepend("r");
    }
    return ltx;
}

char* PyplotExportManager::getColor(PadProperties::Color c) const {
    return Form("(%.3g,%.3g,%.3g)", c.red, c.green, c.blue);
}

void PyplotExportManager::WriteToFile(const char* filename, const PadProperties& pp) const {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return;
    }

    // write default header for configuration (import libraries) and set output name
    InitFile(ofs);

    ofs << "fig, ax = plt.subplots()" << std::endl;

    // >>> plot data
    SetTitleAndAxis(ofs, pp);

    // draw data from files
    SetData(ofs, pp);

    // configure legend
    SetLegend(ofs, pp);

    // plot data <<<

    // plot other graphical elements
    SetDecorators(ofs, pp);

    TString outfile(gSystem->BaseName(filename));    // outfile : *.py
    outfile.Replace(outfile.Index(ext_), 4, ".pdf"); // outfile : *.pdf

    ofs << "plt.savefig(\'" << outfile << "\')\n"
        << "# plt.show()\n"
        << "# plt.close()"
        << std::endl;

    ofs.close();
}

void PyplotExportManager::InitFile(std::ofstream& ofs) const {
    ofs << "import numpy as np\n"
        << "import matplotlib.pyplot as plt\n"
        << std::endl;
}

void PyplotExportManager::SetTitleAndAxis(std::ofstream& ofs, const PadProperties& pp) const {
    // set title
    if (pp.title.Length()) {
        ofs << "ax.set_title(" << FormatLabel(pp.title) << ")" << std::endl;
    }
    // draw axis
    ofs << "ax.set_xlabel(" << FormatLabel(pp.xaxis.title) << ")" << std::endl;
    ofs << "ax.set_ylabel(" << FormatLabel(pp.yaxis.title) << ")" << std::endl;
    ofs << "ax.set_xlim(" << pp.xaxis.min << ", " << pp.xaxis.max << ")" << std::endl;
    ofs << "ax.set_ylim(" << pp.yaxis.min << ", " << pp.yaxis.max << ")" << std::endl;
    // log axis
    if (pp.xaxis.log) {
        if (pp.yaxis.log)
            ofs << "ax.loglog()" << std::endl;
        else
            ofs << "ax.semilogx()" << std::endl;
    }
    else if (pp.yaxis.log)
        ofs << "ax.semilogy()" << std::endl;
    // axis color
    const PadProperties::Color black(0, 0, 0);
    if (pp.xaxis.color != black) {
        auto c = pp.xaxis.color;
        ofs << "cx = " << getColor(c) << "\n"
            << "ax.xaxis.label.set_color(cx)\n"
            << "ax.spines[\"bottom\"].set_edgecolor(cx)\n"
            << "ax.tick_params(axis='x', colors=cx)\n"
            << std::endl;
    }
    if (pp.yaxis.color != black) {
        auto c = pp.yaxis.color;
        ofs << "cy = " << getColor(c) << "\n"
            << "ax.yaxis.label.set_color(cy)\n"
            << "ax.spines[\"left\"].set_edgecolor(cy)\n"
            << "ax.tick_params(axis='y', colors=cy)\n"
            << std::endl;
    }
    ofs << std::endl;
}

void PyplotExportManager::SetData(std::ofstream& ofs, const PadProperties& pp) const {
    int n = pp.datasets.size();
    for (int i = 0; i < n; i++) {
        const auto di = pp.datasets[i];
        // read data file
        ofs << Form("d%d = np.transpose(np.loadtxt(\"%s\"))", i + 1, di.file.first.Data()) << std::endl;
        // setup options
        std::vector<std::pair<std::string, std::string>> options;
        // errors (if any)
        int ncol = pp.datasets[i].file.second;
        if (ncol == 3) {
            // y error bars
            options.push_back({"yerr", Form("d%d[2]", i + 1)});
        }
        else if (ncol == 4) {
            // x and y error bars
            options.push_back({"xerr", Form("d%d[2]", i + 1)});
            options.push_back({"yerr", Form("d%d[3]", i + 1)});
        }
        // line style
        auto li = pp.datasets[i].line;
        if (li.style) {
            options.push_back({"color", getColor(li.color)});
            options.push_back({"", ""});
            if (py_line.count(li.style))
                options.push_back({"ls", py_line.at(li.style)});
            else
                options.push_back({"ls", "\'-\'"});
            if (pp.datasets[i].type == Histo1D)
                options.push_back({"ds", "\"steps-mid\""});
            if (li.size != 1) // 1 is the default size
                options.push_back({"lw", std::to_string(li.size)});
        }
        // marker style
        auto mi = pp.datasets[i].marker;
        if (mi.style) {
            if (!li.style)
                options.push_back({"color", getColor(mi.color)});
            options.push_back({"", ""});
            options.push_back({"marker", py_marker.at(mi.style)});
            if (mi.size != 10) // 10 is the default marker size
                options.push_back({"ms", std::to_string(mi.size / 10.)});
            if ((mi.style >= 24 && mi.style <= 28) || mi.style == 30 || mi.style == 32 || mi.style == 46)
                options.push_back({"fillstyle", "\'none\'"});
            else if (li.style && mi.color != li.color) {
                options.push_back({"mfc", getColor(mi.color)});
                options.push_back({"mec", getColor(mi.color)});
            }
        }
        if (pp.legend)
            options.push_back({"label", FormatLabel(pp.datasets[i].label).Data()});

        // plot it
        if (ncol >= 3) // with error bars
            ofs << Form("ax.errorbar(d%d[0], d%d[1], ", i + 1, i + 1);
        else // without error bars
            ofs << Form("ax.plot(d%d[0], d%d[1], ", i + 1, i + 1);
        int nleft = options.size();
        for (const auto& opt : options) {
            if (opt.first.size())
                ofs << opt.first << "=" << opt.second << ", ";
            else if (nleft > 5)
                ofs << "\n         ";
            nleft--;
        }
        ofs << ")\n"
            << std::endl;
    }
}

void PyplotExportManager::SetLegend(std::ofstream& ofs, const PadProperties& pp) const {
    if (pp.legend) {
        ofs << "ax.legend(loc=\'"
            << (pp.legend > 2 ? "lower" : "upper")
            << (pp.legend % 2 ? " left" : " right")
            << "\')\n"
            << std::endl;
    }
}

void PyplotExportManager::SetDecorators(std::ofstream& ofs, const PadProperties& pp) const {
    if (pp.decorators.size()) {
        for (const auto& d : pp.decorators) {
            ofs << std::endl;
            switch (d.type) {
                case Line: {
                    if (!d.pos.isok) {
                        std::cerr << "Warning : uninitialized line position" << std::endl;
                        continue;
                    }
                    // draw the line
                    ofs << "ax.annotate(\"\", "
                        << "(" << d.pos.x1 << ", " << d.pos.y1 << "), "
                        << "(" << d.pos.x2 << ", " << d.pos.y2 << "), "
                        << "arrowprops=dict(";
                    // set line/arrow properties
                    // line width
                    auto line = d.properties;
                    if (line.size > 1)
                        ofs << "lw=" << line.size << ", ";
                    // line style
                    if (line.style)
                        ofs << "ls=" << py_line.at(line.style) << ", ";
                    // color
                    if (d.properties.color != Black)
                        ofs << "color=" << getColor(d.properties.color) << ", ";
                    // arrow tip
                    int arrow = 0;
                    if (d.label.Contains('>')) arrow += 1;
                    if (d.label.Contains('<')) arrow += 2;
                    if (d.label.Contains("|>") || d.label.Contains("<|")) arrow += 10;
                    ofs << "arrowstyle=" << py_arrow.at(arrow) << "), ";
                    ofs << ")" << std::endl;
                    break;
                }
                case BareText: {
                    ofs << "ax.text(" << d.pos.x1 << ", " << d.pos.y1 << ", "
                        << FormatLabel(d.label) << ", "
                        << py_align.at(d.properties.style) << ", ";
                    if (d.properties.color != Black)
                        ofs << "c=" << getColor(d.properties.color) << ", ";
                    ofs << ")" << std::endl;
                    break;
                }
                default:
                    std::cerr << "Warning : decorator not implemented in GLE" << std::endl;
                    break;
            }
        }
    }
}

} // namespace Expad
