#ifndef DATATYPE_HH
#define DATATYPE_HH

class TObject;

namespace Expad {

enum DataType {
    Undefined = 0,
    Histo1D = 1,
    Histo2D = 11,
    Histo3D = 21,
    Graph1D = 2,
    Graph2D = 12,
    Function1D = 3,
    Function2D = 13,
    MultiGraph1D = 4,
    TextBox = 101,
    BareText = 102,
    Legend = 103
};

DataType GetDataType(const TObject* obj);

int GetDataDimension(DataType d);

} // namespace Expad

#endif
