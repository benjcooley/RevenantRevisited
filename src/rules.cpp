// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  Rules.cpp - TRules object module                     *
// *************************************************************************

#include <algorithm>
#include <array>
#include <string>

#include "revenant.h"
#include "parse.h"
#include "object.h"
#include "playscreen.h"
#include "rules.h"
#include "revutils.h"
#include "logging.h"

static char errorparsingtag[] = "Error parsing tag %s";

// Tag is tester
#define TAGIS(t) (!stricmp(tag, t))

// REVSYNC: the raw number rows of DAMAGEMODS, STATREQS and SKILLMODS
// (0x00489e55, 0x0048927e, 0x004892f2): NUMBER tokens with ',' between them,
// no expressions or #defines.
static void ReadNumberRow(TToken &t, int32_t *values, int32_t count, const char *expected)
{
    for (int32_t c = 0; c < count; c++)
    {
        if (c > 0)
        {
            if (!t.Is(","))
                t.Error("',' Expected");
            t.WhiteGet();
        }
        if (t.Type() != TKN_NUMBER)
            t.Error(expected);
        values[c] = t.Index();
        t.WhiteGet();
    }
}

// *********************
// * SClassData Object *
// *********************

// REVSYNC: SClassData::Load @ 0x004891c0
// Loads the class info from the "RULES.DEF" file
bool SClassData::Load(char *aname, TToken &t)
{
    strncpyz(name, aname, RESNAMELEN);
    weapons = 0xffff;

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Char block BEGIN expected");
    t.LineGet();

    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
        if (t.Type() != TKN_IDENT)
            t.Error("Class data keyword expected");

        char tag[40];
        strncpyz(tag, t.Text(), 40);
        t.WhiteGet();

        bool ok = true;
        if (TAGIS("STATREQS"))
        {
            // ClearPlayer / SetPlayerLevel apply value i to attribute
            // PLRSTAT_FIRST + i (Strn Cons Agil Rflx Mind Luck). rules.def's
            // comment row says "str,con,agl,rflx,luck,mind"; the shipped
            // code gives the fifth value to Mind and the sixth to Luck.
            static_assert(NUM_PLRSTATS == 6, "STATREQS row width changed");
            ReadNumberRow(t, statreqs, NUM_PLRSTATS, "Stat requirement value expected");
        }
        else if (TAGIS("SKILLMODS"))
        {
            static_assert(NUM_SKILLS == 11, "SKILLMODS row width changed");
            ReadNumberRow(t, skillmods, NUM_SKILLS, "Skill modifier expected");
        }
        else if (TAGIS("HEALTHMOD"))
            ok = Parse(t, "%i", &healthmod);
        else if (TAGIS("FATIGUEMOD"))
            ok = Parse(t, "%i", &fatiguemod);
        else if (TAGIS("MANAMOD"))
            ok = Parse(t, "%i", &manamod);
        else if (TAGIS("WEAPONS"))
            ok = Parse(t, "%i", &weapons);  // e.g. WM_BLUDGEON | WM_BOW | WM_CROSSBOW
        else
            t.Error("Invalid class tag %s", t.Text());  // retail names the token after the tag

        if (!ok)
            t.Error(errorparsingtag, tag);

        if (t.Type() != TKN_RETURN)
            t.Error("Return expected");

        t.LineGet();
    }

    if (!t.Is("END"))
        t.Error("Class block END expected");
    t.Get();

    return true;
}

// ********************
// * SItemData Object *
// ********************

// REVSYNC: the WEAPON / ARMOR block loaders 0x0048ac30 / 0x0048b1a0:
// BASICMODS (eight numbers), DESCRIPTION (a dialog tag), STATLINE (the rest
// of the line, kept as PlayerStats::ReadStatLine joins it).
bool SItemData::Load(const char *aname, TToken &t)
{
    name = aname;

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Item block BEGIN expected");
    t.LineGet();

    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
        if (t.Type() != TKN_IDENT)
            t.Error("Item data keyword expected");

        char tag[40];
        strncpyz(tag, t.Text(), 40);
        t.WhiteGet();

        bool ok = true;
        if (TAGIS("BASICMODS"))
        {
            ok = Parse(t, "%i, %i, %i, %i, %i, %i, %i, %i",
                       &basicmods[0], &basicmods[1], &basicmods[2], &basicmods[3],
                       &basicmods[4], &basicmods[5], &basicmods[6], &basicmods[7]);
        }
        else if (TAGIS("DESCRIPTION"))
        {
            char tagname[RESNAMELEN];
            ok = Parse(t, "%32t", tagname);
            if (ok)
                description = tagname;
        }
        else if (TAGIS("STATLINE"))
        {
            statline = PlayerStats::ReadStatLine(t);
        }
        else
            t.Error("Invalid character tag %s", tag);

        if (!ok)
            t.Error(errorparsingtag, tag);
        if (t.Type() != TKN_RETURN)
            t.Error("Return expected");
        t.LineGet();
    }

    if (!t.Is("END"))
        t.Error("Item block END expected");
    t.Get();

    return true;
}

// ********************
// * SCharData Object *
// ********************

// Destroy SCharData
SCharData::~SCharData()
{
    attacks.Clear();
}

// REVSYNC: SCharData::Load @ 0x00489850
// Loads the character from the "CHAR.DEF" file. Every tag is parsed into its
// field here; the object type is found later by name (TRules::BindTypes).
bool SCharData::Load(char *aname, TToken &t)
{
    strncpyz(name, aname, RESNAMELEN);

    if (!stricmp(name, "Default"))
    {
        objtype = -1; objclass = -1;
    }
    else
    {
        objtype = -2; objclass = -2;
    }

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Char block BEGIN expected");

  // Now get first trigger token
    t.LineGet();

    PSCharAttackData lastattack = nullptr;
    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
        if (t.Type() != TKN_IDENT)
            t.Error("Char data keyword expected");

        char tag[40];
        strncpyz(tag, t.Text(), 40);

        t.WhiteGet();
        bool ok = true;

      // The attack tags. Each fills a zeroed record, which the array copies;
      // the IMPACT tags after it add to the stored copy.
        if (TAGIS("ATTACK"))
        {
            SCharAttackData ad;
            memset(static_cast<void *>(&ad), 0, sizeof(ad));

            ok = Parse(t, "%30s, %i, %i, %30s, %30s, %30s, %30s, "
                "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i",
                ad.attackname, &ad.flags, &ad.button,
                ad.responsename, ad.blockname, ad.missname, ad.chainname,
                &ad.blocktime, &ad.impacttime, &ad.nextwait, &ad.chainexptime,
                &ad.mindist, &ad.maxdist, &ad.hitminrange, &ad.hitmaxrange,
                &ad.hitangle, &ad.damagemod, &ad.fatigue, &ad.attackskill,
                &ad.weaponmask, &ad.weaponskill, &ad.attackpcnt,
                &ad.swipeframeon, &ad.swipeframeoff);
            if (ok)
            {
                if (ad.responsename[0] != 0)
                    ad.flags = (ad.flags & ~CA_SPECIAL) | CA_RESPONSE;
                lastattack = &attacks[attacks.Add(ad)];
            }
        }
        else if (TAGIS("MAGICATTACK"))
        {
            SCharAttackData ad;
            memset(static_cast<void *>(&ad), 0, sizeof(ad));

            ok = Parse(t, "%30s, %i, %i, %i, %i, %i, %31s, %i, %i, %i, %i, %i",
                ad.attackname, &ad.flags, &ad.button,
                &ad.attackpcnt, &ad.mindist, &ad.maxdist,
                ad.spellname, &ad.spellsource.x, &ad.spellsource.y, &ad.spellsource.z,
                &ad.condition, &ad.conditionvalue);
            if (ok)
            {
                ad.flags |= CA_MAGICATTACK;
                lastattack = &attacks[attacks.Add(ad)];
            }
        }
        else if (TAGIS("FATIGUEATTACK"))
        {
          // An ATTACK with maxfatigue before the swipe frames; no response
          // rule.
            SCharAttackData ad;
            memset(static_cast<void *>(&ad), 0, sizeof(ad));

            ok = Parse(t, "%30s, %i, %i, %30s, %30s, %30s, %30s, "
                "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i",
                ad.attackname, &ad.flags, &ad.button,
                ad.responsename, ad.blockname, ad.missname, ad.chainname,
                &ad.blocktime, &ad.impacttime, &ad.nextwait, &ad.chainexptime,
                &ad.mindist, &ad.maxdist, &ad.hitminrange, &ad.hitmaxrange,
                &ad.hitangle, &ad.damagemod, &ad.fatigue, &ad.attackskill,
                &ad.weaponmask, &ad.weaponskill, &ad.attackpcnt, &ad.maxfatigue,
                &ad.swipeframeon, &ad.swipeframeoff);
            if (ok)
            {
                ad.flags |= CA_FATIGUEATTACK;
                lastattack = &attacks[attacks.Add(ad)];
            }
        }
        else if (TAGIS("PLAYANIM"))
        {
            SCharAttackData ad;
            memset(static_cast<void *>(&ad), 0, sizeof(ad));

            ok = Parse(t, "%30s, %i, %i, %i, %i, %i",
                ad.attackname, &ad.flags, &ad.button,
                &ad.mindist, &ad.maxdist, &ad.attackpcnt);
            if (ok)
            {
                ad.flags |= CA_PLAYANIM;
                lastattack = &attacks[attacks.Add(ad)];
            }
        }
        else if (TAGIS("IMPACT") || TAGIS("CHARIMPACT"))
        {
            const bool charimpact = TAGIS("CHARIMPACT");
            if (charimpact)
            {
                if (numimpacts >= MAXCHARIMPACTS)
                    t.Error("Too many CHARIMPACT tags for this character");
            }
            else
            {
                if (!lastattack)
                    t.Error("ATTACK tag must preceed its IMPACT tags");
                else if (lastattack->flags & CA_MAGICATTACK)
                    t.Error("IMPACT can not follow a MAGICATTACK tag");

                if (lastattack->numimpacts >= MAXATTACKIMPACTS)
                    t.Error("Too many IMPACT tags for this ATTACK");
            }

            SCharAttackImpact ai;
            memset(static_cast<void *>(&ai), 0, sizeof(ai));

            ok = Parse(t, "%30s, %i, %30s, %i, %i, %i, %i, %i",
                ai.impactname, &ai.flags, ai.loopname,
                &ai.looptime, &ai.damagemin, &ai.damagemax, &ai.snapdist, &ai.snaptime);

            if (ai.loopname[0] != 0 && !(ai.flags & (CAI_STUN | CAI_KNOCKDOWN | CAI_DEATH)))
                t.Error("Used 'loop' animation for impact with no STUN, KNOCKDOWN, or DEATH flag");

            if (ok)
            {
                if (charimpact)
                {
                    ai.index = numimpacts;
                    ai.flags |= CAI_CHARIMPACT;
                    impacts[numimpacts++] = ai;
                }
                else
                {
                    ai.index = lastattack->numimpacts;
                    ai.flags &= ~CAI_CHARIMPACT;
                    lastattack->impacts[lastattack->numimpacts++] = ai;
                }
            }
        }
        else if (TAGIS("CLASS"))
        {
          // The class by name, among the CLASS blocks loaded so far (the
          // lookup is GetClass's, inline); a name that isn't one is fatal.
            char classname[RESNAMELEN] = {};
            ok = Parse(t, "%30s", classname);
            if (ok)
                classdata = Rules.GetClass(classname);
            if (!classdata)
                FatalError("Unable to find character class \"%s\"", classname);
        }
        else if (TAGIS("DAMAGEMODS"))
        {
            ReadNumberRow(t, damagemods, NUMDAMAGETYPES, "Damage type modifier expected");
        }
        else if (TAGIS("FLAGS"))
        {
          // REVSYNC: 0x00489ecb passes the flags' value, not their address,
          // so the parse writes through a pointer equal to the current flags.
          // No shipped CHARACTER uses the tag.
            ok = Parse(t, "%i", flags);
        }
        else if (TAGIS("BLOCK"))
            ok = Parse(t, "%i, %i, %i", &blockfreq, &blockmin, &blockmax);
        else if (TAGIS("PLAYERBLOCK"))
            ok = Parse(t, "%i, %i, %i", &playerblockmin, &playerblockstep, &playerblockinc);
        else if (TAGIS("ARMOR"))
            ok = Parse(t, "%i", &armorvalue);
        else if (TAGIS("DEFENSEMOD"))
            ok = Parse(t, "%i", &defensemod);
        else if (TAGIS("ATTACKMOD"))
            ok = Parse(t, "%i", &attackmod);
        else if (TAGIS("ATTACKFREQ"))
            ok = Parse(t, "%i, %i", &minattackfreq, &maxattackfreq);
        else if (TAGIS("MAGICFREQ"))
            ok = Parse(t, "%i, %i", &minmagicfreq, &maxmagicfreq);
        else if (TAGIS("WEAPONTYPE"))
            ok = Parse(t, "%i", &weapontype);
        else if (TAGIS("WEAPONDAMAGE"))
            ok = Parse(t, "%i", &weapondamage);
        else if (TAGIS("BLOCKSOUNDS"))
            ok = Parse(t, "%31s", blocksounds);
        else if (TAGIS("MISSSOUNDS"))
            ok = Parse(t, "%31s", misssounds);
        else if (TAGIS("GROUPS"))
            ok = Parse(t, "%80s", groups);
        else if (TAGIS("ENEMIES"))
            ok = Parse(t, "%80s", enemies);
        else if (TAGIS("SIGHT"))
            ok = Parse(t, "%i, %i, %i, %i", &sightmin, &sightmax, &sightrange, &sightangle);
        else if (TAGIS("HEARING"))
            ok = Parse(t, "%i, %i, %i", &hearingmin, &hearingmax, &hearingrange);
        else if (TAGIS("MANA"))
            ok = Parse(t, "%i", &mana);
        else if (TAGIS("FATIGUE"))
            ok = Parse(t, "%i", &fatigue);
        else if (TAGIS("HEALTH"))
            ok = Parse(t, "%i", &health);
        else if (TAGIS("COMBATRANGE"))
        {
          // One value: the far end is 64 past it (also when the parse fails).
            ok = Parse(t, "%i", &combatrangemin);
            if (ok && t.Is(","))
                ok = Parse(t, ", %i", &combatrangemax);
            else
                combatrangemax = combatrangemin + 64;
        }
        else if (TAGIS("MAXATTACKRANGE"))
        {
            ok = Parse(t, "%i", &maxattackrange);
            if (!ok)
                maxattackrange = 32;
        }
        else if (TAGIS("BLEEDER") || TAGIS("BLEADER"))
        {
            ok = Parse(t, "%i", &bleeder);
            if (!ok)
                flags |= 0x10;  // retail 0x0048a815, then the parse error
        }
        else if (TAGIS("WALKSPEED"))
            ok = Parse(t, "%i", &walkspeed);
        else if (TAGIS("SNEAKSPEED"))
            ok = Parse(t, "%i", &sneakspeed);
        else if (TAGIS("RUNSPEED"))
            ok = Parse(t, "%i", &runspeed);
        else if (TAGIS("COMBATWALKSPEED"))
            ok = Parse(t, "%i", &combatwalkspeed);
        else if (TAGIS("BODYTYPE"))
            ok = Parse(t, "%30s", bodytype);
        else if (TAGIS("SWIPECOLOR"))
            ok = Parse(t, "%b, %b, %b", &swipecolor.red, &swipecolor.green, &swipecolor.blue);
        else if (TAGIS("SWIPEFULL"))
            ok = Parse(t, "%b", &swipefull);
        else if (TAGIS("ATTACHEFFECT"))
        {
          // The first free slot; with all four used the line isn't parsed
          // (and the "Return expected" check below stops the load).
            auto free = std::find_if(std::begin(attacheffects), std::end(attacheffects),
                                     [](const SCharAttachEffect &e) { return !e.used; });
            if (free != std::end(attacheffects))
            {
                ok = Parse(t, "%20s, %i, %i, %i, %20s, %20s", free->name,
                           &free->values[0], &free->values[1], &free->values[2], free->arg5, free->arg6);
                if (!stricmp(free->arg5, "none"))
                    free->arg5[0] = 0;
                if (!stricmp(free->arg6, "none"))
                    free->arg6[0] = 0;
                free->used = true;
            }
        }
        else if (TAGIS("ARROWPOS"))
            ok = Parse(t, "%i, %i, %i", &arrowpos.x, &arrowpos.y, &arrowpos.z);
        else if (TAGIS("ARROWSPEED"))
            ok = Parse(t, "%i", &arrowspeed);
        else if (TAGIS("BOWWAIT"))
            ok = Parse(t, "%i", &bowwait);
        else if (TAGIS("BOWAIMSPEED"))
            ok = Parse(t, "%i", &bowaimspeed);
        else if (TAGIS("RETREATAT"))
            ok = Parse(t, "%i", &retreatat);
        else if (TAGIS("RETREATATMANA"))
            ok = Parse(t, "%i", &retreatatmana);
        else if (TAGIS("RETREATFOR"))
            ok = Parse(t, "%i", &retreatfor);
        else if (TAGIS("RUNFATIGUE"))
            ok = Parse(t, "%i, %i", &runfatigue[0], &runfatigue[1]);
        else if (TAGIS("POISONCHANCE"))
            ok = Parse(t, "%i", &poisonchance);
        else if (TAGIS("NOPARALYZE"))
            noparalyze = true;
        else if (TAGIS("LABELHEIGHT"))
            ok = Parse(t, "%i", &labelheight);
        else
            t.Error("Invalid character tag %s", t.Text());  // retail names the token after the tag

        if (!ok)
            t.Error(errorparsingtag, tag);

        if (t.Type() != TKN_RETURN)
            t.Error("Return expected");

        t.LineGet();
    }

  // The label height and the arrow's start, by character when the block
  // didn't give them (names case-blind).
    if (labelheight == -1)
    {
        if (!stricmp(aname, "bayne"))
            labelheight = 120;
        else if (!stricmp(aname, "navarro"))
            labelheight = 80;
        else
            labelheight = 100;
    }
    if (arrowpos.x == -1)
    {
        if (!stricmp(aname, "bayne"))
            arrowpos = S3DPoint(-5, -15, 80);
        else if (!stricmp(aname, "morganna"))
            arrowpos = S3DPoint(-10, -15, 50);
        else if (!stricmp(aname, "navarro"))
            arrowpos = S3DPoint(-10, -15, 30);
        else
            arrowpos = S3DPoint(-5, -15, 60);
    }

    if (!t.Is("END"))
        t.Error("Char block END expected");
    t.Get();

    return true;
}

// *****************
// * TRules Object *
// *****************

// REVSYNC: TRules::Initialize @ 0x0048b690
// Initializes character data stuff: the defaults rules.def may override,
// then the files.
bool TRules::Initialize()
{
    if (initialized)
        return true;

    chardata.DeleteAll();
    classdata.DeleteAll();
    def = nullptr;
    statlevels = {};
    weapons.clear();
    armors.clear();

    healthrecovrate = fatiguerecovrate = manarecovrate = 1;
    poisondamagerate = 1;
    tohitcenter = 50;
    tohitrangechar = 10;
    tohitrangeplyr = 10;
    tohitblock = 25;
    tohitface = 25;
    ammodata = {20, 6, 4, 1, 25};

    if (!Load())
        return false;

    initialized = true;

  // REVSYNC-DIVERGENCE: 0x0048b690 doesn't bind; retail binds at the end of
  // LoadClasses (0x0047654f), which runs after Initialize (0x004861cb, then
  // 0x00486202). The port loads the classes first, so it binds here too.
  // Binding is idempotent: whichever of the two loads comes second binds.
    BindTypes();

    return true;
}

// Closes the area manager (idempotent — second call is a no-op).
void TRules::Close()
{
    if (!initialized)
        return;
    chardata.DeleteAll();
    classdata.DeleteAll();
    def = nullptr;
    statlevels = {};
    weapons.clear();
    armors.clear();
    initialized = false;
}

// REVSYNC: TRules::Load @ 0x0048b990
// Loads the game rules: six files through the same tag loop, so any of them
// may hold any block. rules.def from ClassDefPath (required); stats.def from
// ClassDefPath when it's there ("no longer used"); char.def, weapon.def,
// armor.def and equip.def from ImageryPath when it has them (imagery.rvi
// does), else ClassDefPath, else not at all. No install ships equip.def.
bool TRules::Load()
{
    chardata.DeleteAll();
    classdata.DeleteAll();
    weapons.clear();
    armors.clear();
  // REVSYNC-DIVERGENCE: retail leaves `def` pointing at the freed "Default"
  // until the files give a new one.
    def = nullptr;

    struct SRulesFile
    {
        const char *name;
        bool imagery;           // ImageryPath first
    };
    static constexpr SRulesFile kFiles[] = {
        {"rules.def", false}, {"stats.def", false}, {"char.def", true},
        {"weapon.def", true}, {"armor.def", true}, {"equip.def", true},
    };
    for (const SRulesFile &f : kFiles)
    {
        const std::string file = f.imagery ? rev_first_existing(ImageryPath, ClassDefPath, f.name)
                                           : std::string(ClassDefPath) + f.name;
      // rules.def is required (LoadFile stops on a missing one); the rest are read when they're there.
        if (&f != &kFiles[0] && !rev_file_exists(file.c_str()))
            continue;
        if (!LoadFile(file.c_str()))
            return false;
    }

    const auto with_statline = [](const std::vector<SItemData> &items)
        { return std::count_if(items.begin(), items.end(), [](const SItemData &d) { return !d.statline.empty(); }); };
    log_info("[rules] %d classes, %d characters, %d weapons and %d armor (%d with STATLINE); "
             "per level H/F/M %d/%d/%d",
             (int)classdata.NumItems(), (int)chardata.NumItems(), (int)weapons.size(), (int)armors.size(),
             (int)(with_statline(weapons) + with_statline(armors)),
             (int)healthperlevel, (int)fatigueperlevel, (int)manaperlevel);
    return true;
}

// One rules file through the tag loop of 0x0048b990. A block's name is
// parsed with a bounded %Ns where retail's `%s\n` copies it whole.
// REVSYNC-DIVERGENCE: retail's name buffers are 64 bytes (32 for CLASS) on
// its stack; a longer name overran them. The port truncates it instead.
bool TRules::LoadFile(const char* fname)
{
    FILE *fp = rev_fopen(fname, "rb");
    if (!fp)
        FatalError("Unable to find file %s", fname);    // TToken::Open 0x004789c0

    TFileParseStream s(fp, fname);
    TToken t(s);

    if (!t.DefineGet())
        t.Error("Syntax error in header");

    while (t.Type() != TKN_EOF)
    {
        if (t.Type() != TKN_IDENT)
            t.Error("Rules block name or tag expected");

        char tag[40];
        strncpyz(tag, t.Text(), 40);

        t.WhiteGet();
        bool ok = true;

        if (TAGIS("DAYLENGTH"))
            ok = Parse(t, "%i", &daylength);
        else if (TAGIS("TWILIGHT"))
        {
            ok = Parse(t, "%i, %i", &twilight, &twilightsteps);
            if (GameSpeed == 5)
                twilightsteps = ConvertMinutesToFrames(twilight); // Smooth ambient fading
        }
        else if (TAGIS("STEALTH"))
            ok = Parse(t, "%i, %i, %i", &maxstealth, &sneakstealth, &minstealth);
        else if (TAGIS("CHARACTER"))
        {
            char charname[MAXNAMELEN];
            ok = Parse(t, "%64s\n", charname);
            if (ok)
            {
                for (int32_t c = 0; c < chardata.NumItems(); c++)
                {
                    if (chardata.Used(c) && !stricmp(chardata[c]->name, charname))
                        t.Error("More than one %s in CHAR.DEF file", charname);
                }

                PSCharData data = new SCharData;
                if (!data->Load(charname, t))
                    t.Error("Error loading char data");
                chardata.Add(data);

              // The default char data object ("Default": objtype -1)
                if (data->objtype == -1)
                    def = data;

                t.Get();
            }
        }
        else if (TAGIS("CLASS"))
        {
            char classname[RESNAMELEN];
            ok = Parse(t, "%32s\n", classname);
            if (ok)
            {
                PSClassData cl = new SClassData;
                if (!cl->Load(classname, t))
                    t.Error("Error loading class data");
                classdata.Add(cl);
                t.Get();
            }
        }
        else if (TAGIS("WEAPON") || TAGIS("ARMOR"))
        {
          // One WEAPON.DEF / ARMOR.DEF entry. After a weapon block retail gets
          // the next token, after an armor block the next non-blank one.
            const bool weapon = TAGIS("WEAPON");
            char itemname[MAXNAMELEN];
            ok = Parse(t, "%64s\n", itemname);
            if (ok)
            {
                std::vector<SItemData> &items = weapon ? weapons : armors;
                for (const SItemData &d : items)
                {
                    if (!stricmp(d.name.c_str(), itemname))
                        t.Error(weapon ? "More than one %s in WEAPON.DEF file" : "More than one %s in ARMOR.DEF file",
                                itemname);
                }

                SItemData item;
                if (!item.Load(itemname, t))
                    t.Error(weapon ? "Error loading weapon data" : "Error loading armor data");
                items.push_back(std::move(item));

                if (weapon)
                    t.Get();
                else
                    t.WhiteGet();
            }
        }
        else if (TAGIS("HEALTHDATA"))
            ok = Parse(t, "%i, %i, %i", &healthperlevel, &healthrecovval, &healthrecovrate);
        else if (TAGIS("FATIGUEDATA"))
            ok = Parse(t, "%i, %i, %i", &fatigueperlevel, &fatiguerecovval, &fatiguerecovrate);
        else if (TAGIS("MANADATA"))
            ok = Parse(t, "%i, %i, %i", &manaperlevel, &manarecovval, &manarecovrate);
        else if (TAGIS("POISONDATA"))
            ok = Parse(t, "%i, %i", &poisondamageval, &poisondamagerate);
        else if (TAGIS("AMMODATA"))
        {
          // Four values; the fifth keeps its default.
            ok = Parse(t, "%i, %i, %i, %i", &ammodata[0], &ammodata[1], &ammodata[2], &ammodata[3]);
        }
        else if (TAGIS("TOHITCENTER"))
            ok = Parse(t, "%i", &tohitcenter);
        else if (TAGIS("TOHITRANGECHAR"))
            ok = Parse(t, "%i", &tohitrangechar);
        else if (TAGIS("TOHITRANGEPLYR"))
            ok = Parse(t, "%i", &tohitrangeplyr);
        else if (TAGIS("TOHITBLOCK"))
            ok = Parse(t, "%i", &tohitblock);
        else if (TAGIS("TOHITFACE"))
            ok = Parse(t, "%i", &tohitface);
        else if (TAGIS("TOHITDAMAGE"))
        {
          // A BEGIN/END block of exactly five `ENTRY <MinValue> <DamagePercent>`
          // lines: fewer is a parse error; the tag's result is the last
          // entry's.
            t.WhiteGet();
            t.DoBegin();
            for (int32_t i = 0; i < kToHitDamageRows; i++)
            {
                if (!t.Is("ENTRY"))
                {
                    ok = false;
                    break;
                }
                t.Get();
                t.WhiteGet();
                int32_t minvalue = 0, damagepercent = 0;
                ok = Parse(t, "%i %i", &minvalue, &damagepercent);
                if (ok)
                    tohitdamage[i] = {minvalue, damagepercent};
                t.WhiteGet();
            }
            t.Get();
            t.WhiteGet();
        }
        else if (TAGIS("STATLEVEL"))
        {
          // REVSYNC: 0x0048b990 -> 0x0049c9f0
            char tablename[RESNAMELEN];
            ok = Parse(t, "%32s\n", tablename);
            if (ok)
            {
                const int32_t table = PlayerStats::TStatLevels::FindTable(tablename);
                if (table < 0)
                    t.Error("Invalid Stat Level Type in Rules.def");
                if (!statlevels.Parse(t, table))
                    t.Error("Error loading statlevel data");
                t.WhiteGet();           // retail 0x00479580
            }
        }
        else
            t.Error("Invalid block or tag %s", t.Text());  // retail names the token after the tag

        if (!ok)
            t.Error(errorparsingtag, tag);

        if (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
            t.Error("Return expected");

        if (!t.DefineGet())
            t.Error("Rules file syntax error");
    }

    fclose(fp);

    return true;
}

// REVSYNC: TRules::BindTypes @ 0x0048cab0
// Every type of every object class, by name: a CHARACTER whose name is a
// player or character type takes that type (BindType 0x0048c930). A name
// that is no type keeps -2 and is never returned by GetCharData.
void TRules::BindTypes()
{
    for (int32_t c = 0; c < TObjectClass::NumClasses(); c++)
    {
        const TObjectClass *cl = TObjectClass::GetClass(c);
        if (!cl)
            continue;
        for (int32_t type = 0; type < cl->NumTypes(); type++)
        {
            if (const SObjectInfo *info = cl->GetObjType(type))
                BindType(info->name, type);
        }
    }
}

// REVSYNC: TRules::BindType @ 0x0048c930
// The class is the first one (by id) with a type of that name, whichever
// class the type index came from. Weapons and armor bind too in retail
// (+0xc4/+0xc8, +0xc0/+0xc4 of their records, and 0x0048c7f0 copies their
// numbers into the class); the port finds those entries by name instead
// (GetItemData).
void TRules::BindType(const char *name, int32_t objtype)
{
    int32_t objclass = -1;
    for (int32_t c = 0; c < TObjectClass::NumClasses(); c++)
    {
        const TObjectClass *cl = TObjectClass::GetClass(c);
        if (cl && cl->FindObjType(name) >= 0)
        {
            objclass = cl->ClassId();
            break;
        }
    }
    if (objclass != OBJCLASS_PLAYER && objclass != OBJCLASS_CHARACTER)
        return;

    for (int32_t c = 0; c < chardata.NumItems(); c++)
    {
        if (chardata.Used(c) && !stricmp(chardata[c]->name, name))
        {
            chardata[c]->objtype = objtype;
            chardata[c]->objclass = objclass;
            return;
        }
    }
}

PSClassData TRules::GetClass(char *name)
{
    for (int32_t c = 0; c < classdata.NumItems(); c++)
    {
        if (!stricmp(classdata[c]->name, name))
            return classdata[c];
    }

    return nullptr;
}

PSCharData TRules::GetCharData(int32_t objtype, int32_t objclass)
{
    for (int32_t c = 0; c < chardata.NumItems(); c++)
    {
        if (chardata[c]->objtype == objtype && chardata[c]->objclass == objclass)
            return chardata[c];
    }

    if (def)
        return def;

    // Fall back to a default-constructed SCharData so the 76+
    // dereference sites in character.cpp don't need null guards.
    // Field initializers on SCharData / SClassData (rules.h) zero-
    // init every member; we wire ->classdata to a static SClassData
    // so SkillPcnt-style chained accesses also resolve. Real
    // entries should come back when rules.def gets per-class CHAR
    // data and Initialize() loads them.
    static SClassData s_fallback_class;
    static SCharData  s_fallback = []{
        SCharData d;
        d.classdata = &s_fallback_class;
        // SCharData ctor sets walk/run/sneak/combat speeds to -1 as a
        // "uninitialized; will be set from rules.def" sentinel. Without
        // rules.def CHAR entries every gated `chardata->walkspeed > 0`
        // check in TCharacter::Move fails and the player can't translate.
        // Wire reasonable defaults so the fallback alone is enough to
        // walk -- real per-class entries will override these when
        // rules.def CHAR data comes back.
        d.walkspeed        = 6;
        d.runspeed         = 12;
        d.sneakspeed       = 3;
        d.combatwalkspeed  = 4;
        return d;
    }();
    return &s_fallback;
}

// REVSYNC: 0x0048cc90 -- experience a skill needs to reach 'level'; 0 for
// level 0, the level-30 figure past 30. TRules::Initialize (0x0048b690)
// builds the table (Rules +0x168) from a formula, not from rules.def: 300
// for level 1, then 100 * i + 300 more for each level after.
int32_t TRules::SkillExpForLevel(int32_t level) const
{
    static constexpr std::array<int32_t, kMaxSkillLevel> kSkillExp = [] {
        std::array<int32_t, kMaxSkillLevel> table{};
        table[0] = 300;
        for (int32_t i = 1; i < kMaxSkillLevel; i++)
            table[i] = table[i - 1] + 100 * i + 300;
        return table;
    }();

    if (level == 0)
        return 0;
    return kSkillExp[std::clamp(level - 1, 0, kMaxSkillLevel - 1)];   // retail doesn't guard < 0
}

// REVSYNC: 0x0048cc40 -- experience a player needs to reach 'level'; 0 for
// level 0, the level-30 figure past 30. TRules::Initialize (0x0048b690)
// builds the table (Rules +0xf0): 0 for level 1, 300 for level 2, then
// (5 * i + 10) * 20 more for each level after.
int32_t TRules::ExpForLevel(int32_t level) const
{
    static constexpr std::array<int32_t, kMaxPlayerLevel> kExp = [] {
        std::array<int32_t, kMaxPlayerLevel> table{};
        table[1] = 300;
        for (int32_t i = 2; i < kMaxPlayerLevel; i++)
            table[i] = table[i - 1] + (i * 5 + 10) * 20;
        return table;
    }();

    if (level == 0)
        return 0;
    return kExp[std::clamp(level - 1, 0, kMaxPlayerLevel - 1)];   // retail doesn't guard < 0
}

// REVSYNC: 0x0048cb50 -- the WEAPON.DEF / ARMOR.DEF entry of an object type,
// by class and type. Entries are bound to types by name (0x0048c930).
const SItemData *TRules::GetItemData(int32_t objclass, const char *type) const
{
    const std::vector<SItemData> *items = nullptr;
    if (objclass == OBJCLASS_WEAPON)
        items = &weapons;
    else if (objclass == OBJCLASS_ARMOR)
        items = &armors;
    if (!items || !type)
        return nullptr;

    for (const SItemData &item : *items)
    {
        if (stricmp(item.name.c_str(), type) == 0)
            return &item;
    }
    return nullptr;
}
