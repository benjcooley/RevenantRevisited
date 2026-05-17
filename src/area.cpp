// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     area.h - Game Area Manager                        *
// ************************************************************************* 

#include "revenant.h"
#include "logging.h"
#include "parse.h"
#include "sound.h"
#include "mappane.h"
#include "textbar.h"
#include "script.h"
#include "player.h"
#include "maprenderer.h"
#include "playscreen.h"
#include "revisited_defaults.h"
#include "revisited_settings.h"
#include "textbar.h"
#include "area.h"

// *****************************
// * TArea - Basic area object *
// *****************************

static S3DPoint lastpos;
static int32_t lastlevel;
static int32_t lastdaylight;
static int32_t lastdayflag;

// Initializes and loads an area
TArea::TArea()
{
    flags = 0;
    name[0] = '\0';
    rects.Clear();
    scriptfile[0] = '\0';
    amblight = 30;
    nightamblight = 10;
    ambcolor.red = ambcolor.green = ambcolor.blue = 255;
    nightambcolor.red = nightambcolor.green = nightambcolor.blue = 255;
    level = -1; // All levels
    rects.Clear();

  // CD audio music playing
    cdplaynum = 0;
    cdplaylistsize = 0;
    cdplayisrandom = false;
    cdplaypause = 0;
}

// Gets rid of area
TArea::~TArea()
{
    Exit();
}

// Loads the area from the "AREA.DEF" file
bool TArea::Load(char *aname, TToken &t)
{
    strncpyz(name, aname, MAXNAMELEN);

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        t.Error("Area block BEGIN expected");
    
  // Now get first trigger token
    t.LineGet();
    
  // Iterate through the triggers and setup trigger list
    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
      // Parse trigger tags now
        if (t.Type() != TKN_IDENT)
            t.Error("Area keyword expected");

      // Tags...
        if (t.Is("LEVEL"))
        {
            t.WhiteGet();

            if (t.Type() == TKN_NUMBER)
            {
                level = t.Index();
                t.WhiteGet();
            }
            else if (t.Is("ALL"))
            {
                level = -1;
                t.WhiteGet();
            }
            
            if (t.Type() != TKN_RETURN)
                t.Error("'LEVEL <ALL|level>' expected");
            
            t.LineGet();
        }
        else if (t.Is("RECT"))
        {
            SRect r;
            if (!Parse(t, "RECT %i, %i, %i, %i\n", &r.left, &r.top, &r.right, &r.bottom))
                t.Error("'RECT left top right bottom' expected");
            int32_t temp;
            if (r.right < r.left)
                { temp = r.left; r.left = r.right; r.right = temp; }
            if (r.bottom < r.top)
                { temp = r.top; r.top = r.bottom; r.bottom = temp; }
            rects.Add(r);
        }
        else if (t.Is("SCRIPT"))
        {
            if (!Parse(t, "SCRIPT %s\n", scriptfile))
                t.Error("'SCRIPT \"filename\"' expected");
            flags |= AREA_LOADSCRIPTS;
        }
        else if (t.Is("AMBLIGHT"))
        {
            if (!Parse(t, "AMBLIGHT %i\n", &amblight))
                t.Error("'AMBLIGHT light' expected");

            flags |= AREA_SETAMBIENT;
        }
        else if (t.Is("AMBCOLOR"))
        {
            if (!Parse(t, "AMBCOLOR %b, %b, %b\n", &ambcolor.red, &ambcolor.green, &ambcolor.blue))
                t.Error("'AMBCOLOR red green blue' expected");

            flags |= AREA_SETAMBIENT;
        }
        else if (t.Is("NIGHTAMBLIGHT"))
        {
            if (!Parse(t, "NIGHTAMBLIGHT %i\n", &nightamblight))
                t.Error("'NIGHTAMBLIGHT light' expected");

            flags |= AREA_DONIGHT;
        }
        else if (t.Is("NIGHTAMBCOLOR"))
        {
            if (!Parse(t, "NIGHTAMBCOLOR %b, %b, %b\n", &nightambcolor.red, &nightambcolor.green, &nightambcolor.blue))
                t.Error("'NIGHTAMBCOLOR red green blue' expected");

            flags |= AREA_DONIGHT;
        }
        else if (t.Is("CDPLAYLIST"))
        {
            char *cdplaylisterr = "'CDPLAYLIST <RANDOM> <PAUSE pause> TRACKS track1 track2 track3 ...' expected";

            t.WhiteGet();
            if (t.Is("RANDOM"))
            {
                cdplayisrandom = true;
                t.WhiteGet();
            }
            else 
                cdplayisrandom = false;

            if (t.Is("PAUSE"))
            {
                if (!Parse(t, "PAUSE %d", &cdplaypause))
                    t.Error(cdplaylisterr);
            }

            if (!t.Is("TRACKS"))
                    t.Error(cdplaylisterr);

            t.WhiteGet();

            cdplaylistsize = 0;
            while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
            {
                if (cdplaylistsize >= MAXPLAYLISTSIZE)
                    t.Error("CDPLAYLIST too many tracks");

                if (t.Type() != TKN_NUMBER)
                    t.Error(cdplaylisterr);

                cdplaylist[cdplaylistsize] = t.Index();
                cdplaylistsize++;

                t.WhiteGet();
            }
            t.LineGet();

            flags |= AREA_PLAYCDMUSIC;
        }
        else if (t.Is("POINTLIGHTINT"))
        {
            if (!Parse(t, "POINTLIGHTINT %f\n", &point_light_int_mul))
                t.Error("'POINTLIGHTINT value' expected");
            flags |= AREA_SETLIGHTING;
        }
        else if (t.Is("POINTLIGHTRANGE"))
        {
            if (!Parse(t, "POINTLIGHTRANGE %f\n", &point_light_range_mul))
                t.Error("'POINTLIGHTRANGE value' expected");
            flags |= AREA_SETLIGHTING;
        }
        else
        {
            // Retail added area tags (AUDIOENV, ...) not in the pre-release
            // source. Skip to the next line rather than aborting.
            log_warn("[area] skipping unknown tag '%s'", t.Text());
            while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
                t.Get();
            t.LineGet();
        }
    }

    if (!t.Is("END"))
        t.Error("Area block END expected");

  // If no rects for area, add a default super huge rect
    if (rects.NumItems() <= 0)
    {
        SRect r;
        r.left = -1000000000;
        r.top = -1000000000;
        r.right = 1000000000;
        r.bottom = 1000000000;
        rects.Add(r);
    }

    // Layered defaults: after area.def parsing, give the auto-generated
    // baked table (revisited_defaults.cpp) a chance to overwrite per-area
    // lighting fields with values captured from the debug-panel "Bake
    // Defaults to Source" button. Lookup by area name; no-op when the
    // area is missing from the baked table. See [[project-revisited-defaults]].
    ApplyBakedAreaDefaults(name, this);

    return true;
}

void TArea::SetClassicLighting(int32_t amb, const SColor &amb_c,
                               int32_t night_amb, const SColor &night_c,
                               double point_int_mul, double point_range_mul)
{
    amblight              = amb;
    ambcolor              = amb_c;
    nightamblight         = night_amb;
    nightambcolor         = night_c;
    point_light_int_mul   = point_int_mul;
    point_light_range_mul = point_range_mul;
    flags |= AREA_SETAMBIENT;
}

bool TArea::In(S3DPoint &pos, int32_t lev)
{
    if (level != -1 && lev != level)
        return false;

    SPoint p;
    p.x = pos.x;
    p.y = pos.y;

    for (int32_t c = 0; c < rects.NumItems(); c++)
    {
        if (rects[c].In(p))
            return true;
    }

    return false;
}

// Initializes ambient sounds for area
void TArea::InitAmbientSounds()
{
}

// Deinitializes ambient sounds for area
void TArea::CloseAmbientSounds()
{
}

void TArea::PlayAmbientSounds()
{
    // Play ambient sound effects here
}

// Initializes data needed to play CD tracks
void TArea::InitCDMusic()
{
  // Initialize CD play params
    if (CDPlaying())
        CDStop();   

  // Initialize play params
    cdplaynum = -1;
    cdplaystart = tickcount();
    cdplaywait = 0;
    cdplaylength = 0;
}

// Deinitializes CD play system
void TArea::CloseCDMusic()
{
    // Do nothing (allow music to continue playing)
}

// Plays CD music tracks
void TArea::PlayCDMusic()
{
    if ((int32_t)tickcount() - cdplaystart >= cdplaywait)
    {
        if (cdplayisrandom)
        {
            int32_t newplaynum;
            do {
                newplaynum = random(0, cdplaylistsize - 1);
            } while (newplaynum == cdplaynum && cdplaylistsize > 1);
            cdplaynum = newplaynum;
        }
        else
        {
            cdplaynum++;
            if (cdplaynum >= cdplaylistsize)
                cdplaynum = 0;
        }

        cdplaylength = CDTrackLength(cdplaylist[cdplaynum]);
        cdplaywait = cdplaylength + cdplaypause * 1000;
        if (cdplaywait <= 1000)
            cdplaywait = 3 * 60 * 1000; // Wait 3 minutes if no track found
        CDPlayTrack(cdplaylist[cdplaynum]);
        cdplaystart = tickcount();
    }
    else if ((int32_t)tickcount() - cdplaystart >= cdplaylength)
        PlayAmbientSounds();
}

// Gets the current ambient colors based on the time of day
void TArea::GetCurrentAmbient(int32_t &ambient, SColor &color)
{
    if (flags & AREA_DONIGHT)
    {
        int32_t daylight = PlayScreen.Daylight();

        if (daylight == 0)
        {
            ambient = nightamblight;
            color = nightambcolor;
        }
        else if (daylight == 255)
        {
            ambient = amblight;
            color = ambcolor;
        }
        else
        {

            ambient = amblight * daylight / 255 + nightamblight * (255 - daylight) / 255;
            color.red = (uint8_t)((int32_t)((int32_t)ambcolor.red * daylight / 255 + 
                (int32_t)nightambcolor.red * (255 - daylight) / 255));
            color.green = (uint8_t)((int32_t)((int32_t)ambcolor.green * daylight / 255 + 
                (int32_t)nightambcolor.green * (255 - daylight) / 255));
            color.blue = (uint8_t)((int32_t)((int32_t)ambcolor.blue * daylight / 255 + 
                (int32_t)nightambcolor.blue * (255 - daylight) / 255));
        }
    }
    else
    {
        ambient = amblight;
        color = ambcolor;
    }
}

// Called by the game screen Pulse() function to update area stuff
void TArea::Pulse()
{
    if ((flags & AREA_PLAYCDMUSIC) && cdplaylistsize > 0)
        PlayCDMusic();

    if (flags & AREA_PLAYAMBIENT)
        PlayAmbientSounds();

    int32_t dayflag = PlayScreen.DayTimeFlag();
    if (flags & AREA_DONIGHT)
    {
        if ((dayflag != lastdayflag) && !(dayflag == DAY_DAYTIME) && !(dayflag == DAY_NIGHT)) 
        {
            char *daynames[6] = {"midnight", "morning", "daytime", "noon", "evening", "night"};
            TextBar.Print("It was %s", daynames[dayflag]);
        }

        int32_t daylight = PlayScreen.Daylight();
        if (daylight != lastdaylight)
        {
            int32_t ambient;
            SColor color;

            GetCurrentAmbient(ambient, color);
            MapPane.SetAmbient(ambient, color, false); // false=Allow fading to continue

            lastdaylight = daylight;
        }
    }
    lastdayflag = dayflag;
}

// Called by the game screen Animate() function to update area stuff
void TArea::Animate(bool draw)
{

}

// Called when the player enters the area
void TArea::Enter()
{
    S3DPoint pos;
    int32_t level;

    MapPane.GetMapPos(pos);
    level = MapPane.GetMapLevel();

    if (flags & AREA_PLAYERIN)
        return;

    flags |= AREA_PLAYERIN;

  // Initialize cd play music
    if (flags & AREA_PLAYCDMUSIC)
        InitCDMusic();

  // Initialize ambient sounds
    if (flags & AREA_PLAYAMBIENT)
        InitAmbientSounds();

  // Set ambient values
    if (flags & AREA_SETAMBIENT)
    {
        int32_t ambient;
        SColor color;
        GetCurrentAmbient(ambient, color);

        if (level != lastlevel || dist(pos.x, pos.y, lastpos.x, lastpos.y) > 1024)
            MapPane.SetAmbient(ambient, color);
        else
            MapPane.FadeAmbient(ambient, color, FRAMERATE * 3, 10);
    }

  // Revisited per-area lighting overrides. Per-area POINTLIGHTINT /
  // POINTLIGHTRANGE *compose* with the global RevisitedSettings defaults
  // (multiply), so transitioning between areas always pushes a coherent
  // value: areas without overrides have a default of 1.0 and effectively
  // push just the global. The composition holds even without --revisited
  // because RevisitedSettings still has its baked-in defaults.
    if (TMapRenderer *mr = PlayScreen.MapRenderer())
    {
        const float int_mul   = float(RevisitedSettings.point_light_int_mul   * point_light_int_mul);
        const float range_mul = float(RevisitedSettings.point_light_range_mul * point_light_range_mul);
        mr->SetPointLightMultipliers(int_mul, range_mul);
    }

  // Load local scripts
    if (flags & AREA_LOADSCRIPTS)
        ScriptManager.Load(scriptfile, this);

  // Put "Entered" line in the status line
    if (Player)
        TextBar.Print("%s entered %s", Player->GetName(), name);
}

// Called when the player exits the area
void TArea::Exit()
{
    if (!(flags & AREA_PLAYERIN))
        return;

    flags &= ~AREA_PLAYERIN;

  // Close cd play music
    if (flags & AREA_PLAYCDMUSIC)
        CloseCDMusic();

  // Close ambient sounds
    if (flags & AREA_PLAYAMBIENT)
        CloseAmbientSounds();

  // Eliminate local scripts
    if (flags & AREA_LOADSCRIPTS)
        ScriptManager.Clear(this);
}

// **************************************
// * TAreaManager - Area manager object *
// **************************************

// Initialize the area manager
bool TAreaManager::Initialize()
{
    if (initialized)
        return true;

    areas.DeleteAll();

  // Clear global last values
    lastpos.x = -100000; lastpos.y = -100000; lastpos.z = -100000;
    lastlevel = 255;
    lastdaylight = -1;
    lastdayflag = -1;

    if (!Load())
        return false;

    initialized = true;

    return true;
}

// Closes the area manager
void TAreaManager::Close()
{
    areas.DeleteAll();
}

// Loads all areas from the "AREA.DEF" file
bool TAreaManager::Load()
{
    char fname[MAXPATHLEN];
    sprintf(fname, "%s%s", ClassDefPath, "area.def");

    FILE *fp = rev_fopen(fname, "rb");
    if (!fp)
        FatalError("Unable to find game area file AREA.DEF");

    TFileParseStream s(fp, fname);
    TToken t(s);

    if (!t.DefineGet())
        t.Error("Syntax error in header");

    while (t.Type() != TKN_EOF)
    {

        char areaname[MAXNAMELEN];
        if (!Parse(t, "AREA %s\n", areaname))
            t.Error("AREA \"name\" expected");

        PTArea area = new TArea;

        if (!area->Load(areaname, t))
            t.Error("Error loading area");
        
        areas.Add(area);

        if (!t.DefineGet())
            t.Error("Syntax error between area blocks");
    }
    
    fclose(fp);

    return true;
}

// Called by the game screen Pulse() function to update area stuff.
//
// Retail behavior (recon/classes_original/cls_0x41c410.cpp:41c410:35-94):
// area.def is an ordered list; specific sub-areas (with RECTs) follow
// general areas (no RECT) so the SPECIFIC entry overrides. Resolution
// is "single active area, last match wins":
//
//   * Iterate all areas, remember the LAST one whose In(pos, level) is true.
//   * Exit() every previously-active area that isn't the winner.
//   * Enter() the winner (if it wasn't already entered).
//   * Pulse() only the winner.
//
// This matters because Enter() also runs SCRIPT and AMBSOUND/AUDIOENV/
// CDPLAYLIST hooks — firing those for both Forest AND Misthaven simultaneously
// would double-load scripts and mismatch audio.
void TAreaManager::Pulse()
{
    S3DPoint pos;
    MapPane.GetMapPos(pos);
    const int32_t level = MapPane.GetMapLevel();

  // Find the last matching area.
    PTArea winner = nullptr;
    for (int32_t c = 0; c < areas.NumItems(); c++)
    {
        if (areas[c]->In(pos, level))
            winner = areas[c];
    }

  // Exit every previously-active area that isn't the winner.
    for (int32_t c = 0; c < areas.NumItems(); c++)
    {
        PTArea area = areas[c];
        if (area == winner)
            continue;
        if (area->GetFlags() & AREA_PLAYERIN)
            area->Exit();
    }

  // Enter the winner (if any) and Pulse it.
    if (winner)
    {
        if (!(winner->GetFlags() & AREA_PLAYERIN))
            winner->Enter();
        winner->Pulse();
    }

  // Save last pos/level so distance-gated crossfade in TArea::Enter
  // can decide between snap and FadeAmbient.
    lastpos = pos;
    lastlevel = level;
}

PTArea TAreaManager::CurrentArea()
{
    for (int32_t c = 0; c < areas.NumItems(); c++)
        if (areas[c]->GetFlags() & AREA_PLAYERIN)
            return areas[c];
    return nullptr;
}

// Called by the game screen Animate() function to update area stuff
void TAreaManager::Animate(bool draw)
{
    for (int32_t c = 0; c < areas.NumItems(); c++)
    {
        PTArea area = areas[c];

        if (area->GetFlags() & AREA_PLAYERIN)
            area->Animate(draw);
    }
}
