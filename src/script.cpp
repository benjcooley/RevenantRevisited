// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    Script.cpp - Script functions                      *
// *************************************************************************

#include <stdio.h>
#include <io.h>
#include <ctype.h>
#include <math.h>

#include "revenant.h"
#include "script.h"
#include "parse.h"
#include "object.h"
#include "command.h"
#include "mappane.h"
#include "dialog.h"
#include "file.h"
#include "exit.h"
#include "player.h"
#include "textbar.h"

int32_t TScript::pauseall = false;

TObjectInstance* TakenObject = nullptr;
TObjectInstance* DroppedObject = nullptr;

void ScriptError(char *buf, int32_t linenum)
{
    char buf2[100];
    sprintf(buf2, "Script error at line %d: %s\n", linenum, buf);
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

TScript::TScript(TScriptProto* prototype)
{
    topproto = curproto = prototype;
    ip = nullptr;
    priority = 0;
    depth = 0;
    lastpriority = 0;
    newtrigger = 0;
    newtriggerstr[0] = nullptr;
    block[0].conditional = COND_UNDEF;
    block[0].loopstart = nullptr;
}

TScript::TScript() 
{ 
    topproto = curproto = nullptr; 
    ip = nullptr; 
    priority = 0; 
    depth = 0; 
    newtrigger = 0; 
    lastpriority = 0;
    block[depth].conditional = COND_UNDEF; 
    block[depth].loopstart = nullptr; 
//  ScriptManager.AddScript(this); 
}

TScript::~TScript()
{
}

void TScript::SetText(char *buf)
{
    topproto->SetBuffer(buf);
    ScriptManager.SetScriptsDirty();
}
 
void TScript::Start(TScriptProto* proto, int32_t pos, int32_t newpriority)
{
    if (!proto)
        curproto = topproto;
    else
        curproto = proto;

    if (pos < 0 || pos >= curproto->Length())
        return;

    ip = curproto->Text() + pos;
}

void TScript::StartTrigger(TScriptProto* proto, PSScriptTrigger st)
{
    Start(proto, st->pos, st->priority);
    trigger = st->type;         // Set current trigger we're going to do
    newtrigger = 0;             // Set manual new trigger (if any) to 0
    newtriggerstr[0] = nullptr;    // Set manual new trigger key (if any) to nullptr
}

bool TScript::Triggered(PSScriptTrigger st, int32_t priority, TObjectInstance* context)
{
    bool retval = false;

  // Are we already doing this trigger?
    if (curtrigger == st || st->priority < priority)
        return false;

  // ******************
  // !!!!!REMEMBER!!!!!
  // ******************
  
  // This function is being called often, so 
  // make sure you don't do anything to intense
  // here.  KEEP IT SIMPLE AND FAST!

    switch (st->type)
    {
      case TRIGGER_ALWAYS:      // This trigger will always fire if it has a higherer priority
      {
        retval = true;
        break;
      }
      case TRIGGER_TRIGGER:     // This is the manually fired trigger (triggered by script 'trigger' command)
      {
        if (newtrigger == TRIGGER_TRIGGER && 
          !stricmp(st->name, newtriggerstr))
            retval = true;
        break;
      }
      case TRIGGER_DIALOG:      // This trigger is triggered by clicking on a friendly character
      {
        if (newtrigger == TRIGGER_DIALOG)
            retval = true;
        break;
      }
      case TRIGGER_PROXIMITY:   // This allow you to set a floating proximity field for a character
      {                         // Slightly slow, so be careful where you use this
        if (newtrigger == TRIGGER_PROXIMITY)
            retval = true;
        else 
        {
            if (Player && !stricmp(st->name, Player->GetName()))
                retval = context->Pos().InRange(Player->Pos(), st->dist);
            else
            {
//              TObjectInstance* inst = MapPane.ObjectInRange(context->Pos(), st->dist, OBJSET_CHARACTER);
//              if (st->name[0] == nullptr || !stricmp(st->name, inst->GetName()))
//                  retval = true;
            }
        }
        break;
      }
      case TRIGGER_CUBE:        // Nice and fast cube trigger
      {
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
            if (inst && (st->name[0] == nullptr || !stricmp(st->name, inst->GetName())))
                retval = true;
        }
        break;
      }
      case TRIGGER_ACTIVATE:    // This trigger is triggered when object is activated
      {
        if (newtrigger == TRIGGER_ACTIVATE)
            retval = true;
        break;
      }
      case TRIGGER_USE:         // Triggered when character uses an object
      {
        if (newtrigger == TRIGGER_USE && 
          !stricmp(st->name, newtriggerstr))
            retval = true;
        break;
      }
      case TRIGGER_GIVE:        // Triggered when this character give another char an object
      {
        if (newtrigger == TRIGGER_GIVE && 
          !stricmp(st->name, newtriggerstr))
            retval = true;
        break;
      }
      case TRIGGER_GET:         // Triggered when this character gets an object
      {
        if (newtrigger == TRIGGER_GET && 
          !stricmp(st->name, newtriggerstr))
            retval = true;
        break;
      }
      case TRIGGER_COMBAT:      // Triggered when this character enters combat mode
      {
        if (newtrigger == TRIGGER_COMBAT)
            retval = true;
        break;
      }
      case TRIGGER_DEAD:        // Triggered when we die (Blahhh.. uuhhhh!!)
      {
        if (newtrigger == TRIGGER_DEAD)
            retval = true;
        break;
      }
    }

    return retval;
}

#define MAXITERATIONS   6000            // for catching endless loops

void TScript::Continue(TObjectInstance* context)
{
    if (priority & SCRIPT_PAUSED || pauseall)
        return;

  // Setup stream and token
    TStringParseStream s(curproto->text, curproto->len);
    TToken t(s);

    // Check Triggers For Interruption
    // *******************************

    TScriptProto* proto = topproto;
    bool foundtrigger = false;
    while (proto)
    {
        for (int32_t c = 0; c < proto->NumTriggers(); c++)
        {
            if (Triggered(&proto->triggers[c], priority, context))  // Check to see if script is triggered
            {
                foundtrigger = true;
                StartTrigger(proto, &proto->triggers[c]);
                break;
            }
        }
        if (foundtrigger)
            break;
        proto = proto->ParentProto();
    }

    // Now Continue Script
    // *******************

    // now execute the current thread
    s.SetPos((uint32_t)ip);

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

        uint32_t thisline = s.GetPos();            // hang onto it in case of loop
        if (thisline > (uint32_t)ip && !isspace(*((char *)thisline)) &&
                                    !isspace(*((char *)thisline - 1)))
            thisline--;                         // token code jacks the pointer sometimes

        t.Get();
        t.SkipBlanks();

        if (t.Type() == TKN_SYMBOL && t.Code() == ':')
        {
            // label - ignore this line
            t.SkipLine();
        }
        else if (t.Type() == TKN_IDENT || t.Type() == TKN_KEYWORD)
        {
            // call the command interpreter
            int32_t bits = CommandInterpreter(context, t);  // ****** MAIN COMMAND PROCESSOR HERE *****

            // interpret the return code(s)
            if (bits & CMD_DELETED)
                return;                 // ack!

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
                block[++depth].loopstart = nullptr;
                block[depth].conditional = COND_UNDEF;
            }

            if (bits & CMD_END)
            {
                if (--depth < 0)
                    ScriptError("END without matching BEGIN", t.LineNum());
            }

            if (block[depth].loopstart != nullptr)
            {
                s.SetPos(block[depth].loopstart);
                block[depth].loopstart = nullptr;
            }

            if (bits & CMD_LOOP)
                block[depth].loopstart = thisline;

            if (bits & CMD_WAIT)
            {
                ip = (char *)s.GetPos();
                break;
            }

            if (bits & CMD_JUMP)
            {
                s.SetPos((uint32_t)ip);
            }
        }
        else
            ScriptError("Bad token in trigger block", t.LineNum());

        // skip over any extra crap on the line
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

    // Nothing to trigger
    if (ip == nullptr) // Script done
    {
        lastpriority = 0;
        if (!DialogPane.IsHidden() &&               // Dialog pane is still up and...
          DialogPane.GetCharacter() == context)     // This script was using the dialog pane
            DialogPane.Hide();
    }
}

void TScript::Jump(TObjectInstance* context, char *label)
{
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
                // we have a match
                t.SkipLine();
                break;
            }
        }

        t.LineGet();
    }

    ip = (char *)s.GetPos();        // save the new position
    depth = 1;          // a bit hacky - probably needs to count the begin/end pairs..
}

void TScript::Break()
{
    priority |= SCRIPT_PAUSED;
}

void TScript::Resume()
{
    priority &= ~SCRIPT_PAUSED;
}

void TScript::End()
{
    ip = nullptr;
    lastpriority = priority = 0;
}

// ****************
// * TScriptProto *
// ****************

TScriptProto::TScriptProto(TScriptProto* pparent, void *powner, char *pfilename, char *pbuffer)
{
    parent = pparent;
    owner = powner;
    filename = new char[strlen(pfilename) + 1];
    strcpy(filename, pfilename);
    name = nullptr;
    text = nullptr;
    len = 0;
    if (pbuffer)
        SetBuffer(pbuffer);
}

TScriptProto::~TScriptProto()
{
    if (name)
        free(name);
    if (filename)
        free(filename);
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
    len = strlen(text);

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

bool TScriptProto::ParseCriteria(TToken &t)
{
    if (t.Is("CONTEXT") || t.Is("OBJTYPE") || t.Is("OBJECT"))
    {
        t.WhiteGet();
        if (t.Type() != TKN_IDENT && t.Type() != TKN_TEXT)
            ScriptError("Expected object context identifier", t.LineNum());
        name = strdup(t.Text());
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

int32_t TScriptProto::ParseScript(TToken &t)
{
    char buf[80];
    char trigname[20];

  // Clear current list of triggers before we begin
    triggers.Clear();

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
    int32_t depth = 0;

  // Now get first trigger token
    t.LineGet();
    
  // Iterate through the triggers and setup trigger list
    while (t.Type() != TKN_EOF && !t.Is("END"))
    {
      // Parse trigger tags now
        if (t.Type() != TKN_IDENT)
            ScriptError("Trigger identifier expected", t.LineNum());

      // Setup basic trigger structure
        SScriptTrigger st;
        memset(&st, 0, sizeof(SScriptTrigger));
        st.type = 0;

      // Tags...
        strcpy(trigname, t.Text());
        if (t.Is("ALWAYS"))                 // ALWAYS trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_ALWAYS;
        }
        else if (t.Is("TRIGGER"))                   // ALWAYS trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_TRIGGER;

            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
        }
        else if (t.Is("DIALOG"))            // DIALOG trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_DIALOG;
        }
        else if (t.Is("PROXIMITY"))         // CUBE trigger
        {
            t.WhiteGet();

            st.type = TRIGGER_PROXIMITY;
            st.dist = 256;

            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }

            if (t.Type() == TKN_NUMBER)
                Parse(t, "%i", &st.dist);
        }
        else if (t.Is("CUBE"))              // CUBE trigger
        {
            t.WhiteGet();

            st.type = TRIGGER_CUBE;

            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
            else
                strncpyz(st.name, name, MAXSCRIPTNAME);

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
        else if (t.Is("ACTIVATE"))          // ACTIVATE trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_ACTIVATE;
        }
        else if (t.Is("USE"))               // USE trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_USE;
            
            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
        }
        else if (t.Is("GIVE"))              // GIVE trigger
        {
            t.WhiteGet();
            st.type = TRIGGER_GIVE;
            
            if (t.Type() == TKN_IDENT || t.Type() == TKN_TEXT)
            {
                strncpyz(st.name, t.Text(), MAXSCRIPTNAME);
                t.WhiteGet();
            }
        }
        else if (t.Is("GET"))               // GET trigger
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
            sprintf(buf, "Unknown trigger %s", t.Text());
            ScriptError(buf, t.LineNum());
        }

        if (t.Type() != TKN_RETURN)
        {
            sprintf(buf, "Bad parameter for %s trigger", trigname);
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

    int32_t len = (int32_t)(t.GetPos() - start - 4); // Get length of buffer - 4 for end token (END\n)
    text = new char[len+1];
    memcpy(text, (void *)start, len);
    text[len] = 0;

    t.LineGet();

    return len;
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

bool TGameState::Load(char *filename)
{
    char fname[MAXPATHLEN];
    sprintf(fname, "%s%s", ClassDefPath, filename);

    FILE *fp = fopen(fname, "rb");
    if (!fp)
        FatalError("Unable to find game state file %s", filename);

    TFileParseStream s(fp, fname);
    TToken t(s);

    t.LineGet();

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

    FILE *fp = popen(fname, "wt");
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

// ******************
// * TScriptManager *
// ******************

bool TScriptManager::Initialize()
{
    scripts.Clear();

    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(nullptr);
        _RPT0(_CRT_ERROR, "Memory Error");
    }

    return (Load("master.s") && gamestate.Load("state.def"));
}

void TScriptManager::Close()
{
    if (Editor)
    {
        Save("master.s");
        gamestate.Save("state.def");
    }

    scripts.DeleteAll();
}

bool TScriptManager::Load(char *filename, void *owner)
{
    char fname[MAXPATHLEN];

    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(nullptr);
        _RPT0(_CRT_ERROR, "Memory Error");
    }

    sprintf(fname, "%s%s", ClassDefPath, filename);

    FILE *fp = TryOpen(fname, "rb");
    if (fp == nullptr)
        FatalError("Unable to find game master script file %s", filename);

    bool retval = true;
    int32_t bufsize = _filelength(fileno(fp));
    char *buffer = new char[bufsize+1];

    if (fread(buffer, 1, bufsize, fp) < (size_t)bufsize)
        retval = false;
    else
    {
        buffer[bufsize] = 0;            // null-terminate it
        ParseScripts(buffer, filename, owner);
    }

    delete buffer;
    fclose(fp);

    if (MapPane.IsOpen())
        MapPane.Notify(N_SCRIPTADDED, nullptr);

    scriptsdirty = false;

    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(nullptr);
        _RPT0(_CRT_ERROR, "Memory Error");
    }

    return retval;
}

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
}   

TScriptProto* TScriptManager::FindScriptProto(char *name)
{
    for (int32_t c = 0; c < scripts.NumItems(); c++)
    {
        if (!stricmp(scripts[c]->name, name))
            return scripts[c];
    }
    
    return nullptr;
}

bool TScriptManager::ReloadStates()
{
    return gamestate.Load("state.def");
}

void TScriptManager::ParseScripts(char *buffer, char *filename, void *owner)
{
    TStringParseStream s(buffer, strlen(buffer));
    TToken t(s);

    t.Get();

    while (t.Type() != TKN_EOF)
    {
        TScriptProto* script = new TScriptProto(nullptr, owner, filename, buffer);
        script->ParseScript(t);

        for (int32_t c = 0; c < scripts.NumItems(); c++) // Add into unused entries
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


// This finds all the scripts which apply to a given object instance and put pointers to
// them into the new script object (which is returned).
PTScript TScriptManager::ObjectScript(TObjectInstance* inst)
{
    for (int32_t c = 0; c < scripts.NumItems(); c++)
    {
        if (!scripts.Used(c))
            continue;

        if (scripts[c]->FitsCriteria(inst))
        {
            PTScript newscript = new TScript(scripts[c]);
            return newscript;
        }
    }

    return nullptr;
}

struct { char *name; int32_t val; } LocalScriptVals[] =
{
    { "Name",               0   },
    { "IsShopObject",       0   },
    { "Tab",                0   },

    { nullptr }        // terminator
};

int32_t TScriptManager::FindLocalVal(char *name)
{
    for (int32_t i = 0; LocalScriptVals[i].name; i++)
        if (stricmp(name, LocalScriptVals[i].name) == 0)
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

int32_t TScriptManager::GameState(char *name)
{
    // first check the constants set up by the currently executing script trigger
    int32_t index = FindLocalVal(name);
    if (index >= 0)
        return GetLocalVal(index);

    // if it's not there, check the game state variables (globals)
    return gamestate.State(name);
}