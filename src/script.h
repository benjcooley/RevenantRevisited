// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     script.h - Script functions                       *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "parse.h"
#include "command.h"

#define TRIGGER_NONE      0
#define TRIGGER_ALWAYS    1     // Always running script
#define TRIGGER_TRIGGER   2     // Manual trigger (triggered by the 'trigger' command in script)
#define TRIGGER_DIALOG    3     // Dialog trigger (triggered when char clicked)
#define TRIGGER_PROXIMITY 4     // Triggered a character gets within a certain range 
#define TRIGGER_CUBE      5     // Trigged when character or object enters given cube
#define TRIGGER_ACTIVATE  6     // Triggered when object is activated
#define TRIGGER_USE       7     // Triggered when character uses something
#define TRIGGER_GIVE      8     // Triggered when character gives something
#define TRIGGER_GET       9     // Triggered when character gets something
#define TRIGGER_COMBAT    10    // Triggered when character goes into combat mode
#define TRIGGER_DEAD      11    // Triggered when character dies

// ****************
// * TScriptProto *
// ****************

// Prototype class for scripts.

#define MAXSCRIPTNAME 20
#define MAXTRIGGERS 32

_STRUCTDEF(SScriptTrigger)
struct SScriptTrigger
{
    int32_t type;
    uint32_t pos;
    char name[MAXSCRIPTNAME];
    S3DRect cube;
    int32_t dist;
    int32_t priority;
};

typedef TVirtualArray<SScriptTrigger, 0, 4> TTriggerArray;

_CLASSDEF(TScriptProto)
class TScriptProto
{
  public:
    TScriptProto();// { name = nullptr; text = nullptr; next = nullptr; }
    TScriptProto(TScriptProto* pparent, void *powner, char *pfilename, char *pbuffer);
    ~TScriptProto();// { if (name) delete name; if (text) delete text; }
    bool ParseCriteria(TToken &t);
        // Parse criteria for the proto
    int32_t ParseScript(TToken &t);
        // Parse out the next block of text as the script
    bool WriteScript(FILE *fp);
        // Write script proto to file
    void SetBuffer(char *buffer);
        // Copy buffer into script's text
    void GetBuffer(char *buffer, int32_t buflen);
        // Umm...I guess it get's the buffer
    int32_t Length(){return len;}
        // Returns the lenghth of the script in bytes
    char *Text(){return text;}
        // Returns a pointer to the text
    TScriptProto* ParentProto(){return parent;}
        // Returns a pointer to the parent prototype

    bool FitsCriteria(TObjectInstance* inst);
        // Check to see if given object instance will use this script

    int32_t NumTriggers(){return numtriggers;}

    char *name;                             // Text for criteria
    char *text;                             // Text of script
//  TScriptProto* next;                     // Next in list
    TScriptProto* parent;                   // The parent in list
    TTriggerArray triggers;                 // Trigger array
    void *owner;                            // Pointer to an owner for script
    char *filename;
    int32_t len;
    int32_t numtriggers;                        // Number of triggers for this prototype
};

typedef TPointerArray<TScriptProto, 64, 64> TScriptProtoArray;

// ***********
// * TScript *
// ***********

// Script object which contains pseudo-code directing the attached object's actions.

#define SCRIPT_PAUSED       (1 << 16)       // indicates script is currently on hold

#define MAXDEPTH        10

#define COND_UNDEF      0xDEAF      // arbitrary, as int32_t as it is not true or false

_STRUCTDEF(SScriptBlock)
struct SScriptBlock
{
    uint32_t loopstart;                // Location to loop back to
    bool conditional;               // State of conditional for block
};

class TScript
{
  public:
    TScript();// { proto = nullptr; ip = nullptr; priority = 0; depth = 0; newtrigger = 0; lastpriority = 0;
                //block[depth].conditional = COND_UNDEF; block[depth].loopstart = nullptr; }
        // Init script without data
    TScript(TScriptProto* prototype);
        // Init script from the given buffer
    ~TScript();
        // Destory the script without saving

    bool Load(char *filename);
        // Read the script from the given file
    bool Save(char *filename);
        // Outputs script to given file

    void SetText(char *buf);
        // Copy contents of buf as script's new text
    char *Text() { if (curproto) return curproto->text; else return nullptr; }
        // Get a pointer to the script's text

    void Start(TScriptProto* proto = nullptr, int32_t pos = 0, int32_t newpriority = 0);
        // Begin script execution at the given location and priority
    void StartTrigger(TScriptProto* proto, PSScriptTrigger st);
        // Begin triger 
    void Continue(TObjectInstance* context);
        // Continue script exectuion
    void Jump(TObjectInstance* context, char *label);
        // Jump to the label
    void Break();
        // Temporarily interrupt script execution
    void Resume();
        // Restart an interrupted script
    bool IsPaused() { return (priority & SCRIPT_PAUSED); }
        // Check execution status of script
    void End();
        // Terminate script execution
    bool Running() { return priority > 0; }
        // Returns true if the script is already running
    void Trigger(int32_t newtrig, const char *triggerstr = nullptr)
      { newtrigger = newtrig; if (triggerstr) strcpy(newtriggerstr, triggerstr); }
        // Manually triggers the given script handler
    int32_t GetTrigger() { return trigger; }
        // Returns current trigger type executing
    int32_t GetPriority() { return priority; }
        // Returns the priority of the script executing (also is id of specific script block)
    TScriptProto* GetScriptProto() { return proto; }
        // Returns the prototype for this script

    static void PauseAllScripts(){ pauseall = true; }
        // Causes all scripts to pause
    static void ResumeAllScripts(){ pauseall = false; }
        // Causes all scripts to resume playing

  private:
    bool Triggered(PSScriptTrigger st, int32_t priority, TObjectInstance* context);
        // Returns true if the current block was triggered

    static bool pauseall;                   // True if all scripts paused

    TScriptProto* proto;                    // Pointer to script prototype
    TScriptProto* topproto;                 // Pointer to the top prototype
    TScriptProto* curproto;                 // Pointer to the current prototype
    int32_t newtrigger;                         // Next trigger type to execute
    int32_t trigger;                            // Current trigger type executing 
    char newtriggerstr[MAXSCRIPTNAME];      // Name of what is triggering

    char *ip;                               // Next line to execute
    int32_t priority;                           // Priority of current ip (is also the id of trigger block)
    int32_t lastpriority;                       // Last trigger executed (is also the id of trigger block)

    SScriptBlock block[MAXDEPTH];           // For conditionals, loops etc
    int32_t depth;                              // Number of blocks deep

    PSScriptTrigger curtrigger;             // The Current Trigger??
};

// **************
// * TGameState *
// **************

#define MAXGAMESTATES       4096

#define STATE_INVALID       -2000000000

class TGameState
{
  public:
    TGameState() { numstates = 0; }
    ~TGameState()
        { for (int32_t i = 0; i < numstates; i++) if (statename[i]) delete statename[i]; }

    // Load and save gamestates to master definition file
    bool Load(char *filename);
    bool Save(char *filename);

    // Access functions
    int32_t NumStates() { return numstates; }
    int32_t State(int32_t index)
        { if ((uint32_t)index < (uint32_t)numstates) return state[index]; return STATE_INVALID; }
    void SetState(int32_t index, int32_t newval)
        { if ((uint32_t)index < (uint32_t)numstates) state[index] = newval; }
    char *StateName(int32_t index)
        { if ((uint32_t)index < (uint32_t)numstates) return statename[index]; return nullptr; }

    // Search functions
    int32_t FindStateIndex(char *name)
        { for (int32_t i = 0; i < numstates; i++)
            if (!stricmp(name, statename[i])) return i; return -1; }

    int32_t State(char *name) { return State(FindStateIndex(name)); }
    void SetState(char *name, int32_t newval) { SetState(FindStateIndex(name), newval); }

  private:
    int32_t numstates;
    int32_t state[MAXGAMESTATES];
    char *statename[MAXGAMESTATES];
};

// ******************
// * TScriptManager *
// ******************

// Manages all the scripts in the game.

class TScriptManager
{
  public:
    TScriptManager() { scriptsdirty = false; }

    bool Initialize();
    void Close();

    void ParseScripts(char *buffer, char *filename, void *owner);
        // Parse a buffer and chunk it into scripts

    bool Load(char *filename, void *owner = nullptr);
        // Read the script from the given file
    bool Save(char *filename, void *owner = nullptr);
        // Outputs script to given file
    void Clear(void *owner);
        // Clears all scripts for the given owner

    bool ReloadStates();
        // Reloads initial values for game states

    PTScript ObjectScript(TObjectInstance* inst);
        // Find script for the given instance

    int32_t GameState(char *name);
        // Find the gamestate's value (if context, get context state val)
    void SetGameState(char *name, int32_t newval) { gamestate.SetState(name, newval); }
        // Find the gamestate's value

    int32_t FindLocalVal(char *name);
        // Find a value local to the current trigger
    void SetLocalVal(int32_t index, int32_t value);
    void SetLocalVal(char *name, int32_t value) { SetLocalVal(FindLocalVal(name), value); }
        // Set a local value for use by the scripts
    int32_t GetLocalVal(int32_t index);
    int32_t GetLocalVal(char *name) { return GetLocalVal(FindLocalVal(name)); }
        // Get a local value

    void SetScriptsDirty() { scriptsdirty = true; }
        // For manual changes to the script

    TScriptProto* FindScriptProto(char *name);
        // Used to find the parent prototype

  private:
    TGameState gamestate;                   // Game states for scripts
    TScriptProtoArray scripts;              // Pointer to head of linked list

    bool scriptsdirty;                      // Whether scripts have been saved to disk
};
