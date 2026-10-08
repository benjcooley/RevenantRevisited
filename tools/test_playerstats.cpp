// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Revenant Revisited 2026                          *
// *      test_playerstats.cpp - The shipped game's player-stat rules      *
// *************************************************************************
//
// Pins src/playerstats.{h,cpp} to the retail formulas and the STATLINE
// interpreter in docs/gameplay/forensics/PLAYER_STATS.md. Expected values
// are worked by hand from the decompiles, with the shipped rules.def /
// armor.def / spell.def numbers. Run via build/test_playerstats.
// *************************************************************************

#include "../src/charstats.h"
#include "../src/parse.h"
#include "../src/playerstats.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <strings.h>
#include <vector>

// parse.cpp's engine hooks. A fatal parse error becomes a C++ exception so a
// test can expect it.
void FatalError(const char *error, const char *extra)
{
    throw std::runtime_error(std::string(error ? error : "") + (extra ? extra : ""));
}

void Error(const char *error, const char *extra)
{
    std::fprintf(stderr, "Error: %s%s\n", error ? error : "", extra ? extra : "");
}

int32_t stricmp(const char *a, const char *b) { return strcasecmp(a, b); }

namespace {

using namespace PlayerStats;

// Shipped rules.def: HEALTHDATA 25, FATIGUEDATA 3, MANADATA 25; class
// Revenant HEALTHMOD 0, FATIGUEMOD 0, MANAMOD 5.
constexpr int32_t kHealthPerLevel  = 25;
constexpr int32_t kFatiguePerLevel = 3;
constexpr int32_t kManaPerLevel    = 25;
constexpr int32_t kRevenantManaMod = 5;

// ---- maxima ---------------------------------------------------------------

TEST(Maxima, LockeAtLevelOne)
{
    // Cons 12 and Mind 14 are worth 0% in the STATLEVEL tables.
    EXPECT_EQ(MaxHealth(1, kHealthPerLevel, 0, 0, 0, 0), 100);    // (75 + 25) * 100 / 100
    EXPECT_EQ(MaxFatigue(1, kFatiguePerLevel, 0, 0, 0, 0), 78);   // (75 + 3) * 200 / 200
    EXPECT_EQ(MaxMana(1, kManaPerLevel, 0, 0, 0, kRevenantManaMod), 105);   // 100 * 105 / 100
}

TEST(Maxima, FlatPercentStatAndClass)
{
    // (100 + 25 * 4 + 75) * (100 + 15 + 10 + 40) / 100 = 275 * 165 / 100
    EXPECT_EQ(MaxHealth(4, kHealthPerLevel, 100, 15, 10, 40), 453);
    // (75 + 3 * 4 + 0) * (200 + 100 - 20 + 2 * 10) / 200 = 87 * 300 / 200
    EXPECT_EQ(MaxFatigue(4, kFatiguePerLevel, 0, 100, 10, -20), 130);
    // (300 + 75 + 25 * 2) * (100 + 0 - 30 + 50) / 100 = 425 * 120 / 100
    EXPECT_EQ(MaxMana(2, kManaPerLevel, 300, 0, -30, 50), 510);
}

TEST(Maxima, LevelZeroStillHasTheBase)
{
    // `playerlevel 0`: the 75 keeps every maximum above zero.
    EXPECT_EQ(MaxHealth(0, kHealthPerLevel, 0, 0, 0, 0), 75);
    EXPECT_EQ(MaxFatigue(0, kFatiguePerLevel, 0, 0, 0, 0), 75);
    EXPECT_EQ(MaxMana(0, kManaPerLevel, 0, 0, 0, kRevenantManaMod), 78);   // 75 * 105 / 100
}

TEST(Maxima, NegativeBonusTruncatesTowardZero)
{
    // (75 + 25) * (100 - 30 - 20) / 100 = 50; 78 * (200 - 61) / 200 = 54.21 -> 54
    EXPECT_EQ(MaxHealth(1, kHealthPerLevel, 0, 0, -30, -20), 50);
    EXPECT_EQ(MaxFatigue(1, kFatiguePerLevel, 0, -1, -30, 0), 54);
}

// ---- experience -------------------------------------------------------------

TEST(KillExp, AboveLevelPaysPerLevel)
{
    // Level 1: base (5 + 10) * 20 = 300; 300 / 15 = 20 per level above.
    EXPECT_EQ(KillExp(2, 1), 20);
    EXPECT_EQ(KillExp(6, 1), 100);
}

TEST(KillExp, AtOrBelowLevelDividesTheBase)
{
    EXPECT_EQ(KillExp(1, 1), 10);   // 300 / 30
    EXPECT_EQ(KillExp(0, 1), 5);    // 300 / 60
    EXPECT_EQ(KillExp(5, 7), 10);   // (35 + 10) * 20 = 900; d = -2: 900 / 90
}

TEST(KillExp, FarBelowIsOne)
{
    EXPECT_EQ(KillExp(1, 5), 1);    // d = -4
    EXPECT_EQ(KillExp(0, 30), 1);
}

TEST(StatTime, FramesToHundredths)
{
    EXPECT_EQ(StatTime(2880), 12000);   // spell.def TIME 2880: two minutes
    EXPECT_EQ(StatTime(240), 1000);
    EXPECT_EQ(StatTime(650), 2708);     // 2708.33 truncated
    EXPECT_EQ(StatTime(0), 0);
}

// ---- STATLEVEL ----------------------------------------------------------------

TEST(StatLevels, TableNamesAreExact)
{
    EXPECT_EQ(TStatLevels::FindTable("Strength"), PLRSTAT_STRN);
    EXPECT_EQ(TStatLevels::FindTable("Constitution"), PLRSTAT_CONS);
    EXPECT_EQ(TStatLevels::FindTable("Agility"), PLRSTAT_AGIL);
    EXPECT_EQ(TStatLevels::FindTable("Reflexes"), PLRSTAT_RFLX);
    EXPECT_EQ(TStatLevels::FindTable("Mind"), PLRSTAT_MIND);
    EXPECT_EQ(TStatLevels::FindTable("Luck"), PLRSTAT_LUCK);
    EXPECT_EQ(TStatLevels::FindTable("mind"), -1);
    EXPECT_EQ(TStatLevels::FindTable("Wisdom"), -1);
}

TEST(StatLevels, ParsesEntriesAndKeepsTheFill)
{
    char text[] =
        "BEGIN\n"
        "\t// Level Health%\n"
        "\tENTRY\t0\t-30\n"
        "\tENTRY\t12\t0\n"
        "\tENTRY\t16\t5\n"
        "\tENTRY\t30\t30\n"
        "\tENTRY\t31\t99\n"
        "END\n";
    TStringParseStream stream(text);
    TToken t(stream);
    t.Get();

    TStatLevels levels;
    ASSERT_TRUE(levels.Parse(t, PLRSTAT_CONS));
    EXPECT_EQ(levels.Get(PLRSTAT_CONS, 0), -30);
    EXPECT_EQ(levels.Get(PLRSTAT_CONS, 12), 0);
    EXPECT_EQ(levels.Get(PLRSTAT_CONS, 16), 5);
    EXPECT_EQ(levels.Get(PLRSTAT_CONS, 30), 30);
    EXPECT_EQ(levels.Get(PLRSTAT_CONS, 13), TStatLevels::kUnset);   // no ENTRY 13
    EXPECT_EQ(levels.Get(PLRSTAT_CONS, 31), 0);                     // past the table
    EXPECT_EQ(levels.Get(PLRSTAT_CONS, -1), 0);
    EXPECT_EQ(levels.Get(PLRSTAT_MIND, 12), TStatLevels::kUnset);   // another table
}

// ---- STATLINE -------------------------------------------------------------------

// A stat table shaped like the PLAYER object stats.
struct StatLineFixture : ::testing::Test
{
    std::vector<int32_t> stats = std::vector<int32_t>(SKC_FIRST + NUM_SKILLS, 0);
    TFindStat find = [](const char *name) -> int32_t {
        static const std::pair<const char *, int32_t> kStats[] = {
            { "Fatigue", CHRSTAT_FIRST + CHRSTAT_FATIGUE },
            { "DmgResPoison", CHRRESIST_FIRST + CHRRESIST_POISON },
            { "DmgResMagical", CHRRESIST_FIRST + CHRRESIST_MAGICAL },
            { "MaxHealthPct", PLRVAL_FIRST + PLRVAL_MAXHEALTHPCT },
            { "MaxManaPct", PLRVAL_FIRST + PLRVAL_MAXMANAPCT },
            { "Strn", PLRSTAT_FIRST + PLRSTAT_STRN },
            { "Agil", PLRSTAT_FIRST + PLRSTAT_AGIL },
            { "Rflx", PLRSTAT_FIRST + PLRSTAT_RFLX },
            { "Attack", SK_FIRST + SK_ATTACK },
            { "Hands", SK_FIRST + SK_HANDS },
        };
        for (const auto &[stat, id] : kStats)
            if (strcasecmp(stat, name) == 0)
                return id;
        return -1;
    };

    int32_t &At(int32_t id) { return stats[id]; }
};

TEST_F(StatLineFixture, ReadStatLineJoinsTokens)
{
    char text[] = "STATLINE DmgResPoison 6  // gauntlets\nEND\n";
    TStringParseStream stream(text);
    TToken t(stream);
    t.Get();           // STATLINE
    t.WhiteGet();      // first value token
    EXPECT_EQ(ReadStatLine(t), " DmgResPoison 6");
    EXPECT_EQ(t.Type(), TKN_RETURN);
}

TEST_F(StatLineFixture, AddsEachNamedStat)
{
    At(PLRSTAT_FIRST + PLRSTAT_AGIL) = 14;
    const SStatLineResult r = ApplyStatLine(" DmgResmagical 4 RFLX -1 AGIL -1", stats, find);
    EXPECT_TRUE(r.complete);
    EXPECT_EQ(At(CHRRESIST_FIRST + CHRRESIST_MAGICAL), 4);
    EXPECT_EQ(At(PLRSTAT_FIRST + PLRSTAT_RFLX), -1);
    EXPECT_EQ(At(PLRSTAT_FIRST + PLRSTAT_AGIL), 13);
    ASSERT_EQ(r.mods.size(), 3u);
    EXPECT_EQ(r.mods[1].stat, "RFLX");
    EXPECT_EQ(r.mods[1].value, "-1");
    EXPECT_EQ(r.duration, -1);
    EXPECT_FALSE(r.initialize);
}

TEST_F(StatLineFixture, PercentScalesTheCopy)
{
    At(SK_FIRST + SK_ATTACK) = 30;
    At(CHRSTAT_FIRST + CHRSTAT_FATIGUE) = 78;
    const SStatLineResult r = ApplyStatLine(" Attack % 2 Fatigue % 4", stats, find);
    EXPECT_TRUE(r.complete);
    EXPECT_EQ(At(SK_FIRST + SK_ATTACK), 30);                // 30 * 102 / 100 = 30.6
    EXPECT_EQ(At(CHRSTAT_FIRST + CHRSTAT_FATIGUE), 81);     // 78 * 104 / 100 = 81.12
}

TEST_F(StatLineFixture, TrailingPercentStopsTheLine)
{
    // "Hands 15%" in armor.def: 15 is added, then '%' is no stat or keyword.
    const SStatLineResult r = ApplyStatLine(" Hands 15 %", stats, find);
    EXPECT_FALSE(r.complete);
    EXPECT_EQ(At(SK_FIRST + SK_HANDS), 15);
}

TEST_F(StatLineFixture, UnknownNamesStopTheLine)
{
    // "FAT %15": FAT is a unique id, not a stat name.
    At(CHRSTAT_FIRST + CHRSTAT_FATIGUE) = 50;
    const SStatLineResult r = ApplyStatLine(" FAT % 15 Strn 2", stats, find);
    EXPECT_FALSE(r.complete);
    EXPECT_EQ(At(CHRSTAT_FIRST + CHRSTAT_FATIGUE), 50);
    EXPECT_EQ(At(PLRSTAT_FIRST + PLRSTAT_STRN), 0);
}

TEST_F(StatLineFixture, ParenthesisSetsFromItsOwnText)
{
    // Stored lines separate "(5)" into "( 5 )": the '(' token alone sets 0.
    At(PLRSTAT_FIRST + PLRSTAT_STRN) = 16;
    ApplyStatLine(" Strn ( 5 )", stats, find);
    EXPECT_EQ(At(PLRSTAT_FIRST + PLRSTAT_STRN), 0);
}

TEST_F(StatLineFixture, PlusReadsTheBlankAfterIt)
{
    At(PLRSTAT_FIRST + PLRSTAT_STRN) = 16;
    ApplyStatLine(" Strn + 2", stats, find);
    EXPECT_EQ(At(PLRSTAT_FIRST + PLRSTAT_STRN), 16);
}

TEST_F(StatLineFixture, TimedEffectFirstPass)
{
    const SStatLineResult r = ApplyStatLine(" STRN 2 TIME 2880 INITIALIZE", stats, find);
    EXPECT_TRUE(r.complete);
    EXPECT_TRUE(r.initialize);
    EXPECT_EQ(r.duration, 12000);
    EXPECT_EQ(At(PLRSTAT_FIRST + PLRSTAT_STRN), 2);
}

TEST_F(StatLineFixture, BadValuesAreNotedAndReread)
{
    // A stat's bad value is looked at again as the next word: here TIME.
    const SStatLineResult r = ApplyStatLine(" Strn TIME y", stats, find);
    EXPECT_TRUE(r.complete);
    ASSERT_EQ(r.notes.size(), 2u);
    EXPECT_EQ(r.notes[0].kind, SStatLineResult::ENote::BadMod);
    EXPECT_EQ(r.notes[0].mod.stat, "Strn");
    EXPECT_EQ(r.notes[0].mod.value, "TIME");
    EXPECT_EQ(r.notes[1].kind, SStatLineResult::ENote::BadTime);
    EXPECT_EQ(r.notes[1].mod.value, "y");
    EXPECT_EQ(r.duration, -1);
    EXPECT_EQ(At(PLRSTAT_FIRST + PLRSTAT_STRN), 0);
}

TEST_F(StatLineFixture, EffectAndTagAreSkipped)
{
    const SStatLineResult r = ApplyStatLine(" EFFECT Strn 1 TAG Agil 1", stats, find);
    EXPECT_TRUE(r.complete);
    EXPECT_EQ(At(PLRSTAT_FIRST + PLRSTAT_STRN), 1);
    EXPECT_EQ(At(PLRSTAT_FIRST + PLRSTAT_AGIL), 1);
}

TEST_F(StatLineFixture, TwoPercentStats)
{
    At(PLRVAL_FIRST + PLRVAL_MAXHEALTHPCT) = 0;
    ApplyStatLine(" MaxManaPct 15 MaxHealthPct 15", stats, find);
    EXPECT_EQ(At(PLRVAL_FIRST + PLRVAL_MAXMANAPCT), 15);
    EXPECT_EQ(At(PLRVAL_FIRST + PLRVAL_MAXHEALTHPCT), 15);
}

} // namespace
