// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   dialog.cpp - TDialogPane module                     *
// *************************************************************************

#include "revenant.h"
#include "dialog.h"
#include "display.h"
#include "multi.h"
#include "playscreen.h"
#include "character.h"
#include "command.h"
#include "bitmap.h"
#include "mappane.h"
#include "module.h"
#include "revutils.h"
#include "script.h"
#include "player.h"
#include "logging.h"

#include <algorithm>
#include <string>

// ****************************************************************************
// * TDialogList - Stores language specific dialog and message lines for game *
// ****************************************************************************

namespace {

// Lookups compare at most this many characters of the probe (0x0049d6d0).
constexpr size_t kMaxProbe = 39;

std::string UpperTag(const char *tag, size_t maxlen = std::string::npos)
{
    std::string up(tag ? tag : "");
    if (up.size() > maxlen)
        up.resize(maxlen);
    for (char &c : up)
        c = (char)toupper((unsigned char)c);
    return up;
}

}  // namespace

// Reads one dialog file: a #define header, then `TAG "line"` per line
// (retail Parse("%63t %4091s")), sorted by tag. Duplicate tags keep file
// order and the first one answers (retail's bsearch picks either).
bool TDialogList::LoadTable(const char *path, TTable &table)
{
    table.clear();
    FILE *fp = rev_fopen(path, "rb");
    if (!fp)
        return false;

    TFileParseStream s(fp, path);
    TToken t(s);
    if (!t.DefineGet())
        t.Error("Syntax error in header");

    while (t.Type() != TKN_EOF)
    {
        char tag[64];
        char line[4092];
        if (!Parse(t, "%63t %4091s", tag, line))
            t.Error("tag \"line\" expected");
        if (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
            t.Error("RETURN expected");
        table.push_back({UpperTag(tag), line});
        t.DefineGet();
    }
    fclose(fp);

    std::stable_sort(table.begin(), table.end(),
                     [](const SLine &a, const SLine &b) { return a.tag < b.tag; });
    return true;
}

int32_t TDialogList::Find(const TTable &table, const std::string &tag)
{
    const auto it = std::lower_bound(table.begin(), table.end(), tag,
                                     [](const SLine &l, const std::string &t) { return l.tag < t; });
    if (it == table.end() || it->tag != tag)
        return -1;
    return (int32_t)(it - table.begin());
}

// REVSYNC: 0x0049ceb0
bool TDialogList::Initialize()
{
    if (initialized)
        return true;
    const std::string path = std::string(ClassDefPath) + Language.CStr() + ".def";
    if (!LoadTable(path.c_str(), base))
    {
        log_error("[dialog] base dialog list '%s' not found", path.c_str());
        return false;
    }
    log_info("[dialog] base list %s: %d lines", path.c_str(), (int)base.size());
    initialized = true;
    return true;
}

// REVSYNC: 0x0049d2a0
bool TDialogList::LoadModule()
{
    misses.clear();
    const std::string language = Language.CStr();
    for (const std::string &file : {language + "_dialog.def", language + ".def",
                                    std::string("english.def")})
    {
        const std::string path = ModuleManager.ModuleFilePath(file.c_str());
        if (!path.empty() && rev_file_exists(path.c_str()) && LoadTable(path.c_str(), module))
        {
            log_info("[dialog] module list %s: %d lines", path.c_str(), (int)module.size());
            return true;
        }
    }
    module.clear();
    log_error("[dialog] the active module has no dialog list");
    return false;
}

void TDialogList::Close()
{
    base.clear();
    module.clear();
    misses.clear();
    initialized = false;
}

// REVSYNC: 0x0049d6d0
int32_t TDialogList::FindLine(const char *tag) const
{
    const std::string probe = UpperTag(tag, kMaxProbe);
    const int32_t inbase = Find(base, probe);
    if (inbase >= 0)
        return inbase;
    const int32_t inmodule = Find(module, probe);
    return inmodule >= 0 ? (int32_t)base.size() + inmodule : -1;
}

// REVSYNC: 0x0049d780
const char *TDialogList::GetLine(int32_t id) const
{
    if (id >= 0 && id < (int32_t)base.size())
        return base[id].line.c_str();
    id -= (int32_t)base.size();
    if (id >= 0 && id < (int32_t)module.size())
        return module[id].line.c_str();
    return "[badid]";
}

// REVSYNC: 0x0049d7c0
const char *TDialogList::GetTag(int32_t id) const
{
    if (id >= 0 && id < (int32_t)base.size())
        return base[id].tag.c_str();
    id -= (int32_t)base.size();
    if (id >= 0 && id < (int32_t)module.size())
        return module[id].tag.c_str();
    return "[badid]";
}

// REVSYNC: 0x0049d800
const char *TDialogList::GetLine(const char *tag) const
{
    const int32_t id = FindLine(tag);
    if (id >= 0)
        return GetLine(id);
    auto [it, added] = misses.try_emplace(tag ? tag : "");
    if (added)
        it->second = "[" + it->first + "]";
    return it->second.c_str();
}

// ************************************************************
// * TDialogPane - Shows the dialog options for the character *
// ************************************************************

TDialogPane DialogPane;

static TObjectInstance* DlgContext;
static bool savecontrolon;
static bool saveisfullscreen;

void SetDialogContext(TObjectInstance* context)
{
    DlgContext = context;
}

// Translates a dialog line into `outbuf` (at most buflen - 1 characters):
// [me] becomes the player's name, [chr] the name of the object whose command
// is running, and [[ / ]] are literal brackets.
// REVSYNC: 0x00533dd0 -- retail's lead-byte test is inverted, so its
// substitution never runs; no shipped line contains '[', so the text matches.
char *DialogLine(const char *line, char *outbuf, int32_t buflen)
{
    if (!outbuf || buflen <= 0)
        return outbuf;

    std::string out;
    for (const char *p = line ? line : ""; *p; )
    {
        if (*p != '[')
        {
            out += *p++;
            continue;
        }
        p++;
        if (*p == '[' || *p == ']')
        {
            out += *p++;
            continue;
        }
        std::string tag;
        while (*p && *p != ']')
            tag += *p++;
        if (*p == ']')
            p++;

        const TObjectInstance* named = nullptr;
        if (!stricmp(tag.c_str(), "me"))
            named = Player;
        else if (!stricmp(tag.c_str(), "chr"))
            named = DlgContext;
        if (named && named->GetName())
            out += named->GetName();
    }

    snprintf(outbuf, (size_t)buflen, "%s", out.c_str());
    return outbuf;
}

// ****************
// * TDialogEntry *
// ****************

TDialogEntry::TDialogEntry(TObjectInstance* speaker, EMode mode, std::vector<std::string> texts,
                           std::vector<std::string> labels, int32_t ticks)
  : speaker(speaker), mode(mode), ticksleft(ticks), texts(std::move(texts)), labels(std::move(labels))
{
    if (this->texts.size() > kMaxTexts)
        this->texts.resize(kMaxTexts);
}

void TDialogEntry::Place()
{
    placed = true;
}

TObjectInstance* TDialogEntry::Speaker() const
{
    return speaker.Get();
}

// REVSYNC: 0x005348f0. An entry isn't timed until the pane has placed it.
void TDialogEntry::Pulse()
{
    if (!placed)
        return;

    if (ticksleft >= 0)
    {
        if (ticksleft == 0)
            Dismiss();
        ticksleft--;
    }

    if (fade < fadetarget)
        fade++;
    else if (fade > fadetarget)
        fade--;
}

// REVSYNC: 0x00534a40
void TDialogEntry::Dismiss()
{
    dismissed = true;
    fadetarget = 0;
}

// ***************
// * TDialogPane *
// ***************

// REVSYNC: 0x00534fd0
bool TDialogPane::Initialize()
{
    if (IsOpen())
        return true;
    if (!TPane::Initialize())
        return false;
    entries.clear();
    responses = nullptr;
    choicetexts.clear();
    choicelabels.clear();
    chosen = -1;
    committed = false;
    return true;
}

// REVSYNC: 0x00535060
void TDialogPane::Close()
{
    if (!IsOpen())
        return;
    entries.clear();
    responses = nullptr;
    choicetexts.clear();
    choicelabels.clear();
    TPane::Close();
}

// REVSYNC: 0x00535120 -- hidden, ignoring input, entries and choices gone.
// (Retail forgot the entries without freeing them.)
void TDialogPane::Hide()
{
    TPane::Hide();
    entries.clear();
    responses = nullptr;
    choicetexts.clear();
    choicelabels.clear();
}

// REVSYNC: 0x005351d0, once per simulation tick: age the entries, place new
// ones, drop the faded, and commit a picked response -- after the scripts
// have pulsed, so the waiting script has seen it (TScript::WaitSatisfied).
void TDialogPane::Pulse()
{
    for (const std::unique_ptr<TDialogEntry>& entry : entries)
        entry->Pulse();

    // Layout: retail stacks the entries and slides them to their slots here.
    // Positions belong to the presentation; the runtime only needs to know
    // an entry is on screen, which starts its clock.
    for (const std::unique_ptr<TDialogEntry>& entry : entries)
        if (!entry->IsPlaced())
            entry->Place();

    DeleteGoneEntries();

    if (committed && chosen >= 0)
    {
        log_info("[dialog] choice %d committed (label '%s')", chosen + 1, choicelabels[chosen].c_str());
        committed = false;
        PlayScreen.SetControlOn(savedcontrol);
        if (responses)
            responses->Dismiss();
        responses = nullptr;
        answering.Clear();
        controlonwhilechoosing = false;
    }
}

void TDialogPane::DeleteGoneEntries()
{
    const auto gone = std::remove_if(entries.begin(), entries.end(),
                                     [this](const std::unique_ptr<TDialogEntry>& entry) {
        if (!entry->IsGone())
            return false;
        if (entry.get() == responses)
            responses = nullptr;
        return true;
    });
    entries.erase(gone, entries.end());
}

// REVSYNC: 0x00535610 (key-down only). Space silences the spoken lines;
// 1-6 pick a choice while the responses are up.
void TDialogPane::KeyPress(int32_t key, bool down)
{
    if (!down)
        return;
    if (key == ' ')
        SkipSpeech();
    else if (key >= '1' && key <= '6')
    {
        const int32_t index = key - '1';
        if (responses && index < (int32_t)choicelabels.size())
            Choose(index);
    }
}

// REVSYNC: 0x00535760 -- the joystick's skip button (0x40a, "JOY2" in
// retail's key table) silences the spoken lines.
void TDialogPane::Joystick(int32_t key, bool down)
{
    if (down && key == VK_JOYBUTTON3)
        SkipSpeech();
}

void TDialogPane::Choose(int32_t index)
{
    chosen = index;
    committed = true;
}

// REVSYNC: 0x00535870
void TDialogPane::AddChoice(const char *label, const char *text)
{
    if (!IsOpen() && !responses)
        return;
    if (chosen >= 0)                        // answered: a new list begins
        ClearResponses();
    if ((int32_t)choicelabels.size() >= kMaxChoices)
        return;
    choicetexts.emplace_back(text ? text : "");
    choicelabels.emplace_back(label ? label : "");
}

// REVSYNC: 0x00535e90. The choices show as their dialog lines in quotes.
bool TDialogPane::ShowResponses(TObjectInstance* player, bool controlon)
{
    if (choicelabels.empty() || !player)
        return false;

    answering = player;
    controlonwhilechoosing = controlon;
    savedcontrol = PlayScreen.IsControlOn();
    PlayScreen.SetControlOn(controlonwhilechoosing);

    std::vector<std::string> lines;
    for (const std::string& tag : choicetexts)
        lines.push_back("\"" + std::string(DialogList.GetLine(tag.c_str())) + "\"");
    entries.push_back(std::make_unique<TDialogEntry>(player, TDialogEntry::EMode::Responses,
                                                     std::move(lines), choicelabels,
                                                     TDialogEntry::kNoTimeout));
    responses = entries.back().get();
    log_info("[dialog] %d choice(s) shown", (int)choicelabels.size());
    return true;
}

// REVSYNC: 0x00535b90
void TDialogPane::AddSpeech(TObjectInstance* speaker, const char *text, int32_t ticks)
{
    if (!speaker || !text || ticks <= 0)
        return;
    const TDialogEntry::EMode mode = speaker->ObjClass() == OBJCLASS_PLAYER
                                   ? TDialogEntry::EMode::PlayerSpeech
                                   : TDialogEntry::EMode::NpcSpeech;
    entries.push_back(std::make_unique<TDialogEntry>(speaker, mode, std::vector<std::string>{text},
                                                     std::vector<std::string>{}, ticks));
}

// REVSYNC: 0x00536010. Retail's StopTalking (0x004d6000) also stops the
// speaker's voice; that arrives with the speech port.
void TDialogPane::SkipSpeech()
{
    if (!IsOpen() || responses)
        return;
    for (const std::unique_ptr<TDialogEntry>& entry : entries)
    {
        if (TObjectInstance* speaker = entry->Speaker(); speaker && speaker->IsCharacter())
            static_cast<TCharacter*>(speaker)->ForceCommandDone();
        entry->Dismiss();
    }
}

// REVSYNC: 0x00535a10 -- control is not given back here.
void TDialogPane::ClearResponses()
{
    if (responses)
        responses->Dismiss();
    responses = nullptr;
    choicetexts.clear();
    choicelabels.clear();
    chosen = -1;
    committed = false;
}

// REVSYNC: 0x00535d80
void TDialogPane::ClearSpeech(bool now)
{
    if (!IsOpen())
        return;
    SkipSpeech();
    ClearResponses();
    if (now)
        entries.clear();
    else
        for (const std::unique_ptr<TDialogEntry>& entry : entries)
            entry->Dismiss();
}

// REVSYNC: 0x005360f0 (its tail -- give control back, jump the answering
// player's script to "Finish", Hide -- is unreachable in retail).
void TDialogPane::ResetForLoad()
{
    if (responses)
        responses->Dismiss();
    responses = nullptr;
    choicetexts.clear();
    choicelabels.clear();
    entries.clear();
}

const char *TDialogPane::CommittedLabel() const
{
    if (!committed || chosen < 0 || chosen >= (int32_t)choicelabels.size())
        return nullptr;
    return choicelabels[chosen].c_str();
}

const char *TDialogPane::ChosenText() const
{
    if (chosen < 0 || chosen >= (int32_t)choicetexts.size())
        return nullptr;
    return choicetexts[chosen].c_str();
}

// REVSYNC: choice @ 0x004282f0 -- choice <label> <text-part>... The text is
// in practice the dialog tag of the line the choice shows. From a script the
// choice goes through the script (TScript::AddChoice); from the console,
// straight to the pane.
COMMAND(CmdChoice)
{
    if (t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;
    const std::string label = t.Text();

    t.WhiteGet();
    if (t.Type() != TKN_TEXT && t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;

    std::string text;
    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
    {
        if (t.Type() == TKN_TEXT)
            text += t.Text();
        else if (t.Type() == TKN_IDENT)
        {
            // Retail appends a prototype number or string variable named
            // here (not ported); any other identifier replaces the text
            // with its DialogLine form -- the identifier itself.
            char buf[256];
            text = DialogLine(t.Text(), buf, sizeof(buf));
        }
        t.WhiteGet();                       // numbers and symbols are skipped
    }

    if (script)
        script->AddChoice(label.c_str(), text.c_str());
    else
        DialogPane.AddChoice(label.c_str(), text.c_str());
    return 0;
}
