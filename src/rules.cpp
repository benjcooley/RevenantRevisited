// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  Rules.cpp - TRules object module                     *
// *************************************************************************

#include <algorithm>
#include <array>

#include "revenant.h"
#include "parse.h"
#include "character.h"
#include "player.h"
#include "weapon.h"
#include "playscreen.h"
#include "rules.h"
#include "revutils.h"
#include "logging.h"

extern TObjectClass CharacterClass;
extern TObjectClass PlayerClass;

static char errorparsingtag[] = "Error parsing tag %s";

// Tag is tester
#define TAGIS(t) (!stricmp(tag, t))

// *********************
// * SClassData Object *
// *********************

// Loads the character from the "CHAR.DEF" file
bool SClassData::Load(char *aname, TToken &t)
{
    strncpyz(name, aname, RESNAMELEN);

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Char block BEGIN expected");
    
  // Now get first trigger token
    t.LineGet();
    
  // Iterate through the triggers and setup trigger list
    bool ok;
    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
      // Parse trigger tags now
        if (t.Type() != TKN_IDENT)
            t.Error("Class data keyword expected");

        char tag[40];
        strncpyz(tag, t.Text(), 40);

        t.WhiteGet();
        ok = true;

      // Tags...
        if (TAGIS("STATREQS"))
        {
            // REVSYNC: 0x004891c0 reads the six values in file order, and
            // ClearPlayer / SetPlayerLevel apply value i to attribute
            // PLRSTAT_FIRST + i (Strn Cons Agil Rflx Mind Luck). rules.def's
            // comment row says "str,con,agl,rflx,luck,mind"; the shipped
            // code gives the fifth value to Mind and the sixth to Luck.
            ok = Parse(t, "%i, %i, %i, %i, %i, %i",
                       &statreqs[0], &statreqs[1], &statreqs[2],
                       &statreqs[3], &statreqs[4], &statreqs[5]);
            static_assert(NUM_PLRSTATS == 6, "STATREQS row width changed");
        }
        else if (TAGIS("SKILLMODS"))
        {
            ok = Parse(t, "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i",
                       &skillmods[0], &skillmods[1], &skillmods[2],
                       &skillmods[3], &skillmods[4], &skillmods[5],
                       &skillmods[6], &skillmods[7], &skillmods[8],
                       &skillmods[9], &skillmods[10]);
            static_assert(NUM_SKILLS == 11, "SKILLMODS row width changed");
        }
        else if (TAGIS("HEALTHMOD"))
        {
            ok = Parse(t, "%i", &healthmod);
        }
        else if (TAGIS("FATIGUEMOD"))
        {
            ok = Parse(t, "%i", &fatiguemod);
        }
        else if (TAGIS("MANAMOD"))
        {
            ok = Parse(t, "%i", &manamod);
        }
        else
        {
            // Retail added per-class tags (WM_*, etc.) that didn't exist
            // in the pre-release source. Skip the line.
            log_warn("[rules] skipping unknown class tag '%s'", tag);
            while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
                t.Get();
            t.LineGet();
            continue;
        }

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

// Construct SCharData
SCharData::SCharData()
{
    objtype = -1;
    objclass = OBJCLASS_CHARACTER;

    flags = 0;
    for (int32_t c = 0; c < NUMDAMAGETYPES; c++)
        damagemods[0] = 0;

    health = 25;
    fatigue = 25;
    mana = 0;

    blockfreq = 10;
    blockmin = 5;
    blockmax = 15;

    combatrangemin = 128;
    combatrangemax = combatrangemin + 64;
    
    playerblockmin = 10;
    playerblockstep = 5;
    playerblockinc = 10;

    strcpy(blocksounds, "clang1,clang2,clang3");
    strcpy(misssounds, "");

    weapontype = WT_HAND;
    weapondamage = 2;
    armorvalue = 1;
    defensemod = 0;
    attackmod = 0;

    sightmin = 30; sightmax = 100; sightrange = 64 * 5; sightangle = 64;
    hearingmin = 10; hearingmax = 50; hearingrange = 64 * 5;

    minattackfreq = 100; maxattackfreq = 250;

    walkspeed = -1; runspeed = -1; sneakspeed = -1; combatwalkspeed = -1; 

    arrowpos.x = 15; arrowpos.y = 15; arrowpos.z = 60;  // In front of by 15, and up at 45
    arrowspeed = 20;                       // 20 units per tick for arrow speed

    bowwait = 12;                          // Half a second
    bowaimspeed = 8;                       // Pivot speed when aiming bow

    maxattackrange = 32;

    strcpy(bodytype, "normal"); // Default male normal size
}

// Destroy SCharData
SCharData::~SCharData()
{
    attacks.Clear();
}

// Loads the character from the "CHAR.DEF" file
bool SCharData::Load(char *aname, TToken &t)
{
    strncpyz(name, aname, RESNAMELEN);

    if (!stricmp(name, "Default"))
    {
        objtype = -1; objclass = -1;
    }
    else
    {
        objtype = CharacterClass.FindObjType(name);
        if (objtype < 0)
        {
            objtype = PlayerClass.FindObjType(name);
            if (objtype < 0)
                t.Error("Invalid character type %s", name);
            else
                objclass = OBJCLASS_PLAYER;
        }
        else
            objclass = OBJCLASS_CHARACTER;
    }

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Char block BEGIN expected");
    
  // Now get first trigger token
    t.LineGet();
    
  // Iterate through the triggers and setup trigger list
    bool ok;
    PSCharAttackData lastattack = nullptr;
    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
      // Parse trigger tags now
        if (t.Type() != TKN_IDENT)
            t.Error("Char data keyword expected");

        char tag[40];
        strncpyz(tag, t.Text(), 40);

        t.WhiteGet();
        ok = true;

      // Tags...
      // FATIGUEATTACK is a retail variant of ATTACK with the same field
      // layout — just used by the AI to choose lower-impact moves when
      // the character is tired. We don't have a dedicated CA_FATIGUE flag
      // (that's a Demo-2+ concern); treat it as a plain ATTACK for now so
      // the parse advances and the trailing IMPACT attaches correctly.
        if (TAGIS("ATTACK") || TAGIS("FATIGUEATTACK"))
        {
            SCharAttackData ad;

            memset(&ad, 0, sizeof(SCharAttackData));

          // Pre-release ATTACK ends at attackpcnt (22 fields). Retail char.def
          // appends swipeframeon, swipeframeoff (24 fields) and FATIGUEATTACK
          // adds one more trailing int (25 fields, likely fatigue cost).
          // Parse the 22-field base, then consume any trailing tokens up
          // to end-of-line so the outer "Return expected" check doesn't
          // trip on the retail extras.
            ok = Parse(t, "%30s, %i, %i, %30s, %30s, %30s, %30s, "
                "%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i",
                ad.attackname, &ad.flags, &ad.button,
                ad.responsename, ad.blockname, ad.missname, ad.chainname,
                &ad.blocktime, &ad.impacttime, &ad.nextwait, &ad.chainexptime,
                &ad.mindist, &ad.maxdist, &ad.hitminrange, &ad.hitmaxrange,
                &ad.hitangle, &ad.damagemod, &ad.fatigue, &ad.attackskill,
                &ad.weaponmask, &ad.weaponskill, &ad.attackpcnt);
            if (ok)
            {
              // Eat any trailing fields the retail format added.
                while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
                    t.Get();
            }

            if (ok)
            {
                if (ad.responsename[0] != 0)
                    ad.flags = (ad.flags & ~CA_SPECIAL) | CA_RESPONSE;

                int32_t num = attacks.Add(ad);
                lastattack = &(attacks[num]);
            }
        }
        else if (TAGIS("MAGICATTACK"))
        {
            SCharAttackData ad;

            memset(&ad, 0, sizeof(SCharAttackData));

            ok = Parse(t, "%30s, %i, %i, %i, %i, %i, %31s, %i, %i, %i",
                ad.attackname, &ad.flags, &ad.button,
                &ad.attackpcnt, &ad.mindist, &ad.maxdist,
                ad.spellname, &ad.spellsource.x, &ad.spellsource.y, &ad.spellsource.z);

            if (ok)
            {
              // Retail MAGICATTACK adds two trailing fields (stat requirement,
              // stat value). Discard them — Demo 1 doesn't gate magic on stats.
                while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
                    t.Get();

                ad.flags |= CA_MAGICATTACK;

                int32_t num = attacks.Add(ad);
                lastattack = &(attacks[num]);
            }
        }
        else if (TAGIS("PLAYANIM"))
        {
            SCharAttackData ad;

            memset(&ad, 0, sizeof(SCharAttackData));

            ok = Parse(t, "%30s, %i, %i, %i, %i, %i",
                ad.attackname, &ad.flags, &ad.button,        
                &ad.mindist, &ad.maxdist, &ad.attackpcnt);

            if (ok)
            {
                ad.flags |= CA_PLAYANIM;

                int32_t num = attacks.Add(ad);
                lastattack = &(attacks[num]);
            }
        }
        else if (TAGIS("IMPACT") || TAGIS("CHARIMPACT"))
        {
            if (TAGIS("CHARIMPACT"))
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

            memset(&ai, 0, sizeof(SCharAttackImpact));

            ok = Parse(t, "%30s, %i, %30s, %i, %i, %i, %i, %i",
                ai.impactname, &ai.flags, ai.loopname, 
                &ai.looptime, &ai.damagemin, &ai.damagemax, &ai.snapdist, &ai.snaptime);

            if (ai.loopname[0] != 0)
            {
                if (!(ai.flags & (CAI_STUN | CAI_KNOCKDOWN | CAI_DEATH)))
                    t.Error("Used 'loop' animation for impact with no STUN, KNOCKDOWN, or DEATH flag");
            }

            if (ok)
            {
                if (TAGIS("CHARIMPACT"))
                {
                    memcpy(&(impacts[numimpacts]), &ai, 
                        sizeof(SCharAttackImpact));
                    numimpacts++;
                }
                else
                {
                    memcpy(&(lastattack->impacts[lastattack->numimpacts]), &ai, 
                        sizeof(SCharAttackImpact));
                    lastattack->numimpacts++;
                }
            }
        }
        else if (TAGIS("CLASS"))
        {
            if (objclass == OBJCLASS_CHARACTER)
                t.Error("Monsters/NPC's do not have character classes");

            char classname[RESNAMELEN];
            ok = Parse(t, "%30s", classname);
            if (ok)
                classdata = Rules.GetClass(classname);
        }
        else if (TAGIS("DAMAGEMODS"))
        {
            for (int32_t c = 0; c < NUMDAMAGETYPES; c++)
            {
                if (c > 0)
                {
                    if (!t.Is(","))
                        t.Error("',' Expected");

                    t.WhiteGet();
                }
            
                if (t.Type() != TKN_NUMBER)
                    t.Error("Damage type modifier expected");

                damagemods[c] = t.Index();

                t.WhiteGet();
            }
        }
        else if (TAGIS("FLAGS"))
        {
            ok = Parse(t, "%i", flags);
        }
        else if (TAGIS("BLOCK"))
        {
            ok = Parse(t, "%i, %i, %i", &blockfreq, &blockmin, &blockmax);
        }
        else if (TAGIS("PLAYERBLOCK"))
        {
            ok = Parse(t, "%i, %i, %i", &playerblockmin, &playerblockstep, &playerblockinc);
        }
        else if (TAGIS("ARMOR"))
        {
            ok = Parse(t, "%i", &armorvalue);
        }
        else if (TAGIS("DEFENSEMOD"))
        {
            ok = Parse(t, "%i", &defensemod);
        }
        else if (TAGIS("ATTACKMOD"))
        {
            ok = Parse(t, "%i", &attackmod);
        }
        else if (TAGIS("ATTACKFREQ"))
        {
            ok = Parse(t, "%i, %i", &minattackfreq, &maxattackfreq);
        }
        else if (TAGIS("WEAPONTYPE"))
        {
            ok = Parse(t, "%i", &weapontype);
        }
        else if (TAGIS("WEAPONDAMAGE"))
        {
            ok = Parse(t, "%i", &weapondamage);
        }
        else if (TAGIS("BLOCKSOUNDS"))
        {
            ok = Parse(t, "%31s", blocksounds);
        }
        else if (TAGIS("MISSSOUNDS"))
        {
            ok = Parse(t, "%31s", misssounds);
        }
        else if (TAGIS("GROUPS"))
        {
            ok = Parse(t, "%48s", groups);
        }
        else if (TAGIS("ENEMIES"))
        {
            ok = Parse(t, "%48s", enemies);
        }
        else if (TAGIS("SIGHT"))
        {
            ok = Parse(t, "%i, %i, %i, %i", &sightmin, &sightmax, &sightrange, &sightangle);
        }
        else if (TAGIS("HEARING"))
        {
            ok = Parse(t, "%i, %i, %i", &hearingmin, &hearingmax, &hearingrange);
        }
        else if (TAGIS("MANA"))
        {
            ok = Parse(t, "%i", &mana);
        }
        else if (TAGIS("FATIGUE"))
        {
            ok = Parse(t, "%i", &fatigue);
        }
        else if (TAGIS("HEALTH"))
        {
            ok = Parse(t, "%i", &health);
        }
        else if (TAGIS("COMBATRANGE"))
        {
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
        else if (TAGIS("WALKSPEED"))
        {
            ok = Parse(t, "%i", &walkspeed);
        }
        else if (TAGIS("SNEAKSPEED"))
        {
            ok = Parse(t, "%i", &sneakspeed);
        }
        else if (TAGIS("RUNSPEED"))
        {
            ok = Parse(t, "%i", &runspeed);
        }
        else if (TAGIS("COMBATWALKSPEED"))
        {
            ok = Parse(t, "%i", &combatwalkspeed);
        }
        else if (TAGIS("BODYTYPE"))
        {
            ok = Parse(t, "%30s", bodytype);
        }
        else if (TAGIS("SWIPECOLOR"))
        {
            ok = Parse(t, "%b, %b, %b", &swipecolor.red, &swipecolor.green, &swipecolor.blue);
        }
        else if (TAGIS("ARROWPOS"))
        {
            ok = Parse(t, "%i, %i, %i", &arrowpos.x, &arrowpos.y, &arrowpos.z);
        }
        else if (TAGIS("ARROWSPEED"))
        {
            ok = Parse(t, "%i", &arrowspeed);
        }
        else if (TAGIS("BOWWAIT"))
        {
            ok = Parse(t, "%i", &bowwait);
        }
        else if (TAGIS("BOWAIMSPEED"))
        {
            ok = Parse(t, "%i", &bowaimspeed);
        }
        else
        {
          // Retail char.def added per-character tags the pre-release source
          // doesn't recognize (MAGICFREQ, RUNFATIGUE, ATTRACTABLEWITH, ...).
          // Skip the payload through end-of-line (and a BEGIN/END block if
          // present) so the load doesn't fatal-error on Locke at line 119.
            fprintf(stderr, "[rules] skipping unknown char tag '%s'\n", tag);
            while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
                t.Get();
            t.LineGet();
            if (t.Type() == TKN_KEYWORD && t.Code() == KEY_BEGIN)
            {
                t.SkipBlock();
                t.LineGet();
            }
            continue;
        }

        if (!ok)
            t.Error(errorparsingtag, tag);

        if (t.Type() != TKN_RETURN)
            t.Error("Return expected");

        t.LineGet();
    }

    if (objclass == OBJCLASS_PLAYER && !classdata)
        t.Error("Class required for player characters");

    if (!t.Is("END"))
        t.Error("Char block END expected");
    t.Get();

    return true;
}

// *****************
// * TRules Object *
// *****************

// Initializes character data stuff
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

    if (!Load())
        return false;

    initialized = true;

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

// Loads the game rules from "RULES.DEF" plus the character roster in
// "CHAR.DEF". rules.def is required (global rules tags); char.def is optional
// but holds the 60-character retail roster (Araknid, Issathi, Druhgs, Golems,
// etc.). When both files define the same CHARACTER name, the later load
// replaces the earlier one, so char.def wins (retail-authoritative).
// REVSYNC: Load @ 0x0048b990 — rules.def from ClassDefPath; char.def,
// weapon.def and armor.def from ImageryPath when it has them (imagery.rvi
// does), else ClassDefPath. All go through the same tag loop. Retail also
// tries equip.def, which no install ships, and stats.def ("no longer used").
bool TRules::Load()
{
    const std::string rules_file = std::string(ClassDefPath) + "rules.def";
    if (!LoadFile(rules_file.c_str(), /*required=*/true))
        return false;

    for (const char *name : { "char.def", "weapon.def", "armor.def" })
    {
        const std::string file = rev_first_existing(ImageryPath, ClassDefPath, name);
        if (rev_file_exists(file.c_str()))
            LoadFile(file.c_str(), /*required=*/false);
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

bool TRules::LoadFile(const char* fname, bool required)
{
    FILE *fp = rev_fopen(fname, "rb");
    if (!fp)
    {
        if (required)
            FatalError("Unable to find character info file %s", fname);
        return false;
    }

    TFileParseStream s(fp, fname);
    TToken t(s);

    if (!t.DefineGet())
        t.Error("Syntax error in header");

  // Do block loop
    bool ok;
    while (t.Type() != TKN_EOF)
    {
      // Parse trigger tags now
        if (t.Type() != TKN_IDENT)
            t.Error("Rules block name or tag expected");

        char tag[40];
        strncpyz(tag, t.Text(), 40);

        t.WhiteGet();
        ok = true;

      // Tags...
        if (TAGIS("DAYLENGTH"))
        {
            ok = Parse(t, "%i", &daylength);
        }
        else if (TAGIS("TWILIGHT"))
        {
            ok = Parse(t, "%i, %i", &twilight, &twilightsteps);
            if (GameSpeed == 5)
                twilightsteps = ConvertMinutesToFrames(twilight); // Smooth ambient fading
        }
        else if (TAGIS("HEALTHDATA"))
        {
            ok = Parse(t, "%i, %i, %i", &healthperlevel, &healthrecovval, &healthrecovrate);
        }
        else if (TAGIS("FATIGUEDATA"))
        {
            ok = Parse(t, "%i, %i, %i", &fatigueperlevel, &fatiguerecovval, &fatiguerecovrate);
        }
        else if (TAGIS("MANADATA"))
        {
            ok = Parse(t, "%i, %i, %i", &manaperlevel, &manarecovval, &manarecovrate);
        }
        else if (TAGIS("POISONDATA"))
        {
            ok = Parse(t, "%i, %i", &poisondamageval, &poisondamagerate);
        }
        else if (TAGIS("STEALTH"))
        {
            ok = Parse(t, "%i, %i, %i", &maxstealth, &sneakstealth, &minstealth);
        }
        else if (TAGIS("CHARACTER"))
        {

            char charname[MAXNAMELEN];
            ok = Parse(t, "%s\n", charname);
            
            if (ok)
            {
                PSCharData data = new SCharData;

              // If the same CHARACTER name was already loaded (e.g. rules.def
              // had it and char.def has it too), the later definition wins.
              // Drop the earlier slot before adding the new one.
                int32_t existing = -1;
                for (int32_t c = 0; c < chardata.NumItems(); c++)
                {
                    if (chardata.Used(c) && !stricmp(chardata[c]->name, charname))
                    {
                        existing = c;
                        break;
                    }
                }

                if (!data->Load(charname, t))
                    t.Error("Error loading char data");

                if (existing >= 0)
                    chardata.Delete(existing);
                chardata.Add(data);

              // Set default char data object (-1 for objtype)
                if (data->objtype < 0)
                    def = data;

                t.Get();
            }
        }
        else if (TAGIS("CLASS"))
        {
            char classname[RESNAMELEN];
            ok = Parse(t, "%s\n", classname);

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
          // REVSYNC: 0x0048b990 -- one WEAPON.DEF / ARMOR.DEF entry. Retail
          // stops on a name given twice; the port keeps the later entry.
            const bool weapon = TAGIS("WEAPON");
            char itemname[MAXNAMELEN];
            ok = Parse(t, "%64s\n", itemname);

            if (ok)
            {
                SItemData item;
                if (!item.Load(itemname, t))
                    t.Error(weapon ? "Error loading weapon data" : "Error loading armor data");

                std::vector<SItemData> &items = weapon ? weapons : armors;
                const auto same = std::find_if(items.begin(), items.end(),
                    [&](const SItemData &d) { return stricmp(d.name.c_str(), itemname) == 0; });
                if (same != items.end())
                    *same = std::move(item);
                else
                    items.push_back(std::move(item));

                t.WhiteGet();           // retail 0x00479580: a blank after END is allowed
            }
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
        {
            // Retail added rules tags (TOHIT*, AMMODATA, etc.) that didn't
            // exist in the pre-release source. Skip the tag's payload. If
            // the tag introduces a BEGIN/END block, skip the whole block;
            // otherwise just skip to end-of-line. Leave current token at the
            // trailing RETURN so the "Return expected" check below passes.
            log_warn("[rules] skipping unknown tag '%s'", tag);
            while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
                t.Get();
            t.LineGet();
            if (t.Type() == TKN_KEYWORD && t.Code() == KEY_BEGIN)
            {
                t.SkipBlock();
                t.LineGet();
            }
            continue;
        }

        if (!ok)
            t.Error(errorparsingtag, tag);

        if (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
            t.Error("Return expected");
            
        if (!t.DefineGet())
            t.Error("RULES.DEF syntax error");
    }
    
    fclose(fp);

    return true;
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
