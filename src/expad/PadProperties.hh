#ifndef PADPROPERTIES_HH
#define PADPROPERTIES_HH

#include "DataType.hh"

#include "TString.h"

#include <utility>
#include <vector>

namespace REx {

struct PadProperties {
    struct Color {
        double red;
        double green;
        double blue;
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

    struct Coord {
        bool isok = 0; // to avoid drawing ill-initialized object at (0,0)
        double x1 = 0;
        double x2 = 0;
        double y1 = 0;
        double y2 = 0;
        void set(double x, double y, double xmax = 0, double ymax = 0);
    };

    struct Decorator {
        DataType type;
        DrawingStyle properties;
        TString label;
        Coord pos;
        Decorator();
    };

    TString title;
    AxisProperties xaxis;
    AxisProperties yaxis;
    std::vector<Data> datasets;
    std::vector<Decorator> decorators;
    unsigned short legend = 0;
};

bool operator==(const PadProperties::Color& lc, const PadProperties::Color& rc);
bool operator!=(const PadProperties::Color& lc, const PadProperties::Color& rc);

static const PadProperties::Color Black(0, 0, 0);

} // namespace REx

#endif
