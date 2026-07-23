#include "PadProperties.hh"

#include <unordered_map>

namespace {
const std::unordered_map<int, TString> named_colors = {
    {kWhite, "white"},
    {kBlack, "black"},
    {2, "red"},
    {3, "green"},
    {4, "blue"},
    {5, "yellow"},
    {6, "magenta"},
    {7, "cyan"},
    {EColor::kRed, "red"},
    {EColor::kGreen, "green"},
    {EColor::kBlue, "blue"},
    {EColor::kYellow, "yellow"},
    {EColor::kMagenta, "magenta"},
    {EColor::kCyan, "cyan"},
    {EColor::kOrange, "orange"},
    {EColor::kViolet, "violet"},
    {EColor::kGray, "gray"},
};

} // namespace

namespace REx {
PadProperties::Color::Color(double r, double g, double b, double a) : red(r), green(g), blue(b), alpha(a) {}

PadProperties::Color::Color() : Color(0, 0, 0, 1) {};

PadProperties::Color::Color(TColor* c) : Color() {
    if (c) {
        red = c->GetRed();
        green = c->GetGreen();
        blue = c->GetBlue();
        alpha = c->GetAlpha();
        if (named_colors.count(c->GetNumber())) {
            name = named_colors.at(c->GetNumber());
        }
    }
};

/// @brief Convert color to string (rgb version)
/// @param with_alpha true to include transparency
/// @return string describing the color with rgb values
TString PadProperties::Color::rgb_str(bool with_alpha) const {
    TString str;
    if (with_alpha)
        str.Form("(%.3g,%.3g,%.3g,%3g)", alpha, red, green, blue);
    else
        str.Form("(%.3g,%.3g,%.3g)", red, green, blue);
    return str;
}

/// @brief Convert color to string (hex version)
/// @param with_alpha true to include transparency
/// @return string describing the color with hexadecimal values
TString PadProperties::Color::hex_str(bool with_alpha) const {
    TString str;
    int r = 255 * red;
    int g = 255 * green;
    int b = 255 * blue;
    if (with_alpha) {
        int a = 255 * alpha;
        str.Form("0x%02X%02X%02X%02X", a, r, g, b);
    }
    else {
        str.Form("0x%02X%02X%02X", r, g, b);
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

PadProperties::Decorator::Decorator() : properties(), label(), pos() {
    type = Undefined;
}

void PadProperties::Coord::set(double x, double y, double xmax, double ymax) {
    x1 = x;
    y1 = y;
    x2 = xmax;
    y2 = ymax;
    isok = true;
}

} // namespace REx
