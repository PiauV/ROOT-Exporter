# ROOT Exporter (REx)

**REx** (ROOT Exporter) is a C++ library for exporting figures generated with the [ROOT framework](https://root.cern.ch/) into scripts for external plotting tools. 

During export, datasets from the plot are saved as ASCII files using the **RTT** (ROOT To Text) sub-library.

**Version:** 0.1.0

## Installation

REx can be built and installed using CMake:
```bash
cd <rex-folder>
mkdir build && cd build
cmake -S .. -B .
cmake --build .
cmake --install .
```

### Tests

The REx test suite, **TREx** (Testing REx), can be run with CTest:
```bash
cd <rex-folder>/build
ctest [-V] [--output-on-failure]
```

## Features

### ROOT To Text (RTT)

The RTT sub-library is used to export ROOT data objects (TGraph, TH1, TH2, TF1) to ASCII files. 

Use the global instance `gRTT` and its `SaveObject` method:
```c++
TH1D *h = new TH1D("h","h",100,0.,1.);
// Fill the histogram...
// (...)
gRTT->SaveObject(h); // Creates 'h.txt'
gRTT->SaveObject(h,"h_errors.dat","E"); // Creates 'h_errors.dat' with error bars

// Signature: gRTT->SaveObject(TObject *obj, const char* filename, Option_t * option="");
```

### Export Plots and Datasets (ExPad)

The ExPad component injects the plot formatting (colors, styles, legends, annotations, etc.) into a script that will be processed by an external plotting tool. 

**Currently supported tools:**
- [gle](https://glx.sourceforge.io/)
- [gnuplot](http://www.gnuplot.info/)
- [python (matplotlib)](https://matplotlib.org/stable/)

**Example**: Export to a Python script
```c++
TCanvas *c = new TCanvas("c","c");
// Draw objects in the canvas...
// (...)
auto rex_python = new REx::PyplotExportManager();
rex_python->ExportPad(c, "my_exported_canvas.py");
```
## Usage

### Loading the library in ROOT

To use REx during an interactive ROOT session, you need to load the desired library:

```c++
gSystem->Load("<path-to-rex-install>/libRTT.so"); // RTT only
// or
gSystem->Load("<path-to-rex-install>/libExPad.so"); // Full REx library (ExPad + RTT)
```

**Tip:** Add the above line to your `rootlogon` file for automatic loading at ROOT startup.

### Examples

Simple usage examples for RTT and ExPad are provided in `doc/examples` and installed in `<install-dir>/share/REx/examples`:
- `rtt_example.C`
- `expad_example.C`

To run them:
```c++
[root] > gSystem->Load("path/to/libExPad.[dll/so]")
[root] > .x rtt_example.C
[root] > .x expad_example.C
```

The TREx tests could also serve as examples for advanced REx features.

## Additional Notes

- **Compatibility:** REx should be compatible with ROOT 6 and legacy ROOT 5.34.
- **License:** GNU Lesser General Public License v3, see files COPYING and COPYING.LESSER.
