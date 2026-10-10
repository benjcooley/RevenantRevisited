// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   character.h - TCharacter module                     *
// *************************************************************************

#ifndef _CHARACTER_H
#define _CHARACTER_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _COMPLEXOBJ_H
#include "complexobj.h"
#endif

#ifndef _RULES_H
#include "rules.h"
#endif

#ifndef _CHARSTATS_H
#include "charstats.h"
#endif

#ifndef _SPELL_H
#include "spell.h"
#endif

#include <vector>

// FindChar flags
#define FINDCHAR_ENEMY     1    // Find only enemies
#define FINDCHAR_HEAR      2    // Find only characters we can hear
#define FINDCHAR_SEE       4    // Find only characters we can see


// Maximum number of characters a character can remember seeing
#define MAXHASSEEN 8
_STRUCTDEF(SHasSeen)
struct SHasSeen
{
    TCharacter* chr;            // Has seen this character
    int32_t time;                   // Game ticks when last seen
    bool noautocombat;          // Prevents player from toggling autocombat
};

// the default invoke animation delay
#define INVOKE_DELAY    20

_CLASSDEF(TCharacter)
class TCharacter : public TComplexObject
{
  public:
    void ClearChar();       // Clear out working vars of char

    TCharacter(TObjectImagery* newim) : TComplexObject(newim) { ClearChar(); }
    TCharacter(SObjectDef* def, TObjectImagery* newim) : TComplexObject(def, newim) { ClearChar(); }

    int32_t Distance(const TObjectInstance* inst) const override;
      // REVSYNC: TCharacter::Distance @ 0x004d61b0 -- edge to edge: the
      // centre distance less this character's Radius and, for a character
      // target, its Radius; never below 0
    bool IsValidTarget(TCharacter* target);
    void SetRunsAI(bool on) { if (on) charflags |= kCharFlagPlayerAI; else charflags &= ~kCharFlagPlayerAI; }
      // A player's AI switch (retail charflags 0x100000; AI() gates on it)
      // REVSYNC: 0x004cd990 -- may `target` be fought: there, alive, not
      // invisible, within combat range, and the player only while he has
      // control or demo mode is on

    using FindCharactersSeam = int32_t (*)(TCharacter* self, TCharacter* chars[], int32_t maxchars,
        int32_t range, int32_t angle, int32_t anglerange, int32_t flags);
    static inline FindCharactersSeam findCharactersSeam = nullptr;
    using BlockedSeam = bool (*)(TCharacter* self, const S3DPoint& pos, const S3DPoint& newpos, uint32_t bits,
        TCharacter** bychar);
    static inline BlockedSeam blockedSeam = nullptr;
      // Likewise for Blocked (retail FindClearPath 0x004c39d0)
    using CanSeeSeam = bool (*)(TCharacter* self, TCharacter* chr, int32_t angle);
    static inline CanSeeSeam canSeeSeam = nullptr;
    using BlockSeam = bool (*)(TCharacter* self, int32_t frames);
    static inline BlockSeam blockSeam = nullptr;
      // Likewise for Block (retail 0x004d2e30), which IsValidAttack calls on
      // a target about to be hit glancingly
    using CastSeam = bool (*)(TCharacter* self, const char* spell, TObjectInstance** targets, int32_t numtargs,
        const S3DPoint* source);
    static inline CastSeam castSeam = nullptr;
    using IsEnemySeam = bool (*)(TCharacter* self, TCharacter* other);
    static inline IsEnemySeam isEnemySeam = nullptr;
      // Likewise for IsEnemy (retail 0x004c89c0)
    using BeginFightingSeam = bool (*)(TCharacter* self, TCharacter* target, ACTION action);
    static inline BeginFightingSeam beginFightingSeam = nullptr;
      // Likewise for BeginFighting (retail 0x004d3b90)
    using DamageSeam = void (*)(TCharacter* self, int32_t damage, int32_t damagetype, int32_t modifier,
        TActionBlock* action, TCharacter* attacker);
    static inline DamageSeam damageSeam = nullptr;
      // Likewise for Damage (retail 0x004c4950); the seam owns `action` as Damage does
    using EffectBurstSeam = void (*)(TCharacter* self, const char* name, int32_t height);
    static inline EffectBurstSeam effectBurstSeam = nullptr;
      // Likewise for EffectBurst (retail 0x004c85d0)
      // Likewise for CastByName (retail SpellList::Find 0x0053f010 + Cast
      // 0x004d5c20), which DoAttack calls for a MAGICATTACK
      // Likewise for CanSeeCharacter (retail 0x004cd540)
    using NearbyCharactersSeam = std::vector<TCharacter*> (*)(const S3DPoint& pos, int32_t range);
    static inline NearbyCharactersSeam nearbyCharactersSeam = nullptr;
      // Likewise for the characters CharBlocking walks (retail's map iterator
      // 0x0044ceb0 / 0x0044d080), in map order
      // Retail A/B fixtures only (retailab_combat.cpp): when set, it answers
      // FindCharacters instead of the map, as the retail fixture's seam at
      // FindCharacters 0x004cd690 does (docs/gameplay/COMBAT_DOJO.md §6.3).

    bool IsAnimatorPermanent() const override { return true; }
        // Characters always own a TObjectAnimator from construction. See
        // TObjectInstance::IsAnimatorPermanent for the contract.

    int32_t CursorType(TObjectInstance* inst = nullptr) override;
        // Talk icon if they are friendly, attack icon if aggressive, hand if dead
    virtual bool Use(TObjectInstance* user, int32_t with = -1);
        // Talk to or attack character
    virtual bool DrawShadow() { return false; }
        // no shadow // Characters get a little shadow that follows them
    virtual void Pulse();
      // Calls ExecuteAction
    virtual void Animate(bool draw);
      // Draw character
    virtual uint32_t Move();
      // Called by system when character moves
    virtual void Notify(int32_t notify, void *ptr);
        // Notify Action (check if objects we depend on are killed)
    virtual int32_t CalculateDamage(int32_t damage, int32_t damagetype, int32_t modifier);
        // Calculate the total damage for the character based on a base damage
        // value (i.e. from the weapon), the damage type (i.e. the value returned from
        // GetDamageType()), and a percentage modifier such as +10%(10) or -30%(-30).  Damage
        // is calculated as damage * (100% + modifier) * (100% + chardmgmodifier).
    virtual void Damage(int32_t damage, int32_t damagetype = DT_NONE, int32_t modifier = 0,
        PTActionBlock action = nullptr, TCharacter* attacker = nullptr);
        // Apply damage to the character.  If character hit, use 'action' action block
        // instead of default "impact" state, or use 'action' block if he dies instead
        // of default "dead" state.  If impact and death are nullptr, uses default "impact"
        // and "dead".  Note that damage is modified based on monsters resistance to 
        // 'damagetype' damage unless damagetype is DT_NONE, in which case the exact
        // damage value in 'damage' is used without ANY modification.
    void RestoreHealth();
        // Cure them of all ailments and set health to max

    virtual void AI();
        // Causes the object to perform its A.I. routines

    virtual const char *DefaultRootState() { return (Sleeping() ? "sleep" : Aggressive() ? "combat" : IsDead() ? "dead" : "walk"); }
      // Returns the default root state for this char

  // Action response functions to trigger character AI
    virtual void SignalMovement(TObjectInstance* actor);
        // Actor is moving
    virtual void SignalHostility(TObjectInstance* actor, TObjectInstance* target);
        // Actor is hostile to target
    virtual void SignalAttack(TObjectInstance* actor, TObjectInstance* target, int32_t flag = 0);
        // Actor is attacking target (retail OnAttacked 0x004cdce0, slot 0x240; flag 2 for a spell)

  // ActionBlock generic function callers
    bool SetWalkMode();
      // Sets walk mode
    bool SetSneakMode();
      // Sets sneak mode
    bool SetRunMode();
      // Sets run mode
    bool Go(int32_t angle = -1);
      // Start a character moving in the given angle and speed (entry point)
    bool Go(S3DPoint vect);
      // Start a character moving in the given movement vector
    bool Goto(int32_t x, int32_t y, TObjectInstance* pickup = nullptr);
      // Causes character to go to x,y; an item given is picked up on arrival
    bool Stop(char *name = nullptr);
      // Stops specified action, or any action if name is nullptr
    bool Disable();
      // Disables character's AI (for freezing, etc.)
    bool Pulp(S3DPoint vel, int32_t piece_count, int32_t blood_count);
      // Causes a character to explode into body parts, blood, & guts
    bool Burn();
      // Causes a character to combust into a seething sweltering mass of flames
    void ClearBurn() { burning = nullptr; }
      // Sets burning pointer to nullptr
    bool Flail();
      // Causes a character to act a fool
    bool KnockBack(S3DPoint frompos);
      // Causes a character to react with a heavy imapct animation, facing towards frompos
    bool Jump();
      // Causes character to jump (in normal mode, use Leap in Combat mode)
    bool Pivot(int32_t angle);
      // Pivots character to given direction (in 32 increments)
    bool FollowChar(TObjectInstance* inst);
      // Causes character to follow another character.
    bool Pickup(TObjectInstance* inst);
      // Causes character to move to and pickup object.
    bool Pull(TObjectInstance* inst);
      // Causes character to pull lever
    bool TryUse();
      // Attempts to use something in the direction character is facing
    bool TryGet();
      // Attempts to get something in the direction character is facing
    bool Say(const char *string, int32_t wait = -1, const char *anim = nullptr, const char *sound = nullptr);
      // Causes character to blather incessantly about something irrelevant
      // (anim is override for animation to play when saying, nullptr is "say")
      // Tag indicates that the say command is a index tag into the DialogList
      // list of dialog lines.  The tag will also be used to play the dialog wave file.
    [[nodiscard]] static int32_t SpeechTicks(int32_t wait, int32_t voicems, const char *line);
      // How long Say holds a line, in ticks (DIALOG.md §3.2): `wait` when
      // given (>= 0), else the voice's length, else the line's (`line` is
      // DialogLine's output).
    bool SayTag(int32_t tagid, int32_t wait = -1, const char *anim = nullptr);
      // Says something given a dialog tag id number
    bool SayTag(const char *tag, int32_t wait = -1, const char *anim = nullptr);
      // Says something given a dialog tag
    bool CastByName(char* name, TObjectInstance* *target = nullptr, int32_t numtargs = 0, S3DPoint* sourcepos = nullptr);
      // Cast a spell by usings its name
    bool CastByTalismans(char* talismans, TObjectInstance* *target = nullptr, int32_t numtargs = 0, S3DPoint* sourcepos = nullptr);
      // Cast a spell by using a talisman list
    bool SetCast(char* ani, TObjectInstance* target, int32_t invoke_delay = INVOKE_DELAY);
      // Set the character to the cast animation
    bool Cast(char* talismans, S3DPoint* sourcepos = nullptr);
      // quick cast a spell
    bool BeginFighting(TCharacter* target = nullptr, ACTION action = ACTION_COMBAT);
      // Engage character in combat
    bool EndFighting();
      // Leave combat mode
    bool BeginCombat(TCharacter* target = nullptr)
      { return BeginFighting(target, ACTION_COMBAT); }
      // Enter combat mode
    bool EndCombat()
      { return EndFighting(); }
      // Leave combat mode
    bool BeginBowMode(TCharacter* target = nullptr)
      { return BeginFighting(target, ACTION_BOW); }
      // Enter bow combat mode
    bool EndBowMode()
      { return EndFighting(); }
      // Leave bow combat mode
    bool DrawBow();
      // Begins drawing bow or crossbow
    bool AimBow(int32_t angle);
      // Causes character to aim at given angle before shooting bow
    bool AimBowLeft();
      // Causes character to pivot to left when aiming bow
    bool AimBowRight();
      // Causes character to pivot to right when aiming bow
    bool ShootBow(int32_t angle);
      // Shoots bow or crossbow
    bool ButtonAttack(int32_t buttonid);
      // Find attack based on the button the player pressed
    bool ButtonAction(int32_t buttonid);
      // Use attack list in RULES.DEF to do an action (usually a PLAYANIM or MAGIC tag)
    bool RandomAttack(int32_t pcnt);
      // Find a random attack (for a monster) based on a value from 1-100
    bool SpecificAttack(int32_t attacknum);
      // Do a specific attack given the attacknum

  // The attack search (retail's signatures). Every search shares one set of
  // out-values -- impact, damage, to-hit, roll -- that its caller starts at
  // -1 (the roll at 100 for a repeated button); the first candidate that
  // gets as far as the damage fixes them for the rest of the search.
    bool IsValidAttack(int32_t attacknum, int32_t &impactnum, int32_t &damage, int32_t &tohit, int32_t &roll,
        int32_t tdist, int32_t button, int32_t pcnt, int32_t dmgpcnt, int32_t flagmask, int32_t flags,
        TCharacter* targ);
      // REVSYNC: 0x004d1120 -- whether attack `attacknum` can be made now at `targ` (null: none,
      // `tdist` then 10000), fixing the out-values on the way
    bool FindButtonAttack(int32_t button, int32_t dmgpcnt, int32_t &attacknum, int32_t &impactnum,
        int32_t &damage, int32_t &tohit, int32_t &roll, bool isaction, TCharacter* targ);
      // REVSYNC: 0x004d1dd0 -- the first valid attack on `button`: responses, then
      // specials, then the rest, each in table order
    bool FindPcntAttack(int32_t pcnt, int32_t dmgpcnt, int32_t &attacknum, int32_t &impactnum,
        int32_t &damage, int32_t &tohit, int32_t &roll);
      // REVSYNC: 0x004d1eb0 -- up to 2n random picks of a valid attack whose
      // percentage is at least `pcnt`, against the fighting target
    bool FindInteractiveAttack(int32_t pcnt, int32_t dmgpcnt, int32_t &attacknum, int32_t &impactnum,
        int32_t &damage, int32_t &tohit, int32_t &roll);
      // REVSYNC: 0x004d1ff0 -- the first valid CA_INTERACTIVE attack, in table order
    bool DoAttack(int32_t attacknum, int32_t impactnum, int32_t damage, int32_t tohit, int32_t roll,
        TCharacter* targ);
      // REVSYNC: 0x004d2120 -- start the chosen attack: an ATTACK block carrying
      // its numbers (or the spell of a MAGICATTACK)
    void CombatAnimName(char *buf, const char *name);
      // REVSYNC: 0x004ce1b0 -- `name` with this character's animation prefix
      // ("c", or the player's weapon/mode prefix); buf holds RESNAMELEN
    const char *AnimPrefix();
      // REVSYNC: slot 0x310, 0x004cdf60 -- the player's animation prefix for his root:
      // h/hr (hand), b/br (bow), c/cr (combat), s (sneak), w/r, t/tr (with a torch)
    bool InteractiveLocked() const;
      // Held in someone's interactive attack (retail's five-line gate opening
      // ButtonAttack, RandomAttack, SpecificAttack, SetFighting, Go, ...): the doing
      // attack is CA_INTERACTIVE or its impact CAI_INTERACTIVE, unless this character
      // is the one making the move (charflags 0x80000)
    bool Swing() { return ButtonAttack(1); }
      // Character swings their weapon at their current target
    bool Thrust() { return ButtonAttack(2); }
      // Character thrusts their weapon at their current target
    bool Chop() { return ButtonAttack(3); }
      // Character chops with weapon at current target
    bool Combo(int32_t num) { return ButtonAttack(3 + num); }
      // Character does combo number num
    bool Block(int32_t frames = -1);
      // Character blocks an attack
    bool StopBlock();
      // Character stops blocking
    bool Dodge();
      // Character dodges an attack
    bool SideStep(char dir = 0);
      // Cartwheel/sidestep: step \xc2\xb190\xc2\xb0 of facing using the "sidestepl"/"sidestepr"
      // animation if the root has it. Retail FUN_004d6220 @ 0x4d6220.
      // dir is 'l' or 'r'; pass 0 for random L/R.
    bool Leap(int32_t angle);
      // Character leaps in the given direction (combat mode only)
    bool PlayAnim(char *string);
      // Causes character to play animation name.
    bool ResolveHit(TCharacter* targ, SCharAttackData* attack, SCharAttackImpact* impact, int32_t damage,
        int32_t tohit, int32_t roll);
      // REVSYNC: 0x004c62b0 -- one character struck by an attack at its impact frame
      // (ResolveAttack calls it for the target and each character in reach)

  // Info functions specific to characters
    bool IsFighting() { return IsCombat() || IsBowMode(); }
      // Returns whether the character is in a fighting mode
    bool IsCombat() { return IsRoot(ACTION_COMBAT); }
      // Returns whether the character is in combat or not
    bool IsHandCombat() { return root->Is("comhand"); }
      // Returns wether we are in hand to hand combat mode
    bool IsBowMode() { return IsRoot(ACTION_BOW); }
      // Returns whether the character is in combat or not
    bool IsBowDrawn();
      // Returns true if bow is drawn and we are in aim mode
    bool IsAttack() { return IsDoing(ACTION_ATTACK); }
      // Returns whether the character is attacking or not
    TCharacter* Fighting() { if (IsFighting()) return (TCharacter*)root->obj; return nullptr; }
      // Return the current target if they are in combat, nullptr if not fighting anyone
    bool SetFighting(TCharacter* newtarget);
      // Sets the current fighting target
    bool IsTalking() { if (doing && doing->Is("say")) return true; return false; }
    void StopTalking();
      // REVSYNC: 0x004d6000 -- silence the voice and end the say action
      // Returns whether character is talking or not
    bool IsWalkMode() { if (root && 
        ((IsCombat() && (root->Is("combat") || root->Is("hand"))) ||
        (IsBowMode() && root->Is("bow")) ||
        root->Is("walk")) ) return true; return false; }
      // Returns whether character is in walk mode. REVSYNC: the unarmed
      // combat roots are "hand" and "handrun" (retail 0x004cf000 reads
      // "hand" 0x005e064c, 0x004c9790 "handrun" 0x005e0610), not 1998's
      // "comhand" / "comhandrun".
    bool IsRunMode() { if (root && 
        ((IsCombat() && (root->Is("combatrun") || root->Is("handrun"))) ||
        (IsBowMode() && root->Is("bowrun")) ||
        root->Is("run")) ) return true; return false; }
      // Returns whether character is in run mode
    bool IsSneakMode() { if (root && root->Is("sneak")) return true; return false; }
      // Returns whether character is in sneak mode
    bool IsMoving()
        { return doing && 
                 doing->action == ACTION_MOVE || 
                 doing->action == ACTION_COMBATMOVE ||
                 doing->action == ACTION_BOWMOVE; }
      // Returns true if character is doing a moving action (result of calling Go())
    bool IsGoto() 
        { return IsMoving() && (doing->target.x != 0 || doing->target.y != 0 || doing->target.z != 0); }
    bool IsDead() { return (Health() <= 0); }
      // Dammit Jim, I'm a corpse not a doctor..
    bool IsEnemy(TCharacter* chr);
      // Returns true if the character is an enemy
    bool IsFinalState();
      // Returns whether character is in their last days
    int32_t SqrRadius() { int32_t s = Radius(); s *= s; return s; }
      // Util function
    bool ExecutingQueued();
      // Find out whether character is executing a queued set of actions
    PSCharData GetCharData() { return chardata; }
      // Returns character data structure for this char
    ACTION GetMoveAction(ACTION action);
      // Returns the move action for the given root action
    ACTION GetLeapAction(ACTION action);
      // Returns the move action for the given root action
    bool IsFlailing() 
        { return doing && doing->action == ACTION_FLAIL; }
      // Returns true if character is acting like a fool (result of calling Go())

    void ForceCommandDone() { forcecommanddone = true; }
      // Forces the current command to be done

    static constexpr uint32_t kCharFlagNoIncidentals = 0x2;
    // Retail charflags (+0x110) bits the combat code reads:
    static constexpr uint32_t kCharFlagNoTurn        = 0x4;       // DoAttack / Block / EndFighting leave the move
                                                                  // angle alone; the AI doesn't acquire (name unknown)
    static constexpr uint32_t kCharFlagNoKill        = 0x80;      // IsValidAttack refuses a killing blow but an
                                                                  // interactive death (setter unidentified)
    static constexpr uint32_t kCharFlagDamageSeventh = 0x10;      // CalculateDamage /7 (not freeze); setter unidentified
    static constexpr uint32_t kCharFlagHalfPhysical  = 0x100;     // CalculateDamage halves physical; setter unidentified
    static constexpr uint32_t kCharFlagHalfMagic     = 0x200;     // CalculateDamage halves magic (6-9); setter unidentified
    static constexpr uint32_t kCharFlagWalkPrefix    = 0x2000;    // CombatAnimName tries a "w" name in the walk root
    static constexpr uint32_t kCharFlagWalkFighter   = 0x4000;    // fights in its walk root (BeginFighting, AI)
    static constexpr uint32_t kCharFlagNotTargetable = 0x8000;    // IsValidTarget refuses (setter unidentified)
    static constexpr uint32_t kCharFlagPlayAnimRoots = 0x10000;   // a PLAYANIM "c..." / "w..." needs that root
    static constexpr uint32_t kCharFlagInteractive   = 0x80000;   // in an interactive move: Go skips its gates
    static constexpr uint32_t kCharFlagPlayerAI      = 0x100000;  // the player runs AI() (retail: set for
                                                                  // net players at 0x0051efc4; the arena's --playerai)
    // AI request bits (retail +0x128), set by OnAttacked 0x004cdce0
    static constexpr uint32_t kRequestAttackNow   = 0x1;          // the AI tries an attack this tick
    static constexpr uint32_t kRequestNoPlayAnim  = 0x2;          // IsValidAttack refuses PLAYANIM attacks
    static constexpr uint32_t kRequestInteractive = 0x4;          // RandomAttack tries an interactive attack first
      // charflags bit (retail +0x110 & 2): `incidentals off`
    void SetIncidentals(bool on)
        { if (on) charflags &= ~kCharFlagNoIncidentals; else charflags |= kCharFlagNoIncidentals; }
      // Incidentals are the random "NN:" variants of a character's root and idle
      // states (fidgets); off, the character always plays the 100% variant
    bool Incidentals() const { return !(charflags & kCharFlagNoIncidentals); }

    // Static access functions
    static TCharacter* CharBlocking(TObjectInstance* inst, const S3DPoint& pos, int32_t radius = 0);
        // Find if a character is blocking movement to this position
    TCharacter* CharBlocking() { return CharBlocking(this, Pos(), Radius()); }
        // Calls static function above with this chars parameters
    bool Blocked(S3DPoint& pos, S3DPoint& newpos, uint32_t bits = 0, int32_t* height = nullptr, TCharacter** bychar = nullptr);
      // Returns true if character would be blocked when going to new position
    uint32_t MoveStep();
      // One tick's displacement (Move repeats it toward a MoveTo target)
    enum class EBlockedBy : uint8_t { None, Hole, Height, Step, NoWalkmap, Character };
    EBlockedBy blockedby = EBlockedBy::None;
      // Which test the last Blocked() refused on (MoveStep's [move] log)
    
  // Miscellaneous functions
    virtual void MoveTo(S3DPoint& newpos) { movepos = newpos; movetopos = true; }
        // Moves object to new position (does walk checking for characters).
        // Use this function instead of SetPos() to avoid moving objects through or onto
        // barriers.
    void SetOnExit();
      // Flags that character is on an exit
    void EffectBurst(char *name, int32_t height = 50);
      // Create a burst effect of the given name
    int32_t GetCombatFlashTicks() { return combatflashticks; }
      // Returns 0 if no flash, or positive number of ticks left if flash being drawn
    void MakeInvisible();
      // Makes character invisible
    void MakeVisible();
      // Makes character visible
    PTSpellManager GetSpellManager() { return &SpellManager; }
      // Get spell manager object

  // Streaming functions
    virtual int32_t ObjVersion() { return 4; }
        // Returns the version id for this object for loading/saving
    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
        // Loads object data from the sector
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector

  // Fading (retail TCharacter +0x194..+0x1a4): fade is the visibility 0..100
  // that Transparency() reports; each Pulse moves it by fade_step (positive
  // fades out) until it reaches fade_limit.
    void Fade(int32_t direction);
      // `fadecharacterin/out`: +1 back to fully visible (living characters only), else out to 0
    void SetFade(int32_t amt, int32_t amt2 = 5, int32_t amt3 = -1);
      // Start a fade from 'amt' (kept when < 0) by 'amt2' per pulse to 'amt3' (-1: no limit)
    int32_t GetFade() const { return fade; }
    void UpdateFade();
      // One pulse of the fade

  // Invisible Spell Functions
    bool IsInvisibleSpell(){return invisible_spell;}
    void SetInvisibleSpell(bool on);
      // Fades to 30 while the spell lasts and back afterwards

  // Teleport functions
    void SetTeleportLevel(int32_t new_level){teleport_level = new_level;}
    void SetTeleportPosition(S3DPoint new_pos)
        {teleport_position.x = new_pos.x; teleport_position.y = new_pos.y; teleport_position.z = new_pos.z;}
    int32_t GetTeleportLevel(void){return teleport_level;}
    S3DPoint GetTeleportPosition(void){return teleport_position;}

  // Functions to remember if characters are seen or not
    bool HasSeenMe(TCharacter* me);
      // Have I been seen by this character?
    void SetHasSeen(TCharacter* me);
      // Add me to the HASSEEN list
    bool HasSeenAutoCombat(TCharacter* me);
      // Should I go into combat automatically against this guy I've just seen?
    void SetHasSeenAutoCombat(bool on);
      // Sets AUTOCOMBAT mode for all recently seen characters.  If set to false, prevents
      // player from automatically entering combat when he moves towards any of the 
      // current batch of enemies.  This is called when EndCombat() is called.

  // Character class stats
    STATFUNC(Radius)


  // Character object stats
    OBJSTATFUNC(Aggressive)
    OBJSTATFUNC(Poisoned)
    OBJSTATFUNC(Sleeping)
    OBJSTATFUNC(Health)
    OBJSTATFUNC(Fatigue)
    OBJSTATFUNC(Mana)

  // Damage resistances and modifier (the shipped game's code-defined stats;
  // their values come from class.def and saves, nothing in the port reads them yet)
    OBJSTAT(DmgResMisc)
    OBJSTAT(DmgResHand)
    OBJSTAT(DmgResPuncture)
    OBJSTAT(DmgResCut)
    OBJSTAT(DmgResChop)
    OBJSTAT(DmgResBludgeon)
    OBJSTAT(DmgResMagical)
    OBJSTAT(DmgResBurn)
    OBJSTAT(DmgResFreeze)
    OBJSTAT(DmgResPoison)
    OBJSTATFUNC(DamageMod)
      // REVSYNC: slot 0x2b0, 0x004d7220 -- percent added to the damage this character deals

   // Calculated stats
    virtual int32_t MaxHealth() { return chardata->health; }
      // Returns monster max health value
    virtual int32_t MaxFatigue() { return chardata->fatigue; }
      // Returns monster max fatigue value
    virtual int32_t MaxMana() { return chardata->mana; }
      // Returns monster max mana value
    bool GetFieldText(const char *field, char *buf, int32_t buflen) override;
      // Character fields of the stat sheet (retail 0x004d5260)
    virtual int32_t BlockPcnt() { return chardata->blockfreq; }
      // Returns percentage of time character will block an attack
    virtual int32_t ArmorValue() { return chardata->armorvalue; }
      // Return armor value
    virtual int32_t DefenseModifier() { return chardata->defensemod + SpellManager.GetDefense(); }
      // Return defense modifier
    virtual int32_t AttackModifier() { return chardata->attackmod + SpellManager.GetOffense(); }
      // Return offense modifier
    virtual int32_t FatigueModifier() { return (Fatigue() * 4 / MaxFatigue()) - 4; }
      // Return fatigue modifier
    virtual int32_t DamageModifier(int32_t damagetype) { return chardata->damagemods[damagetype]; }
      // Returns percentage of damage
    virtual int32_t WeaponType() { return chardata->weapontype; }
      // Returns the type of weapon being used
    virtual int32_t WeaponDamage() { return chardata->weapondamage; }
      // Returns current weapon damage
    virtual int32_t Offense();
      // REVSYNC: slot 0x2c4, 0x004d72e0 -- what this character adds to its to-hit:
      // ATTACKMOD, the type's Value x TOHITRANGECHAR, its spells' offense
    virtual int32_t Defense();
      // REVSYNC: slot 0x2c0, 0x004d72a0 -- what it takes off an attacker's to-hit:
      // DEFENSEMOD, Value x TOHITRANGECHAR, its spells' defense
    virtual int32_t LuckMod() { return 0; }
      // REVSYNC: slot 0x2ec, 0x004d7370 -- added to both sides of a to-hit; 0 but
      // for the player (his Luck STATLEVEL)
    virtual int32_t StrengthMod() { return 0; }
      // REVSYNC: slot 0x2f4, 0x004d7390 -- percent added to the damage; 0 but for the
      // player (his Strength STATLEVEL)
    virtual bool HoldsLight() { return false; }
      // REVSYNC: slot 0x250, 0x004d6de0 -- holding a light source (the player's torch)
    int32_t GetDamageType(int32_t weapontype, int32_t attackflags);
      // Returns the DT_XXX damage type flags for the given weapon type and attack flags
    virtual int32_t Transparency();
      // Controls the transparency of character's imagery (character animator).  Things
      // such as invisibility spells, visibility and stealth can affect this value.
    virtual int32_t Visibility();
      // Returns the total visibility 1-100 for character (based on lights, ambient, and fog, etc.)
    virtual int32_t Hearing(int32_t dist);
      // Returns a 1-100 hearing value which indicates how the average noise will be heard
      // by a monster.  If the monster is sleeping, the listening value is 20% of normal.
    virtual int32_t Sight(int32_t dist);
      // Returns a 1-100 sight value which indicates how the average char will be seen
      // by a monster in the darkness.  If the monster is sleeping, the sight value
      // is always 0.
    virtual int32_t StealthMod() { return 0; }
      // Ordinary characters dont have stealth
    virtual int32_t LastGlimpse() { return glimpse; }
      // What was the last visibility glimpse for the character.  1 is complete
      // and utter invisibility, 100 is plain as day visibility
    virtual int32_t LastNoise() { return noise; }
      // What was the last noise value (1-100) for the character. 1 is pindrop,
      // 100 is pots and pans crashing.
    virtual char *BodyType() { return "na"; }
      // Character's don't use the equipment replacement system
    virtual char *GetCombatRoot(TObjectInstance* oi = nullptr) { return "combat"; }
      // Returns combat root given current weapon or weapon type
    virtual char *GetBowRoot(TObjectInstance* oi = nullptr) { return "bow"; } 
      // Returns bow root given current bow weapon type
    bool CanSeeCharacter(TCharacter* chr, int32_t angle = -1);
      // Returns true if this character can 'see' the last glimpse of 'chr'
    bool IsMagicResistant() { return (magic_resistance > 0.0f); }
      // gets the level of magic resistance
    void SetMagicResistance(float pct) { magic_resistance = pct; }
      // sets the level of magic resistance
    float GetMagicResistance() { return magic_resistance; }
      // gets the level of magic resistance
    bool GetAutoCombat() { return autocombat; }
      // returns the state of this characters autocombat setting
    void SetAutoCombat(bool ac_state) { autocombat = ac_state; }
      // sets the state of this characters autocombat setting

  protected:
   
    virtual void UpdateAction(int32_t bits = 0);
      // Called by Pulse() to update the action blocks


    // Resolve functions - redefine these in derived classes for different functionality
    virtual int32_t ResolveAction(int32_t bits = 0);
      // Call the resolve functions, below
    virtual int32_t ResolveMove(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolveAttack(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolveCombatMove(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolveCombat(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolveBowAim(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolveBowShoot(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolveBlock(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolveImpact(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolveDead(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolveSay(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolvePivot(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolveLeap(PTActionBlock ab, int32_t bits);
    virtual int32_t ResolvePull(PTActionBlock ab, int32_t bits);

    char *GetAngleMoveAnim(int32_t movedir, int32_t facedir, char *root, char *animname, int32_t buflen);
      // Returns the correct angle movce animation given the current movedir, facedir, and root name
    void AdvanceAngles(int32_t faceang, int32_t moveang, int32_t maxturn);
      // Advance the character's facing and moving angle
    int32_t UpdateAngle(int32_t angle);
      // Check path along current angle and adjuct accordingly
    TObjectInstance* FindObjAhead();
      // Find object in front of character
    bool CanHearCharacter(TCharacter* chr);
      // Returns true if this character can hear the last noise made by 'chr'
    int32_t FindCharacters(TCharacter* chars[], int32_t maxchars, 
        int32_t range = 128, int32_t angle = -1, int32_t anglerange = 32, int32_t flags = 0);
      // Finds characters given the above parameters.  Will find all chars in range from
      // direction 'angle' if not -1 with angle range of 32.  Puts the closest character
      // at the beginning of the list, all other characters are in random order.  Returns
      // the number of characters found.
    TCharacter* FindCharacter(int32_t range = 128, int32_t angle = -1, int32_t anglerange = 32, int32_t flags = 0);
      // Calls the FindCharacters function above with only 1 character
      // Finds characters given the above parameters.  Will find all chars in range from
      // direction 'angle' if not -1 with angle range of 32.
    TCharacter* FindCharacterAhead(int32_t angle, int32_t anglerange = 32);
      // Find closest character in this direction
    TCharacter* FindClosestEnemy(int32_t angle = -1, int32_t anglerange = 32);
      // Finds the closest character attacking this character
    TObjectInstance* WanderToWaypoint(const S3DPoint& search_center, int32_t range = 250);
      // Retail TCharacter::AI (FUN_004c8b60) waypoint-search branch. Used by the
      // out-of-sight target chase: when a monster has lost sight of its target, it
      // hops between "waypoint" objects to thread its way toward the target's last
      // known position. Each call: if there's a committed waypoint, tick the commit
      // timer (decrement/clear); otherwise scan reachable waypoints near
      // search_center, pick the closest, commit for ~6 frames. Returns the
      // currently committed waypoint instance (or nullptr if none).
    void ResetStealthValues();
      // Based on character position, lights, and stealth, sets noise and glimpse

    // -- Data members --
    bool autocombat;            // this mimics the global, but works as a way to allow locke to flee in
                                // the face of too many enemies.

    bool movetopos;             // Set to true if character needs to move to movepos
    S3DPoint movepos;           // The position character needs to move to (while walk checking)

    PSCharData chardata;        // Pointer to global character settings for this type of char

    int32_t magictimer = 1;         // retail +0x124: frames until a MAGICATTACK may be tried
                                    // (AI re-arms it from MAGICFREQ; IsValidAttack spends it)

    bool forcecommanddone;      // For skipping past animations
    bool forcenomove;           // For forcing end movement

    uint32_t charflags = 0;        // Character flags (retail +0x110; retail's allocator zeroed it)

    int32_t exittimestamp;          // Frame the character was last on an exit; OF_ONEXIT clears 5 frames on
    bool is_invisible;          // is our character affected by invisibility

  // Move stuff
    int32_t shovedir;               // Last choice (left/right) for going around an obstacle
    
  // Stealth Stuff
    int32_t nextattack = 1;         // retail +0x120: frames until the AI may attack again
                                    // (IsValidAttack refuses a monster's attack while non-zero)
    int32_t glimpse;                // Value from 1-100 indicating how visible last move was
    int32_t noise;                  // Value from 1-100 indicating how quiet last move was

  // Stat recovery stuff
    int32_t lasthealthrecov;        // Game time values for last
    int32_t lastfatiguerecov;       // health, fatigue, and mana recovery
    int32_t lastmanarecov;      

  // Poison damage stuff
    int32_t lastpoisondamage;

  // Attack stuff
    PSCharAttackData lastattack = nullptr; // retail +0x160: the attack last started (ResolveAttack)
    int32_t lastattackticks = 0;     // retail +0x164: GameFrame it started on
    int32_t lasthit = 0;             // retail +0x168: whether it hit its main target (ResolveAttack)
    int32_t chainhits = 0;           // retail +0x16c: chain presses banked (ButtonAttack, at most 3)
    uint32_t requestbits = 0;        // retail +0x128: AI requests (kRequest*), set by OnAttacked
    int32_t attackcount = 0;         // retail +0x12c: decremented by the AI per attack made (no reader found)

  // spells stuff
    TSpellManager SpellManager;  // handles the spells the character casts
    int32_t invokedelay;
    PTActionBlock oldab;

    float magic_resistance;     // between 0.0 and 1.0... percentage of magic resistance

    // Visibility
    int32_t fade = 100;             // retail +0x194, 0..100
    int32_t fade_step = 0;          // retail +0x198, subtracted from fade each pulse
    int32_t fade_limit = 100;       // retail +0x19c, where the fade stops (-1: at 0 or 100)
    int32_t fade_direction = 0;     // retail +0x1a0, -1 out / 1 in / 0 still (no retail reader found)

  // Invisible Spell Addition
    bool invisible_spell = false;   // retail +0x1a4
    int32_t voice = -1;             // retail +0x260: the sound id of the line being spoken, -1 none

  // Teleport Coordinates
    int32_t teleport_level;
    S3DPoint teleport_position;

  // burn stuff
    TObjectInstance* burning;

  // Has seen list  
    SHasSeen hasseen[MAXHASSEEN]; // List of characters seen recently

  // Snap stuff
    int32_t snapticks;                // Total number of frames left in snap move

  // combatflash delay
    int32_t combatflashticks;

  // Diagnostic counters. ai_pulse_count increments at every Pulse()
  // entry, ai_ai_count at every AI() entry. The overlay reads these
  // to confirm whether monsters are being ticked at all (and if so,
  // whether AI() is being reached or short-circuited upstream).
public:
    uint32_t ai_pulse_count = 0;
    uint32_t ai_ai_count    = 0;
    // Debug accessors (overlay reads these). Wrap protected action-block
    // pointers so the diagnostic UI doesn't need to be a friend.
    int32_t  DoingAction() const { return doing ? (int32_t)doing->action : -1; }
    int32_t  RootAction()  const { return root  ? (int32_t)root->action  : -1; }
    int32_t  DesiredAction() const { return desired ? (int32_t)desired->action : -1; }
    const char* DoingName() const { return (doing && doing->name[0]) ? doing->name : "?"; }
    const char* RootName()  const { return (root  && root->name[0])  ? root->name  : "?"; }
    const char* DesiredName() const { return (desired && desired->name[0]) ? desired->name : "?"; }
    int32_t  DoingTargetX() const { return doing ? doing->target.x : 0; }
    int32_t  DoingTargetY() const { return doing ? doing->target.y : 0; }
    int32_t  NextAttack()   const { return nextattack; }
protected:

  // Last bow shot ticks (so we don't shoot bow too fast)
    int32_t lastbowshot = 0;

  // AI fix, this variable keeps track of the last position we saw the enemy at
    S3DPoint target_last_position{};
  // AI fix, this variable keeps track of when to save the last position of the enemy
    int32_t last_position_count = 0;
    int32_t last_position_distance = 0;
    S3DPoint last_position_start_point{};
    int32_t target_last_angle = 0;

  // Retail wander state (from FUN_004c8b60 lines 199-205).
  // field_map.md: 0x238 = wander_target  (retail mbr_0x8d / param_1[0x8d])
  // field_map.md: 0x248 = wander_commit  (retail mbr_0x92 / param_1[0x92])
  // The AI body caches the currently-targeted waypoint instance here and
  // ticks `wander_commit` down each frame while we walk toward it. While
  // commit > 0 we keep the same waypoint; when the search re-finds the
  // same waypoint as already-committed we treat that as arrival and clear,
  // letting the next tick pick a different one. Replaces the pre-release
  // version's missing waypoint state (it had only a closest-waypoint
  // search per tick which pingponged at arrival). Source-side we keep
  // wander_target as a TSafeRef for safe-pointer semantics.
    TSafeRef<TObjectInstance> wander_target;
    int32_t  wander_commit = 0;

  // retail +0x288: the item a Goto carries (the map pane's walk to an item
  // out of reach, 0x004cedb0), picked up when that walk arrives (0x004c6155,
  // 0x004c7fc3). Kept until then, or until a Goto carries another.
    TSafeRef<TObjectInstance> gotoitem;

  // Retail +0x254 / +0x258 / +0x25c: the retreat (COMBAT_ATTACK_CHOICE.md
  // §3.10.6). The AI's tail, every tick it runs: retreating = latch =
  // (Health <= RETREATAT || latch) && frames != 0, then frames counts down.
  // Damage arms the frames with RETREATFOR when a hit leaves 1 <= Health <=
  // RETREATAT; StartRetreat (0x004d5fc0) with 96 and sets both flags. A
  // retreating character runs straight away from its target (and takes
  // no new one); ResolveCombat doesn't face it; a blocked step (MoveStep)
  // clears all three. (The 1998 AI body below reads them as "target out
  // of sight" until its retail port, C3b.)
    bool     retreating = false;
    bool     retreatlatch = false;
    int32_t  retreatframes = 0;
  // Retail +0x234: an object the AI walks toward and ResolveCombat faces
  // when no visible target overrides it (written by AI 0x004c8b60 and
  // WanderToWaypoint 0x004c9790; no port writer yet).
    TObjectInstance* ai_lookat = nullptr;
  // Retail +0x28c / +0x290: the player's last attack button and how many
  // times running it was pressed (ButtonAttack 0x004d2480's same-button
  // rule); SetFighting resets them.
  // Retail +0x280: the per-monster AI kind (AI_PerMonster 0x004c9b70 sets
  // it; 1 is Baez, whom magic can't hurt). No port writer yet.
    int32_t monsterkind  = 0;
    int32_t lastbutton   = -1;
    int32_t buttonrepeat = 0;
};

DEFINE_BUILDER("Character", TCharacter)

#endif
