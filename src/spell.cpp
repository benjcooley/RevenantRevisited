// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                          Spell.cpp - Spell                            *
// *************************************************************************

#include "spell.h"

#include "logging.h"
#include "revutils.h"

#include "3dimage.h"
#include "character.h"
#include "complexobj.h"
#include "dialog.h"
#include "effect.h"
#include "imagery.h"
#include "mappane.h"
#include "parse.h"
#include "player.h"
#include "sound.h"
#include "textbar.h"

#include <algorithm>
#include <vector>

extern TObjectClass EffectClass;
extern TObjectClass TalismanClass;

// ********************
// * SSpellData Object *
// ********************

SSpellData::~SSpellData()
{
    delete [] desc;
    for (int32_t i = 0; i < variants.NumItems(); i++)
    {
        SSpellVariant &var = variants[i];
        if (var.controldata)
            delete [] var.controldata->attachmultiple;
        delete var.controldata;
        delete [] var.statline;
    }
}

// REVSYNC: SSpellData::Load @ 0x0053e4e0 -- one SPELL block; any tag but
// these is fatal. A VARIANT may be followed by its CONTROLDATA block.
bool SSpellData::Load(char *aname, TToken &t)
{
    strncpy(name, aname, NAMELEN - 1);
    name[NAMELEN - 1] = 0;
    objname[0] = 0;
    iconname[0] = 0;
    invoke[0] = 0;

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Spell def BEGIN expected");
    t.LineGet();

    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
        if (t.Type() != TKN_IDENT)
            t.Error("Spell def keyword expected");

        if (t.Is("NAME"))
        {
            if (!Parse(t, "NAME %30s\n", objname))
                t.Error("Error parsing NAME tag");
        }
        else if (t.Is("POISONCHANCE"))
        {
            if (!Parse(t, "POISONCHANCE %i\n", &poisonchance))
                t.Error("Error parsing POISONCHANCE tag");
        }
        else if (t.Is("ICONNAME"))
        {
            if (!Parse(t, "ICONNAME %30s\n", iconname))
                t.Error("Error parsing NAME tag");          // retail's text
        }
        else if (t.Is("DESCRIPTION"))
        {
            char buf[1024];
            if (!Parse(t, "DESCRIPTION %s\n", buf))
                t.Error("Error parsing DESCRIPTION tag");
            delete [] desc;
            desc = new char[strlen(buf) + 1];
            strcpy(desc, buf);
        }
        else if (t.Is("DAMAGETYPE"))
        {
            if (!Parse(t, "DAMAGETYPE %i\n", &damagetype))
                t.Error("Error parsing DAMAGETYPE tag");
        }
        else if (t.Is("FLAGS"))
        {
            if (!Parse(t, "FLAGS %i\n", &flags))
                t.Error("Error parsing FLAGS tag");
        }
        else if (t.Is("ANIMATION"))
        {
            if (!Parse(t, "ANIMATION %30s\n", invoke))
                t.Error("Error parsing ANIMATION tag");
        }
        else if (t.Is("DELAY"))
        {
            if (!Parse(t, "DELAY %i\n", &effectstart))
                t.Error("Error parsing DELAY tag");
        }
        else if (t.Is("VARIANT"))
        {
            SSpellVariant var;
            char talismans[256];
            if (!Parse(t, "VARIANT %s, %i, %s, %s, %i, %i, %i, %i, %i, %i, %i, %i\n", var.name, &var.type,
                       talismans, var.effect, &var.mana, &var.nextspellwait, &var.mindamage, &var.maxdamage,
                       &var.skilllevel, &var.height, &var.facing, &var.ani_delay))
                t.Error("Error parsing VARIANT tag");
            strncpy(var.talismans, talismans, kVariantTalismanCodes);
            var.talismans[kVariantTalismanCodes] = 0;

            // The CONTROLDATA block, when the next line opens one: each tag
            // through SSpellControlData::Load until its END.
            t.SkipBlanks();
            if (t.Is("CONTROLDATA"))
            {
                var.controldata = new SSpellControlData;
                if (!Parse(t, "CONTROLDATA %s", var.controldata->name))
                    t.Error("Error parsing CONTROLDATA tag");
                t.SkipBlanks();
                if (t.IsBegin())
                {
                    t.DoBegin();
                    do
                    {
                        var.controldata->Load(t, var);
                        t.SkipBlanks();
                    } while (!t.IsEnd());
                    t.DoEnd();
                }
            }
            variants.Add(var);
        }
        else if (t.Is("LIGHT"))
        {
            if (!Parse(t, "LIGHT <COLOR %b,%b,%b> <INT %i> <MULT %i> <POS %i,%i,%i> <FADEIN %i> <FADEOUT %i>\n",
                       &light.color[0], &light.color[1], &light.color[2], &light.intensity, &light.mult,
                       &light.pos[0], &light.pos[1], &light.pos[2], &light.fadein, &light.fadeout))
                t.Error("Error parsing spell LIGHT tag");
        }
        else
            t.Error("Invalid spell tag %s", t.Text());
    }

    if (!t.Is("END"))
        t.Error("Spell def END expected");
    t.WhiteGet();

    return true;
}

// REVSYNC: LoadControlData @ 0x0053dd10 -- one tag of a CONTROLDATA block
// (the caller skips to the next). STATLINE goes to the variant.
bool SSpellControlData::Load(TToken &t, SSpellVariant &variant)
{
    char buf[256];
    if (t.Is("RADIUS"))
    {
        if (!Parse(t, "RADIUS %i", &radius))
            t.Error("Error parsing RADIUS tag");
    }
    else if (t.Is("HITS"))
    {
        if (!Parse(t, "HITS %i", &hits))
            t.Error("Error parsing HITS tag");
    }
    else if (t.Is("DURATION"))
    {
        if (!Parse(t, "DURATION %i %i", &duration, &duration2))
            t.Error("Error parsing DURATION tag");
    }
    else if (t.Is("SHAKE"))
    {
        if (!Parse(t, "SHAKE %i", &shake))
            t.Error("Error parsing SHAKE tag");
        flags |= SCF_SHAKE;
    }
    else if (t.Is("PLAY"))
    {
        if (!Parse(t, "PLAY %s", sound))
            t.Error("Error parsing PLAY tag");
        flags |= SCF_PLAY;
    }
    else if (t.Is("PLAYONCE"))
    {
        if (!Parse(t, "PLAYONCE %s", sound))
            t.Error("Error parsing PLAY tag");             // retail's text
        flags |= SCF_PLAYONCE;
    }
    else if (t.Is("POS"))
    {
        if (!Parse(t, "POS %i %i %i", &pos[0], &pos[1], &pos[2]))
            t.Error("Error parsing POS tag");
        posset = 1;
    }
    else if (t.Is("PATTERN"))
    {
        if (!Parse(t, "PATTERN %s", buf))
            t.Error("Error parsing PATTERN tag");
        static const char *const patterns[] = {"RANDOM", "CIRCLE", "LINE", "CLUSTER", "X", "SPIRAL"};
        int32_t found = SPAT_NONE;
        for (int32_t i = 0; i < 6 && found == SPAT_NONE; i++)
            if (!stricmp(buf, patterns[i]))
                found = SPAT_RANDOM + i;
        if (found == SPAT_NONE)
            t.Error("Unidentified PATTERN type.");
        pattern = found;
    }
    else if (t.Is("ONCASTER"))
    {
        oncaster = 1;
        t.Get();
    }
    else if (t.Is("HITTARGET"))
    {
        hittarget = 1;
        t.Get();
    }
    else if (t.Is("FOLLOW"))
    {
        t.Get();
        follow = 1;
    }
    else if (t.Is("ATTACHMULTIPLE"))
    {
        // One object-map name per HIT, so HITS must come first.
        if (hits > kMaxAttachMultiple)
            t.Error("Error parsing ATTACHMULTIPLE tag");
        attachset = 1;
        delete [] attachmultiple;
        attachmultiple = new char[(hits > 0 ? hits : 0) * RESNAMELEN + 1]();
        t.WhiteGet();
        for (int32_t i = 0; i < hits; i++)
        {
            if (!Parse(t, "%s ", buf))
                t.Error("Error parsing ATTACHMULTIPLE tag");
            else
                strcpy(attachmultiple + i * RESNAMELEN, buf);
        }
    }
    else if (t.Is("ATTACH"))
    {
        if (!Parse(t, "ATTACH %s", buf))
            t.Error("Error parsing ATTACH tag");
        attachset = 1;
        strncpyz(attach, buf, RESNAMELEN);
    }
    else if (t.Is("MULTIPLETARGETS"))
    {
        t.Get();
        multipletargets = 1;
    }
    else if (t.Is("REPEATDAMAGE"))
    {
        if (!Parse(t, "REPEATDAMAGE %i", &repeatdamage))
            t.Error("Error parsing REPEATDAMAGE tag");
        repeatset = 1;
    }
    else if (t.Is("STATLINE"))
    {
        constexpr int32_t kStatLineLen = 100;
        delete [] variant.statline;
        variant.statline = new char[kStatLineLen]();
        t.WhiteGet();
        t.GetRestOfLine(variant.statline, kStatLineLen);
    }
    else if (t.Is("IMAGERY"))
    {
        if (!Parse(t, "IMAGERY %\\s", buf))
            t.Error("Error parsing IMAGERY tag");
        strncpyz(imageryname, buf, MAXPATHLEN);
        imagery = TObjectImagery::FindImagery(buf);
    }
    else if (t.Is("WAIT"))
    {
        if (!Parse(t, "WAIT %i", &wait))
            t.Error("Error parsing WAIT tag");
    }
    else if (t.Is("RANGEDAMAGE"))
    {
        if (!Parse(t, "RANGEDAMAGE %i %i", &rangedamage[0], &rangedamage[1]))
            t.Error("Error parsing RANGEDAMAGE tag");
    }
    else
    {
        snprintf(buf, sizeof(buf), "Invalid Control tag in Spell.def: %s", t.Text());
        t.Error(buf);
        t.WhiteGet();
    }
    return true;
}

// ***************************
// *    TSpellList Object    *
// ***************************

// Initializes spell data stuff
bool TSpellList::Initialize()
{
    if (initialized)
        return true;

    spelldata.DeleteAll();

    if (!Load())
        return false;

    initialized = true;

    return true;
}

// Closes the spell list (idempotent — safe to call twice).
void TSpellList::Close()
{
    if (!initialized)
        return;
    spelldata.DeleteAll();
    initialized = false;
}

// REVSYNC: TSpellList::Load @ 0x0053ead0 -- every SPELL block of
// <ClassDefPath>spell.def (the packs first, as every rules file).
bool TSpellList::Load()
{
    char fname[MAXPATHLEN];
    sprintf(fname, "%s%s", ClassDefPath, "spell.def");

    FILE *fp = rev_fopen(fname, "rb");
    if (!fp)
        FatalError("Unable to find spell info file SPELL.DEF");

    TFileParseStream s(fp, fname);
    TToken t(s);

    if (!t.DefineGet())
        t.Error("Syntax error in header");

    while (t.Type() != TKN_EOF)
    {
        char spellname[MAXNAMELEN];
        if (!Parse(t, "SPELL %s\n", spellname))
            t.Error("SPELL \"name\" expected");

        PSSpellData data = new SSpellData;

        if (!data->Load(spellname, t))
            t.Error("Error loading spell data");

        spelldata.Add(data);

        if (!t.DefineGet())
            t.Error("Syntax error between spell blocks");
    }

    fclose(fp);

    return true;
}

PSSpellData TSpellList::GetSpellDataByTalismans(const char* talismans)
{
    for (int32_t i = 0; i < spelldata.NumItems(); i++)
        for (int32_t j = 0; j < spelldata[i]->variants.NumItems(); j++)
            if (!stricmp(spelldata[i]->variants[j].talismans, talismans))
                return spelldata[i];
    return nullptr;
}

PSSpellData TSpellList::GetSpellDataByName(const char* name)
{
    for (int32_t i = 0; i < spelldata.NumItems(); i++)
    {
        if (!stricmp(spelldata[i]->name, name))
            return spelldata[i];
        for (int32_t j = 0; j < spelldata[i]->variants.NumItems(); j++)
            if (!stricmp(spelldata[i]->variants[j].name, name))
                return spelldata[i];
    }
    return nullptr;
}

// REVSYNC: 0x0053ee70 (no callers in retail) -- each code counted against
// its TALISMAN type's Code stat, case-blind.
void TSpellList::GetTalList(const char* string, int32_t* tal)
{
    for (int32_t i = 0; i < TalismanClass.NumTypes(); i++)
        tal[i] = 0;

    for (size_t i = 0; i < strlen(string); i++)
        for (int32_t j = 0; j < TalismanClass.NumTypes(); j++)
        {
            const char code = (char)TalismanClass.GetStat(j, "Code");
            if (toupper(code) == toupper(string[i]))
            {
                ++tal[j];
                break;
            }
        }
}

// REVSYNC: 0x0053ef50 (no callers in retail)
bool TSpellList::CompareTalList(const int32_t* tal1, const int32_t* tal2)
{
    for (int32_t i = 0; i < TalismanClass.NumTypes(); i++)
        if (tal1[i] != tal2[i])
            return false;
    return true;
}

PSSpellVariant TSpellList::GetVariantDataByTalismans(const char* talismans)
{
    for (int32_t i = 0; i < spelldata.NumItems(); i++)
        for (int32_t j = 0; j < spelldata[i]->variants.NumItems(); j++)
            if (!stricmp(spelldata[i]->variants[j].talismans, talismans))
                return &spelldata[i]->variants[j];
    return nullptr;
}

PSSpellVariant TSpellList::GetVariantDataByName(const char* name)
{
    for (int32_t i = 0; i < spelldata.NumItems(); i++)
        for (int32_t j = 0; j < spelldata[i]->variants.NumItems(); j++)
            if (!stricmp(spelldata[i]->variants[j].name, name))
                return &spelldata[i]->variants[j];
    return nullptr;
}

// **************
// *** TSpell ***
// **************

// REVSYNC: TSpell::TSpell @ 0x0053f090 -- the targets (1 to 64; with no
// array, the invoker), each rolled for POISONCHANCE (random(0, 100) below it
// poisons); then a STATLINE on a player invoker: his buff flag and the stat
// effect.
TSpell::TSpell(TObjectInstance* invoke,
    TObjectInstance* *targ, int32_t numtargs, S3DPoint* sourcepos,
    PSSpellData dat, PSSpellVariant var, PTSpell mtr)
{
    invoker = invoke;
    invoker_ref = invoke;
    for (auto& target : targets) target = nullptr;
    frame = 0;
    magic_offense = magic_defense = 0;

    if (numtargs < 1)
        numtargs = 1;
    if (numtargs >= MAXSPELLTARGETS)
        numtargs = MAXSPELLTARGETS;
    targetnum = numtargs;
    if (!targ)
        targets[0] = invoker;
    else
        memcpy(targets, targ, numtargs * sizeof(TObjectInstance*));

    spell = dat;
    for (int32_t i = 0; i < targetnum; ++i)
        target_refs[i] = targets[i];
    // Only with a target array. The inclusive roll and strict comparison
    // keep a chance of 100 short of certain.
    // REVSYNC-DIVERGENCE: retail calls SetPoisoned (slot 0x1b4) on whatever
    // the target is; the port only on characters (spells target nothing else).
    if (targ && dat->poisonchance != 0)
        for (int32_t i = 0; i < targetnum; ++i)
        {
            const int32_t roll = random(0, 100);
            if (roll < dat->poisonchance)
                if (auto* character = dynamic_cast<TCharacter*>(targets[i]))
                    character->SetPoisoned(1);
        }
    variant = var;
    timer = -1;
    master = mtr;
    wait = dat->effectstart;

    if (variant->statline && invoker && invoker->ObjClass() == OBJCLASS_PLAYER)
    {
        auto* player = static_cast<TPlayer*>(invoker);
        player->SetBuffed();
        player->AddStatEffect(variant->statline);
    }

    if (sourcepos)
        source = *sourcepos;
    else
        source.x = source.y = source.z = -1;    // Means not used
}

// REVSYNC-DIVERGENCE: retail's destructor (0x00540b20) only frees the effect
// list, so an effect the spell never let go of (no Kill first) keeps a
// dangling spell; the port's let go of it here.
TSpell::~TSpell()
{
    for (const TSafeRef<TObjectInstance>& ref : effects)
        if (auto* fx = dynamic_cast<TEffect*>(ref.Get()); fx && fx->GetSpell() == this)
            fx->SetSpell(nullptr);
}

S3DPoint TSpell::SourceOffset() const
{
    if (source.x == -1 && source.y == -1 && source.z == -1)
        return S3DPoint(0, 0, 0);
    return source;
}

// get by name
void TSpell::SetByName(char* name)
{
    spell = SpellList.GetSpellDataByName(name);
    variant = SpellList.GetVariantDataByName(name);
}

// get by talismans
void TSpell::SetByTalismans(char* talismans)
{
    spell = SpellList.GetSpellDataByTalismans(talismans);
    variant = SpellList.GetVariantDataByTalismans(talismans);
}

TEffect* TSpell::NewEffect(SObjectDef& def, int32_t imagery)
{
    TObjectInstance* made = MapPane.GetInstance(MapPane.NewObject(&def, imagery));
    lasteffect = made;
    auto* fx = dynamic_cast<TEffect*>(made);
    if (!fx)
        return nullptr;
    fx->SetSpell(this);
    effects.emplace_back(made);
    return fx;
}

// The DELAY counts down; at its end the variant's EFFECT is made at the
// invoker plus the source, facing his way with FACING. Nothing ends the
// spell here but a Kill (timer 0): retail has no "no effect, no spell"
// rule (the 1998 code ended it then), and no right-hand source or HEIGHT
// fallback.
// REVSYNC-DIVERGENCE: retail reads the invoker through null when he is gone
// by then (ObjectRemoved cleared him); the port makes no effect.
bool TSpell::Timer()
{
    if (timer > 0)
        --timer;
    if (wait > 0)
    {
        --wait;
        return timer == 0;
    }
    if (wait == 0)
    {
        wait = -1;
        if (TObjectInstance* caster = invoker_ref.Get())
        {
            SObjectDef def{};
            def.objclass = OBJCLASS_EFFECT;
            def.level = (uint16_t)MapPane.GetMapLevel();
            def.pos = caster->Pos() + SourceOffset();
            def.facing = variant->facing ? (uint8_t)caster->GetFace() : 0;
            def.objtype = (short)EffectClass.FindObjType(variant->effect);
            NewEffect(def);
        }
    }
    return timer == 0;
}

// Retail skips it while the game is paused ([0x0065d0c4]), as it skips the
// caster's whole Pulse; that flag isn't identified in the port yet.
void TSpell::Pulse()
{
    for (size_t i = 0; i < effects.size(); i++)
    {
        const TSafeRef<TObjectInstance> ref = effects[i];
        TObjectInstance* fx = ref.Get();
        if (!fx || fx->GetSector())
            continue;
        fx->Pulse();
        if (ref.Get())
            MapPane.AddObject(fx);          // slot 0x8c: into the map if it fits now
    }
}

// REVSYNC-DIVERGENCE: retail removes slot 0 of the list once per effect
// (0x00540af0 takes its index from a zero argument), so the others stay
// listed, spell-less and killed, until the spell is deleted; a second Kill
// meanwhile (an ObjectRemoved for one of them) runs the loop again. The
// port lets go of all of them; the effects end the same way.
void TSpell::Kill()
{
    timer = 0;
    for (const TSafeRef<TObjectInstance>& ref : effects)
    {
        TObjectInstance* fx = ref.Get();
        if (!fx)
            continue;
        if (TObjectInstance* poisoned = PoisonTarget())
            poisoned->SetPoisoned(0);
        fx->SetFlags(OF_KILL);
        if (auto* effect = dynamic_cast<TEffect*>(fx))
            effect->SetSpell(nullptr);
    }
    effects.clear();
}

void TSpell::ObjectRemoved(int32_t notify, void* ptr)
{
    auto gone = [notify, ptr](TObjectInstance* obj) {
        if (!obj)
            return false;
        if (notify == N_DELETINGOBJECT)
            return obj == ptr;
        return notify == N_DELETINGSECTOR && (void*)obj->GetSector() == ptr && !(obj->GetFlags() & OF_NONMAP);
    };
    if (gone(invoker_ref.Get()))
    {
        Kill();
        invoker = nullptr;
        invoker_ref.Clear();
    }
    if (std::any_of(effects.begin(), effects.end(), [&](const TSafeRef<TObjectInstance>& ref) { return gone(ref.Get()); }))
        Kill();
    for (int32_t i = 0; i < targetnum; i++)
        if (gone(target_refs[i].Get()))
            SetTarget(i, nullptr);
}

// REVSYNC: TSpell::Damage @ 0x0053f560 -- the variant's damage range, each
// end cut by the target's magic resistance, rolled; a player caster's
// SpellDamageInc adds its percent, a player target's DmgResMagical takes
// its percent away; the target takes it with the spell's damage type and
// the caster as attacker, and a player caster is offered the kill.
void TSpell::Damage(TObjectInstance* ch)
{
    int32_t mindam = variant->mindamage;
    int32_t maxdam = variant->maxdamage;
    if (!ch)
        return;
    auto* target = static_cast<TCharacter*>(ch);
    if (target->IsMagicResistant())
    {
        // x87 in retail: the float times the int, truncated. A double holds
        // the product exactly for any damage spell.def gives.
        const double resist = target->GetMagicResistance();
        mindam -= (int32_t)(resist * mindam);
        maxdam -= (int32_t)(resist * maxdam);
    }
    int32_t damage = random(mindam, maxdam);
    auto* caster = static_cast<TCharacter*>(invoker);
    TPlayer* player = (caster && caster->ObjClass() == OBJCLASS_PLAYER) ? static_cast<TPlayer*>(caster) : nullptr;
    if (player)
        damage = (player->SpellDamageInc() + 100) * damage / 100;
    if (target->ObjClass() == OBJCLASS_PLAYER)
        damage = (100 - target->DamageModifier(DT_MAGICAL)) * damage / 100;
    target->Damage(damage, spell->damagetype, 0, nullptr, caster);
    if (player)
        player->AwardKillExp(target);
}

namespace
{
// A player's cost: ManaCostPct percent less (truncated), as the cast gate
// and ManaDrain both work it out (0x0053ff9a, 0x0053f6b6).
int32_t ManaCost(TCharacter* caster, int32_t mana)
{
    if (caster->ObjClass() == OBJCLASS_PLAYER)
        mana += -(static_cast<TPlayer*>(caster)->ManaCostPct() * mana) / 100;
    return mana;
}
}  // namespace

void TSpell::ManaDrain()
{
    if (!invoker)
        return;
    if (MagicCheat && invoker->ObjClass() == OBJCLASS_PLAYER)
        return;
    auto* caster = static_cast<TCharacter*>(invoker);
    const int32_t cost = ManaCost(caster, variant->mana);
    caster->SetMana(caster->Mana() - cost);
    const int32_t max = caster->MaxMana();
    if (caster->Mana() > max)
        caster->SetMana(caster->MaxMana());
}

namespace
{
// The characters the map has round `pos` (retail's iterator 0x0044ceb0 with
// flags 0xe0: no inventories, the map square `radius` round it, the loaded
// sectors) on `level` (-1: every loaded level).
std::vector<TCharacter*> CharactersAround(int32_t level, const S3DPoint& pos, int32_t radius)
{
    if (TCharacter::nearbyCharactersSeam)
        return TCharacter::nearbyCharactersSeam(pos, radius);
    std::vector<TCharacter*> around;
    SRect r{pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius};
    for (TMapIterator i(level, &r, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
        around.push_back(static_cast<TCharacter*>(i.Item()));
    return around;
}
}  // namespace

// No attacker, no damage. A target is the attacker's enemy when the attacker
// is a character (anyone else hits every character).
// REVSYNC-DIVERGENCE: retail hands Damage the attacker object whatever it is;
// the port hands it the attacker only when it is a character (every retail
// caller passes its spell's invoker, a character or null).
void AreaDamage(TObjectInstance* attacker, const S3DPoint& pos, int32_t radius, int32_t mindamage,
                int32_t maxdamage, int32_t damagetype, int32_t minradius)
{
    if (!attacker)
        return;
    TCharacter* caster = (attacker->ObjClass() == OBJCLASS_CHARACTER || attacker->ObjClass() == OBJCLASS_PLAYER)
                             ? static_cast<TCharacter*>(attacker)
                             : nullptr;
    TPlayer* player = (caster && caster->ObjClass() == OBJCLASS_PLAYER) ? static_cast<TPlayer*>(caster) : nullptr;

    for (TCharacter* target : CharactersAround(attacker->GetLevel(), pos, radius))
    {
        if (target == caster)
            continue;
        const int32_t distance = ::Distance(pos, target->Pos());
        if (distance > radius || distance < minradius)
            continue;
        if (target->Health() <= 0)
            continue;
        if (caster && !caster->IsEnemy(target))
            continue;
        if (target->IsDoing(ACTION_IMPACT))
            continue;
        int32_t damage = random(mindamage, maxdamage);
        if (player)
            damage = (player->SpellDamageInc() + 100) * damage / 100;
        if (target->ObjClass() == OBJCLASS_PLAYER)
            damage = (100 - target->DamageModifier(DT_MAGICAL)) * damage / 100;
        target->KnockBack(pos, -1);
        target->Damage(damage, damagetype, 0, nullptr, caster);
        if (player)
            player->AwardKillExp(target);
    }
}

// *******************
// *** TStrikeSpell ***
// *******************

namespace
{
// The character `who` fights: his root block's object while that block is
// combat or bow (retail: root action 3 or 0x19, the object at root +0x44).
TObjectInstance* FightingTarget(TObjectInstance* who)
{
    auto* chr = dynamic_cast<TCharacter*>(who);
    return chr ? chr->Fighting() : nullptr;
}

// The CONTROLDATA sound at `at`: PLAY loops it, PLAYONCE plays it once
// (retail's 0x0049b990 with a loop count of 0 / 1), at full volume.
void PlayControlSound(const SSpellControlData& control, S3DPoint at)
{
    const bool loop = control.flags & SCF_PLAY;
    if (!loop && !(control.flags & SCF_PLAYONCE))
        return;
    const int32_t id = SoundPlayer.FindSound(control.sound);
    if (id < 0 || !SoundPlayer.Mount(id))
        return;
    if (TSound* sound = SoundPlayer.GetSound(id))
        sound->SetLooping(loop);
    SoundPlayer.Play(id, 0, 0, &at);
}
}  // namespace

void TStrikeSpell::Pulse()
{
    TSpell::Pulse();
    ++frame;
    const SSpellControlData& control = *variant->controldata;
    if (control.repeatset && frame % 24 == 0 && invoker_ref.Get())
    {
        TObjectInstance* foe = FightingTarget(invoker_ref.Get());
        if (TObjectInstance* struck = striketarget.Get())
        {
            if (foe != struck || struck->Health() <= 0)
            {
                Kill();
                return;
            }
            if (foe)
                Damage(foe);
        }
    }
    if (control.duration != -1 && frame >= control.duration)
    {
        Kill();
        TObjectInstance* caster = invoker_ref.Get();
        TObjectInstance* foe = FightingTarget(caster);
        if (foe && spell->poisonchance && foe != caster)
            foe->SetPoisoned(0);
    }
}

SObjectDef TStrikeSpell::EffectDef(TObjectInstance* target) const
{
    const SSpellControlData& control = *variant->controldata;
    SObjectDef def{};
    def.objclass = OBJCLASS_EFFECT;
    def.level = (uint16_t)MapPane.GetMapLevel();
    def.pos = target->Pos();
    if (control.posset)
        def.pos += S3DPoint(control.pos[0], control.pos[1], control.pos[2]);
    else
    {
        if (!control.attachset)
            def.pos += S3DPoint(50, 50, 50);
        else if (S3DPoint offset; ObjectMapOffset(target, control.attach, offset))
            def.pos += offset;
        def.pos += SourceOffset();
    }
    TObjectInstance* caster = invoker_ref.Get();
    def.facing = (variant->facing && caster) ? (uint8_t)caster->GetFace() : 0;
    def.objtype = (short)EffectClass.FindObjType(variant->effect);
    return def;
}

TEffect* TStrikeSpell::NewStrikeEffect(SObjectDef& def, TObjectInstance* anchor, int32_t angle, int32_t dist, bool orbit)
{
    const SSpellControlData& control = *variant->controldata;
    TEffect* fx = NewEffect(def, control.imagery);
    if (fx && control.follow)
    {
        fx->Follow(anchor, angle, dist, orbit);
        fx->SetFollowTime(control.duration);
    }
    return fx;
}

void TStrikeSpell::Time(TEffect* fx) const
{
    const SSpellControlData& control = *variant->controldata;
    fx->SetLife(control.duration, control.duration2);
    fx->SetStartWait(random(0, control.wait));
}

// The countdowns as TSpell's. At the end of DELAY:
//  1. the anchor: with HITTARGET (and not ONCASTER) the one the caster
//     fights -- none, and the spell is over -- or, with no caster, its
//     first target; otherwise the caster;
//  2. the effect def on the anchor; the struck target (the one the caster
//     fights, or with no caster the anchor) becomes its first target;
//  3. PLAY / PLAYONCE at the def;
//  4. the effects (an anchor out of the map keeps those that follow or
//     attach out of it too):
//     - ATTACHMULTIPLE: HITS of them, each on its attach point (with
//       FOLLOW), timed;
//     - MULTIPLETARGETS: one on each living character within RADIUS but the
//       caster (or with ONCASTER one on the anchor, untimed), then the
//       blast (AreaDamage) around the caster;
//     - else HITS in the PATTERN around the def, timed: at it (none), at a
//       random angle and distance, round a circle, along a line ahead,
//       round a cluster moving outward, in an X, along a spiral.
// REVSYNC-DIVERGENCE: with no caster and no first target retail reads an
// uninitialized anchor, and with neither caster nor HITTARGET it builds on
// none (a crash); the port's spell then ends as with no one to strike.
bool TStrikeSpell::Timer()
{
    if (timer > 0)
        --timer;
    if (wait > 0)
    {
        --wait;
        return timer == 0;
    }
    if (wait < 0)
        return timer == 0;
    wait = -1;

    const SSpellControlData& control = *variant->controldata;
    TObjectInstance* caster = invoker_ref.Get();
    TObjectInstance* anchor = caster;
    if (control.hittarget && !control.oncaster)
        anchor = caster ? FightingTarget(caster) : GetTargetRef(0).Get();
    if (!anchor)
        return true;

    SObjectDef def = EffectDef(anchor);
    striketarget = caster ? FightingTarget(caster) : anchor;
    SetTarget(0, striketarget.Get());
    PlayControlSound(control, def.pos);
    if ((control.follow || control.attachset) && (anchor->GetFlags() & OF_NONMAP))
        def.flags |= OF_NONMAP;

    const S3DPoint base = def.pos;
    const int32_t hits = control.hits;
    const int32_t radius = control.radius;
    // Retail's x87 steps, each kept as a float: 255 / HITS, 2R / HITS,
    // 4R / HITS (R the RADIUS, a float too).
    const float r = (float)radius;
    const float turn = (float)(255.0 / hits);
    const float span = (float)(2.0 * r / hits);
    const float span4 = (float)(4.0 * r / hits);
    auto around = [&](int32_t angle, int32_t dist) {
        S3DPoint v;
        ConvertToVector(angle, dist, v);
        def.pos = base + v;
    };
    auto pattern = [&](int32_t angle, int32_t dist) {
        if (TEffect* fx = NewStrikeEffect(def, anchor, angle, dist, true))
            Time(fx);
    };

    if (control.attachset && control.attachmultiple)
    {
        for (int32_t i = 0; i < hits; i++)
            if (TEffect* fx = NewStrikeEffect(def, anchor, 0, 0, false))
            {
                if (control.follow)
                    fx->SetAttachName(control.attachmultiple + i * RESNAMELEN);
                Time(fx);
            }
        return timer == 0;
    }

    if (control.multipletargets && caster)
    {
        const S3DPoint center = caster->Pos();
        if (!control.oncaster)
        {
            for (TCharacter* c : CharactersAround(-1, center, radius))
                if (c != caster && c->Health() > 0)
                {
                    SObjectDef at = EffectDef(c);
                    if (TEffect* fx = NewStrikeEffect(at, anchor, 0, 0, false))
                        Time(fx);
                }
        }
        else
            NewStrikeEffect(def, anchor, 0, 0, false);
        AreaDamage(caster, center, radius, variant->mindamage, variant->maxdamage, spell->damagetype, 0);
        return timer == 0;
    }

    switch (control.pattern)
    {
        case SPAT_NONE:
            for (int32_t i = 0; i < hits; i++)
                if (TEffect* fx = NewStrikeEffect(def, anchor, 0, 0, false))
                    Time(fx);
            break;
        case SPAT_RANDOM:
            for (int32_t i = 0; i < hits; i++)
            {
                const int32_t angle = random(0, 255);
                const int32_t dist = random(0, radius);
                around(angle, dist);
                pattern(angle, dist);
            }
            break;
        case SPAT_CIRCLE:
            for (int32_t i = 0; i < hits; i++)
            {
                const int32_t angle = (int32_t)(i * (double)turn) + 64;
                around(angle, radius);
                pattern(angle, radius);
            }
            break;
        case SPAT_LINE:
            for (int32_t i = 0; i < hits; i++)
            {
                const int32_t dist = (int32_t)(r - i * (double)span);
                around(def.facing, dist);
                pattern(def.facing, dist);
            }
            break;
        case SPAT_CLUSTER:
            for (int32_t i = 0; i < hits; i++)
            {
                const int32_t angle = (int32_t)(i * (double)turn);
                ConvertToVector(angle, (int32_t)((double)(radius - 10) / hits), def.vel);
                around(angle, radius - 10);
                pattern(angle, radius - 10);
            }
            break;
        case SPAT_X:
            for (int32_t side = 0; side < 2; side++)
                for (int32_t i = 0; i < hits / 2; i++)
                {
                    const int32_t angle = def.facing + side * 64;
                    const int32_t dist = (int32_t)(r - i * (double)span4 + span4 * 0.5);
                    around(angle, dist);
                    pattern(angle, dist);
                }
            break;
        case SPAT_SPIRAL:
            for (int32_t i = 0; i < hits; i++)
            {
                const int32_t angle = (int32_t)(i * (double)turn);
                const int32_t dist = (int32_t)(i * (double)span);
                around(angle, dist);
                pattern(angle, dist);
            }
            break;
        default:
            break;
    }
    return timer == 0;
}

void TStrikeSpell::Kill()
{
    TSpell::Kill();
    SoundPlayer.Stop(SoundPlayer.FindSound(variant->controldata->sound));
}

// *********************
// *** TSpellManager ***
// *********************

bool MagicCheat = false;

namespace
{
TSpell* CreateSpell(TObjectInstance* invoker, TObjectInstance** targets, int32_t numtargs, S3DPoint* sourcepos,
                    SSpellData* dat, SSpellVariant* var, TSpell* master)
{
    return new TSpell(invoker, targets, numtargs, sourcepos, dat, var, master);
}

TSpell* CreateStrike(TObjectInstance* invoker, TObjectInstance** targets, int32_t numtargs, S3DPoint* sourcepos,
                     SSpellData* dat, SSpellVariant* var, TSpell* master)
{
    return new TStrikeSpell(invoker, targets, numtargs, sourcepos, dat, var, master);
}

// REVSYNC: the registry 0x00670220 -- "Spell" (0x00540b80 -> creator
// 0x00542200), "Strike" (0x00540ba0 -> 0x00542290), in registration order.
const SSpellClass kSpellClasses[] = {{"Spell", CreateSpell}, {"Strike", CreateStrike}};
}  // namespace

const SSpellClass* FindSpellClass(const char* name)
{
    for (const SSpellClass& c : kSpellClasses)
        if (!stricmp(name, c.name))
            return &c;
    return nullptr;
}

// The mana gate (0x0053ff7f, 0x0053f9c4): the cost against the caster's mana
// unless the cheat is on. The main player hears why.
// REVSYNC-DIVERGENCE: retail prints the dialog line as the text bar's format
// (0x005400fe); the port passes it as text. The shipped lines have no '%'.
bool TSpellManager::CanAfford(TCharacter* caster, const SSpellVariant& variant, bool byname) const
{
    (void)byname;
    const int32_t cost = ManaCost(caster, variant.mana);
    if (caster->Mana() >= cost || MagicCheat)
        return true;
    if (caster == Player)
        TextBar.Print("%s", DialogList.GetLine("SPLMANA"));
    return false;
}

// The skill roll for a player (0x0054004b, 0x0053faa2): Invoke against the
// variant's skill gives the chance -- 100 at or above it, then 80, 40, 20
// for 1, 2, 3 below, 0 further down -- and a roll above it fails. By
// talismans only the main player can fail (any other player casts anyway);
// by name every player can. The main player hears why.
bool TSpellManager::SkillRollPasses(TCharacter* caster, const SSpellVariant& variant, int32_t roll, bool byname) const
{
    const int32_t d = static_cast<TPlayer*>(caster)->Skill(SK_INVOKE) - variant.skilllevel;
    const int32_t chance = d >= 0 ? 100 : d == -1 ? 80 : d == -2 ? 40 : d == -3 ? 20 : 0;
    if (chance >= roll)
        return true;
    if (caster != Player)
        return !byname;
    TextBar.Print("%s", DialogList.GetLine(chance == 0 ? "SPLLVLNEG" : "SPLLVLLOW"));
    return false;
}

// The Invoke skill experience a player earns by casting (0x00540128,
// 0x0053fbab), before the spell exists and whether or not it works: for a
// skill above 0, the level's base (5 Invoke + 15) x 20 / 8, weighted by how
// far the skill is above Invoke (clamped to 9 either way).
void TSpellManager::AwardInvokeExp(TPlayer* player, const SSpellVariant& variant)
{
    (void)player->SkillExp(SK_INVOKE);              // read and unused, as retail does
    const int32_t invoke = player->Skill(SK_INVOKE);
    if (variant.skilllevel <= 0)
        return;
    const int32_t base = (5 * invoke + 15) * 20 / 8;
    int32_t above = variant.skilllevel - invoke;
    above = above < -9 ? -9 : above > 9 ? 9 : above;
    player->AddSkillExp(SK_INVOKE, (above + 10) * base * 10 / 100);
}

// A buff cast by a caster with a buff on (0x005401e5, 0x0053fc23): every
// buff effect around him has its sound stopped, and his own are ended.
// REVSYNC-DIVERGENCE: retail reads the flag through a null caster (a crash);
// the port asks only when there is one.
void TSpellManager::EndOtherBuffs(TCharacter* caster)
{
    std::vector<TObjectInstance*> around;
    if (nearbySeam)
        around = nearbySeam(caster);
    else
        for (TMapIterator i(*caster, CHECK_NONE, OBJSET_ANIMATE); i; i++)
            around.push_back(i.Item());
    for (TObjectInstance* obj : around)
    {
        if (obj->ObjClass() != OBJCLASS_EFFECT)
            continue;
        auto* fx = static_cast<TEffect*>(obj);
        TSpell* other = fx->GetSpell();
        if (!other || !other->VariantData() || !other->VariantData()->statline)
            continue;
        if (const SSpellControlData* cd = other->VariantData()->controldata)
        {
            const int32_t sound = SoundPlayer.FindSound(cd->sound);
            if (sound)
                SoundPlayer.Stop(sound);
        }
        if (other->GetInvoker() == caster)
            fx->KillThisEffect();
    }
}

// The cast once it's paid for (0x005403e2 / 0x0053fe05 on): the invoke
// animation with the first target, the cooldown, the mana, the list.
void TSpellManager::Commit(TSpell* spell, TCharacter* caster, TObjectInstance** targets, SSpellData* spelldata,
                           SSpellVariant* variant)
{
    if (caster)
        caster->SetCast(spelldata->invoke, targets ? targets[0] : nullptr, variant->ani_delay);
    wait = variant->nextspellwait;
    spell->ManaDrain();
    Add(spell);
}

void TSpellManager::Add(TSpell* spell)
{
    for (int32_t i = 0; i < spells.NumItems(); i++)
        if (!spells.Used(i))
        {
            spells.Set(spell, i);
            return;
        }
    spells.Add(spell);
}

// The first spell and variant with these talismans; the cooldown, unless
// in the editor; with an invoker the mana, the skill roll (not for a skill
// below 0 or under the cheat; only the main player can fail it) and a
// player's Invoke experience; a buff ends the caster's others; the spell of
// its CONTROLDATA class (else "Spell").
// REVSYNC-DIVERGENCE: an unknown CONTROLDATA class calls through null in
// retail (0x005402c5); the port refuses the cast. Shipped data names only
// "Strike".
bool TSpellManager::CastByTalismans(char* talismans, TObjectInstance* invoker,
    TObjectInstance* *targets, int32_t numtargs, S3DPoint* sourcepos, PTSpell mst)
{
    SSpellData* spelldata = SpellList.GetSpellDataByTalismans(talismans);
    SSpellVariant* variant = SpellList.GetVariantDataByTalismans(talismans);
    if (!spelldata || !variant)
        return false;
    if (wait != 0 && !Editor)
        return false;

    auto* caster = static_cast<TCharacter*>(invoker);
    if (caster)
    {
        if (!CanAfford(caster, *variant, false))
            return false;
        const bool player = caster->ObjClass() == OBJCLASS_PLAYER;
        if (variant->skilllevel >= 0 && !MagicCheat)
        {
            const int32_t roll = random(1, 100);
            if (player && !SkillRollPasses(caster, *variant, roll, false))
                return false;
        }
        if (player)
            AwardInvokeExp(static_cast<TPlayer*>(caster), *variant);
    }

    const SSpellClass* cls = FindSpellClass(variant->controldata ? variant->controldata->name : "spell");
    if (variant->statline && caster && caster->IsBuffed())
        EndOtherBuffs(caster);
    if (!cls)
        return false;
    TSpell* spell = cls->create(invoker, targets, numtargs, sourcepos, spelldata, variant, mst);
    spell->SetData(SpellList.GetSpellDataByTalismans(talismans), SpellList.GetVariantDataByTalismans(talismans));
    Commit(spell, caster, targets, spelldata, variant);
    return true;
}

// As CastByTalismans, by name: the spell whose SPELL or variant name
// matches, the first variant of that name; a cooldown even in the editor;
// the skill roll drawn for every invoker, a player's chance and experience
// skipped under the cheat; a spell the class didn't make refuses.
bool TSpellManager::CastByName(char* name, TObjectInstance* invoker,
    TObjectInstance* *targets, int32_t numtargs, S3DPoint* sourcepos, PTSpell mst)
{
    if (castByNameSeam)
        return castByNameSeam(this, name, invoker, targets, numtargs, sourcepos, mst);
    SSpellData* spelldata = SpellList.GetSpellDataByName(name);
    SSpellVariant* variant = SpellList.GetVariantDataByName(name);
    if (!spelldata || !variant)
        return false;
    if (wait != 0)
        return false;

    auto* caster = static_cast<TCharacter*>(invoker);
    if (caster)
    {
        if (!CanAfford(caster, *variant, true))
            return false;
        const int32_t roll = random(1, 100);
        if (caster->ObjClass() == OBJCLASS_PLAYER && !MagicCheat)
        {
            if (!SkillRollPasses(caster, *variant, roll, true))
                return false;
            AwardInvokeExp(static_cast<TPlayer*>(caster), *variant);
        }
    }

    const SSpellClass* cls = FindSpellClass(variant->controldata ? variant->controldata->name : "spell");
    if (variant->statline && caster && caster->IsBuffed())
        EndOtherBuffs(caster);
    if (!cls)
        return false;
    TSpell* spell = cls->create(invoker, targets, numtargs, sourcepos, spelldata, variant, mst);
    if (!spell)
        return false;
    spell->SetData(SpellList.GetSpellDataByName(name), SpellList.GetVariantDataByName(name));
    Commit(spell, caster, targets, spelldata, variant);
    return true;
}

// REVSYNC: TSpellManager::Pulse @ 0x00540750 -- the cooldown; then each
// spell's Pulse and Timer, and one over is killed and deleted (its slot
// left vacant; the next index is not skipped).
void TSpellManager::Pulse()
{
    if (wait > 0)
        --wait;

    const int32_t size = spells.NumItems();
    for (int32_t i = 0; i < size; ++i)
    {
        if (!spells.Used(i))
            continue;
        TSpell* spell = spells[i];
        spell->Pulse();
        // Its effects' pulses can remove objects, and a removal can delete a
        // spell (ObjectRemoved): this one is carried on only if still here.
        if (!spells.Used(i) || spells[i] != spell)
            continue;
        if (spell->Timer())
        {
            spell->Kill();
            spells.Delete(i);
        }
    }
}

void TSpellManager::ObjectRemoved(int32_t notify, void* ptr)
{
    const int32_t size = spells.NumItems();
    for (int32_t i = 0; i < size; ++i)
    {
        if (!spells.Used(i))
            continue;
        TSpell* spell = spells[i];
        spell->ObjectRemoved(notify, ptr);
        if (spell->Timer())
            spells.Delete(i);
    }
}

int32_t TSpellManager::GetDefense()
{
    int32_t defense = 0;

    int32_t size = spells.NumItems();
    
    for(int32_t i = 0; i < size; ++i)
    {
        if (!spells.Used(i))
            continue;

        defense += spells[i]->GetDefense();
    }

    return defense;
}

int32_t TSpellManager::GetOffense()
{
    int32_t offense = 0;

    int32_t size = spells.NumItems();
    
    for(int32_t i = 0; i < size; ++i)
    {
        if (!spells.Used(i))
            continue;

        offense += spells[i]->GetOffense();
    }

    return offense;
}

int32_t TSpellManager::GetSpellCount(char* spell)
{
    int32_t count = 0;

    int32_t size = spells.NumItems();

    for(int32_t i = 0; i < size; ++i)
    {
        if (!spells.Used(i))
            continue;

        if (!stricmp(spells[i]->VariantData()->name, spell))
            ++count;
    }

    return count;
}
