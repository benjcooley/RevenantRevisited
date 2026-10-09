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
#include "effect.h"
#include "imagery.h"
#include "mappane.h"
#include "parse.h"
#include "statusbar.h"


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

// TSpell Constructor
TSpell::TSpell(TObjectInstance* invoke, 
    TObjectInstance* *targ, int32_t numtargs, S3DPoint* sourcepos, 
    PSSpellData dat, PSSpellVariant var, PTSpell mtr)
{ 
    invoker = invoke;
    invoker_ref = invoke;
    effect.Clear();
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
    // Retail 0x53f10b..143 procs only with an explicit target array. The
    // inclusive roll and strict comparison intentionally preserve chance100.
    if (targ && dat && dat->poisonchance != 0)
        for (int32_t i = 0; i < targetnum; ++i)
        {
            const int32_t roll = random(0, 100);
            if (roll < dat->poisonchance)
                if (auto* character = dynamic_cast<TCharacter*>(targets[i]))
                    character->SetPoisoned(true);
        }
    variant = var; 
    timer = -1; 
    master = mtr;
    wait = dat->effectstart;

    if (sourcepos)
        source = *sourcepos;
    else
        source.x = source.y = source.z = -1;    // Means not used
}

TSpell::~TSpell()
{
    // The manager owns this spell. The effect can outlive its invoker or
    // finish first; resolve its generation-checked identity before use.
    if (auto* live_effect = dynamic_cast<TEffect*>(effect.Get()))
        if (live_effect->GetSpell() == this)
            live_effect->SetSpell(nullptr);
    effect.Clear();
}

// Returns source pos, or right hand pos if 'source' is (-1,-1,-1).
bool TSpell::GetSourcePos(S3DPoint& sourcepos)
{
    if (source.x != -1 || source.y != -1 || source.z != -1) // If source used...
    {
        sourcepos = source;
        return true;
    }

  // If no source, use character's hand as the source
    sourcepos.x = sourcepos.y = sourcepos.z = 0;
    if (invoker->GetAnimator() && (invoker->GetImagery()->GetHeader()->imageryid != OBJIMAGE_MESH3D))
    {
        if (!((PT3DAnimator)invoker->GetAnimator())->GetObjectMapPos("rhand", sourcepos))
            return false;
        int32_t z = sourcepos.z;
        ConvertToVector(invoker->GetFace(), Distance(sourcepos), sourcepos);
        sourcepos.z = z;
    }

    return true;
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

bool TSpell::Timer()
{
    if (timer > 0) 
        --timer; 
    
    if (!effect && wait < 0) 
        timer = 0;

    if (wait > 0)
        --wait;
    else if (wait == 0)
    {
        --wait;

        // create the effect id
        SObjectDef def;
        memset(&def, 0, sizeof(SObjectDef));
        def.objclass = OBJCLASS_EFFECT;
        def.level = MapPane.GetMapLevel();
        invoker->GetPos(def.pos);
        S3DPoint spos;
        if (GetSourcePos(spos))
            def.pos += spos;
        else
            def.pos.z += variant->height;  // Height is only used as a last resort
        if (variant->facing)
            def.facing = invoker->GetFace();
        else
            def.facing = 0;
        def.objtype = EffectClass.FindObjType(variant->effect);

        effect = MapPane.GetInstance(MapPane.NewObject(&def));

        if (effect)
            ((PTEffect)effect.Get())->SetSpell(this);
    }

    return timer == 0; 
}

void TSpell::Damage(TObjectInstance* ch)
{
    int32_t mindam = variant->mindamage;
    int32_t maxdam = variant->maxdamage;

    if (((PTCharacter)ch)->IsMagicResistant())
    {
        // take the magic resistance as the percentage of damage to allow...
        mindam -= (int32_t)(mindam * ((PTCharacter)ch)->GetMagicResistance());
        maxdam -= (int32_t)(maxdam * ((PTCharacter)ch)->GetMagicResistance());
    }

    ((PTCharacter)ch)->Damage(random(mindam, maxdam), spell->damagetype);
}

void TSpell::ManaDrain()
{   
    ((PTCharacter)invoker)->SetMana(((PTCharacter)invoker)->Mana() - variant->mana);
    if (((PTCharacter)invoker)->Mana() > ((PTCharacter)invoker)->MaxMana())
        ((PTCharacter)invoker)->SetMana(((PTCharacter)invoker)->MaxMana());
    if (((PTCharacter)invoker) == ((PTCharacter)Player))
        StaminaBar.ChangeLevel(((PTCharacter)invoker)->Mana() * 1000 / ((PTCharacter)invoker)->MaxMana());
}

// *********************
// *** TSpellManager ***
// *********************

// cast a spell by using its name
bool TSpellManager::CastByName(char* name, TObjectInstance* invoker, 
    TObjectInstance* *targets, int32_t numtargs, S3DPoint* sourcepos, PTSpell mst)
{
    PSSpellData spell_data = SpellList.GetSpellDataByName(name);
    PSSpellVariant variant_data = SpellList.GetVariantDataByName(name);

    if (!spell_data || !variant_data || wait || ((PTCharacter)invoker)->Mana() < variant_data->mana)
        return false;

    PTSpell spell = new TSpell(invoker, targets, numtargs, sourcepos, spell_data, variant_data, mst);
    spell->SetByName(name);

    ((PTCharacter)invoker)->SetCast(spell_data->invoke, (targets)?(targets[0]):nullptr, variant_data->ani_delay);
    wait = variant_data->nextspellwait;

    spell->ManaDrain();

    spells.Add(spell);

    return true;
}

// cast a spell by using the talismans
bool TSpellManager::CastByTalismans(char* talismans, TObjectInstance* invoker, 
    TObjectInstance* *targets, int32_t numtargs, S3DPoint* sourcepos, PTSpell mst)
{
    PSSpellData spell_data = SpellList.GetSpellDataByTalismans(talismans);
    PSSpellVariant variant_data = SpellList.GetVariantDataByTalismans(talismans);

    if (!spell_data || !variant_data || wait || ((PTCharacter)invoker)->Mana() < variant_data->mana)
        return false;

    PTSpell spell = new TSpell(invoker, targets, numtargs, sourcepos, spell_data, variant_data, mst);
    spell->SetByTalismans(talismans);

    ((PTCharacter)invoker)->SetCast(spell_data->invoke, (targets)?(targets[0]):nullptr, variant_data->ani_delay);
    wait = variant_data->nextspellwait;

    spell->ManaDrain();
    
    spells.Add(spell);

    return true;
}

void TSpellManager::Pulse()
{
    if (wait > 0)
        --wait;

    int32_t size = spells.NumItems();
    
    for(int32_t i = 0; i < size; ++i) 
    {
        if (!spells.Used(i))
            continue;

        spells[i]->Pulse(); 
        
        if (spells[i]->Timer())
            spells.Delete(i); // leaves a vacant slot; the next index is not skipped
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
