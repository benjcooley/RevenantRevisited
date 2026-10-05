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

// Dialog line translator... Note: Dialog TAGS are listed in DLGTAG.TXT

char *DialogLine(const char *line, char *outbuf, int32_t buflen)
{
    char buf[128];
    char tag[20];
    char data[128];
    const char *p;
    char *b, *d;

    b = buf;
    for (p = line; *p != '\0'; )
    {
        if (*p == '[')
        {
            p++;
            if (*p == '[' || *p == ']')
            {
                *b++ = *p++;
                continue;
            }
            char *t = tag;
            while (*p && *p != ']')
                *t++ = *p++;
            if (*p == ']')
                p++;
            *t++ = '\0';

            data[0] = '\0';
            if (!stricmp(tag, "me"))                // "me" is locke
                strcpy(data, Player->GetName());
            else if (!stricmp(tag, "chr") && DlgContext != nullptr) // "chr" is the character talking
                strcpy(data, DlgContext->GetName());
            
            d = data;
            while (*d)
                *b++ = *d++;
        }
        else
            *b++ = *p++;
    }

    *b = '\0';

    return strncpyz(outbuf, buf, buflen);
}

bool TDialogPane::Initialize()
{
    if (IsOpen())
        return true;

    if (!TPane::Initialize())
        return false;

    dialogdata = TMulti::LoadMulti("dialog.dat");
    choice = -1;
    freshresponse = false;

    for (int32_t i = 0; i < MAXCHOICES; i++)
        choices[i] = label[i] = nullptr;

    return true;
}

void TDialogPane::Close()
{
    if (!IsOpen())
        return;

    TPane::Close();

    if (dialogdata)
        delete dialogdata;
}

void TDialogPane::Show()
{
    if (!IsHidden() || !IsOpen() || PlayScreen.IsDemoMode())
        return;

    TPane::Show();

    if (dialogdata)
    {
        saveisfullscreen = PlayScreen.IsFullScreen();
        PlayScreen.SetFullScreen(false);
        PlayScreen.HideLowerPanes();
        savecontrolon = PlayScreen.IsControlOn();
        PlayScreen.SetControlOn(false);     // don't want them wandering off or anything

//      OldScrollLock = ScrollLock;
//      ScrollLock = false;
    }
    SetDirty(true);
}

void TDialogPane::Hide()
{
    if (IsHidden() || !IsOpen())
        return;

    TPane::Hide();

    PlayScreen.ShowLowerPanes();
    PlayScreen.Redraw();
    PlayScreen.SetControlOn(savecontrolon);
    PlayScreen.SetFullScreen(saveisfullscreen);

    character = nullptr;

//  ScrollLock = OldScrollLock;
}

#define CHOICEHEIGHT    21

void TDialogPane::DrawBackground()
{
    if (IsDirty())
    {
        Display.Put(0, 0, dialogdata->Bitmap("background"), DM_BACKGROUND);

        for (int32_t i = 0; i < numchoices; i++)
        {
            SColor color = { 255, 0, 50 };
            // (1998 pane, replaced by the retail entry manager; WriteText
            // isn't const-correct yet.)
            char *line = const_cast<char *>(DialogList.GetLine(choices[i]));
            Display.WriteText(line, 32, (i * CHOICEHEIGHT) + 4, 1, GameData->Font("choicefont"),
                                ((grabslot == i || choice == i) && highlighted) ? &color : nullptr);
        }

        SetDirty(false);
    }
}

void TDialogPane::Animate(bool draw)
{
}

void TDialogPane::KeyPress(int32_t key, bool down)
{
    if (down)
    {
        switch (key)
        {
            case '1':
                SetChoice(0);
                break;
            case '2':
                SetChoice(1);
                break;
            case '3':
                SetChoice(2);
                break;
            case '4':
                SetChoice(3);
                break;
            case ' ':
                Skip();
                break;
            case VK_ESCAPE:
                Skip();

                if (character)
                    character->ScriptJump("Finish");

                Close();
        }
    }
}

void TDialogPane::MouseClick(int32_t button, int32_t x, int32_t y)
{
    if (button == MB_LEFTDOWN)
    {
        if (numchoices > 0)
        {
            highlighted = true;
            grabslot = OnSlot(x, y);
            SetDirty(true);
        }
        else
            Skip();
    }
    else if (button == MB_LEFTUP && grabslot >= 0)
    {
        if (OnSlot(x, y) == grabslot)
            SetChoice(grabslot);

        grabslot = -1;
    }
}

void TDialogPane::MouseMove(int32_t button, int32_t x, int32_t y)
{
    if (button == MB_LEFTDOWN)
    {
        int32_t onslot = OnSlot(x, y);

        if ((onslot == grabslot && !highlighted) ||
            (onslot != grabslot && highlighted))
        {
            highlighted = !highlighted;
            SetDirty(true);
        }
    }
}

int32_t TDialogPane::OnSlot(int32_t x, int32_t y)
{
    if (!InPane(x, y))
        return -1;

    y -= 6;
    if (y < 0)
        return -1;

    y /= CHOICEHEIGHT;
    return y;
}

void TDialogPane::AddChoice(char *lab, char *txt)
{
    if (!IsOpen())
        return;

    if (choice >= 0)
        ResetResponses();

    if (numchoices >= MAXCHOICES)
        return;

    if (!txt)
        choices[numchoices] = nullptr;
    else
        choices[numchoices] = _strdup(txt);

    label[numchoices] = lab ? _strdup(lab) : nullptr;

    numchoices++;

    SetDirty(true);
}

void TDialogPane::ResetResponses()
{
    if (!IsOpen())
        return;

    for (int32_t i = 0; i < numchoices; i++)
    {
        if (choices[i])
            free(choices[i]);

        if (label[i])
            free(label[i]);
    }

    if (numchoices > 0)
        SetDirty(true);     // only redraw if there wasn't anything there before

    numchoices = 0;
    choice = -1;
    freshresponse = false;
    grabslot = -1;
}

void TDialogPane::Skip()
{
    if (!IsOpen())
        return;

    if (character && character->IsTalking())
        character->ForceCommandDone();
}


COMMAND(CmdChoice)
{
    if (t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;

    char buf[80];
    strcpy(buf, t.Text());

    t.WhiteGet();
    if (t.Type() != TKN_TEXT && t.Type() != TKN_IDENT)
        return CMD_BADPARAMS;

    DialogPane.AddChoice(buf, const_cast<char *>(t.Text()));

    t.Get();
    return 0;
}
