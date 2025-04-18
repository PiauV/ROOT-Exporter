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
        Color();
        Color(double r, double g, double b, double a = 1);
        TString rgb_str(bool with_alpha = false) const;
        TString hex_str(bool with_alpha = false) const;
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
        Data();
        Data(const Data& d);
    };

    TString title;
    AxisProperties xaxis;
    AxisProperties yaxis;
    std::vector<Data> datasets;
    unsigned short legend = 0;
};

bool operator==(const PadProperties::Color& lc, const PadProperties::Color& rc);
bool operator!=(const PadProperties::Color& lc, const PadProperties::Color& rc);

} // namespace Expad

#endif
