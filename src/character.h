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

// Wait types
#define WAIT_NOTHING        0
#define WAIT_RESPONSE       1
#define WAIT_CHAR_DONE      2
#define WAIT_TICKS          3

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

    virtual int32_t CursorType(TObjectInstance* inst = nullptr);
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

    virtual char *DefaultRootState() { return (Sleeping() ? "sleep" : Aggressive() ? "combat" : IsDead() ? "dead" : "walk"); }
      // Returns the default root state for this char

  // Action response functions to trigger character AI
    virtual void SignalMovement(TObjectInstance* actor);
        // Actor is moving
    virtual void SignalHostility(TObjectInstance* actor, TObjectInstance* target);
        // Actor is hostile to target
    virtual void SignalAttack(TObjectInstance* actor, TObjectInstance* target);
        // Actor is attacking target

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
    bool Goto(int32_t x, int32_t y);
      // Causes character to go to x,y.
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
    bool Say(char *string, int32_t wait = -1, char *anim = nullptr, char *sound = nullptr);
      // Causes character to blather incessantly about something irrelevant
      // (anim is override for animation to play when saying, nullptr is "say")
      // Tag indicates that the say command is a index tag into the DialogList
      // list of dialog lines.  The tag will also be used to play the dialog wave file.
    bool SayTag(int32_t tagid, int32_t wait = -1, char *anim = nullptr);
      // Says something given a dialog tag id number
    bool SayTag(char *tag, int32_t wait = -1, char *anim = nullptr);
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
    bool Leap(int32_t angle);
      // Character leaps in the given direction (combat mode only)
    bool PlayAnim(char *string);
      // Causes character to play animation name.

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
      // Returns whether character is talking or not
    bool IsWalkMode() { if (root && 
        ((IsCombat() && (root->Is("combat") || root->Is("comhand"))) ||
        (IsBowMode() && root->Is("bow")) ||
        root->Is("walk")) ) return true; return false; }
      // Returns whether character is in walk mode
    bool IsRunMode() { if (root && 
        ((IsCombat() && (root->Is("combatrun") || root->Is("comhandrun"))) ||
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

  // Wait functions
    void Wait(int32_t waitlen);
      // Wait for specified number of frames to elapse
    void WaitChar(TObjectInstance* inst) { if (doing) doing->obj = inst; waittype = WAIT_CHAR_DONE; }
      // Waits for another character to finish his current action
    void WaitResponse() { waittype = WAIT_RESPONSE; }
      // Waits for the player to pick a response in the response panel
    void ForceCommandDone() { forcecommanddone = true; }
      // Forces the current command to be done

    // Static access functions
    static TCharacter* CharBlocking(TObjectInstance* inst, S3DPoint& pos, int32_t radius = 0);
        // Find if a character is blocking movement to this position
    TCharacter* CharBlocking() { return CharBlocking(this, Pos(), Radius()); }
        // Calls static function above with this chars parameters
    bool Blocked(S3DPoint& pos, S3DPoint& newpos, uint32_t bits = 0, int32_t* height = nullptr, TCharacter** bychar = nullptr);
      // Returns true if character would be blocked when going to new position
    
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

  // invisibilty functions
    void SetFade(int32_t amt, int32_t amt2 = 5, int32_t amt3 = -1);
    int32_t GetFade(void);
    void UpdateFade(void);

  // Invisible Spell Functions
    bool IsInvisibleSpell(){return invisible_spell;}
    void SetInvisibleSpell(bool new_val){invisible_spell = new_val;}

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

   // Calculated stats
    virtual int32_t MaxHealth() { return chardata->health; }
      // Returns monster max health value
    virtual int32_t MaxFatigue() { return chardata->fatigue; }
      // Returns monster max fatigue value
    virtual int32_t MaxMana() { return chardata->mana; }
      // Returns monster max mana value
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

    bool ResolveHit(TCharacter* targ, 
        PSCharAttackData attack, PSCharAttackImpact attackimpact, int32_t attackdamage);
    // This function is called by the ResolveAttack() function to resolve hits for
    // multiple characters.  The characters are usually found by calling the FindCharacters()
    // function, then calling this function for each character found.

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
    void ResetStealthValues();
      // Based on character position, lights, and stealth, sets noise and glimpse

  // Attack functions   
    bool IsValidAttack(int32_t attacknum, int32_t &impactnum, int32_t &damage,
        int32_t tdist, int32_t id, int32_t pcnt, int32_t dmgpcnt, int32_t flagmask, int32_t flags);
      // Checks attack to see if attack is valid or not, returns true if valid, and the correct impact and
      // damage value for the attack in 'impactnum' and 'damage'.  Must give function target distance in 'tdist',
      // button id in 'id' or -1 if no button, random attack pcnt in 'pcnt' or -1 if no random attack pcnt, 
      // the random damage percentage 1-100 in 'dmgpcnt', and the attack flagmask and flags to specify what
      // kinds of attacks we're looking for.
    bool FindButtonAttack(int32_t id, int32_t dmgpcnt, int32_t &attacknum, int32_t &impactnum, int32_t &damage, bool isaction = false);
      // Finds a valid attack given an attack id (i.e. controller button), and the given random damage
      // percentage 1-100.
      // Returns the correct attacknum, impactnum, and damage value for the found attack.
      // If 'isaction' is set, will find an attack entry with a CA_ACTION flag set instead (called by ButtonAction())
    bool FindPcntAttack(int32_t id, int32_t dmgpcnt, int32_t &attacknum, int32_t &impactnum, int32_t &damage);
      // Finds a valid attack given a randomly generated percentage (0-100) number and a given damage percentage.
      // Returns the correct attacknum, impactnum, and damage value for the found attack.
    bool DoAttack(int32_t attacknum, int32_t impactnum, int32_t damage);
      // Executes a particular attack (using index into SCharData's attack array)

    // -- Data members --
    bool autocombat;            // this mimics the global, but works as a way to allow locke to flee in
                                // the face of too many enemies.

    bool movetopos;             // Set to true if character needs to move to movepos
    S3DPoint movepos;           // The position character needs to move to (while walk checking)

    PSCharData chardata;        // Pointer to global character settings for this type of char

    int32_t waittype;               // Wait for this before continuing script execution
    int32_t waitticks;              // Number of ticks to wait for no action block wait

    bool forcecommanddone;      // For skipping past animations
    bool forcenomove;           // For forcing end movement

    uint32_t charflags;            // Character flags

    int32_t exittimestamp;          // When timestamp is +2 frames from current frame, OF_ONEXIT is cleared
    bool is_invisible;          // is our character affected by invisibility

  // Move stuff
    int32_t shovedir;               // Last choice (left/right) for going around an obstacle
    
  // Stealth Stuff
    int32_t nextattack;             // Ticks till next attack
    int32_t glimpse;                // Value from 1-100 indicating how visible last move was
    int32_t noise;                  // Value from 1-100 indicating how quiet last move was

  // Stat recovery stuff
    int32_t lasthealthrecov;        // Game time values for last
    int32_t lastfatiguerecov;       // health, fatigue, and mana recovery
    int32_t lastmanarecov;      

  // Poison damage stuff
    int32_t lastpoisondamage;

  // Attack stuff
    PSCharAttackData lastattack; // Last attack
    int32_t lastattackticks;         // Game ticks when last attack occured
    int32_t chainhits;               // Number of hits in a chain attack

  // spells stuff
    TSpellManager SpellManager;  // handles the spells the character casts
    int32_t invokedelay;
    PTActionBlock oldab;

    float magic_resistance;     // between 0.0 and 1.0... percentage of magic resistance

    // Visibility
    int32_t fade;
    int32_t fade_step;
    int32_t fade_limit;

  // Invisible Spell Addition
    bool invisible_spell;

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

  // Last bow shot ticks (so we don't shoot bow too fast)
    int32_t lastbowshot;

  // AI fix, this variable keeps track of the last position we saw the enemy at
    S3DPoint target_last_position;
  // AI fix, this variable keeps track of when to save the last position of the enemy
    int32_t last_position_count;
    bool target_out_of_sight;
    int32_t last_position_distance;
    S3DPoint last_position_start_point;
    int32_t target_last_angle;
};

DEFINE_BUILDER("Character", TCharacter)

#endif
