// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     area.h - Game Area Manager                        *
// ************************************************************************* 

#include "revenant.h"
#include "audio_backend.h"
#include "logging.h"
#include "parse.h"
#include "sound.h"
#include "mappane.h"
#include "textbar.h"
#include "script.h"
#include "player.h"
#include "playscreen.h"
#include "textbar.h"
#include "area.h"

#include <cstdio>

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

  // Music playlist
    cdplaynum = 0;
    cdplaylistsize = 0;
    cdplayisrandom = false;
    cdplaypause = 0;

  // Ambient sound + audio environment + background effect
    ambsound[0] = '\0';
    ambsoundid  = -1;
    audioenv    = 0;
    bgeffect[0] = '\0';
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
        else if (t.Is("AMBSOUND"))
        {
            // AMBSOUND "<name>" — looping background SFX. Name resolves
            // through SoundPlayer.FindSound, so it can come from disk OR
            // resources.rvr.
            if (!Parse(t, "AMBSOUND %s\n", ambsound))
                t.Error("'AMBSOUND \"name\"' expected");
            flags |= AREA_PLAYAMBIENT;
        }
        else if (t.Is("AUDIOENV"))
        {
            // AUDIOENV <preset-id> — EAX environment (reverb) preset. We
            // parse + store it; routing to a miniaudio reverb effect is a
            // follow-up. 0 (GENERIC) is the no-op default.
            if (!Parse(t, "AUDIOENV %i\n", &audioenv))
                t.Error("'AUDIOENV <preset-id>' expected");
        }
        else if (t.Is("BGEFFECT"))
        {
            // BGEFFECT "<name>" — area-scoped background visual effect
            // (e.g. lab plasma haze). Owned by the VFX track; gameflow
            // just records the name so the VFX system can pick it up
            // when an area becomes active.
            if (!Parse(t, "BGEFFECT %s\n", bgeffect))
                t.Error("'BGEFFECT \"name\"' expected");
        }
        else
        {
            // Truly unknown tag — log once per area and skip the line.
            log_warn("[area] '%s': skipping unknown tag '%s'", name, t.Text());
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

    return true;
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

// Ambient SFX (AMBSOUND "<name>") — mount the named sound, mark it
// looping, and start it at a low background level. Volume is in
// DirectSound hundredths-of-a-dB attenuation (0 = full, -10000 = mute);
// -2000 ≈ -10dB keeps the loop comfortably behind in-world SFX + music.
static constexpr int32_t kAmbientVolume = -2000;

void TArea::InitAmbientSounds()
{
    if (!ambsound[0]) return;

    ambsoundid = SoundPlayer.FindSound(ambsound);
    if (ambsoundid < 0) {
        log_warn("[area] '%s': AMBSOUND '%s' not found in sound registry", name, ambsound);
        return;
    }
    if (!SoundPlayer.Mount(ambsoundid)) {
        log_warn("[area] '%s': AMBSOUND '%s' failed to mount", name, ambsound);
        ambsoundid = -1;
        return;
    }

    if (PTSound s = SoundPlayer.GetSound(ambsoundid))
        s->SetLooping(true);

    SoundPlayer.Play(ambsoundid, kAmbientVolume, /*freq*/ 0, /*spos*/ nullptr);
    log_info("[area] '%s': ambient '%s' looping", name, ambsound);
}

void TArea::CloseAmbientSounds()
{
    if (ambsoundid < 0) return;
    SoundPlayer.Stop(ambsoundid);
    SoundPlayer.Unmount(ambsoundid);
    ambsoundid = -1;
}

void TArea::PlayAmbientSounds()
{
    // Loop is supposed to run continuously while we're in the area; this
    // is a defensive restart in case anything stops it (engine pause that
    // didn't resume cleanly, UpdateDying flagging a duplicate as dying,
    // etc.). No-op when already playing.
    if (ambsoundid < 0) return;
    if (PTSound s = SoundPlayer.GetSound(ambsoundid)) {
        if (!s->IsPlaying())
            SoundPlayer.Play(ambsoundid, kAmbientVolume, /*freq*/ 0, /*spos*/ nullptr);
    }
}

// Resolve a 1998-era CD track number (the AREA.DEF CDPLAYLIST values) to
// a vorbis path under <install>/MUSIC/. Tracks are 1-based in the source
// data and the ripped GOG file names follow `TrackNN.ogg`. We anchor on
// RunPath (the install dir, read-only assets) rather than ResourcePath —
// makepath() rewrites a leading "." in ResourcePath to SavePath, which
// is the writable per-user dir and doesn't host the music tree.
static bool BuildMusicTrackPath(int32_t track, char* out, size_t out_len)
{
    return std::snprintf(out, out_len, "%sMUSIC/Track%02d.ogg",
                         RunPath, track) > 0;
}

// Initializes data needed to play this area's music playlist.
void TArea::InitCDMusic()
{
    audio::MusicStop();

    cdplaynum    = -1;
    cdplaystart  = 0;
    cdplaywait   = 0;
    cdplaylength = 0;
}

// Deinitializes the playlist (current track keeps playing — areas hand
// the music torch to one another).
void TArea::CloseCDMusic()
{
}

// Advances the playlist. Called from the area's per-tick update. The
// caller-visible behavior matches 1998: pick the next track when the
// previous one finishes, with `cdplaypause` seconds of silence between.
void TArea::PlayCDMusic()
{
    if (cdplaylistsize <= 0) return;
    if (audio::MusicPlaying()) return;

    const int32_t now = static_cast<int32_t>(tickcount());

    // Hold the silence gap after a track ends.
    if (cdplaystart != 0 && (now - cdplaystart) < cdplaywait) {
        PlayAmbientSounds();
        return;
    }

    if (cdplayisrandom) {
        int32_t pick;
        do {
            pick = random(0, cdplaylistsize - 1);
        } while (pick == cdplaynum && cdplaylistsize > 1);
        cdplaynum = pick;
    } else {
        cdplaynum = (cdplaynum + 1) % cdplaylistsize;
    }

    char path[MAXPATHLEN];
    if (!BuildMusicTrackPath(cdplaylist[cdplaynum], path, sizeof(path)) ||
        !audio::MusicPlayFile(path, /*looping*/ false)) {
        // Failed to start — back off so we don't hammer disk on missing files.
        cdplaystart  = now;
        cdplaywait   = 60 * 1000;
        return;
    }

    cdplaystart  = now;
    // Without per-track length info, defer the "between tracks" delay to
    // *after* the track finishes (MusicPlaying() goes false), at which
    // point we still want to honour cdplaypause seconds of silence.
    cdplaywait   = cdplaypause * 1000;
    cdplaylength = 0;
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

  // Load local scripts
    if (flags & AREA_LOADSCRIPTS)
        ScriptManager.Load(scriptfile, this);

  // Put "Entered" line in the status line
    if (Player)
        TextBar.Print("%s entered %s", Player->GetName(), name);

    log_info("[area] entered '%s' (level=%d, audioenv=%d%s%s%s)", name, this->level,
             audioenv,
             ambsound[0] ? ", ambsound=" : "", ambsound[0] ? ambsound : "",
             bgeffect[0] ? ", bgeffect set" : "");
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
