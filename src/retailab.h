// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *       retailab.h - the port's side of the retail A/B dumps            *
// *************************************************************************
//
// `Revenant --retail-ab=<target> --ab-cases=<file> --ab-out=<file>` runs one
// of the port's functions over every case in the case file and writes one
// JSON line per case, in the same schema the retail fixture in the
// emulator's gameflow slot writes for the original function. The compare
// command (tools/retail_ab/retail_ab.py) runs both and diffs them.
// docs/gameflow/RETAIL_AB.md.
//
// It runs from sokol_main, before the window and the engine start: a target
// sets up only what its function reads.

#pragma once

namespace RetailAB
{
    // If argv asks for a retail A/B dump, run it and return true with the
    // process exit code in `exitcode`; else return false.
    bool Run(int argc, char* argv[], int& exitcode);
}
