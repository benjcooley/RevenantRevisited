// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *     combattrace.h - a tick-by-tick record of fights, for replays      *
// *************************************************************************
//
// `--combattrace=<file>` writes one line per game tick for every character
// in a fight (and every one traced before, so deaths and the aftermath
// stay in view), plus a line per combat event: an attack chosen, its
// resolution, damage taken, a death. With --seed and --fixedstep a run
// repeats exactly, so two traces of one scenario must be identical; the
// arena (tools/combatarena/arena.py, docs/gameplay/COMBAT_DOJO.md §9)
// runs scenarios and compares the traces.
//
// Lines are tab-separated: tick, kind ("char", "event", "tick"), then
// key=value fields. Positions and angles are the game's integers.

#pragma once

#include <cstdint>

class TCharacter;

namespace CombatTrace
{
    // Starts the trace into `path`; false if it can't be written.
    bool Open(const char* path);

    // Also records every random number drawn in ticks [from, to]: the
    // value and the asking code's address as an offset into the
    // executable (`atos -o build/Revenant -l 0x100000000 <0x100000000+at>`),
    // the same in every run. For finding where two runs part.
    void TraceRandom(int64_t from, int64_t to);

    [[nodiscard]] bool Enabled();

    // After each game tick (PlayScreen pulse): the tick's character lines.
    void Tick(int64_t tick);

    // A combat event of `who`: `what` ("attack", "hit", "miss", "block",
    // "damage", "death", ...) and printf-style detail ("key=value ...").
    void Event(const TCharacter* who, const char* what, const char* fmt = nullptr, ...);
}
