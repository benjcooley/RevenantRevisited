// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Revenant Revisited 2026                          *
// *       playerstats.cpp - The shipped game's player-stat arithmetic     *
// *************************************************************************

#include "playerstats.h"

#include "parse.h"

#include <cstdlib>
#include <cstring>

namespace PlayerStats
{

// Retail multiplies the frame count by the float at 0x005b535c on the x87
// stack and truncates (_ftol). The constant is 0x40855556, a hair above
// 100/24 (whose nearest float is 0x40855555), so whole seconds come out
// exact: 2880 frames are 12000, not 11999.
int32_t StatTime(int32_t frames)
{
    constexpr float kTicksPerFrame = 4.1666669845581055f;   // 0x40855556
    return static_cast<int32_t>(static_cast<double>(frames) * static_cast<double>(kTicksPerFrame));
}

// ********************
// * STATLEVEL tables *
// ********************

// REVSYNC: 0x0049c9f0 -- the names compare exactly (memcmp), in table order.
int32_t TStatLevels::FindTable(const char *name)
{
    static constexpr const char *kNames[kTables] =
        { "Strength", "Constitution", "Agility", "Reflexes", "Mind", "Luck" };
    if (!name)
        return -1;
    for (int32_t i = 0; i < kTables; i++)
    {
        if (std::strcmp(name, kNames[i]) == 0)
            return i;
    }
    return -1;
}

// REVSYNC: 0x0049c9f0 -- BEGIN, then `ENTRY level value` lines to END.
bool TStatLevels::Parse(TToken &t, int32_t table)
{
    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Statlevel block BEGIN expected");
    t.LineGet();

    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
        if (t.Type() != TKN_IDENT)
            t.Error("Statlevel keyword expected");
        if (!t.Is("ENTRY"))
            t.Error("Invalid character tag %s", t.Text());
        t.WhiteGet();

        int32_t level = 0;
        int32_t value = 0;
        if (!::Parse(t, "%i %i", &level, &value))
            t.Error("Error parsing tag %s", "ENTRY");
        Set(table, level, value);

        if (t.Type() != TKN_RETURN)
            t.Error("Return expected");
        t.LineGet();
    }

    if (!t.Is("END"))
        t.Error("Statlevel block END expected");
    t.Get();
    return true;
}

void TStatLevels::Set(int32_t table, int32_t level, int32_t value)
{
    if (table < 0 || table >= kTables || level < 0 || level >= kLevels)
        return;
    tables[table][level] = value;
}

// REVSYNC: 0x0049cbf0 (through 0x0048cc20). Retail reads past the table for
// a negative value; the port answers 0, as it does above 30.
int32_t TStatLevels::Get(int32_t table, int32_t value) const
{
    if (table < 0 || table >= kTables || value < 0 || value >= kLevels)
        return 0;
    return tables[table][value];
}

// ************
// * STATLINE *
// ************

SStatLineResult ApplyStatLine(const char *line, std::vector<int32_t> &stats,
                              const TFindStat &findstat)
{
    SStatLineResult result;

  // TStringParseStream reads a mutable buffer.
    std::string text = line ? line : "";
    TStringParseStream stream(text.data(), static_cast<int32_t>(text.size()));
    TToken t(stream);

    auto stat_at = [&stats](int32_t id) -> int32_t *
        { return (id >= 0 && id < static_cast<int32_t>(stats.size())) ? &stats[id] : nullptr; };

  // Retail steps with Get + WhiteGet (0x00478a10, 0x00479580); in a stored
  // line, where single blanks separate the tokens, that moves to the next.
    auto next = [&t] { t.Get(); t.WhiteGet(); };

    t.WhiteGet();
    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
    {
        const int32_t id = findstat(t.Text());
        if (id >= 0)
        {
            SStatLineMod mod;
            mod.stat = t.Text();
            next();
            mod.value = t.Text();
            result.mods.push_back(mod);

            int32_t *stat = stat_at(id);
            const char first = mod.value.empty() ? '\0' : mod.value[0];
            if (first == '(')
            {
                if (stat)
                    *stat = std::atoi(mod.value.c_str() + 1);
            }
            else if (first == '%')
            {
                t.WhiteGet();
                if (t.Type() != TKN_NUMBER)
                    continue;           // retail looks at this token again
                if (stat)
                    *stat = (t.Index() + 100) * *stat / 100;
            }
            else if (first == '+')
            {
                t.Get();
                if (stat)
                    *stat += t.Index();
            }
            else if (t.Type() == TKN_NUMBER)
            {
                if (stat)
                    *stat += t.Index();
            }
            else
            {
                result.notes.push_back({ SStatLineResult::ENote::BadMod, mod });
                continue;               // retail looks at this token again
            }
            next();
        }
        else if (t.Is("EFFECT") || t.Is("TAG"))
        {
          // REVSYNC-DIVERGENCE: retail's word test doesn't consume the token,
          // so it would look at it forever. No shipped STATLINE has either.
            t.WhiteGet();
        }
        else if (t.Is("INITIALIZE"))
        {
            result.initialize = true;
            next();
        }
        else if (t.Is("TIME"))
        {
            next();
            if (t.Type() == TKN_NUMBER)
                result.duration = StatTime(t.Index());
            else
                result.notes.push_back({ SStatLineResult::ENote::BadTime, { "TIME", t.Text() } });
            next();
        }
        else
            return result;              // the rest of the line is ignored
    }

    result.complete = true;
    return result;
}

std::string ReadStatLine(TToken &t)
{
    std::string line;
    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
    {
        line += ' ';
        line += t.Text();
        t.WhiteGet();
    }
    return line;
}

}  // namespace PlayerStats
