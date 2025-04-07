#ifndef PADPROPERTIES_HH
#define PADPROPERTIES_HH

#include "DataType.hh"

#include "TString.h"

#include <vector>

namespace Expad {

struct PadProperties {
    struct Color {
        double red = 0.0;
        double blue = 0.0;
        double green = 0.0;
        double alpha = 0.0;
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
        const TObject* obj;
        DataType type;
        TString label;
        DrawingStyle marker;
        DrawingStyle line;
    };

    TString title;
    AxisProperties xaxis;
    AxisProperties yaxis;
    std::vector<Data> datasets;
    unsigned short legend = 0;
};

} // namespace Expad

#endif
