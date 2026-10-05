// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    Script.cpp - Script functions                      *
// *                                                                       *
// * REVSYNC: per-method retail sync 2026-05-17. See script.h header for   *
// * the class-level map; individual method `REVSYNC:` markers below cite  *
// * the recon/discovered/cls_*.cpp addresses each implementation was      *
// * cross-checked against.                                                *
// *                                                                       *
// * Hot-path note: TScript::Continue / Triggered keep the pre-release C++ *
// * because the retail bodies hook engine subsystems (dialog HUD, player  *
// * FSM, multi-context vftable slots 0x148/0x14c/0x154/0x1c0) that have   *
// * not been ported yet. The trigger machine shape — TRIGGER_* opcodes,   *
// * priority gating, MAXITERATIONS infinite-loop guard, BEGIN/END depth   *
// * stack — matches retail. TODO(revsync) markers flag each divergence.   *
// *************************************************************************

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "revenant.h"
#include "script.h"
#include "logging.h"
#include "parse.h"
#include "object.h"
#include "command.h"
#include "mappane.h"
#include "dialog.h"
#include "file.h"
#include "module.h"
#include "revutils.h"
#include "exit.h"
#include "player.h"
#include "stream.h"
#include "textbar.h"

#include <algorithm>

bool TScript::pauseall = false;

TObjectInstance* TakenObject = nullptr;
TObjectInstance* DroppedObject = nullptr;

void ScriptError(const char *buf, int32_t linenum)
{
    char buf2[100];
    snprintf(buf2, sizeof(buf2), "Script error at line %d: %s\n", linenum, buf);
    if (Editor)
        Output(buf2);
    else
        TextBar.Print(buf2);
}

void SkipBlock(TToken &t)
{
    if (!t.SkipBlock())
        ScriptError("BEGIN without matching END", 0);
}

// ***********
// * TScript *
// ***********

// REVSYNC: ctor @ 0x00492170
TScript::TScript(TScriptProto* prototype)
{
    topproto = curproto = prototype;
    proto = prototype;
    block[0].conditional = COND_UNDEF;
    block[0].loopstart = 0;
    ScriptManager.RegisterScript(this);
}

TScript::TScript()
{
    block[0].conditional = COND_UNDEF;
    block[0].loopstart = 0;
    ScriptManager.RegisterScript(this);
}

TScript::~TScript()
{
    ScriptManager.UnregisterScript(this);
}

// REVSYNC: SetText @ 0x004944c0 — retail allocates via FUN_00482ef0 (strdup-like) and
// clears the upper script flags except bit 0x20000 (paused-by-context). Our pre-release
// path mirrors only the proto-buffer side; the flag bookkeeping isn't observable
// because our Continue() doesn't gate on those bits.
void TScript::SetText(char *buf)
{
    if (topproto)
        topproto->SetBuffer(buf);
    ScriptManager.SetScriptsDirty();
}

void TScript::Start(TScriptProto* startproto, int32_t pos, int32_t /*newpriority*/)
{
    if (!startproto)
        curproto = topproto;
    else
        curproto = startproto;

    if (!curproto || pos < 0 || pos >= curproto->Length())
        return;

    ip = curproto->Text() + pos;
}

void TScript::StartTrigger(TScriptProto* startproto, PSScriptTrigger st)
{
    Start(startproto, st->pos, st->priority);
    trigger = st->type;         // Set current trigger we're going to do
    newtrigger = 0;             // Set manual new trigger (if any) to 0
    newtriggerstr[0] = '\0';    // Set manual new trigger key (if any) to nullptr
    curtrigger = st;
}

// REVSYNC: Triggered @ 0x00492d70 — retail body is 199 lines of engine-coupled
// logic (dialog FSM, player combat state, vftable slot 0x8c/0x154/0x1c0 calls).
// We keep the pre-release per-trigger-type switch because it produces the right
// observable behaviour for the trigger types our ported engine actually emits.
// TODO(revsync): once Player/Dialog/Combat are retail-synced, replace this with
// the retail Triggered body and route through the new VFT slots.
bool TScript::Triggered(PSScriptTrigger st, int32_t curpriority, TObjectInstance* context)
{
    bool retval = false;

  // Are we already doing this trigger?
    if (curtrigger == st || st->priority < curpriority)
        return false;

  // ******************
  // !!!!!REMEMBER!!!!!
  // ******************
  //
  // This function is being called often, so make sure you don't do anything
  // too intense here. KEEP IT SIMPLE AND FAST!

    switch (st->type)
    {
      case TRIGGER_ALWAYS:      // This trigger will always fire if it has a higherer priority
        retval = true;
        break;

      case TRIGGER_TRIGGER:     // Manually fired trigger (script 'trigger' command)
        if (newtrigger == TRIGGER_TRIGGER && !stricmp(st->name, newtriggerstr))
            retval = true;
        break;

      case TRIGGER_DIALOG:      // Player clicked a friendly character
        if (newtrigger == TRIGGER_DIALOG)
            retval = true;
        break;

      case TRIGGER_PROXIMITY:   // Floating proximity field — slightly slow
        if (newtrigger == TRIGGER_PROXIMITY)
            retval = true;
        else if (Player && !stricmp(st->name, Player->GetName()))
            retval = context->Pos().InRange(Player->Pos(), st->dist);
        else
        {
            // TODO(revsync): retail also walks the moving-object set here
            //   (FUN_00492d70 LAB_00492ea2 path). Disabled in pre-release.
        }
        break;

      case TRIGGER_CUBE:        // Fast cube trigger
        if (newtrigger == TRIGGER_CUBE)
            retval = true;
        else if (!stricmp(st->name, context->GetName()))
        {
            if (st->cube.In(context->Pos()))
                retval = true;
        }
        else if (Player && !stricmp(st->name, Player->GetName()))
        {
            if (st->cube.In(Player->Pos()))
                retval = true;
        }
        else
        {
            TObjectInstance* inst = MapPane.ObjectInCube(&st->cube, OBJSET_MOVING);
            if (inst && (st->name[0] == '\0' || !stricmp(st->name, inst->GetName())))
                retval = true;
        }
        break;

      case TRIGGER_ACTIVATE:    // Object activated
        if (newtrigger == TRIGGER_ACTIVATE)
            retval = true;
        break;

      case TRIGGER_USE:         // Character used an object
        if (newtrigger == TRIGGER_USE && !stricmp(st->name, newtriggerstr))
            retval = true;
        break;

      case TRIGGER_GIVE:        // Character gave another char an object
        if (newtrigger == TRIGGER_GIVE && !stricmp(st->name, newtriggerstr))
            retval = true;
        break;

      case TRIGGER_GET:         // Character got an object
        if (newtrigger == TRIGGER_GET && !stricmp(st->name, newtriggerstr))
            retval = true;
        break;

      case TRIGGER_COMBAT:      // Entered combat mode
        if (newtrigger == TRIGGER_COMBAT)
            retval = true;
        break;

      case TRIGGER_DEAD:        // Died (Blahhh.. uuhhhh!!)
        if (newtrigger == TRIGGER_DEAD)
            retval = true;
        break;
    }

    return retval;
}

#define MAXITERATIONS   6000            // for catching endless loops

// REVSYNC: Continue @ 0x004933d0 — see header note. The retail body is 458 lines
// of engine-coupled logic; we keep the pre-release executor which matches the
// retail outer loop (trigger scan → token interpreter → CMD_* flag handling →
// MAXITERATIONS guard) but skips the context-vftable plumbing.
void TScript::Continue(TObjectInstance* context)
{
    if (priority & SCRIPT_PAUSED || pauseall)
        return;

    if (!curproto)
        return;

  // Setup stream and token
    TStringParseStream s(curproto->text, curproto->len);
    TToken t(s);

    // Check Triggers For Interruption
    // *******************************

    TScriptProto* p = topproto;
    bool foundtrigger = false;
    while (p)
    {
        for (int32_t c = 0; c < p->NumTriggers(); c++)
        {
            if (Triggered(&p->triggers[c], priority, context))
            {
                foundtrigger = true;
                StartTrigger(p, &p->triggers[c]);
                break;
            }
        }
        if (foundtrigger)
            break;
        p = p->ParentProto();
    }

    // Now Continue Script
    // *******************

    s.SetPos((uint32_t)(uintptr_t)ip);

    int32_t iterations = MAXITERATIONS;

    while (ip && (!(priority & SCRIPT_PAUSED)))
    {
      // Prevent main char self running demo script from interrupting this script...
        if (Player && context != Player && Player->GetScript() &&
            Player->GetScript()->Running() &&
            Player->GetScript()->GetTrigger() == TRIGGER_ALWAYS)
        {
            Player->GetScript()->End();
        }

        uint32_t thisline = s.GetPos();
        if (thisline > (uint32_t)(uintptr_t)ip && !isspace(*((char *)(uintptr_t)thisline)) &&
                                    !isspace(*((char *)(uintptr_t)thisline - 1)))
            thisline--;                         // token code jacks the pointer sometimes

        t.Get();
        t.SkipBlanks();

        if (t.Type() == TKN_SYMBOL && t.Code() == ':')
        {
            t.SkipLine();
        }
        else if (t.Type() == TKN_IDENT || t.Type() == TKN_KEYWORD)
        {
            int32_t bits = CommandInterpreter(context, t, 0, this);  // ****** MAIN COMMAND PROCESSOR HERE *****

            if (bits & CMD_DELETED)
                return;

            if (bits & CMD_CONDTRUE)
            {
                block[depth].conditional = true;
            }
            else if (bits & CMD_CONDFALSE)
            {
                t.LineGet();
                if (t.Is("BEGIN"))
                    SkipBlock(t);
                else
                    t.SkipLine();
                block[depth].conditional = false;
            }

            if (bits & CMD_ELSE)
            {
                if (block[depth].conditional == COND_UNDEF)
                    ScriptError("ELSE without matching IF", t.LineNum());
                else if (block[depth].conditional == true)
                {
                    t.LineGet();
                    if (t.Is("BEGIN"))
                        SkipBlock(t);
                    else
                        t.SkipLine();
                    block[depth].conditional = COND_UNDEF;
                }
            }

            if (bits & CMD_SKIPBLOCK)
                SkipBlock(t);

            if (bits & CMD_BEGIN)
            {
                block[++depth].loopstart = 0;
                block[depth].conditional = COND_UNDEF;
            }

            if (bits & CMD_END)
            {
                if (--depth < 0)
                    ScriptError("END without matching BEGIN", t.LineNum());
            }

            if (block[depth].loopstart != 0)
            {
                s.SetPos(block[depth].loopstart);
                block[depth].loopstart = 0;
            }

            if (bits & CMD_LOOP)
                block[depth].loopstart = thisline;

            if (bits & CMD_WAIT)
            {
                ip = (char *)(uintptr_t)s.GetPos();
                break;
            }

            if (bits & CMD_JUMP)
            {
                s.SetPos((uint32_t)(uintptr_t)ip);
            }
        }
        else
            ScriptError("Bad token in trigger block", t.LineNum());

        while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
            t.Get();

        if (depth < 1 || iterations-- < 1)
        {
            if (iterations < 1)
                ScriptError("Infinite loop detected\n", t.LineNum());

            ip = nullptr;
            priority = 0;
        }
    }

    if (ip == nullptr) // Script done
    {
        lastpriority = 0;
        if (!DialogPane.IsHidden() &&
            DialogPane.GetCharacter() == context)
            DialogPane.Hide();
    }
}

// REVSYNC: Jump @ 0x00493fa0 — retail walks to the current trigger by counting
// BEGIN/END pairs (1000-newpriority iterations) then matches `:label`. Our
// pre-release port has the same control-flow shape; the priority counter
// initial value matches.
void TScript::Jump(TObjectInstance* /*context*/, char *label)
{
    if (!curproto || !curproto->text)
        return;

    TStringParseStream s(curproto->text, strlen(curproto->text));
    TToken t(s);

    int32_t newpriority = 1000;

    // skip down to the current trigger
    while (t.Type() != TKN_EOF && --newpriority > priority)
    {
        SkipBlock(t);
        t.SkipBlanks();
    }

    // find the label
    while (1)
    {
        t.SkipBlanks();

        if (t.Type() == TKN_EOF)
        {
            ScriptError("Jump to an unknown label attempted", t.LineNum());
            Start();        // reset the script
            return;
        }

        if (t.Type() == TKN_SYMBOL && t.Code() == ':')
        {
            t.Get();
            if (t.Is(label))
            {
                t.SkipLine();
                break;
            }
        }

        t.LineGet();
    }

    ip = (char *)(uintptr_t)s.GetPos();
    depth = 1;          // a bit hacky - probably needs to count the begin/end pairs..
}

// REVSYNC: Break @ 0x004942a0
void TScript::Break()
{
    priority |= SCRIPT_PAUSED;
}

// REVSYNC: Resume @ 0x004942b0
void TScript::Resume()
{
    priority &= ~SCRIPT_PAUSED;
}

// REVSYNC: End @ 0x00493e40 — retail additionally rolls back PlayerFSM bits
// (FUN_0051d680_SetPlayerState) on flags 1/2/4/8. Those subsystems aren't
// ported yet; basic IP/priority teardown matches.
// TODO(revsync): re-port flag rollback once Player FSM exists.
void TScript::End()
{
    ip = nullptr;
    lastpriority = priority = 0;
}

void TScript::Reset()
{
    End();
    curproto = topproto;
    newtrigger = trigger = 0;
    newtriggerstr[0] = '\0';
    curtrigger = nullptr;
    depth = 0;
    block[0] = SScriptBlock{};
}

// ****************
// * TScriptProto *
// ****************

TScriptProto::TScriptProto() = default;

// REVSYNC: ctor @ 0x004946f0 — (parent, owner, filename, buffer). Retail allocates
// the trigger array struct (0x50 bytes) and zeros it (which `triggers` does via
// its member ctor), strdup's filename, and if buffer is non-null kicks off
// ParseScript via the inline TStringParseStream / TToken in the ctor frame.
TScriptProto::TScriptProto(TScriptProto* pparent, void *powner, char *pfilename, char *pbuffer)
{
    parent = pparent;
    owner = powner;
    if (pfilename)
    {
        filename = new char[strlen(pfilename) + 1];
        strcpy(filename, pfilename);
    }
    if (pbuffer)
    {
        TStringParseStream s(pbuffer, strlen(pbuffer));
        TToken t(s);
        t.LineGet();
        ParseScript(t);
    }
}

// REVSYNC: dtor @ 0x004948e0 — frees name/filename/text plus walks the trigger
// array slots (which TVirtualArray does for us in its dtor). Retail also walks
// param_1[+0x1c]/+0x20 a second time to clear a tail allocation; that's the
// TVirtualArray's interior buffer which our destructor handles automatically.
TScriptProto::~TScriptProto()
{
    if (name)
        delete[] name;
    if (filename)
        delete[] filename;
    if (text)
        free(text);
}

bool TScriptProto::FitsCriteria(TObjectInstance* inst)
{
    if (name && *name && stricmp(name, inst->GetName()) == 0)
        return true;
    return false;
}

void TScriptProto::SetBuffer(char *buffer)
{
    if (!buffer || strlen(buffer) < 1)
        return;

    if (text)
        free(text);

    text = strdup(buffer);
    len = (int32_t)strlen(text);

    TStringParseStream s(text, len);
    TToken t(s);

    t.LineGet();
//  ParseScript(t);
}

void TScriptProto::GetBuffer(char *buffer, int32_t buflen)
{
    if (!buffer || buflen <= 0)
        return;

    strncpyz(buffer, text, buflen);
}

// REVSYNC: ParseCriteria @ 0x00494c50.
bool TScriptProto::ParseCriteria(TToken &t)
{
    if (t.Is("CONTEXT") || t.Is("OBJTYPE") || t.Is("OBJECT"))
    {
        t.WhiteGet();
        if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
            ScriptError("Expected object context identifier", t.LineNum());

        if (name)
            delete[] name;
        name = new char[strlen(t.Text()) + 1];
        strcpy(name, t.Text());

        t.WhiteGet();

        if (t.Is("PARENT"))
        {
            t.WhiteGet();
            if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
                ScriptError("Expected object parent identifier", t.LineNum());

            parent = ScriptManager.FindScriptProto(t.Text());

            t.WhiteGet();
        }

        t.LineGet();
    }
    else
    {
        t.SkipLine();
        return false;
    }

    return true;
}

#define TRIGBUFGROW 512
#define MAXTRIGSIZE 256

// REVSYNC: ParseScript @ 0x00494e20. Trigger tag→type mapping verified against
// the retail switch ladder. Cube parsing canonicalises (x1,y1,z1)..(x2,y2,z2)
// into (min,max) per retail.
int32_t TScriptProto::ParseScript(TToken &t)
{
    char buf[80];
    char trigname[20];

  // Clear current list of triggers before we begin
    triggers.Clear();
    numtriggers = 0;

  // Skip initial blanks
    t.SkipBlanks();

  // Parse script header line
    ParseCriteria(t);

    t.SkipBlanks();
    if (!t.Is("BEGIN"))
        ScriptError("Object block BEGIN expected", t.LineNum());

    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
        t.Get();

  // Mark line after begin as beginning of script buffer
    uint32_t start = t.GetPos();

  // Now get first trigger token
    t.LineGet();

  // Iterate through the triggers and setup trigger list
    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
        if (t.Type() != TKN_IDENT)
            ScriptError("Trigger identifier expected", t.LineNum());

        SScriptTrigger st;
        memset(&st, 0, sizeof(SScriptTrigger));
        st.type = 0;

        strncpyz(trigname, t.Text(), sizeof(trigname));

        if (t.Is("ALWAYS"))
        {
            t.WhiteGet();
            st.type = TRIGGER_ALWAYS;
        }
        else if (t.Is("TRIGGER"))
        {
            t.WhiteGet();
            st.type = TRIGGER_TRIGGER;

            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
        }
        else if (t.Is("DIALOG"))
        {
            t.WhiteGet();
            st.type = TRIGGER_DIALOG;
        }
        else if (t.Is("PROXIMITY"))
        {
            t.WhiteGet();

            st.type = TRIGGER_PROXIMITY;
            st.dist = 256;          // REVSYNC: retail default uStack_338 = 0x100

            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }

            if (t.Type() == TKN_NUMBER)
                Parse(t, "%i", &st.dist);
        }
        else if (t.Is("CUBE"))
        {
            t.WhiteGet();

            st.type = TRIGGER_CUBE;

            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
            else
            {
                if (name) strncpyz(st.name, name, MAXSCRIPTNAME);
                else st.name[0] = '\0';
            }

            int32_t x1, y1, z1, x2, y2, z2;

            if (!Parse(t, "%i,%i,%i %i,%i,%i", &x1, &y1, &z1, &x2, &y2, &z2))
                ScriptError("Invalid cube trigger", t.LineNum());

            st.cube.beg.x = min(x1, x2);
            st.cube.beg.y = min(y1, y2);
            st.cube.beg.z = min(z1, z2);
            st.cube.end.x = max(x1, x2);
            st.cube.end.y = max(y1, y2);
            st.cube.end.z = max(z1, z2);
        }
        else if (t.Is("ACTIVATE"))
        {
            t.WhiteGet();
            st.type = TRIGGER_ACTIVATE;
        }
        else if (t.Is("USE"))
        {
            t.WhiteGet();
            st.type = TRIGGER_USE;

            // REVSYNC: retail (LAB_00494ffb path) falls back to proto->name when
            //   no identifier follows USE. Pre-release left it empty.
            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
            else if (name)
            {
                strncpyz(st.name, name, MAXSCRIPTNAME);
            }
        }
        else if (t.Is("GIVE"))
        {
            t.WhiteGet();
            st.type = TRIGGER_GIVE;

            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
        }
        else if (t.Is("GET"))
        {
            t.WhiteGet();
            st.type = TRIGGER_GET;

            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
        }
        else if (t.Is("COMBAT"))
        {
            t.WhiteGet();
            st.type = TRIGGER_COMBAT;
        }
        else if (t.Is("DEAD"))
        {
            t.WhiteGet();
            st.type = TRIGGER_DEAD;
        }
        else
        {
            // REVSYNC: retail aborts the whole file on unknown trigger
            //   (LOAD STOPPED, returns 0xffffffff). We escalate to a
            //   non-fatal error to keep editing flow tolerant.
            snprintf(buf, sizeof(buf), "Unknown trigger %s", t.Text());
            ScriptError(buf, t.LineNum());
        }

        if (t.Type() != TKN_RETURN)
        {
            snprintf(buf, sizeof(buf), "Bad parameter for %s trigger", trigname);
            ScriptError(buf, t.LineNum());
            t.SkipLine();
        }

        st.pos = t.GetPos() - start;

        t.SkipBlanks();

        if (st.type != 0)
        {
            triggers.Add(st);
            numtriggers++;
        }

        if (!t.Is("BEGIN"))
        {
            ScriptError("Trigger BEGIN expected", t.LineNum());
            t.SkipLine();
        }

        t.SkipBlock();

        t.SkipBlanks();
    }

    if (!t.Is("END"))
        ScriptError("Object block END expected", t.LineNum());

    // Extract the trigger body from the full-buffer copy SetBuffer left on
    // text. start/GetPos are offsets into the parser's source buffer, which
    // has the same contents. The "-4" trims the trailing "END\n".
    int32_t bodylen = (int32_t)(t.GetPos() - start - 4);
    if (bodylen < 0) bodylen = 0;
    char *newtext = (char *)malloc(bodylen + 1);
    if (text)
    {
        memcpy(newtext, text + start, bodylen);
        free(text);
    }
    newtext[bodylen] = 0;
    text = newtext;
    len = bodylen;

    t.LineGet();

    return bodylen;
}

bool TScriptProto::WriteScript(FILE *fp)
{
    if (fprintf(fp, "OBJECT \"%s\"\r\n", name) < 0)
        return false;

    if (fputs("begin\r\n", fp) == EOF || fputs(text, fp) == EOF || fputs("end\r\n\r\n", fp) == EOF)
        return false;

    return true;
}

// **************
// * TGameState *
// **************

// REVSYNC: Load @ 0x00495cf0 — text .def parser; the active module's copy of
// the file, else the shared one.
bool TGameState::Load(char *filename)
{
    const std::string fname_str = ModuleManager.DataFilePath(filename);
    const char *fname = fname_str.c_str();

    FILE *fp = rev_fopen(fname, "rb");
    if (!fp)
        FatalError("Unable to find game state file %s", filename);

    TFileParseStream s(fp, fname);
    TToken t(s);

    t.LineGet();

    // Free any existing names before we reset numstates.
    for (int32_t i = 0; i < numstates; i++)
    {
        if (statename[i])
        {
            delete[] statename[i];
            statename[i] = nullptr;
        }
    }
    numstates = 0;

    while (t.Type() != TKN_EOF)
    {
        t.SkipBlanks();
        if (t.Type() == TKN_IDENT)
        {
            statename[numstates] = new char[strlen(t.Text()) + 1];
            strcpy(statename[numstates], t.Text());
            t.WhiteGet();

            if (t.Is("="))
                t.WhiteGet();

            if (t.Type() != TKN_NUMBER)
                FatalError("Invalid init value for game state in %s", filename);

            state[numstates] = t.Index();

            numstates++;

            t.LineGet();
        }
        else
            FatalError("Expected gamestate identifier in %s", filename);
    }

    fclose(fp);

    return true;
}

bool TGameState::Save(char *filename)
{
    char fname[MAXPATHLEN];
    sprintf(fname, "%s%s", ClassDefPath, filename);

    FILE *fp = rev_fopen(fname, "wt");
    if (!fp)
        return false;

    fprintf(fp, "// ********* Revenant Game States Save File ********\n"
                "// -------------------------------------------------\n\n"
                "// Revenant - Copyright 1998 Cinematix Studios, Inc.\n\n");

    for (int32_t c = 0; c < numstates; c++)
    {
        fprintf(fp, "%s=%d\n", statename[c], state[c]);
    }

    fclose(fp);

    return true;
}

namespace {

// Names are stored with the high bit set on every byte (retail ORs on save,
// XORs on load; state names are ASCII, so the two agree).
constexpr uint8_t kStateNameMask = 0x80;

}  // namespace

// REVSYNC: LoadStream @ 0x00496110
bool TGameState::LoadStream(TInputStream &is)
{
    if (is.Remaining() < 4)
        return false;
    int32_t count = 0;
    is >> count;

    for (int32_t i = 0; i < count; i++)
    {
        if (is.Remaining() < 1)
            return false;
        uint8_t length = 0;
        is >> length;
        if (is.Remaining() < length + 4)
            return false;

        char name[256];
        for (int32_t c = 0; c < length; c++)
        {
            uint8_t byte = 0;
            is >> byte;
            name[c] = (char)(byte ^ kStateNameMask);
        }
        name[length] = '\0';

        int32_t value = 0;
        is >> value;

        const int32_t index = FindStateIndex(name);
        if (index >= 0)
        {
            state[index] = value;
            continue;
        }
        if (numstates >= MAXGAMESTATES)
        {
            log_warn("[gamestate] save holds more than %d states; dropping '%s'",
                     MAXGAMESTATES, name);
            continue;
        }
        statename[numstates] = new char[length + 1];
        strcpy(statename[numstates], name);
        state[numstates] = value;
        numstates++;
    }
    return true;
}

// REVSYNC: SaveStream @ 0x004974d0
void TGameState::SaveStream(TOutputStream &os) const
{
    os.MakeFreeSpace(4);
    os << numstates;

    for (int32_t i = 0; i < numstates; i++)
    {
        const char *name = statename[i] ? statename[i] : "";
        const size_t length = std::min<size_t>(strlen(name), 255);
        os.MakeFreeSpace((int32_t)length + 5);
        os << (uint8_t)length;
        for (size_t c = 0; c < length; c++)
            os << (uint8_t)(name[c] | kStateNameMask);
        os << state[i];
    }
}

// ******************
// * TScriptManager *
// ******************

// REVSYNC: Initialize @ 0x00496240 — clears the 3 internal arrays then loads
// master.s + state.def. Retail zeros raw memory; we rely on the array ctors.
bool TScriptManager::Initialize()
{
    scripts.Clear();
    fileowners.Clear();

    const bool master_ok = Load("master.s");
    const bool state_ok  = gamestate.Load("state.def");
    log_info("[script] Initialize: master.s=%s state.def=%s, %d proto(s), %d gamestate(s)",
             master_ok ? "ok" : "FAIL",
             state_ok  ? "ok" : "FAIL",
             scripts.NumItems(),
             gamestate.NumStates());
    return master_ok && state_ok;
}

// REVSYNC: Close @ 0x00496330 — Save if editor + delete protos + delete
// file/owner records. Retail also clears the empty TVirtualArray slots; our
// containers do that in their destructors. REVSYNC-DIVERGENCE: retail also
// deleted the live script instances here; in the port their objects own and
// delete them (see the TScriptManager comment in script.h).
void TScriptManager::Close()
{
    if (Editor)
    {
        Save("master.s");
        gamestate.Save("state.def");
    }

    // Delete protos.
    scripts.DeleteAll();

    // Free file/owner records.
    for (int32_t i = 0; i < fileowners.NumItems(); i++)
    {
        if (fileowners.Used(i))
            fileowners.Delete(i);
    }
    fileowners.Clear();
}

// REVSYNC: Load @ 0x00496490 — slurps file, ParseScripts, registers owner.
// The active module's copy of the script, else the shared one.
bool TScriptManager::Load(char *filename, void *owner)
{
    const std::string fname_str = ModuleManager.DataFilePath(filename);
    const char *fname = fname_str.c_str();

    const int before = scripts.NumItems();

    FILE *fp = TryOpen(fname, "rb");
    if (fp == nullptr)
        FatalError("Unable to find game master script file %s", filename);

    bool retval = true;
    fseek(fp, 0, SEEK_END);
    int32_t bufsize = (int32_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buffer = new char[bufsize + 1];

    if (fread(buffer, 1, bufsize, fp) < (size_t)bufsize)
        retval = false;
    else
    {
        buffer[bufsize] = 0;
        ParseScripts(buffer, filename, owner);
    }

    delete[] buffer;
    fclose(fp);

    if (MapPane.IsOpen())
        MapPane.Notify(N_SCRIPTADDED, nullptr);

    scriptsdirty = false;

    // REVSYNC: register filename↔owner pair (retail trails the parse with this).
    // If a record for this filename already exists, just refresh its owner;
    // otherwise allocate a new record and add it.
    bool found = false;
    for (int32_t i = 0; i < fileowners.NumItems(); i++)
    {
        if (!fileowners.Used(i)) continue;
        if (!stricmp(fileowners[i]->filename, filename))
        {
            fileowners[i]->owner = owner;
            found = true;
            break;
        }
    }
    if (!found)
    {
        SScriptFileOwner *rec = new SScriptFileOwner();
        strncpyz(rec->filename, filename, sizeof(rec->filename));
        rec->owner = owner;
        fileowners.Add(rec);
    }

    log_info("[script] Load('%s'): +%d proto(s) (total %d)", filename,
             scripts.NumItems() - before, scripts.NumItems());
    return retval;
}

// REVSYNC: Save @ 0x00496690 — text serialise dirty protos for the owner.
bool TScriptManager::Save(char *filename, void *owner)
{
    if (!scriptsdirty)
        return true;

    char fname[MAXPATHLEN];

    sprintf(fname, "%s%s", ClassDefPath, filename);

    FILE *fp = TryOpen(fname, "wt");
    if (fp == nullptr)
        return false;

    fprintf(fp, "// ************** Revenant Script File  ************\n"
                "// -------------------------------------------------\n\n"
                "// Revenant - Copyright 1998 Cinematix Studios, Inc.\n\n");

    bool retval = true;
    for (int32_t c = 0; c < scripts.NumItems(); c++)
    {
        if (!scripts.Used(c))
            continue;

        if (scripts[c]->owner == owner)
        {
            if (!scripts[c]->WriteScript(fp))
                retval = false;
        }
    }

    fclose(fp);

    scriptsdirty = false;

    return retval;
}

// REVSYNC: Clear @ 0x004967a0 — drop protos AND file/owner records for owner.
// Retail walks the file/owner list backwards (so Collapse() doesn't shift
// indexes under us); we mirror that.
void TScriptManager::Clear(void *owner)
{
    for (int32_t c = 0; c < scripts.NumItems(); c++)
    {
        if (!scripts.Used(c))
            continue;

        if (scripts[c]->owner == owner)
        {
            MapPane.Notify(N_SCRIPTDELETED, scripts[c]);
            scripts.Delete(c);
        }
    }

    // REVSYNC: also prune file/owner records for this owner (retail second loop)
    for (int32_t c = fileowners.NumItems() - 1; c >= 0; c--)
    {
        if (!fileowners.Used(c)) continue;
        if (fileowners[c]->owner == owner)
            fileowners.Delete(c);
    }
}

TScriptProto* TScriptManager::FindScriptProto(const char *fname)
{
    for (int32_t c = 0; c < scripts.NumItems(); c++)
    {
        if (!scripts.Used(c)) continue;
        if (scripts[c]->name && !stricmp(scripts[c]->name, fname))
            return scripts[c];
    }

    return nullptr;
}

// REVSYNC: ReloadStates @ 0x004975c0
bool TScriptManager::ReloadStates()
{
    return gamestate.Load("state.def");
}

// REVSYNC: 0x00496e20
void TScriptManager::ResetScripts()
{
    for (TScript* script : instances)
        script->Reset();
}

void TScriptManager::RegisterScript(TScript* script)
{
    instances.push_back(script);
}

void TScriptManager::UnregisterScript(TScript* script)
{
    const auto it = std::find(instances.begin(), instances.end(), script);
    if (it != instances.end())
    {
        *it = instances.back();
        instances.pop_back();
    }
}

// REVSYNC: ParseScripts @ 0x00496860 — chunk buffer into TScriptProtos. Retail
// also handles a hot-reload diff path (mark-protos-stale → reparse → reattach
// to instances) which we don't exercise in the cold-load case; the per-proto
// allocation + ParseScript + registry insertion is identical.
void TScriptManager::ParseScripts(char *buffer, char *filename, void *owner)
{
    TStringParseStream s(buffer, strlen(buffer));
    TToken t(s);

    t.Get();

    while (t.Type() != TKN_EOF)
    {
        TScriptProto* script = new TScriptProto(nullptr, owner, filename, nullptr);
        script->ParseScript(t);

        int32_t c;
        for (c = 0; c < scripts.NumItems(); c++)
        {
            if (!scripts.Used(c))
            {
                scripts.Set(script, c);
                break;
            }
        }
        if (c >= scripts.NumItems())
            scripts.Add(script);
    }
}

// REVSYNC: ObjectScript @ 0x00497370 — match proto by instance name first
// (against inst+0x38 in retail), then fall back to class/type-name lookup
// (against inst+0x4c → TObjectClass.name in retail). Retail allocates the new
// TScript via FUN_00482fb0(0xe8) then runs the ctor; the `FUN_0041c840`
// follow-up call returns a registry id stored at +0x14 — we don't need that
// runtime id in our port (no global instance handle table).
PTScript TScriptManager::ObjectScript(TObjectInstance* inst)
{
    if (!inst) return nullptr;

    // Pass 1: match instance name.
    const char *instname = inst->GetName();
    if (instname && *instname)
    {
        for (int32_t c = 0; c < scripts.NumItems(); c++)
        {
            if (!scripts.Used(c)) continue;
            TScriptProto *sp = scripts[c];
            if (!sp->name || !*sp->name) continue;
            if (stricmp(sp->name, instname) == 0)
            {
                PTScript ns = new TScript(sp);
                log_info("[script] attached '%s' -> obj %s", sp->name, instname);
                return ns;
            }
        }
    }

    // Pass 2: match class/type name.
    TObjectClass *cls = TObjectClass::GetClass(inst->ObjClass());
    if (cls && cls->ClassName() && *cls->ClassName())
    {
        const char *classname = cls->ClassName();
        for (int32_t c = 0; c < scripts.NumItems(); c++)
        {
            if (!scripts.Used(c)) continue;
            TScriptProto *sp = scripts[c];
            if (!sp->name || !*sp->name) continue;
            if (stricmp(sp->name, classname) == 0)
            {
                PTScript ns = new TScript(sp);
                log_info("[script] attached '%s' -> obj %s (class match)", sp->name,
                         instname ? instname : "(unnamed)");
                return ns;
            }
        }
    }

    return nullptr;
}

// REVSYNC: AddScript @ 0x00497120 — editor / scripted dynamic-proto path. Retail
// builds a synthetic OBJECT block in a scratch buffer if no matching proto
// exists, parses it via ParseScript, then ctors a new TScript bound to the
// proto. Used by the editor's Insert-script and by a couple of script commands.
// Not exercised on the current boot path; preserved for parity.
PTScript TScriptManager::AddScript(TObjectInstance* /*inst*/, char *name, void *owner, char *buffer)
{
    // Find or create the matching proto.
    TScriptProto *sp = nullptr;
    for (int32_t c = 0; c < scripts.NumItems(); c++)
    {
        if (!scripts.Used(c)) continue;
        if (scripts[c]->name && !stricmp(scripts[c]->name, name))
        {
            sp = scripts[c];
            break;
        }
    }

    if (!sp)
    {
        const size_t synthlen = (buffer ? strlen(buffer) : 0) + strlen(name) + 32;
        char *synth = new char[synthlen];
        snprintf(synth, synthlen, "OBJECT %s\r\nBEGIN\r\n%s\r\nEND\r\n", name,
                 buffer ? buffer : "");

        sp = new TScriptProto(nullptr, owner, name, synth);
        scripts.Add(sp);
        delete[] synth;
    }

    PTScript ns = new TScript(sp);
    return ns;
}

// REVSYNC: LocalScriptVals — retail static table at PTR_DAT_005da0b0; same
// {Name, IsShopObject, Tab} entries pre-release used.
static struct { const char *name; int32_t val; } LocalScriptVals[] =
{
    { "Name",               0 },
    { "IsShopObject",       0 },
    { "Tab",                0 },
    { nullptr,              0 }     // terminator
};

int32_t TScriptManager::FindLocalVal(const char *fname)
{
    for (int32_t i = 0; LocalScriptVals[i].name; i++)
        if (stricmp(fname, LocalScriptVals[i].name) == 0)
            return i;

    return -1;
}

void TScriptManager::SetLocalVal(int32_t index, int32_t val)
{
    if (index >= 0)
        LocalScriptVals[index].val = val;
}

int32_t TScriptManager::GetLocalVal(int32_t index)
{
    if (index >= 0)
        return LocalScriptVals[index].val;

    return 0;
}

// REVSYNC: GameState lookup @ 0x004975d0 — local table first then global gamestate.
int32_t TScriptManager::GameState(const char *fname)
{
    // first check the constants set up by the currently executing script trigger
    int32_t index = FindLocalVal(fname);
    if (index >= 0)
        return GetLocalVal(index);

    // if it's not there, check the game state variables (globals)
    return gamestate.State(fname);
}
