#include "DataType.hh"

#include "TObject.h"

namespace REx {

DataType GetDataType(const TObject* obj) {
    // histogram
    if (obj->InheritsFrom("TH1")) {
        if (obj->InheritsFrom("TH3")) {
            return Histo3D;
        }
        if (obj->InheritsFrom("TH2")) {
            return Histo2D;
        }
        return Histo1D;
    }
    else if (obj->InheritsFrom("TGraph")) {
        return Graph1D;
    }
    else if (obj->InheritsFrom("TGraph2D")) {
        return Graph2D;
    }
    else if (obj->InheritsFrom("TMultiGraph")) {
        return MultiGraph1D;
    }
    else if (obj->InheritsFrom("TF1")) {
        if (obj->InheritsFrom("TF2")) {
            return Function2D;
        }
        return Function1D;
    }
    else if (obj->InheritsFrom("TPaveText")) {
        return TextBox;
    }
    else if (obj->InheritsFrom("TText")) {
        return BareText;
    }
    else if (obj->InheritsFrom("TLegend")) {
        return Legend;
    }
    else if (obj->InheritsFrom("TLine")) {
        return Line;
    }
    // else if (obj->InheritsFrom("TBox")) {
    //     return Box;
    // }
    // else if (obj->InheritsFrom("TEllipse")) {
    //     return Ellipse;
    // }
    return Undefined;
}

int GetDataDimension(DataType d) {
    if (d == 0)
        return -1;
    else if (d >= 100)
        return 0;
    else
        return d / 10 + 1;
}

} // namespace REx
