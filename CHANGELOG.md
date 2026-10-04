# Changelog

## [0.2.3] - 2026-10-04
 - [Added] Auto-tagging workflow (Github Action) 
 - [Fixed] Minor fix in RTT : header of 2D histograms for GLE (x and y ranges)
 - [Fixed] Remove duplicate function declaration in Expad
 - [Fixed] Escape '#' symbol (when it is not related to TLatex)
 - [Fixed] Automatic RTT filename generation when a folder is given as path

## [0.2.2] - 2026-09-17
 - [Added] New RTT options : verbose (V) and overwriting protection (P)
 - [Added] New Expad function : ExportCurrentPad(filename) (equivalent to ExportPad(gPad, filename))

## [0.2.1] - 2026-08-29
 - [Fixed] Consistent case for function parameters
 - [Fixed] Code fixes (error/warnings during build)
 - [Added] Continous integration workflow (Github Action)

## [0.2.0] - 2026-08-25
 - [Added] Functions can be exported as a formula with parameters, instead of generating datafiles (RTT + ExPad)
 - [Added] Some colornames are supported in ExPad (instead of writing full rgb code)
 - [Added] Export functions from TGraph ListOfFunctions (as it was already the case for TH1)

## [0.1.2] - 2026-02-22
 - [Added] Verbose parameter of all export managers can be set directly in their constructor
 - [Added] A unique filename is now given to data files saved through ExPad (even if the associated objects have the same name)
 - [Fixed] Minor bug fixes in ExPad

## [0.1.1] - 2025-11-10
 - [Added] Support of THStack in RTT and ExPad (with and without 'nostack' option) 
 - [Added] Export functions from TH1 ListOfFunctions
 - [Fixed] Minor bug fixes in ExPad
 - [Fixed] Missing pcm files in installation folder

## [0.1.0] - 2025-09-16

Initial release
