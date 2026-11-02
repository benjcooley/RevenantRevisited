// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Revenant Revisited 2026                          *
// *        playerstats.h - The shipped game's player-stat arithmetic      *
// *************************************************************************
//
// The parts of the retail player-stat rules that need only numbers and
// text: the maxima of health, fatigue and mana, the STATLEVEL tables of
// rules.def, kill experience, and the STATLINE interpreter that equipment
// and stat effects apply to the player's modified stats. TPlayer and
// TRules own the data and call these; keeping them free of the object
// model lets tools/test_playerstats.cpp check them against the retail
// formulas. docs/gameplay/forensics/PLAYER_STATS.md.
//
// *************************************************************************

#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class TToken;

namespace PlayerStats
{

inline constexpr int32_t kVitalBase = 75;   // the 0x4b in all three maxima
inline constexpr int32_t kStatCap   = 30;   // attributes and skills, in the modified copy

// REVSYNC: TPlayer::MaxHealth @ 0x00520630. `conslevel` is Constitution's
// STATLEVEL entry for the player's Cons, `classmod` the class HEALTHMOD.
[[nodiscard]] constexpr int32_t MaxHealth(int32_t level, int32_t perlevel, int32_t flat,
                                          int32_t pct, int32_t conslevel, int32_t classmod)
{
    return (flat + kVitalBase + perlevel * level) * (100 + pct + conslevel + classmod) / 100;
}

// REVSYNC: TPlayer::MaxFatigue @ 0x005206d0. Constitution counts double,
// over a base of 200.
[[nodiscard]] constexpr int32_t MaxFatigue(int32_t level, int32_t perlevel, int32_t flat,
                                           int32_t pct, int32_t conslevel, int32_t classmod)
{
    return (flat + kVitalBase + perlevel * level) * (200 + pct + classmod + 2 * conslevel) / 200;
}

// REVSYNC: TPlayer::MaxMana @ 0x00520770. `mindlevel` is Mind's STATLEVEL
// entry for the player's Mind, `classmod` the class MANAMOD.
[[nodiscard]] constexpr int32_t MaxMana(int32_t level, int32_t perlevel, int32_t flat,
                                        int32_t pct, int32_t mindlevel, int32_t classmod)
{
    return (flat + kVitalBase + perlevel * level) * (100 + pct + mindlevel + classmod) / 100;
}

// REVSYNC: 0x0051a5b0 -- the experience a player of level `level` earns for
// overcoming something worth `value` (a creature's Value stat, another
// player's Level): the further above the player, the more; at four or more
// levels below, 1.
[[nodiscard]] constexpr int32_t KillExp(int32_t value, int32_t level)
{
    const int32_t base = (level * 5 + 10) * 20;
    const int32_t diff = value - level;
    if (diff > 0)
        return (base / 15) * diff;
    if (diff < -3)
        return 1;
    return base / (30 - 30 * diff);
}

// REVSYNC: STATLINE TIME (0x0051cc40) -- a duration in 24 Hz frames as the
// player clock counts it, 1/100 s (x retail's float 4.166667, truncated).
[[nodiscard]] int32_t StatTime(int32_t frames);

// ********************
// * STATLEVEL tables *
// ********************

// REVSYNC: the STATLEVEL blocks of rules.def (tables 0x0049c9b0, parser
// 0x0049c9f0, lookup 0x0049cbf0): for each attribute, the percent an
// attribute value of 0..30 is worth. Tables in attribute order (charstats.h
// PLRSTAT_*), entries the file doesn't give hold retail's fill value.
class TStatLevels
{
  public:
    static constexpr int32_t kTables = 6;
    static constexpr int32_t kLevels = 31;
    static constexpr int32_t kUnset  = -2000000;    // 0xffe17b80

    [[nodiscard]] static int32_t FindTable(const char *name);
      // "Strength", "Constitution", "Agility", "Reflexes", "Mind", "Luck"
      // (exact case) -> 0..5; -1 for any other name
    bool Parse(TToken &t, int32_t table);
      // The block after `STATLEVEL "<name>"`: BEGIN, `ENTRY <level> <value>`
      // lines, END. Errors through TToken::Error.
    void Set(int32_t table, int32_t level, int32_t value);
      // One entry; a level outside 0..30 is ignored (retail drops >= 31)
    [[nodiscard]] int32_t Get(int32_t table, int32_t value) const;
      // The entry for `value`; 0 for a value outside 0..30 or a bad table

  private:
    std::array<std::array<int32_t, kLevels>, kTables> tables = MakeUnset();

    static constexpr std::array<std::array<int32_t, kLevels>, kTables> MakeUnset()
    {
        std::array<std::array<int32_t, kLevels>, kTables> t{};
        for (auto &row : t)
            for (int32_t &v : row)
                v = kUnset;
        return t;
    }
};

// ************
// * STATLINE *
// ************

// What a stat line names, for the messages TPlayer prints.
struct SStatLineMod
{
    std::string stat;           // the stat's name as written
    std::string value;          // the token after it as written
};

struct SStatLineResult
{
    enum class ENote { BadMod, BadTime };
    struct SNote
    {
        ENote kind = ENote::BadMod;
        SStatLineMod mod;       // BadTime: the token after TIME in `value`
    };

    bool complete   = false;    // read to the end; false when an unknown word stopped it
    bool initialize = false;    // INITIALIZE: a stat effect's first evaluation
    int32_t duration = -1;      // TIME, in 1/100 s; -1 without
    std::vector<SStatLineMod> mods;     // every stat named, in order
    std::vector<SNote> notes;           // malformed values, in order
};

// REVSYNC: the interpreter of TPlayer @ 0x0051cc40, without the effect
// bookkeeping (TPlayer::ApplyStatLine does that). Applies `line` to
// `stats`, the modified copy indexed by stat id; `findstat` maps a stat
// name (case-insensitive) to its id or -1. Per stat name, the next token:
//   number n    stats[id] += n
//   % n         stats[id] = stats[id] * (100 + n) / 100
//   + ...       stats[id] += the next token's number, blanks not skipped
//   (...        stats[id] = atoi of the text after '('
// TIME n sets the duration, INITIALIZE marks a first evaluation, EFFECT and
// TAG are skipped (retail loops on them forever; no shipped line has them),
// and any other word stops the line.
using TFindStat = std::function<int32_t(const char *name)>;
SStatLineResult ApplyStatLine(const char *line, std::vector<int32_t> &stats,
                              const TFindStat &findstat);

// The rest of the current line's tokens, each after a space -- how retail
// stores a STATLINE (record loaders 0x0048ac30 / 0x0048b1a0) and how the
// statmod command builds one (0x0051c550). Leaves `t` at the line's end.
std::string ReadStatLine(TToken &t);

}  // namespace PlayerStats
