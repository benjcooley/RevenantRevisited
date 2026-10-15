// *************************************************************************
// *                      Revenant Revisited 2026                          *
// *      shadowspells.cpp - Shadow Smoke finds the spells he can make       *
// *************************************************************************
//
// `invent` (docs/shadow/ARCHITECTURE.md §8.6). A spell is learned by
// casting it: a combination put together in the spell pane and invoked is
// cast, and the spell book learns it whether the cast came off or not
// (TSpellPane::Invoke, TSpellBookPane::Reveal). So Locke does what a
// player experimenting does: each combination of the talismans in his
// Spell Pouch that makes a spell he doesn't know goes into the pane's
// construct slot and is invoked, one cast at a time. Shadow knows the
// combinations from spell.def, where a player would find them by trying.
// A spell learned that the style uses goes on a free quick-spell ring.

#include "shadowsteps.h"

#include "shadowledger.h"
#include "shadowlog.h"
#include "shadowplan.h"

#include "../player.h"
#include "../revenant.h"
#include "../spell.h"
#include "../spellbook.h"
#include "../spellpane.h"

#include <algorithm>
#include <cstring>
#include <set>
#include <string>
#include <vector>

namespace {

constexpr int32_t kTicksPerSecond = 24;
// A cast's animation and its effect's delay are over well within this.
constexpr int32_t kCastTicks = 3 * kTicksPerSecond;

// The spells a style keeps on its quick-spell rings (ARCHITECTURE.md
// §8.4), by spell.def name, the first ones learned first on.
std::vector<std::string> QuickSpellsFor(EShadowStyle style)
{
    switch (style)
    {
        case EShadowStyle::Melee:  return {"Might", "Swift Strike", "Regeneration", "CureP"};
        case EShadowStyle::Hands:  return {"Fist Mastery", "Shadow Fist", "Regeneration", "CureP"};
        case EShadowStyle::Bow:    return {"Dexterity", "Speed", "Regeneration", "CureP"};
        case EShadowStyle::Spells: return {"Fire Flash", "Fireball", "Lightning", "Heal"};
        case EShadowStyle::Mixed:  return {"Might", "Fire Flash", "Heal", "CureP"};
    }
    return {};
}

// A learned spell the style uses onto the first free ring.
void Ready(const std::string& name, const std::string& talismans, EShadowStyle style, TShadowLog& log)
{
    const std::vector<std::string> wanted = QuickSpellsFor(style);
    if (std::find(wanted.begin(), wanted.end(), name) == wanted.end())
        return;
    for (int32_t ring = QSPELL_1; ring < QSPELL_NUM; ++ring)
        if (!Player->GetQuickSpell(ring)[0])
        {
            Player->SetQuickSpell(ring, talismans.c_str());
            log.Event("quickspell", name + " on ring " + std::to_string(ring));
            return;
        }
}

bool Known(const char* talismans)
{
    for (int32_t i = 0; i < Player->NumKnownSpells(); ++i)
        if (!strcmp(Player->KnownSpell(i), talismans))
            return true;
    return false;
}

class TInventRunner final : public TShadowStepRunner
{
  public:
    using TShadowStepRunner::TShadowStepRunner;

    EShadowStatus Pulse(SShadowContext& context) override
    {
        if (!ShadowGameIdle())
            return EShadowStatus::Running;
        if (!casting.empty())
        {
            if (++waited < kCastTicks)
                return EShadowStatus::Running;
            const bool learned = Known(casting.c_str());
            context.log.Event(learned ? "learned" : "not learned", name + " (" + casting + ")");
            if (learned)
                Ready(name, casting, context.plan.Style(), context.log);
            casting.clear();
        }
        const SSpellData* spell = nullptr;
        const char* talismans = ShadowSpellToInvent(tried, &spell);
        if (!talismans)
        {
            Player->SetQuickSpell(QSPELL_CONSTRUCT, "");
            return EShadowStatus::Done;
        }
        casting = talismans;
        name = spell->name;
        tried.insert(casting);
        // A cast spends mana and, when it comes off, gives Invoke
        // experience; a healing spell heals.
        context.ledger.AllowProgress("invent " + name, context.tick + kCastTicks);
        context.ledger.AllowVitals("cast " + name, Player->MaxHealth(), Player->MaxMana(), context.tick + kCastTicks);
        context.log.Event("invent", name + " (" + casting + ")");
        Player->SetQuickSpell(QSPELL_CONSTRUCT, casting.c_str());
        SpellPane.Invoke();
        waited = 0;
        return EShadowStatus::Running;
    }

  private:
    std::set<std::string> tried;
    std::string           casting;
    std::string           name;
    int32_t               waited = 0;
};

} // namespace

const char* ShadowSpellToInvent(const std::set<std::string>& tried, const SSpellData** spell)
{
    if (!Player)
        return nullptr;
    for (int32_t s = 0; s < SpellList.NumSpells(); ++s)
    {
        SSpellData* data = SpellList.GetSpellData(s);
        for (int32_t v = 0; data && v < data->variants.NumItems(); ++v)
        {
            const char* talismans = data->variants[v].talismans;
            if (!tried.count(talismans) && TSpellBookPane::Learnable(talismans) && !Known(talismans) &&
                Player->HasTalismans(talismans))
            {
                if (spell)
                    *spell = data;
                return talismans;
            }
        }
    }
    return nullptr;
}

std::unique_ptr<TShadowStepRunner> MakeInventRunner(const SShadowStep& step)
{
    return std::make_unique<TInventRunner>(step);
}
