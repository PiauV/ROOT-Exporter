#include "PadProperties.hh"

namespace Expad {
PadProperties::Color::Color(double r, double g, double b, double a) : red(r), green(g), blue(b), alpha(a) {};
PadProperties::Color::Color() : Color(0, 0, 0, 1) {};

TString PadProperties::Color::rgb_str(bool with_alpha) const {
    TString str;
    if (with_alpha)
        str.Form("rgba(%.3g,%.3g,%.3g,%3g)", alpha, red, green, blue);
    else
        str.Form("rgb(%.3g,%.3g,%.3g)", red, green, blue);
    return str;
}

TString PadProperties::Color::hex_str(bool with_alpha) const {
    TString str;
    int r = 255 * red;
    int g = 255 * green;
    int b = 255 * blue;
    if (with_alpha) {
        int a = 255 * alpha;
        str.Form("\"0x%02X%02X%02X%02X\"", a, r, g, b);
    }
    else {
        str.Form("\"0x%02X%02X%02X\"", r, g, b);
    }
    return str;
}

bool operator==(const PadProperties::Color& lc, const PadProperties::Color& rc) {
    double e = 1e-4;
    return (fabs(lc.alpha - rc.alpha) < e && fabs(lc.red - rc.red) < e && fabs(lc.green - rc.green) < e && fabs(lc.blue - rc.blue) < e);
}

bool operator!=(const PadProperties::Color& lc, const PadProperties::Color& rc) {
    return !(lc == rc);
}

PadProperties::Data::Data() : label(), marker(), line(), file() {
    type = Undefined;
}

PadProperties::Data::Data(const Data& d) {
    file = d.file;
    label = d.label;
    line = d.line;
    marker = d.marker;
    type = d.type;
}

} // namespace Expad
