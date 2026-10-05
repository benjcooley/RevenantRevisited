// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     script.h - Script functions                       *
// *                                                                       *
// * REVSYNC: layouts & API synced 2026-05-17 from retail decomps          *
// *   TScript          @ 0x00492170 (ctor, size 0xe8)                     *
// *   TScriptProto     @ 0x004946f0 (ctor, size 0x50)                     *
// *   TGameState       inlined in TScriptManager (32 KB)                  *
// *   TScriptManager   @ size 0x8044 (TGameState + 3x TVirtualArray)      *
// *                                                                       *
// * Trigger interpreter (TScript::Continue / Triggered) retains the       *
// * pre-release C++ implementation because the retail versions are        *
// * entangled with engine subsystems (dialog HUD, player combat FSM,      *
// * many context-vftable hooks) that are not yet ported. The trigger      *
// * matching shape — TRIGGER_* opcodes, per-proto array, priority/depth   *
// * stack, infinite-loop guard — matches retail. See                      *
// * recon/discovered/cls_TScript_Continue_4933d0.cpp for the retail body  *
// * and inline TODO(revsync) comments at the divergence points.           *
// *************************************************************************

#pragma once

#include "revenant.h"

#include "parse.h"
#include "command.h"
#include "saferef.h"

#include <string>
#include <vector>

// REVSYNC: trigger type constants confirmed against retail
//   cls_TScriptProto_ParseScript_494e20.cpp tag→id mapping
//   cls_TScript_Triggered_492d70.cpp switch on st->type
#define TRIGGER_NONE      0
#define TRIGGER_ALWAYS    1     // Always running script
#define TRIGGER_TRIGGER   2     // Manual trigger (triggered by the 'trigger' command in script)
#define TRIGGER_DIALOG    3     // Dialog trigger (triggered when char clicked)
#define TRIGGER_PROXIMITY 4     // Triggered when a character gets within a certain range
#define TRIGGER_CUBE      5     // Triggered when character or object enters given cube
#define TRIGGER_ACTIVATE  6     // Triggered when object is activated
#define TRIGGER_USE       7     // Triggered when character uses something
#define TRIGGER_GIVE      8     // Triggered when character gives something
#define TRIGGER_GET       9     // Triggered when character gets an object
#define TRIGGER_COMBAT    10    // Triggered when character goes into combat mode
#define TRIGGER_DEAD      11    // Triggered when character dies

// ****************
// * TScriptProto *
// ****************

// Prototype class for scripts.

#define MAXSCRIPTNAME 20
#define MAXTRIGGERS 32

// REVSYNC: SScriptTrigger from retail (cls_TScriptProto_ParseScript_494e20.cpp).
//   Field order: type(4) pos(4) name[20] cube(S3DRect, 24) dist(4) priority(4) = 60 bytes,
//   stored in a TVirtualArray on TScriptProto. ParseScript zeroes 0x50 bytes from
//   aiStack_36c — the inline buffer is rounded up but the live struct is 60.
_STRUCTDEF(SScriptTrigger)
struct SScriptTrigger
{
    int32_t  type      = 0;
    uint32_t pos       = 0;
    char     name[MAXSCRIPTNAME] = {};
    S3DRect  cube      = {};
    int32_t  dist      = 0;
    int32_t  priority  = 0;
};

typedef TVirtualArray<SScriptTrigger, 0, 4> TTriggerArray;

// REVSYNC: TScriptProto from cls_TScriptProto_Ctor_4946f0 + cls_TScriptProto_Dtor_4948e0.
//   Retail layout (offsets from ctor decomp):
//     0x00  name        (char*, OBJECT/CONTEXT/OBJTYPE identifier)
//     0x04  text        (char*, body of script between BEGIN/END, malloc'd)
//     0x08  next        (TScriptProto*, sibling in linked list — unused by us)
//     0x0c  parent      (TScriptProto*, ParseCriteria PARENT chain)
//     0x10..0x37 triggers (TVirtualArray<SScriptTrigger,0,4> + numtriggers,
//                          retail packs them inline; we keep our TVirtualArray member
//                          + a NumTriggers() accessor — same observable behaviour)
//     0x38  owner       (void*, area/editor that loaded this proto)
//     0x3c  filename    (char*, source file the proto came from)
//     0x40  len         (int32_t, length of `text`)
//     0x44  numtriggers (int32_t, count maintained beside the trigger array)
// Pre-release field ordering preserved here — modern compilers will lay it out
// differently than retail x86 32-bit but the observable semantics are identical;
// retail save-game compat for this object set isn't tracked yet.
_CLASSDEF(TScriptProto)
class TScriptProto
{
  public:
    TScriptProto();
    TScriptProto(TScriptProto* pparent, void *powner, char *pfilename, char *pbuffer);
    ~TScriptProto();

    // REVSYNC: ParseCriteria @ 0x00494c50
    bool ParseCriteria(TToken &t);
    // REVSYNC: ParseScript @ 0x00494e20 (triggers + body extraction)
    int32_t ParseScript(TToken &t);
    // REVSYNC: implicit via Save @ 0x00496690 (per-proto fprintf body)
    bool WriteScript(FILE *fp);
    void SetBuffer(char *buffer);
    void GetBuffer(char *buffer, int32_t buflen);
    [[nodiscard]] int32_t Length() const { return len; }
    [[nodiscard]] char *Text() const { return text; }
    [[nodiscard]] TScriptProto* ParentProto() const { return parent; }

    bool FitsCriteria(TObjectInstance* inst);
    [[nodiscard]] int32_t NumTriggers() const { return numtriggers; }

    char *name        = nullptr;            // Text for criteria (OBJECT/CONTEXT identifier)
    char *text        = nullptr;            // Text of script body (post-BEGIN..pre-END)
    TScriptProto* parent = nullptr;         // The parent in the proto chain
    TTriggerArray triggers;                 // Parsed trigger table
    void *owner       = nullptr;            // Opaque owner (TArea*, editor, ...)
    char *filename    = nullptr;            // Source filename
    int32_t len       = 0;                  // Body length in bytes (excludes trailing END)
    int32_t numtriggers = 0;                // Live count beside triggers (retail mirrors this)
};

typedef TPointerArray<TScriptProto, 64, 64> TScriptProtoArray;

// ***********
// * TScript *
// ***********

// Per-instance script executor. Retail size is 0xe8 (232 bytes); we don't
// match the retail offset map exactly because the runtime fields touch
// context-vftable slots (dialog HUD, combat FSM) that don't exist on our
// ported TObjectInstance yet. The trigger-machine shape, depth stack, and
// MAXITERATIONS guard are all retail-faithful.

#define SCRIPT_PAUSED       (1 << 16)       // indicates script is currently on hold

#define MAXDEPTH        10

#define COND_UNDEF      0xDEAF      // arbitrary, as int32_t as it is not true or false

_STRUCTDEF(SScriptBlock)
struct SScriptBlock
{
    uint32_t loopstart   = 0;          // Location to loop back to
    int32_t  conditional = COND_UNDEF; // State of conditional for block (uses COND_UNDEF + true/false)
};

// What a script is waiting for (retail TScript +0xb4; SCRIPT_ENGINE.md §5).
// Retail's type 5 is the multiplayer response wait and isn't used here.
enum class EScriptWait : uint8_t
{
    None              = 0,
    Response          = 2,    // the player picks a dialog choice
    CharDone          = 3,    // the object waited on finishes its action
    Frames            = 4,
    ScreenFade        = 6,
    BuySell           = 7,
    Say               = 8,    // the object waited on stops talking
    Death             = 9,
    ResponseControlOn = 10,   // "wait respctrlon"
};

class TScript
{
  public:
    TScript();
    // REVSYNC: ctor @ 0x00492170 — retail param order is (proto, owner); we
    // collapse `owner` into the proto's owner field (read via curproto->owner)
    explicit TScript(TScriptProto* prototype);
    ~TScript();

    TScript(const TScript&)            = delete;
    TScript& operator=(const TScript&) = delete;

    bool Load(char *filename);
    bool Save(char *filename);

    // REVSYNC: SetText @ 0x004944c0
    void SetText(char *buf);
    [[nodiscard]] char *Text() const { if (curproto) return curproto->text; else return nullptr; }

    void Start(TScriptProto* proto = nullptr, int32_t pos = 0, int32_t newpriority = 0);
    void StartTrigger(TScriptProto* proto, PSScriptTrigger st);
    // REVSYNC: Continue @ 0x004933d0 — retail body weaves in dialog/combat
    //   notify hooks via context vftable slots 0x148/0x14c/0x154 that don't
    //   exist on our TObjectInstance yet; we keep the pre-release loop which
    //   has identical observable semantics for the trigger types we exercise.
    //   TODO(revsync): re-port Continue when PlayerFSM + DialogPane are
    //   retail-synced and their vftable slots stabilise.
    // `commanddone`: the owner's current action has finished. Lines run only
    // when it has and nothing is being waited for, or when the wait is over.
    void Continue(TObjectInstance* context, bool commanddone);
    // REVSYNC: Jump @ 0x00493fa0
    void Jump(TObjectInstance* context, const char *label);
    // REVSYNC: Break @ 0x004942a0 — sets SCRIPT_PAUSED bit
    void Break();
    // REVSYNC: Resume @ 0x004942b0 — clears SCRIPT_PAUSED bit
    void Resume();
    [[nodiscard]] bool IsPaused() const { return (priority & SCRIPT_PAUSED) != 0; }
    // REVSYNC: End @ 0x00493e40 — retail also rolls back player FSM state
    //   bits (FUN_0051d680_SetPlayerState) when END runs while the script
    //   has stolen control away from the player. We don't have a player FSM
    //   port yet; the basic teardown matches.
    //   TODO(revsync): wire SetPlayerState rollback once player state machine
    //   is in place.
    void End();
    // REVSYNC: per-instance body of TScriptManager::ResetScripts @ 0x00496e20
    //   — End if running and return to the just-constructed state: top
    //   prototype, no pending or current trigger, no open blocks.
    void Reset();
    [[nodiscard]] bool Running() const { return priority > 0; }

    // REVSYNC: 0x00492640 -- asks for a manual trigger (TRIGGER, DIALOG,
    // ACTIVATE, USE, GIVE, GET, COMBAT, DEAD), which starts at the next
    // Continue. It needs a trigger of that type named `str` or `str2` (a
    // nameless request matches any). `user` and `second` are the objects the
    // event concerns; the block addresses them by their aliases ("user",
    // "item", "enemy"). Refused while another object's trigger runs here.
    bool Trigger(int32_t type, const char *str = nullptr, const char *str2 = nullptr,
                 TObjectInstance* user = nullptr, const char *useras = nullptr,
                 TObjectInstance* other = nullptr, const char *otheras = nullptr);
    // The object one of the running trigger's aliases names, or nullptr.
    [[nodiscard]] TObjectInstance* Alias(const char *name) const;
    // REVSYNC: AddChoice @ 0x004932a0 (single player) -- offers a choice
    // through the dialog pane; the script then owns the dialog until the
    // response (taken flag 4, which `choice` sets in retail).
    void AddChoice(const char *label, const char *text);
    [[nodiscard]] int32_t GetTrigger() const { return trigger; }
    [[nodiscard]] int32_t GetPriority() const { return priority; }
    [[nodiscard]] TScriptProto* GetScriptProto() const { return proto; }

    // ---- Waits (retail; SCRIPT_ENGINE.md §5) -------------------------
    // REVSYNC: SetWait @ 0x00492b00 — no effect while already waiting.
    void SetWait(EScriptWait type, TObjectInstance* object = nullptr, int32_t frames = 0);
    void WaitFrames(int32_t frames)          { SetWait(EScriptWait::Frames, nullptr, frames); } // 0x00492cc0
    void WaitChar(TObjectInstance* object)   { if (object) SetWait(EScriptWait::CharDone, object); } // 0x00492cf0
    void WaitSay(TObjectInstance* object)    { if (object) SetWait(EScriptWait::Say, object); }      // 0x00492d10
    void WaitDeath(TObjectInstance* object)  { if (object) SetWait(EScriptWait::Death, object); }    // 0x00492d30
    [[nodiscard]] bool IsWaiting() const     { return wait != EScriptWait::None; }

    // REVSYNC: User @ 0x00492ac0 — the player this script deals with: the
    // running trigger's "user" when that is a player, else the main player.
    [[nodiscard]] TObjectInstance* User() const;

    static void PauseAllScripts() { pauseall = true; }
    static void ResumeAllScripts() { pauseall = false; }

  private:
    // REVSYNC: 0x004927b0 — does trigger `st` fire now? Records the object
    // that set it off in `triggerer`.
    bool Triggered(PSScriptTrigger st, int32_t priority, TObjectInstance* context);
    // The prototype search of 0x00492640: is there a `type` trigger a request
    // naming `str`/`str2` would start?
    [[nodiscard]] bool HasTrigger(int32_t type, const char *str, const char *str2) const;
    // REVSYNC: 0x00492d70 — is the current wait over?
    bool WaitSatisfied(bool commanddone);

    static bool pauseall;                          // True if all scripts paused

    TScriptProto* proto      = nullptr;            // Pointer to script prototype
    TScriptProto* topproto   = nullptr;            // Pointer to the top prototype
    TScriptProto* curproto   = nullptr;            // Pointer to the current prototype
    int32_t newtrigger       = 0;                  // Next trigger type to execute
    int32_t trigger          = 0;                  // Current trigger type executing
    char    newtriggerstr[MAXSCRIPTNAME] = {};     // +0x20: name the requested trigger matches
    char    newtriggerstr2[MAXSCRIPTNAME] = {};    // +0x34: second name a USE trigger matches

    // Offset of the next line to execute in curproto's text, or kNotRunning.
    // (The 1998 engine kept a raw char*; the parse streams address text by
    // offset, which is also what survives 64-bit pointers.)
    static constexpr int32_t kNotRunning = -1;
    int32_t ip               = kNotRunning;
    int32_t priority         = 0;                  // Priority of current ip (block id)
    int32_t lastpriority     = 0;                  // Last trigger executed

    SScriptBlock block[MAXDEPTH] = {};             // For conditionals, loops etc
    int32_t depth            = 0;                  // Number of blocks deep

    PSScriptTrigger curtrigger = nullptr;          // The current trigger record

    // Retail state (SCRIPT_ENGINE.md §2).
    // +0x00: what the running block took and End gives back. Only the
    // dialog bit is ported; control (1) and the camera (8) follow their
    // commands.
    static constexpr uint32_t kTakenControl = 1;
    static constexpr uint32_t kTakenDialog  = 4;
    static constexpr uint32_t kTakenCamera  = 8;
    uint32_t taken           = 0;
    TSafeRef<TObjectInstance> triggerer;           // +0xc4: what set off the running trigger
    TSafeRef<TObjectInstance> second;              // +0xc8: the other object it concerns
    std::string useralias;                         // +0xcc: the block's name for `triggerer`
    std::string secondalias;                       // +0xd0: the block's name for `second`
    TSafeRef<TObjectInstance> triggerguard;        // +0x10: no re-trigger while this exists
    EScriptWait wait         = EScriptWait::None;  // +0xb4
    int32_t     waitframes   = 0;                  // +0xbc for Frames
    TSafeRef<TObjectInstance> waitobject;          // +0xbc for CharDone/Say/Death
};

// The names a trigger's objects go by in its block (retail's alias strings).
inline constexpr const char *kAliasUser  = "user";
inline constexpr const char *kAliasItem  = "item";
inline constexpr const char *kAliasEnemy = "enemy";

// **************
// * TGameState *
// **************

#define MAXGAMESTATES       4096

// REVSYNC: STATE_INVALID retail sentinel @ cls_TGameState_FindStateValue_4975d0.
// Retail returns 0xfeced300 when the name isn't in either the LocalScriptVals
// static table OR the gamestate registry. Pre-release used -2 000 000 000
// (== 0x88ca6c00); we now match retail so save-game and engine probes line up.
#define STATE_INVALID       ((int32_t)0xfeced300)

// REVSYNC: TGameState layout from cls_TGameState_Load_495cf0 + LoadStream_496110.
//   0x0000  numstates (int32_t)
//   0x0004  state[MAXGAMESTATES]    (int32_t × 4096 = 16 KB)
//   0x4004  statename[MAXGAMESTATES] (char* × 4096 = 16 KB)
// Total inline size = 0x8004. TScriptManager embeds this as its first member.
class TGameState
{
  public:
    TGameState() = default;
    ~TGameState()
    {
        for (int32_t i = 0; i < numstates; i++)
            if (statename[i]) delete[] statename[i];
    }

    // REVSYNC: Load (text .def parser) @ 0x00495cf0
    bool Load(char *filename);
    // Text writer (retail had this for editor; keep for parity)
    bool Save(char *filename);

    // REVSYNC: LoadStream / SaveStream @ 0x00496110 / 0x004974d0 — the game
    //   states in a save file: int32 count, then per state a name (uint8
    //   length + bytes, each OR'd with 0x80) and an int32 value. Loading
    //   updates states by name and appends unknown ones. LoadStream returns
    //   false on a truncated stream. docs/gameflow/forensics/SAVE_GAME.md §3.2.
    bool LoadStream(TInputStream &is);
    void SaveStream(TOutputStream &os) const;

    [[nodiscard]] int32_t NumStates() const { return numstates; }
    [[nodiscard]] int32_t State(int32_t index) const
        { if ((uint32_t)index < (uint32_t)numstates) return state[index]; return STATE_INVALID; }
    void SetState(int32_t index, int32_t newval)
        { if ((uint32_t)index < (uint32_t)numstates) state[index] = newval; }
    [[nodiscard]] char *StateName(int32_t index) const
        { if ((uint32_t)index < (uint32_t)numstates) return statename[index]; return nullptr; }

    [[nodiscard]] int32_t FindStateIndex(const char *name) const
    {
        for (int32_t i = 0; i < numstates; i++)
            if (statename[i] && !stricmp(name, statename[i])) return i;
        return -1;
    }

    [[nodiscard]] int32_t State(const char *name) const { return State(FindStateIndex(name)); }
    void SetState(const char *name, int32_t newval) { SetState(FindStateIndex(name), newval); }

  private:
    int32_t numstates = 0;
    int32_t state[MAXGAMESTATES] = {};
    char   *statename[MAXGAMESTATES] = {};
};

// ******************
// * TScriptManager *
// ******************

// REVSYNC: TScriptManager from cls_TScriptManager_{Initialize,Close,Load,...}.
//   Retail layout:
//     0x0000  TGameState gamestate            (inline, 32 KB)
//     0x8004  TVirtualArray<TScriptProto*>    (proto registry, see scripts below)
//     0x8018  TVirtualArray<TScript*>         (live script instances)
//     0x802c  TVirtualArray<SScriptFileOwner> (filename↔owner registry)
//     0x8040  int32_t scriptsdirty
// We keep scripts as TPointerArray (retail uses TVirtualArray<T*> but the
// API is the same for our usage). The file→owner registry (`fileowners`) is
// needed for retail-faithful Clear/Load semantics — area Exit scans both proto
// and registry rather than only the proto list.
//
// REVSYNC-DIVERGENCE: the live-instance list (`instances`) doesn't own the
// scripts. Every TScript belongs to the object it runs on, which deletes it;
// the script joins the list when constructed and leaves it when destroyed.
// Retail's Close freed the instances itself; with objects already freeing
// theirs, doing both freed each script twice.

// Filename→owner association tracked per Load() call (one record per file).
struct SScriptFileOwner
{
    char  filename[0x80] = {};   // retail: strncpy with cap 0x7f, NUL at 0x7f
    void *owner          = nullptr;
};

typedef TPointerArray<SScriptFileOwner, 64, 64>  TScriptFileOwnerArray;

class TScriptManager
{
  public:
    TScriptManager() = default;

    // REVSYNC: Initialize @ 0x00496240 — clears the 3 arrays then loads master.s + state.def
    bool Initialize();
    // REVSYNC: Close @ 0x00496330 — Save (editor) + delete protos + free file/owner records
    void Close();

    // REVSYNC: ParseScripts @ 0x00496860 — body of Load after slurp; chunks buffer into TScriptProtos.
    //   The retail body additionally re-attaches the new protos to existing
    //   instances (instance-rebuild logic) which we don't exercise yet because
    //   we never hot-reload scripts mid-session. The boot-time effect is the
    //   same: each parsed proto is added to `scripts`.
    void ParseScripts(char *buffer, char *filename, void *owner);

    // REVSYNC: Load @ 0x00496490 — slurps file, ParseScripts, registers file/owner pair
    bool Load(char *filename, void *owner = nullptr);
    // REVSYNC: Save @ 0x00496690 — text serialise dirty protos for the owner
    bool Save(char *filename, void *owner = nullptr);
    // REVSYNC: Clear @ 0x004967a0 — drop protos AND file/owner records owned by `owner`
    void Clear(void *owner);

    // REVSYNC: ReloadStates @ 0x004975c0 — just re-calls gamestate.Load("state.def")
    bool ReloadStates();

    // REVSYNC: 0x00496e20 — reset every live script instance (TScript::Reset).
    //   LoadGame runs this before ReloadStates.
    void ResetScripts();

    // REVSYNC: ObjectScript @ 0x00497370 — find first matching proto for `inst`
    PTScript ObjectScript(TObjectInstance* inst);

    // REVSYNC: AddScript @ 0x00497120 — manual proto-from-source-text path used
    //   by the editor + scripted hot-spawn. Not used by the ported game path
    //   yet; preserved for parity.
    PTScript AddScript(TObjectInstance* inst, char *name, void *owner, char *buffer);

    // REVSYNC: GameState lookup @ 0x004975d0 (TGameState::FindStateValue)
    //   Returns STATE_INVALID (retail 0xfeced300) when name not found.
    int32_t GameState(const char *name);
    void SetGameState(const char *name, int32_t newval) { gamestate.SetState(name, newval); }

    int32_t FindLocalVal(const char *name);
    void SetLocalVal(int32_t index, int32_t value);
    void SetLocalVal(const char *name, int32_t value) { SetLocalVal(FindLocalVal(name), value); }
    int32_t GetLocalVal(int32_t index);
    int32_t GetLocalVal(const char *name) { return GetLocalVal(FindLocalVal(name)); }

    void SetScriptsDirty() { scriptsdirty = true; }

    TScriptProto* FindScriptProto(const char *name);

    // Accessors retained for the editor / debugging
    [[nodiscard]] TGameState &GameStates() { return gamestate; }
    [[nodiscard]] int32_t NumProtos() const { return scripts.NumItems(); }

  private:
    friend class TScript;                   // joins/leaves `instances` itself
    void RegisterScript(TScript* script);
    void UnregisterScript(TScript* script);

    TGameState gamestate;                   // 0x0000 (32 KB inline)
    TScriptProtoArray scripts;              // proto registry
    std::vector<TScript*> instances;        // live scripts (non-owning, see above)
    TScriptFileOwnerArray fileowners;       // filename→owner table (retail per-Load record)

    bool scriptsdirty = false;
};
