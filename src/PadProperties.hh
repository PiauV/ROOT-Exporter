#ifndef PADPROPERTIES_HH
#define PADPROPERTIES_HH

#include "DataType.hh"

#include "TString.h"

#include <utility>
#include <vector>

namespace Expad {

struct PadProperties {
    struct Color {
        double red;
        double blue;
        double green;
        double alpha;
        Color(double r, double g, double b, double a = 1) : red(r), green(g), blue(b), alpha(a) {};
        Color() : Color(0, 0, 0, 1) {};
    };

    struct AxisProperties {
        TString title;
        bool log = false;
        double min = 0.0;
        double max = 0.0;
        Color color;
    };

    struct DrawingStyle {
        unsigned short style = 0;
        unsigned short size = 1;
        Color color;
    };

    struct Data {
        DataType type;
        TString label;
        DrawingStyle marker;
        DrawingStyle line;
        std::pair<TString, int> file; // filename ; number of columns
    };

    TString title;
    AxisProperties xaxis;
    AxisProperties yaxis;
    std::vector<Data> datasets;
    unsigned short legend = 0;
};

inline bool operator==(const PadProperties::Color& lc, const PadProperties::Color& rc) {
    double e = 1e-4;
    return (fabs(lc.alpha - rc.alpha) < e && fabs(lc.red - rc.red) < e && fabs(lc.green - rc.green) < e && fabs(lc.blue - rc.blue) < e);
}

inline bool operator!=(const PadProperties::Color& lc, const PadProperties::Color& rc) {
    return !(lc == rc);
}

} // namespace Expad

#endif
