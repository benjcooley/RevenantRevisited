// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 character.cpp - TCharacter module                     *
// *************************************************************************

#include "character.h"

#include "rules.h"
#include "mappane.h"
#include "display.h"
#include "spell.h"
#include "sound.h"
#include "playscreen.h"
#include "multi.h"
#include "animation.h"
#include "statusbar.h"
#include "dialog.h"
#include "effect.h"
#include "textbar.h"
#include "3dimage.h"
#include "inventory.h"
#include "exit.h"
#include "script.h"
#include "effect.h"
#include "charanimator.h"
#include "weapon.h"
#include "ammo.h"
#include "player.h"
#include "logging.h"

#include <math.h>
#include <string.h>

#define GRIDSNAPSIZE    48

REGISTER_BUILDER(TCharacter)
TObjectClass CharacterClass("CHARACTER", OBJCLASS_CHARACTER, 0);

extern TObjectClass EffectClass;
extern TObjectClass AmmoClass;

// Hard coded character class stats
DEFSTAT(Character, Radius,          RAD,  0, 16, 16, 256)

// Hard coded character object stats
DEFOBJSTAT(Character, Aggressive,   AGGR, CHRFLAG_FIRST + CHRFLAG_AGGRESSIVE, 1, 0, 1)
DEFOBJSTAT(Character, Poisoned,     PSND, CHRFLAG_FIRST + CHRFLAG_POISONED, 0, 0, 1)
DEFOBJSTAT(Character, Sleeping,     SLP,  CHRFLAG_FIRST + CHRFLAG_SLEEPING, 0, 0, 1)

DEFOBJSTAT(Character, Health,       HLT,  CHRSTAT_FIRST + CHRSTAT_HEALTH, 25, 0, 10000)
DEFOBJSTAT(Character, Fatigue,      FAT,  CHRSTAT_FIRST + CHRSTAT_FATIGUE, 25, 0, 10000)
DEFOBJSTAT(Character, Mana,         MAN,  CHRSTAT_FIRST + CHRSTAT_MANA, 25, 0, 10000)

extern TDialogPane DialogPane;

// Some character defines
#define IMPACT_THRESHOLD    4   // how much damage has to be done before they enter impact state
#define MAXENEMYRANGE       512 // Maximum range of enemy
#define MAXTURNRATE         8   // Maximum turn rate
#define MAXCHAINHITS        3   // Maximum number of times player can hit chain btn in row

// Maximum Z hit distance
#define MAXZHITRANGE        40  // Maximum Z hit range

// Returns a turn rate value based on how far character is turning
#define MAKETURNRATE(diff) (MAXTURNRATE + max(0, (diff) - 32) / 32 * (MAXTURNRATE / 2))

#define MAXSEENTIME (FRAMERATE * 10)

// *************************************************************
// *                          Character                        *
// *************************************************************

void TCharacter::ClearChar()
{
    flags |= OF_MOVING | OF_PULSE | OF_ANIMATE | OF_AI; // Is moving object, pulse, animate, and has AI

    autocombat = AutoBeginCombat;
    
    waittype = WAIT_NOTHING;
    waitticks = 0;
    forcecommanddone = false;
    forcenomove = false;
    is_invisible = false;
    magic_resistance = 0.0f;

    // When character loaded or created, make exit timestamp current..
    // Prevents ONEXIT flag from expiring when character is saved on top of a
    // destination exit.
    exittimestamp = CurrentScreen->FrameCount();

  // Set root state
  // NONE: DefaultRootState() will NOT be virtual when ClearChar()
  // is called in the constructor!
    if (!root)
        root = doing = desired = new TActionBlock(DefaultRootState() /* SEE ABOVE NOTE */);
    else
        strcpy(root->name, DefaultRootState());

    int32_t newstate = FindState(root->name);
    if (newstate < 0)
    {
        strcpy(root->name, "walk");
        newstate = FindState(root->name);
    }
    if (newstate < 0)
    {
        strcpy(root->name, "combat");
        newstate = FindState(root->name);
    }
    if (newstate < 0)
        newstate = 0;
    state = prevstate = newstate;

  // Set character data pointer
    chardata = Rules.GetCharData(objtype, objclass);

  // Clear moveto values
    bool movetopos = false;
    S3DPoint movepos;
    movepos.x = movepos.y = movepos.z = 0;

  // Set Burn to nullptr
    ClearBurn();

  // invoke delay
    invokedelay = 0;

  // combatflash
    combatflashticks = 0;

  // Set initial health/fatigue/mana values. If rules are missing chardata
  // (e.g. partial rules.ini parse), skip the stat init rather than deref null.
    if (chardata)
    {
        SetHealth(MaxHealth());
        SetFatigue(MaxFatigue());
        SetMana(MaxMana());
    }

  // Reset AI data
    nextattack = -1;
    chainhits = 0;
    shovedir = -1;                // Block go around direction choice
    glimpse = noise = -1;         // Reset glimpse and noise values
    snapticks = -1;               // No snapping
    lastbowshot = 0;              // Reset bow shot time so we can shoot bow immediately

  // Reset hasseen list
    memset(&hasseen, 0, sizeof(SHasSeen) * MAXHASSEEN);

  // Clear Fade
    fade = 100;
    fade_step = 5;

  // Clear the Invisible Spell
    invisible_spell = false;

  // Clear the poison
    SetPoisoned(false);

  // Clear Teleport Coordinates
    teleport_position.x = -1;
    teleport_position.y = -1;
    teleport_position.z = -1;
    teleport_level = -1;

  // Attach the animator component up front. Characters carry the
  // animator for the lifetime of the object (see IsAnimatorPermanent
  // override) -- no lazy create/free during sector load/unload or
  // state change. ClearChar runs from both TCharacter ctors and from
  // explicit re-init paths (e.g. spawn-time Aggressive reset). The
  // imagery-side builder lookup uses GetClassName() which is keyed
  // off the cl pointer set during TObjectInstance construction, so
  // even ctor-time invocation routes to the correct subclass animator
  // (TCharAnimator / TPlayerAnimator).
    if (imagery && !HasAnimator())
        CreateAnimator();
}

void TCharacter::Pulse()
{
    ai_pulse_count++;
//  if (!HasAnimator())  // Quick hack to fix super slodown... BEN  (if not on screen, ignore me)
//      return;

  // Pulse the animator
    if (TObjectAnimator* anim = GetAnimator())
        anim->Pulse();

  // If this is true, causes the command to be forced to done
    if (forcecommanddone)
    {
        if (doing)
            doing->wait = 0;

        commanddone = true;
        forcecommanddone = false;
    }

  // Combat flash ticks 
    if (combatflashticks > 0)
        combatflashticks--;

  // Now do action processing, etc.
    TComplexObject::Pulse();

  // Signal processing - let other characters know what's going on
    if (IsFighting())
    {
      // Call signal hostility to tell character he's being attacked
        TCharacter* target = Fighting();
        if (target)
            target->SignalHostility(this, target);

      // Update the health/etc. for player's target
        if ((TPlayer*)this == Player && target)
            TextBar.SetHealthDisplay(target->GetName(), target->Health());
    }

    // Check to see if exit flag has expired (exit flags are set by exit objects)
    // Exits search through character list to tag character that it is on an exit.  Characters
    // then check the time stamp and clear themselves after two frames.  This system prevents
    // the old reflective exit bug, where an exit takes a character to another exit, which
    // then takes him back to the first... etc. etc. as the character can only activate an
    // exit when he was previously (in the past two frames) not already on one.
    if (exittimestamp < CurrentScreen->FrameCount() - 2)
        SetFlag(OF_ONEXIT, false);

    // Do blood for impdecap (you can be dead!)
    if (doing->Is("impdecap"))
    {
        if (frame > 5 && frame < 65 && frame % 5 == 0)
        {
            int32_t height = 95;

            extern TObjectClass EffectClass;

            SObjectDef def;
            memset(&def, 0, sizeof(SObjectDef));

            def.objclass = OBJCLASS_EFFECT;
            def.objtype = EffectClass.FindObjType("blood");
            def.level = MapPane.GetMapLevel();
            def.pos = pos;
            def.pos.z += height;
            def.facing = 0;

            int32_t index = MapPane.NewObject(&def);
            TObjectInstance* inst = MapPane.GetInstance(index);
            if (inst)
            {
                ((TBloodEffect*)inst)->SetParams(height, 0, 64, 255, 5, 1);
            }
        }
    }

    if (IsDead())
    {
      // When we're dead, we're dead!
        if (!IsDoing(ACTION_DEAD))
            ForceCommand(new TActionBlock("blood", ACTION_DEAD));

      // Hey Mr. Script.. , I'm dead now...
        if (script)
            script->Trigger(TRIGGER_DEAD);

        return;
    } 
    
    // ****************** all processing below is for alive characters only *****************
    // ********************************************************************************************

    // Is the character doing an autocombo or chain attack?
    if (
        lastattack && 
        PlayScreen.GameFrame() >= lastattackticks + lastattack->nextwait &&
        ((lastattack->flags & CA_AUTOCOMBO) ||  // Is autocombo or..
         ((lastattack->flags & CA_CHAIN) && chainhits > 0)) // Is chain attack..
        )
    {
        bool foundchain = false;

      // Are we in time (note: AUTO's don't expire like chains)
        if (!(lastattack->flags & CA_CHAIN) ||  // We're a chain
            (PlayScreen.GameFrame() - lastattackticks < lastattack->chainexptime)) // We haven't expired
        {
          // Get attack whose chainname matches last attack name    
            for (int32_t c = 0; c < chardata->attacks.NumItems(); c++)
            {
                SCharAttackData* ad = &(chardata->attacks[c]);

                if (!stricmp(lastattack->attackname, ad->chainname))
                {
                    if (lastattack->flags & CA_CHAIN)
                        chainhits--; // Decrease key hits for attack
                    SpecificAttack(c); // Do the next attack boyz
                    foundchain = true;
                    break;
                }
            }
        }

      // Couldn't find another chain or auto so clear chainhits
        if (!foundchain)
            chainhits = 0;
    }

  // PLAYER: Update the character's enemy list... and begin combat if targets in range...
    if (ObjClass() == OBJCLASS_PLAYER && !(PlayScreen.GameFrame() % FRAMERATE))
    {
        TCharacter* targ = FindClosestEnemy(GetFace(), 32); // Updates enemy list and HasSeen list
        if (targ && !targ->IsDead() &&  // Has targ, and targ is alive 
            !targ->IsInvisibleSpell() &&// Target is hidden with the invisible spell
            !IsFighting() &&            // We're not fighting now
            (AutoBeginCombat && autocombat) &&          // AutoBeginCombat option is enabled
            HasSeenAutoCombat(targ) &&  // Haven't already manually ended combat for targ
            Distance(targ) < chardata->combatrangemin &&    // Within combat range
            abs(AngleDiff(GetFace(), AngleTo(targ))) < 32)  // Character is facing
                BeginCombat(targ);

        if ((!targ) && (!autocombat))
            autocombat = true;

    }

    // Do the character's AI now
    AI();

    // Recover health, fatigue, and mana
    // This code checks last recovery time stamps (in game 100ths of a second) to see
    // if the health,fatigue, or mana for a character need to be updated.  The time stamps
    // are actually saved with the character data, so even if a character has been unloaded
    // for a while, when he is reloaded, he will have the proper health,mana,and fatigue based
    // on the elapsed game time.
    int32_t gametime = PlayScreen.GameTime();
    int32_t pcnt;
    if (gametime - lasthealthrecov >= Rules.healthrecovrate)
    {
        if (Health() < MaxHealth())
        {
            pcnt = ((gametime - lasthealthrecov) / Rules.healthrecovrate) * Rules.healthrecovval;
            SetHealth(min(MaxHealth(), Health() + (MaxHealth() * pcnt / 100)));
        }
        lasthealthrecov = gametime;
    }
    if (gametime - lastfatiguerecov >= Rules.fatiguerecovrate)
    {
        if (Fatigue() < MaxFatigue())
        {
            pcnt = ((gametime - lastfatiguerecov) / Rules.fatiguerecovrate) * Rules.fatiguerecovval;
            SetFatigue(min(MaxFatigue(), Fatigue() + (MaxFatigue() * pcnt / 100)));
        }
        lastfatiguerecov = gametime;
    }
    if (gametime - lastmanarecov >= Rules.manarecovrate)
    {
        if (Mana() < MaxMana())
        { 
            pcnt = ((gametime - lastmanarecov) / Rules.manarecovrate) * Rules.manarecovval;
            SetMana(min(MaxMana(), Mana() + (MaxMana() * pcnt / 100)));
        }
        lastmanarecov = gametime;
    }
    if (gametime - lastpoisondamage >= Rules.poisondamagerate)
    {
        if (Poisoned())
        {
            if (lastpoisondamage != -1)
            {
                if (Health() > 1)
                { 
                    pcnt = ((gametime - lastpoisondamage) / Rules.poisondamagerate) * Rules.poisondamageval;
                    if ((MaxHealth() * (float)pcnt / 100.0f) >= 1)
                        SetHealth(int32_t(Health() - max(1,(MaxHealth() * (float)pcnt / 100.0f))));
                    else
                        SetHealth(1);
                }
            }
        }
        lastpoisondamage = gametime;
    }

  // This code below SHOULD be in ResolveInvoke()!
    if (IsDoing(ACTION_INVOKE))
    {
        if (invokedelay > 0)
        {
            invokedelay--;
            if (invokedelay == 0)
            {
                root->interrupt = true;
                SetDesired(root);
            }
        }
    }
        
    // Handle affects
/*  if (IsPoisoined()poison > 0)
    {
        poisonaccum += poison;
        int32_t dam = poisonaccum / POISON_SPEED;
        if (dam)
        {
            Damage(dam, DAMAGE_POISON);
            poisonaccum -= dam * POISON_SPEED;

            // give a chance for it to wear off
//          if (GetResistance(DAMAGE_POISON) >= random(0, 100) || random(0, 20) == 7)
//              poison--;
        }
    }
*/
}

void TCharacter::UpdateAction(int32_t bits)
{
  // Reset stealth values for every move (when not in root state)
    if (PlayScreen.GameFrame() % FRAMERATE)
        ResetStealthValues();
//  if ((commanddone && root != doing) || glimpse < 0 || noise < 0)
//      ResetStealthValues();

  // Set sleep to awake if doing non sleeping root states
    if (!root->Is(ACTION_SLEEP))
        SetSleeping(0);

  // ********************************************************************
  // Animation 'commanddone' state is set, now process this state through
  // the ACTION handlers.  If nothing sets the command state, we set it by
  // seeing if the character is doing anything.  This is defined as either
  // being in the root state, having the current animation completed.
    int32_t comstate = ResolveAction(bits);
    if (comstate == 0)
        comstate = (commanddone || !HasAnimator() || (flags & OF_INVISIBLE)) ? // If done, or not animating..                                        // Done if finished animating
            COM_COMPLETED : COM_EXECUTING;

  // Reset priority when command complete   
    if (comstate == COM_COMPLETED && doing->priority)
        doing->priority = false;

  // Set firstime flag to false
    if (doing->firsttime)
        doing->firsttime = false;

  // ********************************************************************
  // Now continue the script (before the code below sets the Action Block for the
  // next frame.  The script is currently paused if this code is running.  Ordinary
  // script CMD_WAITS always wait on the next command being finished (WAIT_NOTHING).
  // Special waits can set the WAIT_ flags to wait on other things.
    if (GetScript())
    {
        bool continuescript = false;

        if (waittype == WAIT_NOTHING && comstate == COM_COMPLETED) // Ordinary wait action done
            continuescript = true;
        else if (waittype == WAIT_TICKS)                           // Wait for ticks done
        {
            waitticks--;
            if (waitticks <= 0)
                continuescript = true;  
        }
        else if (waittype == WAIT_RESPONSE && DialogPane.HasResponded())  // Wait for a response    
        {
            ScriptJump(DialogPane.GetResponseLabel());
            waittype = WAIT_NOTHING;
            continuescript = true;
            DialogPane.Hide(); // Done with dialog pane, now hide it
        }
        else if (waittype == WAIT_CHAR_DONE && comstate == COM_COMPLETED)
        {
            if (!doing->obj || ((TCharacter*)doing->obj)->IsInRoot())
                continuescript = true;
        }

      // NEXT LINE: Next line in script
        if (continuescript)
        {
            commanddone = true;             // Force animation state to done
            comstate = COM_COMPLETED;       // Force command state to COMPLETED
            if (desired)                    // Break whatever is animating!
                desired->priority = false;
            if (doing)                      // Break whatever is animating!
                doing->priority = false;
            waittype = WAIT_NOTHING;        // Don't wait anymore
            ContinueScript();
        }
    }


  // ********************************************************************
  // Now set the action blocks for the next frame.  The rules are:
  //
  // If an action block is compleded, and it's not a ROOT2ROOT block, it
  // just sits there with the desired and doing doing the same thing.
  // If the animation is a looping animation, it resets the frame and does
  // it again.  If the animation is ROOT2ROOT, it just transitions back to
  // the root state.

  // Cause character to fall if move bits flag has fall
    if (bits & MOVE_FALLING)
        comstate = ForceCommand(new TActionBlock("fall"), bits);
    else
        comstate = TryCommand(desired, bits);

  // ********************************************************************
  // Decrement the wait value (if any)

    if (doing && doing->wait > 0)
        doing->wait--;
}

int32_t TCharacter::ResolveAction(int32_t bits)
{
    int32_t comstate = TComplexObject::ResolveAction(bits);
    if (comstate != 0)
        return comstate;

  // Check for movement 
    if (doing->action == ACTION_MOVE || (IsRunMode() && IsMoving()))
        comstate = ResolveMove(doing, bits);
    else if (doing->action == ACTION_ATTACK) 
        comstate = ResolveAttack(doing, bits);
    else if (doing->action == ACTION_IMPACT || 
             doing->action == ACTION_KNOCKDOWN || 
             doing->action == ACTION_STUN)
        comstate = ResolveImpact(doing, bits);
    else if (doing->action == ACTION_BLOCK)
        comstate = ResolveBlock(doing, bits);
    else if (doing->action == ACTION_COMBATMOVE || 
             doing->action == ACTION_BOWMOVE)
        comstate = ResolveCombatMove(doing, bits);
    else if (doing->action == ACTION_COMBAT || 
             doing->action == ACTION_BOW)
        comstate = ResolveCombat(doing, bits);
    else if (doing->action == ACTION_COMBATLEAP)
        comstate = ResolveLeap(doing, bits);
    else if (doing->action == ACTION_BOWAIM)
        comstate = ResolveBowAim(doing, bits);
    else if (doing->action == ACTION_BOWSHOOT)
        comstate = ResolveBowShoot(doing, bits);
    else if (doing->action == ACTION_SAY)
        comstate = ResolveSay(doing, bits);
    else if (doing->action == ACTION_PULL)
        comstate = ResolvePull(doing, bits);
    else if (doing->action == ACTION_PIVOT)
        comstate = ResolvePivot(doing, bits);
    else if (doing->action == ACTION_DEAD)
        comstate = ResolveDead(doing, bits);
    
    return comstate;
}

void TCharacter::Animate(bool draw)
{
    TObjectInstance::Animate(draw);

    if (HasAnimator() && draw && doing &&
        doing->action == ACTION_SAY &&
        doing->data &&
        doing->wait > 12) // Leave a half a second between sentences
    {
        SRect r;
        Display.GetClipRect(r);

        SColor color = { 0, 150, 255 };

        if (this == (TCharacter*)Player)
        {
            color.red = 200;
            color.green = 0;
            color.blue = 0;
        }

        int32_t x, y;
        WorldToScreen(pos, x, y);

        PlayScreen.AddPostCharText((char *)doing->data, x, y - 120, &color, r.right - r.left + 1 - 32);
    }

    SpellManager.Pulse();
}

void TCharacter::Notify(int32_t notify, void *ptr)
{
    // **** WARNING!!! MAKE SURE YOU CHECK FOR BROKEN LINKS AND DELETED OBJECTS HERE!!! ****
    // If you want to be notified, you must call SetNotify() in your contsructor

    TComplexObject::Notify(notify, ptr);

    if (Fighting() && (notify == N_DELETINGOBJECT || notify == N_DELETINGSECTOR))
    {
        if (NOTIFY_DELETED(ptr, Fighting()))
            SetFighting(nullptr);
    }

  // Check hasseen list for deleted objects
    for (int32_t c = 0; c < MAXHASSEEN; c++)
    {
        
        if (hasseen[c].chr && NOTIFY_DELETED(ptr, hasseen[c].chr))
        {
            hasseen[c].chr = nullptr;
            hasseen[c].time = 0;
            hasseen[c].noautocombat = false;
        }
    }
}

#define MAXZMOVE 32

// REVSYNC: retail TCharacter::Blocked @ 0x4c39d0
//   recon/discovered/cls_0x5a7b98_TCharacter_Blocked_4c39d0.cpp (size 490).
//   Vetted — same shape as retail: GetWalkHeightRadius if moving (else
//   GetWalkHeight), reject on |dz| > 0x21 (= MAXZMOVE+1=33), then char-vs-char
//   blocking via CharBlocking which iterates a 128-radius TMapIterator and
//   returns the closest blocking character.
//
// Retail differences we have NOT folded in (low-impact for AI parity):
//   - Retail tests `param_1[0x3b] != 0` (movetopos active) and SKIPS the
//     char-blocking pass — i.e. ignore other characters as obstacles when
//     moving to a scripted target. Our version always char-blocks. This
//     can manifest as scripted moves stuck behind another NPC; not seen
//     in Demo 1 because nothing scripts MoveTo() through a crowd.
//   - Retail also masks against `(*(byte *)(param_1 + 2) & 0x80)` which
//     is OF_PARALIZE — we already test OF_PARALIZE in Move() before
//     calling Blocked() so the gate is effectively the same.
bool TCharacter::Blocked(S3DPoint &pos, S3DPoint &newpos, uint32_t bits, int32_t *height, TCharacter* *bychar)
{
    int32_t h;
    if (!height)
        height = &h;

  // Get walk map values in current area
    int32_t mindelta, maxdelta;
    if (bits & MOVE_NOTMOVING)
    {
        *height = MapPane.GetWalkHeight(newpos);  // If not moving, just get what's under us
        maxdelta = mindelta = 0;
    }
    else
    {
      // If moving, check Radius() around us to see if changes (deltas) between walk
      // grids go to far up (maxdelta - walls, barriers), or too far down (mindelta -
      // holes, etc.)  Also returns height underneath us.
        MapPane.GetWalkHeightRadius(newpos, Radius(), mindelta, maxdelta, *height);
    }

    int32_t aniflags = GetAniFlags();

    TCharacter* dummybychar;
    if (!bychar)
        bychar = &dummybychar;
    *bychar = nullptr;

  // Are we blocked by other characters?
    bool charblocking = true;
    if (doing->attack && doing->attack->flags & CA_INTERACTIVE)
        charblocking = false;           // No blocking on interactive attacks
    if (aniflags & AF_FLY)
        charblocking = false;           // No blocking on flying moves

  // Are we blocked? 
    bool blocked = !(bits & MOVE_FALLING) && 
        (abs(pos.z - *height) > MAXZMOVE || 
        abs(maxdelta) > MAXZMOVE || abs(mindelta) > MAXZMOVE ||
        *height == 0 || (charblocking && (*bychar = CharBlocking(this, newpos, Radius()))));
         
    return blocked;
}


#define CHARMOVE_MULTIPLIER     (1.0)
uint32_t TCharacter::Move()
{
    if (flags & OF_IMMOBILE || inventnum >= 0 || flags & OF_PARALIZE)
        return MOVE_NOTHING;                

    uint32_t retval = 0;

  // Falling stuff  
    int32_t h = MapPane.GetWalkHeight(pos);
    int32_t d = pos.z - h;
    if (d < -16)
    {
        S3DPoint p = pos;
        p.z = h;
        ForcePos(p);
    }
    else if (d > 16)
    {
        vel.z = max(vel.z - GRAVITY, -TERMINAL_VELOCITY);
//      vel.z = max(vel.z, d);
        retval |= MOVE_FALLING;
    }
    if (d < 1)
        vel.z = 0;

    int32_t targdist = 0;
    if (doing->obj)
        targdist = Distance(doing->obj);

  // Get nextmove
    S3DPoint nextmove;
    GetNextMove(nextmove);

  // Get movement and add to accumulator
    S3DPoint newaccum;
    if (movetopos)              // If movetopos is set, set newaccum to correct deltas
    {
        newaccum.x = (movepos.x - pos.x) * ROLLOVER;
        newaccum.y = (movepos.y - pos.y) * ROLLOVER;
        newaccum.z = (movepos.z - pos.z) * ROLLOVER;

        movepos.x = movepos.y = movepos.z = 0;

        movetopos = false;
    }   
    else                        // Otherwise use nextmove and velocity
    { 
        
        newaccum = accum;
        if (forcenomove)
            forcenomove = false;
        else
        {
            if (IsDoing(ACTION_COMBATMOVE) && !doing->waitpivot && chardata->combatwalkspeed > 0)   // Set combat move speed
                ConvertToVector(moveangle, ROLLOVER * chardata->combatwalkspeed, nextmove);

            else if (IsDoing(ACTION_MOVE) && !doing->waitpivot && root->Is("walk") && chardata->walkspeed > 0)
                ConvertToVector(moveangle, ROLLOVER * chardata->walkspeed, nextmove);

            else if (IsDoing(ACTION_MOVE) && !doing->waitpivot && root->Is("sneak") && chardata->sneakspeed > 0)
                ConvertToVector(moveangle, ROLLOVER * chardata->sneakspeed, nextmove);

            else if (IsDoing(ACTION_MOVE) && !doing->waitpivot && root->Is("run") && chardata->runspeed > 0)
                ConvertToVector(moveangle, ROLLOVER * chardata->runspeed, nextmove);
            newaccum += nextmove;
        }
        newaccum += vel;
    }


  // Clear snap if snap time expired
//  if (snapticks == 0 || Editor || IsDead())
//  {
//      snapticks = -1;
//      S3DPoint newvel;
//      GetVel(newvel);
//      newvel.x = newvel.y = 0;
//      SetVel(newvel);
//  } else if (snapticks > 0)
//      snapticks--;

  // Split into smaller moves if this move too big
    S3DPoint smallaccum;
    int32_t nummoves;
    if (abs(newaccum.x) > abs(newaccum.y) && abs(newaccum.x) >= (MOVECHECKDIST * ROLLOVER))
    {  
        nummoves = 
            (abs(newaccum.x) + (MOVECHECKDIST * ROLLOVER - 1)) / 
                (MOVECHECKDIST * ROLLOVER); // Get rounded up number of moves
        smallaccum.x = newaccum.x / nummoves;
        smallaccum.y = newaccum.y / nummoves;
        smallaccum.z = newaccum.z / nummoves;
    }
    else if (abs(newaccum.y) > abs(newaccum.x) && abs(newaccum.y) >= (MOVECHECKDIST * ROLLOVER))

    {
        nummoves = 
            (abs(newaccum.y) + (MOVECHECKDIST * ROLLOVER - 1)) / 
                (MOVECHECKDIST * ROLLOVER); // Get rounded up number of moves
        smallaccum.x = newaccum.x / nummoves;
        smallaccum.y = newaccum.y / nummoves;
        smallaccum.z = newaccum.z / nummoves;
    }

    else
    {
        smallaccum = newaccum;  // Fits within maximum move range for single step
        nummoves = 1;
    }

  // Loop through small moves do to complete move (usually only one loop for normal moves)
    for (int32_t move = 0; move < nummoves; move++)
    {
        S3DPoint moveaccum = smallaccum;
        S3DPoint newpos = pos;
        rollover(moveaccum.x, newpos.x);
        rollover(moveaccum.y, newpos.y);
        rollover(moveaccum.z, newpos.z);

        if (newpos == pos &&
            nextmove.x == 0 && nextmove.y == 0 && nextmove.z == 0 &&
            vel.x == 0 && vel.y == 0 && vel.z == 0)
                retval |= MOVE_NOTMOVING;

        int32_t nh;

      // Check to see if character is blocked
        if (Blocked(pos, newpos, retval, &nh)) // New position is blocked
        {
          // Check to see if character is totally stuck...
            if (!Blocked(pos, pos, retval, &nh))
                retval |= MOVE_BLOCKED; // If he's not totally stuck, report him as blocked
        }
        else
        {
            shovedir = -1;      // Not blocked anymore!  Reset block direction choice

            vel.x = vel.y = vel.z = 0;

            if (newpos.z != nh)
                if (vel.z < 0)
                    newpos.z = (int32_t)(nh * CHARMOVE_MULTIPLIER);
                else
                    newpos.z += (int32_t)(((nh - newpos.z) / 2) * CHARMOVE_MULTIPLIER);

            if (retval & MOVE_NOTMOVING)
            {
                accum = moveaccum;
                return MOVE_NOTHING;        // can only check this _after_ gravity
            }
        }

        // If blocked, check in front of char at walk granularity intervals for an open
        // path, if we find one, shove the character in that direction, and save the dir
        // in 'shovedir' so we keep shoving in the same direction until the character is 
        // stuck, or he's around the obstacle.

        if (retval & MOVE_BLOCKED) // Try to nudge!
        {
          // REVSYNC: retail TCharacter::MoveStep shove probe @ 0x4c3bc0:304-381
          // (recon/discovered/cls_0x5a7b98_TCharacter_MoveStep_4c3bc0.cpp).
          //
          // shovedir is an *angle offset* (-0x20, +0x20, -0x40, +0x40)
          // relative to moveangle, NOT a final angle. Retail field +0x47.
          // -1 means "not yet committed". When uncommitted, probe all 4
          // offsets at distances 2, 4, 6; pick the (offset, distance)
          // pair with the largest unblocked reach. Once committed, on
          // subsequent ticks only re-probe the committed offset until it
          // either clears (move forward) or fails (stay blocked).
          //
          // Action-state gate (retail 0x4c3bc0:307-309): SKIP the probe
          // entirely when the character is committed to a combat-anchored
          // action with a target — the AI body owns repositioning in that
          // case (e.g. circling, dodging) and a sidestep would fight it.
          // Negated (skip when ALL true): root != null && root->action is
          // ACTION_COMBAT or ACTION_BOW && root->obj != null && local_2c.
          // local_2c is set elsewhere in MoveStep and we don't yet know
          // what it tracks (probably "doing has obstacle awareness");
          // for now we approximate the gate without it.
          // TODO retail: pin down local_2c semantics and add to the gate.
            const bool combat_anchored =
                root && (root->action == ACTION_COMBAT || root->action == ACTION_BOW)
                     && root->obj != nullptr;

            if (!combat_anchored)
            {
                constexpr int32_t kOffsets[4] = { -32, +32, -64, +64 };

                int32_t numoffsets;
                const int32_t *which;
                int32_t single[1] = { shovedir };
                if (shovedir == -1) {
                    numoffsets = 4;
                    which      = kOffsets;
                } else {
                    numoffsets = 1;
                    which      = single;
                }

                int32_t best_step = 0;        // 0 = nothing reachable yet
                int32_t best_off  = -1;
                S3DPoint best_pos = newpos;   // committed candidate position

                for (int32_t k = 0; k < numoffsets; k++)
                {
                    const int32_t off = which[k];
                    for (int32_t step = 2; step < 8; step += 2)  // 2, 4, 6
                    {
                        S3DPoint v;
                        ConvertToVector((moveangle + off) & 0xff, step, v);
                        S3DPoint cand = pos;
                        cand += v;

                        uint32_t probe_bits = retval;  // preserve falling/etc
                        if (Blocked(pos, cand, probe_bits))
                            break;  // this offset's clearance ends here — stop
                                    // probing further distances on the same offset
                        if (step > best_step)
                        {
                            best_step = step;
                            best_off  = off;
                            best_pos  = cand;
                        }
                    }
                }

                if (best_step > 0)
                {
                  // Sanity-recheck the chosen candidate (retail 0x4c3bc0:352
                  // — catches char-blocking races between probe and commit).
                    uint32_t recheck_bits = retval;
                    if (!Blocked(pos, best_pos, recheck_bits))
                    {
                      // Commit (retail 0x4c3bc0:354-362): zero velocity and
                      // accumulator so we don't carry forward momentum into
                      // the obstacle. Without this monsters jitter against
                      // the wall as their forward velocity fights the
                      // sideways shove.
                        shovedir   = best_off;
                        newpos     = best_pos;
                        moveaccum  = S3DPoint{};      // zero accum
                        accum      = S3DPoint{};
                        vel        = S3DPoint{};      // zero velocity
                        retval    &= ~MOVE_BLOCKED;
                    }
                  // else: leave shovedir as-is and stay blocked this tick;
                  // next tick will re-probe the committed offset.
                }
            }

          // If after the probe (or the gate-skip) we're STILL blocked,
          // retail 0x4c3bc0:371-379 invalidates sight tracking — the path
          // to whatever we were tracking is broken, drop the cached sight
          // state so the AI body re-acquires next tick.
            if (retval & MOVE_BLOCKED)
            {
                if (target_out_of_sight)
                {
                    target_out_of_sight      = false;
                    target_out_of_sight_prev = false;
                    sight_lost_ticks         = 0;
                }
            }
        }

      // We've done what we could to go forward... now..
        if (retval & MOVE_BLOCKED)
        {
            break;      // We're blocked.. end the loop
        }
        else            // Set new position add add accumulator
        {
            retval |= MOVE_MOVED;

            accum = moveaccum;

            if (newpos.z < (MapPane.GetWalkHeight(pos) * CHARMOVE_MULTIPLIER))
                newpos.z = (int32_t)(MapPane.GetWalkHeight(pos) * CHARMOVE_MULTIPLIER);

            if (newpos != pos)
                SetPos(newpos);

        }
    }


    return retval;
}

int32_t TCharacter::GetDamageType(int32_t weapontype, int32_t attackflags)
{
    if (weapontype == WT_HAND)
    {
        return DT_HAND;
    }
    else if (weapontype == WT_KNIFE || weapontype == WT_SWORD)
    {
        if (attackflags & CA_THRUST)
            return DT_PUNCTURE;
        else if (attackflags & CA_SLASH)
            return DT_CUT;
        else if (attackflags & CA_CHOP)
            return DT_CHOP;
    }
    else if (weapontype == WT_BLUDGEON)
    {
        if (attackflags & CA_THRUST)
            return DT_NONE;
        else if (attackflags & CA_SLASH)
            return DT_BLUDGEON;
        else if (attackflags & CA_CHOP)
            return DT_BLUDGEON;
    }
    else if (weapontype == WT_AXE)
    {
        if (attackflags & CA_THRUST)
            return DT_NONE;
        else if (attackflags & CA_SLASH)
            return DT_CHOP;
        else if (attackflags & CA_CHOP)
            return DT_CHOP;
    }
    else if (weapontype == WT_BOW)
    {
        return DT_NONE; // Bow can't hurt anybody, only arrows
    }

    return DT_NONE;
}

// Get the total damage amount (based on this function)
int32_t TCharacter::CalculateDamage(int32_t damage, int32_t damagetype, int32_t modifier)
{
    int32_t attackdamage, totaldamage;
    if (damagetype == DT_NONE)
        return 0;
    else
    {
        attackdamage = damage * (100 + modifier) / 100;
        totaldamage = attackdamage * (100 + DamageModifier(damagetype)) / 100;
    }

    return totaldamage;
}

void TCharacter::Damage(int32_t damage, int32_t damagetype, int32_t modifier,
    TActionBlock* action, TCharacter* attacker)
{
  // Calculate total damage
    if (damagetype >= 0)
        damage = CalculateDamage(damage, damagetype, modifier);

  // Apply damage to low level object
    TObjectInstance::Damage(damage);

  // Get impact pointer
    SCharAttackImpact* impactdata = nullptr;

  // Do death...
    if (Health() < 1)
    {
/*      if (!random(0, 2))
        {
            S3DPoint vel;
            vel.x = random(-8, 8);
            vel.y = random(-8, 8);
            vel.z = random(7, 12); 
            int32_t count = random(6, 16);
            Pulp(vel, count, count * 30);
        }
*/
        TActionBlock* death = action;

      // Caller didn't give us a special death to use so...
        if (!death)
        {
          // Find a 'death' impact in impact list (if there is one)
            const char *deathname = "dead";
            impactdata = chardata->impacts;
            int32_t i;
            for (i = 0; i < chardata->numimpacts; i++, impactdata++)
            {
                if (damage >= impactdata->damagemin &&
                    damage <= impactdata->damagemax &&
                    (impactdata->flags & CAI_DEATH) &&
                    (!(impactdata->flags & CAI_WHENSTUNNED) || doing->action == ACTION_STUN) &&
                    (!(impactdata->flags & CAI_WHENDOWN) || doing->action == ACTION_KNOCKDOWN) &&
                    HasActionAni(impactdata->impactname) &&
                    (impactdata->loopname[0] == '\0' || HasActionAni(impactdata->loopname)))
                {
                    deathname = impactdata->impactname;
                    break;
                }
            }

            if (i >= chardata->numimpacts)      // Not found, use default "dead" animation!
                impactdata = nullptr;

            if (!impactdata && !HasActionAni(deathname)) // Check to see if default "death" is there
                return;

            death = new TActionBlock(deathname, ACTION_DEAD);
            death->impact = impactdata;
        }
        else
            impactdata = death->impact;

        death->obj = doing->obj;
        death->damage = damage;
        death->interrupt = true;
        death->priority = true;
        death->loop = true;
        ForceCommand(root);     // Make sure we play "combat to" transitions
        ForceCommand(death);
    }

  // Or do impact...
    else
    {
        TActionBlock* impact = action;

      // Caller didn't give us a special impact to use so...    
        if (!impact)
        {
          // Find default impact from char's default impact list
            const char *impactname = "impact";
            ACTION impactaction = ACTION_IMPACT;
            impactdata = chardata->impacts;
            int32_t i;
            for (i = 0; i < chardata->numimpacts; i++, impactdata++)
            {
                if (damage >= impactdata->damagemin &&
                    damage <= impactdata->damagemax &&
                    !(impactdata->flags & CAI_DEATH) &&
                    (!(impactdata->flags & CAI_WHENSTUNNED) || doing->action == ACTION_STUN) &&
                    (!(impactdata->flags & CAI_WHENDOWN) || doing->action == ACTION_KNOCKDOWN) &&
                    HasActionAni(impactdata->impactname) &&
                    (impactdata->loopname[0] == '\0' || HasActionAni(impactdata->loopname)))
                {
                    impactname = impactdata->impactname;
                    if (impactdata->flags & CAI_STUN)
                        impactaction = ACTION_STUN;
                    else if (impactdata->flags & CAI_KNOCKDOWN)
                        impactaction = ACTION_KNOCKDOWN;
                    break;
                }
            }

            if (i >= chardata->numimpacts)      // Not found, use default "impact" animation!
                impactdata = nullptr;

            if (!impactdata && !HasActionAni(impactname)) // Check to see if default "impact" is there
                return;

            impact = new TActionBlock(impactname, impactaction);
            impact->impact = impactdata;

            if (impactdata)
                impact->wait = impactdata->looptime;
        }
        else
            impactdata = impact->impact;

        impact->obj = doing->obj;
        impact->damage = damage;
        impact->interrupt = true;
        ForceCommand(root);     // Make sure we play "combat to" transitions
        ForceCommand(impact);

        if (impact && impact->attack)
            int32_t q = impact->attack->fatigue;
    }

  // Do snap/push if needed
    if (impactdata && impactdata->snapdist > 0 && attacker)
    {
        S3DPoint snap;
        GetSnapPos(attacker, impactdata->snapdist, snap);
//      if (impactdata->snaptime == 0)      // Immediate snap
//      {
            MoveTo(snap);
//      }
//      else                                // Time based snap
//      {
//          int32_t rollsnap = impactdata->snapdist * ROLLOVER / max(impactdata->snaptime, 1);
//          S3DPoint snapvect;
//          ConvertToVector(attacker->AngleTo(this), rollsnap, snapvect);
//          SetVel(snapvect);
//          snapticks = impactdata->snaptime;
//      }
    }

}

void TCharacter::RestoreHealth()
{
    SetPoisoned(false);
    SetHealth(MaxHealth());
}

// Returns true if character has seen 'me'
bool TCharacter::HasSeenMe(TCharacter* me)
{
    for (int32_t c = 0; c < MAXHASSEEN; c++)
    {
        if (hasseen[c].chr == me &&
            PlayScreen.GameFrame() - hasseen[c].time < MAXSEENTIME)
                return true;
    }

    return false;
}

// Add me to the HASSEEN list
void TCharacter::SetHasSeen(TCharacter* me)
{
    int32_t lowest = 0x7FFFFFFF;
    int32_t lowestnum = 0;

    for (int32_t c = 0; c < MAXHASSEEN; c++)
    {
        if (hasseen[c].chr == me)
        {
            hasseen[c].time = PlayScreen.GameFrame();
            return;
        }
        if (hasseen[c].time < lowest)
        {
            lowest = hasseen[c].time;
            lowestnum = c;
        }
    }

    hasseen[lowestnum].chr = me;
    hasseen[lowestnum].time = PlayScreen.GameFrame();
    hasseen[lowestnum].noautocombat = false;    // First time seen char.. set autocombat on
}

// Returns true if character has seen 'me'
bool TCharacter::HasSeenAutoCombat(TCharacter* me)
{
    for (int32_t c = 0; c < MAXHASSEEN; c++)
    {
        if (hasseen[c].chr == me)
            return !hasseen[c].noautocombat;
    }

    return true;
}

// Returns true if character has seen 'me'
void TCharacter::SetHasSeenAutoCombat(bool on)
{
    for (int32_t c = 0; c < MAXHASSEEN; c++)
    {
        hasseen[c].noautocombat = !on;
    }
}

// Returns the percentage of transparency for character's imagery
int32_t TCharacter::Transparency()
{
    if (Editor ||
        !Player || this == Player || 
        !Aggressive() || 
        IsDead() ||
        Player->HasSeenMe(this))
            return max(0, min(fade, 100));
    else
        return min(fade, 0);
}

// DLS brightness routine (gives brightness given distance)
extern double GetLightBrightness(int32_t dist, int32_t intensity, int32_t multiplier);

// Returns the total visibility 1-100 for character (based on lights, ambient, and fog, etc.)
int32_t TCharacter::Visibility()
{
    int32_t brightness = 0;

    SColor color = MapPane.GetAmbientColor();
    int32_t colorbrightness = ((int32_t)color.red + (int32_t)color.green + (int32_t)color.blue) / 3;
    brightness = MapPane.GetAmbientLight() * colorbrightness / 38;

    for (TMapIterator i(*this, CHECK_MAPRECT | CHECK_NOINVENT, OBJSET_LIGHTS); i; i++)
        brightness += i->GetIllumination(this);
    
    if (brightness > 255)
        brightness = 255;

    return brightness * 100 / 255;
}

void TCharacter::AdvanceAngles(int32_t faceang, int32_t moveang, int32_t maxturn)
{
    int32_t dif;

  // Do face angle
    faceang &= 255;

    dif = faceang - GetFace();
    if (dif > 128)
        dif = dif - 256;
    if (dif < -128)
        dif = dif + 256;

    int32_t newfaceang;
    if (dif < 0)
        newfaceang = (GetFace() + max(dif, -maxturn)) & 255;
    else
        newfaceang = (GetFace() + min(dif, maxturn)) & 255;

  // Do move angle
    moveang &= 255;

    dif = moveang - GetMoveAngle();
    if (dif > 128)
        dif = dif - 256;
    if (dif < -128)
        dif = dif + 256;

    int32_t newmoveang;
    if (dif < 0)
        newmoveang = (GetMoveAngle() + max(dif, -maxturn)) & 255;
    else
        newmoveang = (GetMoveAngle() + min(dif, maxturn)) & 255;

    if (newfaceang != GetFace())
        FaceOnly(newfaceang);

    if (newmoveang != GetMoveAngle())
        SetMoveAngle(newmoveang);
}

#define MAXCHANGE   200
#define STOPHEIGHT  50

int32_t TCharacter::UpdateAngle(int32_t angle)
{
/*
    int32_t ba = angle;
    int32_t bd = 1000000;

    for (int32_t a = angle - 16; a < angle + 16; a += 1)
    {
        S3DPoint vect;
        ConvertToVector(a, 250, vect);

        int32_t lh = MapPane.GetWalkHeight(pos), delta = 0, h;
        S3DPoint targ = pos, i = pos;
        targ += vect;

        if (vect.x == 0)
        {
            for (i.y = pos.y + 1; i.y < targ.y; i.y++)
            {
                h = MapPane.GetWalkHeight(i);
                delta += absval(h - lh);
                lh = h;

                if (h > STOPHEIGHT || delta > MAXCHANGE)
                    break;
            }
        }
        else if (vect.y == 0)
        {
            for (i.x = pos.x + 1; i.x < targ.x; i.x++)
            {
                h = MapPane.GetWalkHeight(i);
                delta += absval(h - lh);
                lh = h;

                if (h > STOPHEIGHT || delta > MAXCHANGE)
                    break;
            }
        }
        else
        {
            int32_t dist = (int32_t)sqrt((double)(sqr(vect.x) + sqr(vect.y)));
            float yr = (float)vect.y / (float)dist;
            float xr = (float)vect.x / (float)dist;
            S3DPoint oldpoint = i;

            for (int32_t j = 1; j < dist; j++)
            {
                i.x = (int32_t)(xr * (float)j);
                i.y = (int32_t)(yr * (float)j);
                i += pos;

                if (oldpoint.x != i.x && oldpoint.y != i.y)
                {
                    h = MapPane.GetWalkHeight(i);
                    delta += absval(h - lh);
                    lh = h;
                }

                oldpoint = i;

                if (h > STOPHEIGHT || delta > MAXCHANGE)
                    break;
            }
        }

        int32_t d = (int32_t)sqrt((double)SQRDIST(i, targ));

        if (d < bd)
        {
            bd = d;
            ba = a;
        }
    }

    return ba;
*/
    int32_t ch = MapPane.GetWalkHeight(pos);
    int32_t nudge = 0;
    S3DPoint c;
    c.z = 0;

    // hum...need to make this keep some sort of static nudge
    // value in order to avoid the quivering affect.
    // possibly need to reset that when the target location
    // is changed, as well.
    // also - in the case of a coridor, need to make sure that
    // no matter what way they are facing it always sends them
    // straight down it.

    for (c.y = pos.y - 32; c.y <= (pos.y + 32); c.y += 16)
        for (c.x = pos.x - 32; c.x <= (pos.x + 32); c.x += 16)
        {
            if (c.y == pos.y && c.x == pos.x)
                continue;

            if (absval(MapPane.GetWalkHeight(c) - ch) < 30)
                continue;

            int32_t dist = (sqr(64) - SQRDIST(c, pos)) / 100;
            dist = min(50, max(1, dist));

            int32_t ang = ConvertToFacing(pos, c) - GetFace();

            int32_t weight = (dist * (64 - absval(ang))) / 100;

            if (weight < 1)
                continue;

            if (ang > 0)
                nudge -= weight;
            else
                nudge += weight;
        }

    return angle;
}

TObjectInstance* TCharacter::FindObjAhead()
{
    S3DPoint pos, v;
    GetPos(pos);
    ConvertToVector(GetFace(), 60, v);
    pos += v;

    int32_t list[MAXFOUNDOBJS];
    int32_t n = MapPane.FindObjectsInRange(pos, list, 60);

    TObjectInstance* best = nullptr;
    int32_t bestdist;

    for (int32_t i = 0; i < n; i++)
    {
        TObjectInstance* oi = MapPane.GetInstance(list[i]);
        if (!oi)
            continue;

        if (oi->CursorType() != CURSOR_NONE || oi->IsInventoryItem())
        {
            int32_t dist = Distance(oi);

            if (best == nullptr || dist <= bestdist)
            {
                best = oi;
                bestdist = dist;
            }
        }
    }

    return best;
}

// ***********************
// * General AI Routines *
// ***********************

int32_t TCharacter::ResolveMove(TActionBlock* ab, int32_t bits)
{
  // Do pivoting before moving (character stays in root neutral state until pivot is done, 
  // then sets the first step action block.  No block checking is done when pivoting
    if (ab->waitpivot)
    {
        Halt(); // Make sure there's no movement

        bool pivotdone = false;
        if (!stricmp(ab->name, root->name)) // Is a normal root animation we're manually turning...
        {
            if (GetFace() == ab->angle)
                pivotdone = true;
            else
                AdvanceAngles(ab->angle, ab->angle, ab->turnrate);
        }
        else if (commanddone)               // Is a special pivot animation which turns itself!
            pivotdone = true;

        if (!pivotdone)
            return 0;
        else
        {
            TActionBlock* newab = nullptr;

            if (HasActionAni(StName(root->name, "f")))
                newab = new TActionBlock(*doing, StName(root->name, "f"));
            else if (HasActionAni(StName(root->name, "l")))
                newab = new TActionBlock(*doing, StName(root->name, "l"));
            else if (HasActionAni(StName(root->name, "r")))
                newab = new TActionBlock(*doing, StName(root->name, "r"));
            else
            {
                SetDesired(root);
                return COM_COMPLETED;
            }

            newab->waitpivot = false;
            newab->turnrate = MAXTURNRATE;
            newab->angle = newab->moveangle = ab->angle;
            ForceCommand(newab);            // Does this command again
            return 0;
        }
    }

  // Character is actually moving... check for blocked!
    if (bits & MOVE_BLOCKED)
    {
        Face(ab->angle);
        ForceCommand(root);
        return COM_COMPLETED;
    }

    bool advanceang = true;

  // Do goto position
    if (ab->target.x != 0 || ab->target.y != 0 || ab->target.z != 0)
    {
        if (dist(pos.x, pos.y, ab->target.x, ab->target.y) < 8)
        {
            ab->target.z = pos.z;
            MoveTo(ab->target);
            ab->nowaitdone = true;
            return COM_COMPLETED;
        }
        else
        {
            ab->angle = ab->moveangle = ConvertToFacing(pos, ab->target);
        }
    }

    SetMoveAngle(GetFace());    // Force move to same direction as face
    if (advanceang)
        AdvanceAngles(ab->angle, ab->angle, ab->turnrate);

    if (commanddone && !ab->stop) // Take another step unless we were stopped
    {
        TActionBlock* newab = new TActionBlock(*doing);
      // Left step
        if (doing->IsLeft(root->name))
            strcpy(newab->name, StName(root->name, "r"));
      // Right step   
        else if (doing->IsRight(root->name))
            strcpy(newab->name, StName(root->name, "l"));
        ForceCommand(newab);
    }

    return COM_EXECUTING;
}

// This function is called by the ResolveAttack() function to resolve hits for
// multiple characters.  The characters are usually found by calling the FindCharacters()
// function, then calling this function for each character.  Returns true if hit.
bool TCharacter::ResolveHit(TCharacter* targ, 
    SCharAttackData* attack, SCharAttackImpact* attackimpact, int32_t attackdamage)
{
    int32_t damage = 0;
    SCharAttackImpact* impact = nullptr;

    if (targ)
    {
        int32_t dist = Distance(targ);
        int32_t angle = FaceAngleTo(targ);

        // Check to see if impact is even possible? 
        if (dist >= attack->hitminrange &&
            dist <= attack->hitmaxrange && 
            abs(targ->pos.z - pos.z) <= MAXZHITRANGE && 
            abs(angle) <= attack->hitangle &&
            !targ->IsDead() &&
            !targ->IsInvisibleSpell())
        {
            // If within hit range, set has seen to true!
            targ->SetHasSeen(this);

            S3DPoint vect;
            ConvertToVector(facing, 4 * ROLLOVER, vect);
            targ->vel += vect;

          // Chance to hit is concatenation of all the below
          // Note: To hit value is based on the following formula
          // 
          //    Armor + DefenseModifier + FatigueModifier(always negative) - AttackModifier
          // 
          //    This TOHIT value should 'almost' always be less than 25.  If it is greater,
          //    the random value will be 1-(tohit + 1) to always give the char a chance to hit.  
          // 
            int32_t targangle = targ->FaceAngleTo(this);
            int32_t tohit = targ->ArmorValue() + targ->DefenseModifier() + 
                targ->FatigueModifier() - AttackModifier();
            int32_t maxroll = max(25, tohit + 1);           
            if (abs(targangle) < 48  &&                 // HALVE hit chances if blocking/dodging
                (targ->IsDoing(ACTION_BLOCK) || targ->IsDoing(ACTION_DODGE)))
                tohit += (maxroll - tohit + 1) * 3 / 4;
            if (abs(targ->FaceAngleTo(this)) >= 48 ||   // If not facing us or...
                targ->IsDoing(ACTION_ATTACK))           // is currently trying to do an attack
                tohit -= (maxroll - tohit + 1) / 2;     // Then DOUBLE hit chances

            // We hit, so set damage value to precalculated attack damage!
            if (random(1, maxroll) > tohit)
            {
                damage = attackdamage; 
                impact = attackimpact; // This can be nullptr;
            }

            // We hit the character (though we may not have done much damage)!
            if (!targ->IsDoing(ACTION_BLOCK) || damage > 0) // ****** CODE FOR IMPACT *******
            {
                // Get interactive DEATH
                TActionBlock* deathab = nullptr;
                TActionBlock* impactab = nullptr;
                TActionBlock* hitab = nullptr;
                if (targ->Health() - damage < 1)
                {
                    if (impact)
                        if ((impact->flags & CAI_DEATH) &&
                            targ->HasActionAni(impact->impactname))
                        {
                            deathab = new TActionBlock(impact->impactname, ACTION_DEAD);
                            deathab->obj = this;
                            deathab->priority = true;   // Don't interrupt period!
                            deathab->attack = attack;
                            deathab->impact = impact;
                            deathab->damage = damage;
                            targ->SetFighting(this);
                        }
                    hitab = deathab;
                }
                else    // Get interactive IMPACT
                {
                    // If we're not fighting anyone, fight the guy who just hit us
                    if ((!targ->Fighting()) && (!GetAutoCombat()))
                    {
                        // not fighting - shall we engage this target?
                        if (dist <= targ->chardata->combatrangemin)
                            targ->BeginCombat(this);
                    }

                    // Get impact animations
                    if (impact && targ->HasActionAni(impact->impactname))
                    {
                        ACTION a;
                        if (!impact)
                            a = ACTION_IMPACT;
                        else if (impact->flags & CAI_STUN)
                            a = ACTION_STUN;
                        else if (impact->flags & CAI_KNOCKDOWN)
                            a = ACTION_KNOCKDOWN;
                        else
                            a = ACTION_IMPACT;
                        impactab = new TActionBlock(impact->impactname, a);
                        impactab->obj = this;
                        impactab->interrupt = true; // Interrupt current char 'doing'
                        impactab->priority = true;  // Don't imterrupt until done!
                        impactab->attack = attack;
                        impactab->impact = impact;
                        impactab->damage = damage;
                        if (impact)
                            impactab->wait = impact->looptime;
                        targ->SetFighting(this); // If special impact, face char to impact
                    }
                    hitab = impactab;
                }

        //        Don't use this anymore... see below...
        //          targ->Damage(damage, 
        //                  GetDamageType(WeaponType(), attack->flags),  
        //                  attack->damagemod,
        //                  impactab, deathab);

          // Note: DamageType and damagemod's are already reflected in 'damage' total so we can get
          // the right impact/death for this attack.  This means we have to use DT_NONE to avoid
          // recalculating damage with damagetype modifiers, etc.
                targ->Damage(damage, DT_NONE, 0, hitab, this);

            }
        }
    }

    return damage > 0;              // Did we miss?
}

// Maximum number of characters we can hit at a time
#define MAXHITCHARS 32  // This should do it

int32_t TCharacter::ResolveAttack(TActionBlock* ab, int32_t bits)
{
    TCharacter* targ = (TCharacter*)ab->obj;
    SCharAttackData* attack = ab->attack;
    SCharAttackImpact* impact = ab->impact;

  // Set last attack stuff
    if (ab->firsttime)
    {
        lastattack = attack;
        lastattackticks = PlayScreen.GameFrame();
    }

  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

    int32_t dist, angle;
    if (targ)
    {
        dist = Distance(targ);
        angle = FaceAngleTo(targ);
    }
    else
    {
        dist = 0;
        angle = 0;
    }

  // Do this
    if (ab->firsttime)
    {
      // Signal our opponent that he's getting wailed on
        if (targ)
            targ->SignalAttack(this, ab->obj);
    }

  // Cause attack to push character... ONLY IF THERE IS SOMETHING *TO* KNOCK BACK!!!!
    if (targ)
        if (attack && 
            !(attack->flags & CA_NOPUSH) && 
            !(attack->flags & CA_INTERACTIVE) && 
            !(GetAniFlags() & AF_FLY) && 
            !(targ->GetAniFlags() & AF_FLY) && 
            (!impact || impact->snapdist <= 0) &&
            !targ->IsDead() &&
            !targ->IsInvisibleSpell() &&
            targ && dist <= (Radius() + targ->Radius() + 10))
        {
            S3DPoint nm;
            if (frame < attack->impacttime) // Before impact.. don't move
            {
                GetNextMove(nm);
                nm.x = nm.y = 0;
                SetNextMove(nm);
            }
            else
            {                                           // After impact, move monster (if not already moving)
                targ->Face((GetFace() + 128) & 255);
                targ->SetMoveAngle(GetFace());
                targ->SetMoveDist(targ->GetMoveDist() + GetMoveDist());
            }
        }

  // If we have target, and frame has reached trigger time for impact, then do the impact.
    if (attack && frame == attack->impacttime)
    {
        bool hit;

      // Do main target character
        if (targ)
            hit = ResolveHit(targ, ab->attack, ab->impact, ab->damage);

      // If this attack allows hits for other characters, do them too...
        if (!(attack->flags & CA_ONETARGET))
        {
            int32_t numchars;
            TCharacter* chars[MAXHITCHARS];

            numchars = FindCharacters(chars, MAXHITCHARS, 
                attack->hitmaxrange, GetFace(), attack->hitangle, FINDCHAR_ENEMY);

            for (int32_t c = 0; c < numchars; c++)
            {
                if (chars[c] != targ)
                    hit = (hit | ResolveHit(chars[c], ab->attack, ab->impact, ab->damage));
            }
        }

      // Do miss if we failed to hit anything   
        if (!hit)
        {
            TActionBlock* missab;

          // Do we play miss animation, or return straightway to combat state? 
            if (!(attack->flags & CA_NOMISS))
            {
                if (HasActionAni(attack->missname))
                {
                    missab = new TActionBlock(attack->missname, ACTION_MISS);
                    missab->attack = attack;
                }
                else
                {
                    missab = new TActionBlock(root->name, ACTION_COMBAT);
                    PlayWave(listrnd(chardata->misssounds));  // Play default sounds
                }
                missab->interrupt = true;
                missab->obj = ab->obj;
                SetDesired(missab);
            }
            
          // Play block sound do sparks
            if (targ && targ->doing->action == ACTION_BLOCK)
            {
                if (attack->flags && CA_SPARKS)
                    EffectBurst("sparks");
                PlayWave(listrnd(chardata->blocksounds));
            }
        }

       // Subtract from fatigue value
//      SetFatigue(max(Fatigue() - attack->fatigue, 0));    

    }

    return 0;
}

int32_t TCharacter::ResolveImpact(TActionBlock* ab, int32_t bits)
{
#if 0
    static int32_t frame;
    if (ab->firsttime)
        frame = 0;

    if (frame < 4)
    {
        int32_t x, y;
        S3DPoint fpos = pos;
        fpos.z += 75;
        S3DPoint vect;
        ConvertToVector(facing, 14, vect);
        fpos += vect;
        WorldToScreen(fpos, x, y);
        PlayScreen.AddPostCharAnim(x, y, 0, GameData->Animation("flashred")->GetFrame(frame++), DM_ALPHA);
    }
#endif

  // Do blood for impale    
    if ((ab->Is("impale") && random(0, 5) == 1))
        EffectBurst("blood", ab->Is("impale") ? 40 : 50);
    
  // Do blood for attack
    if (ab->firsttime && 
        ab->damage > 0 && (!ab->attack || (ab->attack->flags & CA_BLOOD)))
            EffectBurst("blood", ab->Is("impale") ? 40 : 50);

  // Do combat flash for impact firsttime!
    if (ab->firsttime)
        combatflashticks = 3;

    // Return from looping or root impact states.
  // This allows some impacts to be used as death states.  For example, the impact state is
  // a looping root state of the guy on the ground, and the impact has a "impact to combat"
  // state where the guy gets up again and goes back to fight pose.  The impact state will
  // use the get up transition, where the death state will just loop forever in the 'on the ground'
  // animation.
  // It also allows some impacts to have looping "stun" states where the attack sets the
  // wait value of the action block to 'stunwait', and the impact loop plays until 'wait'
  // is exauhsted.
    if (commanddone)
    {
      // If we're done waiting, go back to combat mode
        if (ab->wait <= 0)
        {
            doing->priority = false;
            root->interrupt = true;
            SetDesired(root);
        }
      // If we're done with impact animation, see if we should do the loop animation
      // NOTE: impacts can have a "combat to impact" transition state and "impact_l" looping state without
      // specifying anything for 'loopname'.  'loopname' is provided so that different impacts can end in
      // the same looping stun, knockdown, or death state.
        else if (ab->impact && 
            ab->Is(ab->impact->impactname) &&
            ab->impact->loopname[0] != '\0' &&
            FindState(ab->impact->loopname) >= 0)
        {
            TActionBlock* newab = new TActionBlock(*ab, ab->impact->loopname, ab->action);
            newab->priority = true;
            newab->interrupt = true;
            desired->priority = false;
            doing->priority = false;
            SetDesired(newab);
        }
    }

  // Return from looping or root impact states.
  // This allows some impacts to be used as death states.  For example, the impact state is
  // a looping root state of the guy on the ground, and the impact has a "impact to combat"
  // state where the guy gets up again and goes back to fight pose.  The impact state will
  // use the get up transition, where the death state will just loop forever in the 'on the ground'
  // animation.
  // It also allows some impacts to have looping "stun" states where the attack sets the
  // wait value of the action block to 'stunwait', and the impact loop plays until 'wait'
  // is exauhsted.
    if (commanddone && ab->wait <= 0)
    {
        TActionBlock* newab = new TActionBlock(root->name, ACTION_COMBAT);
        newab->interrupt = true;
        SetDesired(newab);
    }
                        
    return 0;
}

int32_t TCharacter::ResolveBlock(TActionBlock* ab, int32_t bits)
{
    if (ab->wait <= 0 || (doing && doing->stop))
    {
        ab->wait = 0;
        SetDesired(nullptr);
        return COM_COMPLETED;
    }

    return COM_EXECUTING;
}

int32_t TCharacter::ResolveDead(TActionBlock* ab, int32_t bits)
{
  // Do blood for attack
    if (ab->firsttime && (!ab->attack || (ab->attack->flags & CA_BLOOD)))
        EffectBurst("blood", ab->Is("impale") ? 40 : 50);

  // If we're done with dying animation (transition), do the death animation.
    if (commanddone &&
        ab->impact &&
        ab->impact->loopname[0] != '\0' &&
        !ab->Is(ab->impact->loopname) &&
        FindState(ab->impact->loopname) >= 0)
    {
        TActionBlock* newab = new TActionBlock(*ab, ab->impact->loopname, ab->action);
        newab->priority = true;
        newab->interrupt = true;
        desired->priority = false;
        doing->priority = false;
        SetDesired(newab);
        SetRoot(newab);
    }

    return COM_EXECUTING;
}

int32_t TCharacter::ResolveCombat(TActionBlock* ab, int32_t bits)
{
  // Clear goto's when in combat
    ab->target.x = ab->target.y = ab->target.z = 0;

  // If target is dead, change to new target, or end combat
    TCharacter* targ = (TCharacter*)ab->obj;
    TCharacter* newtarg;
    if (!targ || targ->IsDead() || targ->IsInvisibleSpell() ||Distance(targ) > chardata->combatrangemax)
    {
        newtarg = FindClosestEnemy();
        if (newtarg && !newtarg->IsDead())
            SetFighting(newtarg);
        else if (targ && targ->IsDoing(ACTION_DEAD) && !(targ->doing->transition))
        {
            EndCombat();
            return 0;
        }
    }

  // Switch to new target?
    newtarg = FindClosestEnemy(ab->moveangle, 32);
    if (newtarg && newtarg != targ)
    {
      // Switch targets if new target is MUCH (64) closer than old target
        if (!targ || Distance(newtarg) < (Distance(targ) - 64))
        {
            targ = newtarg;
            ab->obj = newtarg;
        }
    }

  // Get angle to current target
    int32_t angle = ab->moveangle;
    int32_t faceangle = ((ab->moveangle + 15) & 0xE0);
    if (targ)
    {
        S3DPoint target;
        targ->GetPos(target);
        if (targ && Distance(targ) <= chardata->combatrangemax)
            angle = ConvertToFacing(pos, target);
        faceangle = ((angle + 15) & 0xE0);
    }

  // Do pivoting before moving (character stays in root neutral state until pivot is done, 
  // then sets the first step action block.  No block checking is done when pivoting
    if (ab->waitpivot)
    {
        Halt(); // Make sure there's no movement

        ab->angle = faceangle;              // round to 8 dirs

        bool pivotdone = false;
        if (!stricmp(ab->name, root->name)) // Is a normal root animation we're manually turning...
        {
            if (GetFace() == ab->angle)
                pivotdone = true;
            else
                AdvanceAngles(ab->angle, ab->moveangle, ab->turnrate);
        }
        else if (commanddone)               // Is a special pivot animation which turns itself!
            pivotdone = true;

        if (!pivotdone)
            return 0;
        else                // Now that pivot is done, start us moving in right direction
        {
            char animname[RESNAMELEN];
            GetAngleMoveAnim(ab->moveangle, angle, root->name, animname, RESNAMELEN);
            TActionBlock* newab = new TActionBlock(animname, doing->action);
            newab->angle = ab->angle;
            newab->moveangle = ab->moveangle;
            newab->obj = ab->obj;
            newab->waitpivot = false;
            newab->turnrate = MAXTURNRATE;
            ForceCommand(newab);            // Does this command again

            return 0;
        }
    }

  // Do movement pivoting (works different for players/monsters)
    if (ObjClass() == OBJCLASS_PLAYER)
    {
        if (IsMoving()) // If player, pivot when 8 direction facing changes
        {
            if (ab->angle != faceangle)
            {
                char animname[RESNAMELEN];
                GetAngleMoveAnim(ab->moveangle, faceangle, root->name, animname, RESNAMELEN);
                TActionBlock* newab = new TActionBlock(animname, doing->action);
                newab->angle = faceangle;
                newab->moveangle = ab->moveangle;
                int32_t anglediff = abs(AngleDiff(GetFace(), newab->angle));
                newab->turnrate = MAKETURNRATE(anglediff);
                newab->obj = ab->obj;
                SetMoveAngle(newab->moveangle);
//              FaceOnly(newab->angle);
                ForceCommand(newab);            // Does this command again

                return 0;
            }
        }
        else if (ab->angle != angle)    // Do non movement player pivoting in all directions
        {
            int32_t anglediff = abs(AngleDiff(GetFace(), angle));
            ab->turnrate = MAKETURNRATE(anglediff);
            ab->angle = ab->moveangle = angle;
        }
    }
    else
    {
        if (ab->angle != angle) // Do movement/non-movement monster pivoting in all directions
        {
            int32_t anglediff = abs(AngleDiff(GetFace(), angle));
            ab->turnrate = MAKETURNRATE(anglediff);
            ab->angle = angle;
            if (ab->action == ACTION_COMBATMOVE)
                ab->moveangle = angle;
            else
                ab->moveangle = GetMoveAngle();
        }
    }

  // Update facing and moving angles
    SetMoveAngle(ab->moveangle);
    AdvanceAngles(ab->angle, ab->moveangle, ab->turnrate);

    return 0;
}

int32_t TCharacter::ResolveCombatMove(TActionBlock* ab, int32_t bits)
{
    if (bits & MOVE_BLOCKED || ab->stop)
    {
        ForceCommand(root);
        return COM_COMPLETED;
    }

    if (!IsDesired(GetMoveAction(root->action)) && !ab->stop) // Set next step if not stopped
        SetDesired(doing);  

    return ResolveCombat(ab, bits);
}

int32_t TCharacter::ResolveBowAim(TActionBlock* ab, int32_t bits)
{
    Halt(); // Make sure there's no movement

    if (ab->stop && GetFace() == ab->angle)
        return COM_COMPLETED;

  // Do turning
    AdvanceAngles(ab->angle, ab->angle, ab->turnrate);

    return COM_EXECUTING; // Allow to loop indefinitely until cancelled
}

int32_t TCharacter::ResolveBowShoot(TActionBlock* ab, int32_t bits)
{
    // generate a new arrow and fire it
    if (ab->firsttime)
    {
        Face(ab->angle);

        SObjectDef def;
        memset(&def, 0, sizeof(SObjectDef));
        def.objclass = OBJCLASS_AMMO;
        def.objtype = AmmoClass.FindObjType("Arrow3D");
        def.flags = OF_WEIGHTLESS;
        GetPos(def.pos);

        S3DPoint shootpos;
        ConvertToVector(GetFace(), ::Distance(chardata->arrowpos), shootpos);
        shootpos.z = chardata->arrowpos.z;
        def.pos += shootpos;

        def.level = MapPane.GetMapLevel();
        ConvertToVector(GetFace(), chardata->arrowspeed * ROLLOVER, def.vel);
        def.facing = GetFace();

        MapPane.NewObject(&def);

        TObjectInstance* arrow = FindObjInventory(OBJCLASS_AMMO, AT_ARROW);
        if (arrow)
            DeleteFromInventory(arrow->GetName(), 1);

        PlayWave("arrow");
    }

    return 0;
}

int32_t TCharacter::ResolveLeap(TActionBlock* ab, int32_t bits)
{
    return ResolveCombat(ab, bits);
}

int32_t TCharacter::ResolvePull(TActionBlock* ab, int32_t bits)
{
    int32_t start;

    if (!ab->data)
        return 0;

    start = ((int32_t *)ab->data)[0];

    if ((start == LEVER_PULLSOUTH) || (start == LEVER_PULLNORTH))
    {
        if (commanddone && ab->obj)
        {
            if (start == LEVER_PULLNORTH)
            {
                if (ab->obj->GetState() == EXIT_OPEN)
                    ab->obj->SetState( EXIT_CLOSING);
            }
            else
            {
                if (ab->obj->GetState() == EXIT_CLOSED)
                    ab->obj->SetState( EXIT_OPENING);
            }
        }
    }

    if ((start == LEVER_PUSHSOUTH) || (start == LEVER_PUSHNORTH))
    {
        if (ab->firsttime && ab->obj)
        {
            if (start == LEVER_PUSHNORTH)
            {
                if (ab->obj->GetState() == EXIT_OPEN)
                    ab->obj->SetState( EXIT_CLOSING);
            }
            else
            {
                if (ab->obj->GetState() == EXIT_CLOSED)
                    ab->obj->SetState( EXIT_OPENING);
            }
        }
    }

    return 0;
}

int32_t TCharacter::ResolveSay(TActionBlock* ab, int32_t bits)
{
    if (ab->wait <= 0 || (doing && doing->stop))
    {
        ab->wait = 0;
        ForceCommand(root);
        return COM_COMPLETED;
    }

    return COM_EXECUTING;
}

int32_t TCharacter::ResolvePivot(TActionBlock* ab, int32_t bits)
{
    Halt(); // Make sure there's no movement

    if (!stricmp(ab->name, root->name)) // Is a normal root animation we're manually turning...
    {
        if (GetFace() == ab->angle)
        {
            SetDesired(nullptr);
            return COM_COMPLETED;
        }
        else
            AdvanceAngles(ab->angle, ab->angle, ab->turnrate);
    }
    else if (commanddone)               // Is a special pivot animation which turns itself!
    {
        SetDesired(nullptr);
        return COM_COMPLETED;
    }

    return COM_EXECUTING;
}

void TCharacter::EffectBurst(char *name, int32_t height)
{
//  return;
    // hack? maybe not... just no blood if i'm burning
    if (burning && !stricmp(name, "blood"))
        return;

    extern TObjectClass EffectClass;

    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));

    def.objclass = OBJCLASS_EFFECT;
    def.objtype = EffectClass.FindObjType(name);

    def.level = MapPane.GetMapLevel();

    if (!stricmp(name, "blood"))
    {
        def.pos = pos;
        def.pos.z += height;

        def.facing = 0;

        int32_t index = MapPane.NewObject(&def);
        TObjectInstance* inst = MapPane.GetInstance(index);
        if (!inst)
            return;

      // TODO retail: the imagery factory currently returns a generic
      // TEffect for "blood" instead of a TBloodEffect (typeinfo for
      // TBloodEffect isn't even emitted — no key function). Calling
      // the virtual SetParams via the wrong vtable crashes. Skip the
      // splatter-params call until the effect-class registration is
      // ported. Combat still kills the target; blood is cosmetic.
        // ((TBloodEffect*)inst)->SetParams(height, (GetFace() + 128) & 255, 0, 80, 20, random(1, 5));
    }
    else
    {
        def.pos = pos;

        int32_t index = MapPane.NewObject(&def);
        TObjectInstance* inst = MapPane.GetInstance(index);
        if (!inst)
            return;

        inst->CreateAnimator();
        TParticle3DAnimator* anim = (TParticle3DAnimator*)inst->GetAnimator();

        int32_t ang = (GetFace() + random(-80, 80)) & 0xff;

        S3DPoint vect;
        ConvertToVector(ang, 100, vect);

        if (anim)
        {
            SParticleParams pr;

            S3DPoint vect0, tpos;
            doing->obj->GetPos(tpos);
            ang = ConvertToFacing(pos, tpos);
            int32_t dist = TObjectInstance::Distance(doing->obj);
            ConvertToVector(ang, dist / 2, vect0);
            vect0.z += 45;

            pr.particles = random(15, 25);
            pr.pos.X = (float)vect0.x;
            pr.pos.Y = (float)vect0.y;
            pr.pos.Z = (float)vect0.z;
            pr.pspread.X = (float)3.0;
            pr.pspread.Y = (float)3.0;
            pr.pspread.Z = (float)3.0;
            pr.dir.X = (float)((float)vect.x / (float)100.0);
            pr.dir.Y = (float)((float)vect.y / (float)100.0);
            pr.dir.Z = (float)((float)vect.z / (float)100.0);
            pr.spread.X = (float)0.5;
            pr.spread.Y = (float)0.5;
            pr.spread.Z = (float)0.5;
            pr.gravity = (float)0.2;
            pr.trails = 1;
            pr.minstart = 0;
            pr.maxstart = 8;
            pr.minlife = 20;
            pr.maxlife = 40;
            pr.bounce = false;
            pr.killobj = true; 
            pr.objflags = 1 << (ObjId() & 0x3);
            pr.seektargets = false;
            pr.numtargets = 0;

            anim->InitParticles(&pr);
        }
    }
}

int32_t TalismanStat(int32_t tal, char *statname)
{
    extern TObjectClass TalismanClass;

    return TalismanClass.GetStat(tal, statname);
}

bool TCharacter::IsFinalState()
{
    if (doing && (doing->Is("collapse") || doing->Is("dead") || doing->Is("impale") || doing->Is("fall")))
        return true;

    return false;
}

bool TCharacter::IsEnemy(TCharacter* chr)
{
  // Is this character attacking me
    if (chr->IsFighting() && chr->Fighting() == this)
        return true;    // That makes me hostile no matter what

  // Is this character a PARTICULAR enemy of mine (i.e. enemy by name)
    if (listin(chardata->enemies, chr->GetName()) ||
        listin(chardata->enemies, chr->GetTypeName()) )
            return chr->Aggressive() || chr->ObjClass() == OBJCLASS_PLAYER; // If I'm aggressive

  // Is this character in a GROUP I don't like
    int32_t numgroups = listnum(chr->chardata->groups);
    for (int32_t c = 0; c < numgroups; c++)
    {
        if (listin(chardata->enemies, listget(chr->chardata->groups, c)))
            return chr->Aggressive() || chr->ObjClass() == OBJCLASS_PLAYER; // If I'm aggressive
    }

    return false;
}

// ***********************
// * General AI Routines *
// ***********************

// REVSYNC: retail TCharacter::AI @ 0x4c8b60
//   recon/discovered/cls_0x5a7b98_TCharacter_AI_4c8b60.cpp (size 2291 / 326 lines).
//   Per-tick monster brain. Retail flow:
//     1. Health<1 -> return (dead).
//     2. Player class with charflags-mask 0x100000 unset -> return.
//     3. OF_DISABLED, NoAI globals, or AI-paused -> Stop(), return.
//     4. AI_PerMonster() per-creature overlay. We currently only handle
//        Araknid (the Demo 1 creature), which has no overlay (returns 0).
//     5. Validate desired->obj as a current target via CanSeeCharacter
//        check; if missing/invalid, every 32 frames sweep FindCharacters
//        to acquire a fresh enemy.
//     6. If we have a target and we're in combat, run the attack tree:
//        out-of-range / blocked -> sidestep or Go(angle); in-range ->
//        RandomAttack pick guarded by a CharBlocking line check; tick
//        nextattack/waitticks counters and reset from chardata when they
//        underflow.
//     7. If no target (or non-combat root) and not moving, wander to the
//        nearest "waypoint" object. The committed waypoint is held in
//        wander_target with a wander_commit watchdog so we don't
//        ping-pong on arrival.
//     8. Tail: target_out_of_sight is the *current* "lost target" flag,
//        target_out_of_sight_prev mirrors it for transition detection,
//        sight_lost_ticks is a frame countdown.
//
// Field-offset cross-references (recon/discovered/field_map.md):
//   doing       (mbr_0xd8  / param_1[0x36])
//   desired     (mbr_0xe0  / param_1[0x38])
//   chardata    (mbr_0xfc  / param_1[0x3f])
//   charflags   (mbr_0x110 / param_1[0x44])
//   nextattack  (mbr_0x120 / param_1[0x48])
//   waitticks   (mbr_0x124 / param_1[0x49])
//   chainhits   (mbr_0x12c / param_1[0x4b])
//   oldab       (mbr_0x160 / param_1[0x58])  — retail uses for "in-progress action"
//   wander_target            (mbr_0x238 / param_1[0x8d])
//   target_last_position     (mbr_0x23c / param_1[0x8f..0x91])
//   wander_commit            (mbr_0x248 / param_1[0x92])
//   target_out_of_sight      (mbr_0x254 / param_1[0x95])
//   target_out_of_sight_prev (mbr_0x258 / param_1[0x96])
//   sight_lost_ticks         (mbr_0x25c / param_1[0x97])
void TCharacter::AI()
{
    ai_ai_count++;

  // (1) Dead -> skip entirely.
    if (Health() < 1)
        return;

  // (2) Player object that's been "AI-disabled" via the player-state bit.
  //     Retail: `if (objclass==0xb && (charflags & 0x100000) == 0) return;`
  //     We don't have the 0x100000 flag yet, so just bail for the player
  //     object — TPlayer overrides AI() anyway.
    if (ObjClass() == OBJCLASS_PLAYER)
        return;

  // (3) Global / object-flag gates. Retail also tests DAT_00668110 (a
  //     "AI globally disabled" flag e.g. cinematic mode); we don't have
  //     that global yet. NoAI is our stand-in.
    if ((flags & OF_DISABLED) || NoAI || Editor)
    {
        if (doing && (doing->action == ACTION_MOVE ||
                      doing->action == ACTION_COMBATMOVE ||
                      doing->action == ACTION_BOWMOVE))
            Stop();
        return;
    }

  // (4) Per-monster behavioural overlay. Retail dispatches on a one-time
  //     name match against {"Baez","Solifuge","Jhaga","Yhagoro"}; every
  //     other creature (including Araknid) takes the default branch and
  //     returns 0, falling through to the generic AI body below.
  //     Demo 1 only ships Araknid, so we leave the overlay as a no-op
  //     stub and TODO the four boss cases. See
  //     recon/discovered/araknid_ai_notes.md for confirmation that
  //     Araknid has no special-case branch.
  //     TODO retail: port AI_PerMonster cases 1..4 (Baez, Solifuge,
  //     Jhaga, Yhagoro) when we actually have those creatures in a demo.

  // (5) Validate / acquire combat target.
  //
  //     Retail reads desired->obj as the current target (the action
  //     block's `obj` field), validates it via IsValidTarget (0x4cd990),
  //     and if missing acquires a new one every 32 frames via
  //     FindCharacters with a 32-unit angle range.
    TCharacter* target = nullptr;
    if (desired && desired->obj &&
        (desired->action == ACTION_COMBAT || desired->action == ACTION_BOW))
    {
        // desired->obj is the combat target.
        target = (TCharacter*)desired->obj;
        if (!target || target->IsDead() ||
            (target->ObjClass() != OBJCLASS_CHARACTER &&
             target->ObjClass() != OBJCLASS_PLAYER))
        {
            target = nullptr;
        }
    }

    if (!target)
    {
      // Periodic enemy sweep — retail only fires once every 32 frames
      // (frame_tick xor charflags & 0x1f != 0) which keeps the cost
      // amortised across the monster population.
        if (Aggressive() && ((CurrentScreen->FrameCount() ^ (int32_t)charflags) & 0x1f) == 0)
        {
            TCharacter* found = nullptr;
            int32_t n = FindCharacters(&found, 1, /*range=*/-1, /*angle=*/-1, /*anglerange=*/32,
                                       FINDCHAR_ENEMY | FINDCHAR_SEE | FINDCHAR_HEAR);
            if (n > 0 && found && !target_out_of_sight)
                BeginFighting(found, ACTION_COMBAT);
            target = Fighting();
        }
    }

  // (6+7) Main per-tick decision tree.
    if (!doing)
        goto sight_tail;

  // ===== Pre-step: cartwheel-pair on blocker =====
  // Retail FUN_004c8b60:96-115. When we're NOT in a move root and
  // another character is blocking our position (FindClearPath returns
  // a char), throw a SideStep one direction; if the blocker isn't our
  // combat target, throw the OPPOSITE side too — that's the "cartwheel
  // both ways" liveliness. The retail action gate is "doing == null OR
  // action ∉ {MOVE, COMBATMOVE, BOWMOVE}", i.e. don't double-step while
  // already moving.
  //
  // We use CharBlocking() in place of retail's FindClearPath(pos, pos)
  // — same semantics for this call (blocker char near current pos).
  // The accum.x/y == 0 gate is retail FUN_004c8b60:98 ("character has
  // no horizontal momentum"): only cartwheel when truly standing still
  // in combat, never mid-step.
    {
        ACTION da = doing->action;
        if (da != ACTION_MOVE && da != ACTION_COMBATMOVE && da != ACTION_BOWMOVE
            && accum.x == 0 && accum.y == 0)
        {
            TCharacter* blocker = CharBlocking(this, pos, Radius());
            if (blocker)
            {
              // Retail line 101-102: cVar2 = diff>=0 ? 'r' : 'l'.
              // (Sidestep moves 90° off facing; retail issues the pair
              // and lets the engine decide which one actually plays.)
                int32_t diff = AngleDiff(GetFace(), AngleTo(blocker));
                char first = (diff >= 0) ? 'r' : 'l';
                SideStep(first);

              // Retail lines 104-114: if blocker is NOT our combat
              // target, fire the opposite-side sidestep too.
                TObjectInstance* combat_target = nullptr;
                if (desired && (desired->action == ACTION_COMBAT ||
                                desired->action == ACTION_BOW))
                    combat_target = desired->obj;
                if (combat_target != (TObjectInstance*)blocker)
                {
                    char second = (first == 'l') ? 'r' : 'l';
                    SideStep(second);
                }
              // Retail then does `goto LAB_004c93ce` — straight to the
              // sight-tail, skipping the attack tree this frame. We
              // mirror that with `goto sight_tail` so the in-range
              // tree below doesn't immediately try to attack.
                goto sight_tail;
            }
        }
    }

  // Special branch: we have an in-flight action that's interactive (e.g.
  // a cast / use). Retail tests `TActionBlock_Is("combat") && !target_out_of_sight && (charflags & 0x4000)`
  // — we approximate by skipping the attack tree when nextattack hasn't
  // armed.
    {
        ACTION da = doing->action;

        if (da == ACTION_COMBAT && target && !target_out_of_sight)
        {
          // ===== In combat, target visible: tick attack/wait, fire =====
            if (nextattack > 0) nextattack--;
            if (waitticks  > 0) waitticks--;

            if (nextattack == 0 || waitticks == 0)
            {
              // Reset interrupt flag (retail clears charflags & 1).

              // Retail gating: skip attack if oldab is set with the
              // "interactive" bit. Approximation: just always allow.
                int32_t tdist = Distance(target);
                if (tdist > chardata->maxattackrange)
                {
                  // Out of range: walk toward target. Retail Walk(angle)
                  // (FUN_004ce350) is essentially Go(angle). The
                  // pre-snapshot hardcoded "> 80" was wrong for
                  // short-reach creatures (e.g. Araknid attkrng=32);
                  // chardata-driven is right.
                    target_out_of_sight = false;
                    Go(AngleTo(target));
                    oldab = nullptr;
                }
                else
                {
                  // In range: try a percentage-driven attack. Retail
                  // first does a CharBlocking line check from us through
                  // 2*radius — if some other character is between us
                  // and the target, sidestep instead of attacking
                  // through them.
                    int32_t pcnt    = random(1, 100);
                    bool    attacked = RandomAttack(pcnt);

                    int32_t reach = Radius() * 2;
                    TCharacter* lineblocker = CharBlocking(this, pos, reach);
                    if (lineblocker && lineblocker != target)
                    {
                        if (!attacked)
                        {
                          // Sidestep around the blocker, retail
                          // FUN_004c8b60:282-291. AngleDiff sign picks
                          // the side: blocker on our right (diff in
                          // +33..+95) → step left; blocker on our left
                          // (-95..-33) → step right; else skip.
                            int32_t angtoblock = AngleTo(lineblocker);
                            int32_t diff       = AngleDiff(GetFace(), angtoblock);
                            if (diff >= 33 && diff <= 95)
                                SideStep('l');
                            else if (diff <= -33 && diff >= -95)
                                SideStep('r');
                          // else: blocker is in front-cone or behind —
                          // sidestep wouldn't help; just skip the dodge.
                        }
                    }

                    if (attacked)
                    {
                      // Attack landed: bump chainhits (retail decrements
                      // mbr_0x4b which is our chainhits — but retail's
                      // semantics are "chain-attack budget remaining",
                      // counting down from MAXCHAINHITS).
                        if (chainhits > 0) chainhits--;
                    }
                }

                // TODO retail: gate `if oldab && (oldab->mbr_0x24 & 0x01000000)`
                // — the retail decompile masks bit 24 of TActionBlock
                // offset 0x24, but in our 92-byte source TActionBlock that
                // offset is `frame` (an int frame number). Either the
                // retail layout differs (TActionBlock is 100 bytes there
                // with 8 bytes of extra fields per field_map.md) or the
                // mask targets a flag that lives somewhere else in our
                // layout. Until we resolve, just always refresh.
                if (nextattack <= 0)
                    nextattack--;
            }

            if (nextattack < 0)
                nextattack = random(chardata->minattackfreq * FRAMERATE / 100,
                                    chardata->maxattackfreq * FRAMERATE / 100);
            // TODO retail: waitticks reset uses chardata + 0x1d8 / 0x1dc
            // (the field directly after attackfreq); field_map.md tags
            // these as mana/fatigue but that doesn't match SCharData
            // semantics. For now just reuse the attackfreq bounds — this
            // gives correct behaviour for "tick down both counters in
            // lockstep, refire when either runs out".
            if (waitticks < 0)
                waitticks = random(chardata->minattackfreq * FRAMERATE / 100,
                                   chardata->maxattackfreq * FRAMERATE / 100);
        }
        else if ((da == ACTION_MOVE || da == ACTION_COMBATMOVE || da == ACTION_BOWMOVE)
                 && target)
        {
          // ===== We're in a move root with a known target: drive the =====
          // ===== move-angle and decide whether to switch to attack    =====
            int32_t tdist = Distance(target);
            if (tdist < chardata->maxattackrange && !target_out_of_sight)
            {
                Stop();
            }
            // TODO retail: same `oldab->mbr_0x24 & 0x01000000` gate as
            // above — see the matching comment block in the COMBAT branch.
            else if (CanSeeCharacter(target))
            {
              // (target_out_of_sight goes false next frame via tail)
                target_last_position = target->Pos();
                doing->moveangle      = AngleTo(target);
                doing->angle          = AngleTo(target);
            }
            else
            {
              // ===== Lost sight: hop waypoints toward target =====
              // Retail FUN_004c8b60 lines 128-244: when we have a target
              // we can't see, we walk between "waypoint" objects to
              // approach the last-known position. Initial search center
              // is target's last known pos; on arrival at a waypoint
              // (within ~5 units) we re-search with center at the
              // target's CURRENT pos to find the next hop.
                last_position_distance = ::Distance(pos, target_last_position);
                last_position_start_point = pos;
                target_out_of_sight = true;
                target_last_angle = ConvertToFacing(pos, target_last_position);

              // If we have a committed waypoint and we've arrived at it,
              // clear the commit so the search below re-picks. Use the
              // target's current pos as the new search center (retail
              // line 209-213: iStack_68 = piVar10[4..6] where piVar10 is
              // the target character).
                constexpr int32_t ARRIVAL_DIST = 5;
                S3DPoint search_center = target_last_position;
                if (TObjectInstance* cmt = wander_target.Get())
                {
                    if (::Distance(pos, cmt->Pos()) < ARRIVAL_DIST)
                    {
                        wander_target = TSafeRef<TObjectInstance>{};
                        wander_commit = 0;
                        search_center = target->Pos();
                    }
                }

                TObjectInstance* wp = WanderToWaypoint(search_center);

              // Walk toward the committed waypoint if we have one,
              // otherwise straight toward the last-known target pos.
                if (wp)
                {
                    doing->moveangle = AngleTo(wp);
                  // Retail also caches the walk-to position in
                  // mbr_0x8f/0x90/0x91; we mirror that into
                  // target_last_position so subsequent frames have a
                  // sensible fallback if the waypoint disappears.
                }
                else
                {
                    doing->moveangle = target_last_angle;
                }
            }
        }
      // No idle-wander branch: retail leaves untargeted monsters alone
      // and lets per-character ALWAYS scripts (System 11) do whatever
      // patrolling the level designer wants. Until System 11 is ported,
      // monsters with no target just stand still.
    }

sight_tail:
  // (8) Tail: tick the sight-lost watchdog. Retail:
  //     out_of_sight = (frame > sight_max && !out_of_sight_prev) || sight_lost_ticks==0 ? 0 : 1;
  //     out_of_sight_prev = out_of_sight;
  //     if (sight_lost_ticks > 0) sight_lost_ticks--;
  //
  // chardata + 0x440 in retail is some "max sight-loss ticks" tunable
  // we don't have; gate on FRAMERATE * a couple seconds for now.
    {
        const int32_t sight_max = FRAMERATE * 4;  // TODO retail: chardata field at +0x440 is unknown
        const int32_t fc = CurrentScreen->FrameCount();
        bool new_oos;
        if ((fc > sight_max && !target_out_of_sight_prev) || sight_lost_ticks == 0)
            new_oos = false;
        else
            new_oos = true;
        target_out_of_sight_prev = new_oos;
        target_out_of_sight      = new_oos;
        if (sight_lost_ticks > 0)
            sight_lost_ticks--;
    }
}

bool TCharacter::CanHearCharacter(TCharacter* chr)
{
    bool hear = true;
    int32_t dist = Distance(chr);
    int32_t noise = chr->LastNoise();
    S3DPoint from, to;
    GetPos(from);
    from.z += LIGHTINGCHARHEIGHT; // Nominal character height
    chr->GetPos(to);
    to.z += LIGHTINGCHARHEIGHT; // Nominal character height
    if (dist > chardata->hearingrange ||    // Within hearing range
         noise < (100 - Hearing(dist)) ||   // Last noise made was too quiet
        !MapPane.LineOfSight(from, to))     // Has line of sight
        hear = false;

    return hear;
}

bool TCharacter::CanSeeCharacter(TCharacter* chr, int32_t angle)
{
    bool see = true;

    if (angle < 1)
        angle = GetFace();
    int32_t dist = Distance(chr);
    int32_t angleto = AngleTo(chr);
    int32_t anglediff = abs(AngleDiff(angle, angleto));

    int32_t glimpse = chr->LastGlimpse();
    if (chardata->flags & CF_INFRAVISION)
        glimpse = 10000; // Always sees
    else if (chardata->flags & CF_LIGHTBLIND)
        glimpse = 100 - glimpse; // Reverse glimpse value so more light is less visible!

    S3DPoint from, to;
    GetPos(from);
    from.z += LIGHTINGCHARHEIGHT; // Nominal character height
    chr->GetPos(to);
    to.z += LIGHTINGCHARHEIGHT; // Nominal character height

    if (dist > chardata->sightrange ||
        anglediff > chardata->sightangle ||
        !MapPane.LineOfSight(from, to) ||
        glimpse < (100 - Sight(dist)))
        see = false;

    return see;
}

// Finds characters in range, with closest guy at head of list
int32_t TCharacter::FindCharacters(TCharacter* chars[], int32_t maxchars, 
    int32_t range, int32_t angle, int32_t anglerange, int32_t flags)
{
    if (maxchars < 1)
        return 0;

    chars[0] = nullptr;
    int32_t bestdist = 10000;
    
    if (range < 0 || (flags & FINDCHAR_HEAR))
        range = max(range, chardata->hearingrange);
    if (range < 0 || (flags & FINDCHAR_SEE))
        range = max(range, chardata->sightrange);

    int32_t numchars = 0;

    for (TMapIterator i(Pos(), range, CHECK_NOINVENT | CHECK_MAPRECT, OBJSET_CHARACTER); i; i++)
    {
        TCharacter* chr = (TCharacter*)i.Item();

        if (chr == this)
            continue;

    // if the invisible spell is cast
        if (chr->IsInvisibleSpell())
            continue;

        if ((flags & FINDCHAR_ENEMY) && (!IsEnemy(chr) || chr->IsDead()))
        {
            SetHasSeen(chr);
            continue;
        }

        int32_t dist = Distance(chr);
        if (dist > range)
            continue;

        int32_t angleto = AngleTo(chr);

     // Do we hear this guy?
        bool hear = true;
        if (flags & FINDCHAR_HEAR)
            hear = CanHearCharacter(chr);

      // Do we see this guy
        bool see = true;
        if (flags & FINDCHAR_SEE)
            see = CanSeeCharacter(chr, angle); // Uses 'angle' if >= 0, otherwise uses facing

      // Set has seen if we see or hear char
        if (flags & (FINDCHAR_HEAR | FINDCHAR_SEE))
        {
            if (hear || see)
                SetHasSeen(chr);
            else
                if (!HasSeenMe(chr))  // Didn't see me, and hasn't seen me in a while...
                    continue;
        }

      // Is this guy in the direction we're checking?   
        if (angle >= 0)
        {
            int32_t diff = abs(AngleDiff(angle, angleto));
            if (diff > anglerange)
                continue;
            dist = dist * ((anglerange + 1) - diff); // Dist gets bigger when diff between angles small
        }
    
      // Put closest guy at head of list
        if (chars[0] != nullptr && dist <= bestdist)
        {
            TCharacter* temp = chars[0];
            chars[0] = chr;
            chr = temp;
            bestdist = dist;
        }

      // Add to end of list
        if (numchars < maxchars)
        {
            chars[numchars] = chr;
            numchars++;
        }
    }

    return numchars;
}

TCharacter* TCharacter::FindCharacter(int32_t range, int32_t angle, int32_t anglerange, int32_t flags)
{
    TCharacter* chr;
    int32_t numchars = FindCharacters(&chr, 1, range, angle, anglerange, flags);
    if (numchars > 0)
        return chr;
    else
        return nullptr;
}

TCharacter* TCharacter::FindCharacterAhead(int32_t angle, int32_t anglerange)
{
    return FindCharacter(512, angle, anglerange, 0);
}

TCharacter* TCharacter::FindClosestEnemy(int32_t angle, int32_t anglerange)
{
    return FindCharacter(-1, angle, anglerange, FINDCHAR_ENEMY | FINDCHAR_SEE | FINDCHAR_HEAR);
}

// REVSYNC: retail TCharacter::AI waypoint-search branch @ 0x4c8b60 lines 149-244
//
// Decompile: recon/discovered/cls_0x5a7b98_TCharacter_AI_4c8b60.cpp
//
// Algorithm (retail):
//   * If we DON'T have a committed waypoint:
//       - FindObjectsInRange around `search_center` (retail mbr_0x8f/0x90/0x91,
//         the cached "walk-to" point). Filter to type-name "waypoint" + reachable.
//         Pick the closest by 2D distance.
//       - If the new closest equals the last committed (rare here since we just
//         nulled it), clear and bail. Else commit + reset timer to 6 frames.
//   * If we DO have a committed waypoint:
//       - Decrement the commit timer. When it hits 0, clear committed (will
//         re-search next call).
//   * Caller is responsible for calling this with the right search_center:
//       - Initial search: target_last_position
//       - On arrival (within ~5 units of committed): target's *current* pos
//   * Walking is the caller's job — this function only manages the committed
//     waypoint state.
TObjectInstance* TCharacter::WanderToWaypoint(const S3DPoint& search_center, int32_t range)
{
    constexpr int32_t COMMIT_FRAMES = 6;  // retail mbr_0x92 reset value (param_1[0x92] = 6)

    TObjectInstance* committed = wander_target.Get();

  // If we have a committed waypoint, just tick the timer. Caller decides
  // whether to call us again with a target-centered search after arrival.
    if (committed)
    {
        if (wander_commit == 0)
        {
            wander_target = TSafeRef<TObjectInstance>{};
            committed = nullptr;
        }
        else
        {
            --wander_commit;
        }
    }

    if (committed)
        return committed;

  // No commit: search for the closest "waypoint"-typed object near
  // search_center. Retail uses TMapPane::FindObjectsInRange (0xfa range,
  // max 10 candidates) and applies a WaypointReachable() filter; our
  // TMapIterator is cheap so we don't cap, but we do match the 250-unit
  // range. WaypointReachable is a TODO — without it, monsters may pick
  // waypoints across walls.
    TObjectInstance* closest = nullptr;
    int32_t          best    = 1000;  // retail's initial "best" sentinel
    for (TMapIterator i(search_center, range, CHECK_NOINVENT | CHECK_MAPRECT, OBJSET_ALL); i; i++)
    {
        TObjectInstance* oi = i.Item();
        if (!oi || oi == (TObjectInstance*)this) continue;
        const char *tn = oi->GetTypeName();
        if (!tn || stricmp(tn, "waypoint") != 0) continue;
        int32_t d = ::Distance(search_center, oi->Pos());
        if (d < best) { best = d; closest = oi; }
    }

    if (!closest)
    {
        wander_target = TSafeRef<TObjectInstance>{};
        wander_commit = 0;
        return nullptr;
    }

  // Fresh pick — commit.
    wander_target = TSafeRef<TObjectInstance>(closest);
    wander_commit = COMMIT_FRAMES;
    return closest;
}

// Returns a 1-100 hearing value which indicates how the average noise will be heard
// by a monster.  If the monster is sleeping, the listening value is 20% of normal. 
// The hearing value is based on the minhearing/maxhearing values in the chardata structure,
// where minhearing is the hearing value at the characters maximum hearing range, and 
// maxhearing is the hearing value right in front of the character.
int32_t TCharacter::Hearing(int32_t dist)
{
    dist = max(0, dist - (Radius() + 32));

    if (dist > chardata->hearingrange)
        return 0;

    int32_t hearing = chardata->hearingmin +
        (chardata->hearingrange - dist) *
        (chardata->hearingmax - chardata->hearingmin) / 
        chardata->hearingrange;

    if (Sleeping())
        hearing = hearing * 20 / 100;

    return hearing;
}

// Returns a 1-100 sight value which indicates how the average char will be seen
// by a monster in the darkness.  The sight value is based on the minsight/maxsight
// values in the chardata structure, where minsight is the sight value at the characters
// maximum sight range, and maxsight is the sight value right in front of the character.
// If the monster is sleeping, the sight value is always 0.
int32_t TCharacter::Sight(int32_t dist)
{
    dist = max(0, dist - (Radius() + 32));

    if (Sleeping() || dist > chardata->sightrange)
        return 0;
    
  // Basically return min + (max - min) * dist/range
    return chardata->sightmin +
        (chardata->sightrange - dist) *
        (chardata->sightmax - chardata->sightmin) / 
        chardata->sightrange;
}

// Resets the noise and glimpse values to control whether monsters see you or not
void TCharacter::ResetStealthValues()
{
  // Figure out stealth!
    int32_t stealthmod;
    if (IsSneakMode())
        stealthmod = Rules.sneakstealth;
    else
        stealthmod = Rules.maxstealth;
    stealthmod = stealthmod * (100 - StealthMod()) / 100;
    if (stealthmod < Rules.minstealth)
        stealthmod = Rules.minstealth;  // Always aleast ten percent

    int32_t r = random(1, 100);

  // Get noise character made!
    noise = r * stealthmod / 100;

  // Get glimpse character made! 
    int32_t visibility = Visibility();
    glimpse = (visibility + (r * visibility / 100)) * stealthmod / 100;
}

void TCharacter::SignalMovement(TObjectInstance* actor)
{
}

void TCharacter::SignalHostility(TObjectInstance* actor, TObjectInstance* target)
{
    if (target != this)
        return;
}

void TCharacter::SignalAttack(TObjectInstance* actor, TObjectInstance* target)
{
    if (target != this)
        return;

    if (actor == doing->obj &&
        random(1, 100) <= BlockPcnt())
          Block();

    if (ObjClass() != OBJCLASS_PLAYER)
    {
        // Monsters auto-target whoever swung at them.
        SetFighting((TCharacter*)actor);
    }
    else
    {
        // Players auto-flip into combat mode when attacked. This is
        // the retail behavior -- the moment a hostile lands a swing
        // (or even attempts one) the player drops out of walk root
        // into combat root, ready to swing back. BeginCombat is
        // idempotent: if we're already in combat targeted at the
        // actor it's a no-op, otherwise it sets fighting + flips
        // root to "combat".
        if (actor && actor != (TObjectInstance*)this)
            BeginCombat((TCharacter*)actor);
    }
}

void TCharacter::SetOnExit()
{
    SetFlag(OF_ONEXIT, true);
    exittimestamp = CurrentScreen->FrameCount();
}

// ***********************************************************************************
// * Access functions - called by script or player to make the character do whatever *
// ***********************************************************************************

bool TCharacter::Go(int32_t angle)
{
    TActionBlock* ab = nullptr;

  // What is our moveaction for this mode
    ACTION moveaction = GetMoveAction(root->action);

    if (!IsDoing(root->action) && !IsDoing(moveaction))
        return false; // Can't move right now

  // Do we start fighting a new character?
    TCharacter* newtarg = FindClosestEnemy(angle, 32); // Hack to get has seen setup

    if (!IsFighting() || IsRunMode())
    {
        if (doing->Is(moveaction) && doing->moveangle == angle && doing->angle == angle)
            return true;

      // Do we need to pivot
        int32_t anglediff = abs(AngleDiff(GetFace(), angle));

        if (IsMoving() && anglediff <= 64)                  // Pivot while moving!
        {
            doing->angle = doing->moveangle = doing->moveangle = angle;
            doing->turnrate = MAKETURNRATE(anglediff);
            return true;
        }

        else if (ObjClass() != OBJCLASS_CHARACTER &&
            (anglediff > 64 ||                          // 90 degrees pivot when moving
            (!IsDoing(moveaction) && anglediff > 32)))      // 45 degrees pivot when stopped
        {
            ab = new TActionBlock(root->name, moveaction);
            ab->waitpivot = true;  // Causes move to wait till pivot done
            ab->turnrate = MAKETURNRATE(anglediff);
        }

        else                                                // No pivot, just go
        {
            if (HasActionAni(StName(root->name, "f")))
                ab = new TActionBlock(StName(root->name, "f"), moveaction);
            else if (HasActionAni(StName(root->name, "l")))
                ab = new TActionBlock(StName(root->name, "l"), moveaction);
            else if (HasActionAni(StName(root->name, "r")))
                ab = new TActionBlock(StName(root->name, "r"), moveaction);
            else
                return false;
        }

        SetMoveAngle(GetFace());        // Make sure move angle is what we expect it to be

        ab->obj = doing->obj;
        ab->angle = ab->moveangle = angle;
        ab->interrupt = true;
        ab->noroot = true;
    }
    else    // Do fighting (targeted) walking
    {
        if (IsMoving() && doing->moveangle == angle)
            return true;

        TCharacter* targ = (TCharacter*)doing->obj;

      // Always check if we're pointing to a new target when we move in a new direction
        if (newtarg && 
            newtarg != (TCharacter*)doing->obj && 
            !newtarg->IsDead() && 
            !newtarg->IsInvisibleSpell() && 
            (!targ || Distance(newtarg) < 64 || (Distance(newtarg) < Distance(targ))) )
            SetFighting(newtarg);

        int32_t facedir;
        if (!doing || !doing->obj ||     // If we're not facing an enemy, set facing to move angle
          (doing->obj && Distance(doing->obj) > chardata->combatrangemax)) // or if we're out of range
            facedir = angle;
        else
        {
            facedir = AngleTo(doing->obj);
//          facedir = ((facedir + 15) & 0xE0);                  // round to 8 dirs
        }
        int32_t faceangle = ((facedir + 15) & 0xE0);

      // Must be doing root, or move, to work
        if (doing->action != root->action && doing->action != moveaction)
            return false;

      // Get the proper move animation for facing and moving direction
        char animname[RESNAMELEN];
        GetAngleMoveAnim(angle, facedir, root->name, animname, RESNAMELEN);

      // Check for pivoting
        int32_t anglediff = abs(AngleDiff(GetFace(), faceangle));
        if (anglediff > 64 ||                           // 90 degrees pivot when moving
            (!IsDoing(moveaction) && anglediff > 32))   // 45 degrees pivot when stopped
        {
            ab = new TActionBlock(root->name, moveaction);
            ab->waitpivot = true;  // Causes move to wait till pivot done
        }

        else                                                // No pivot, just go
        {
            ab = new TActionBlock(animname, moveaction);
            ab->waitpivot = false;
        }

        if (ab && doing) // Copy combat target
            ab->obj = doing->obj;

        ab->turnrate = MAKETURNRATE(anglediff);
        if (ObjClass() == OBJCLASS_CHARACTER)
            ab->angle = facedir;    // All directions
        else
            ab->angle = faceangle;  // Only eight directions
        ab->moveangle = angle;
        ab->interrupt = true;
        ab->noroot = true;
    }

    if (!HasActionAni(ab->name)) // Can't do this
    {
        if (ab != doing && ab != desired && ab != root)
            delete ab;
        return false;
    }

    SetDesired(ab);
    return true;
}

bool TCharacter::Go(S3DPoint vect)
{
    S3DPoint zero;
    memset(&zero, 0, sizeof(S3DPoint));
    int32_t angle = ConvertToFacing(zero, vect);
    return Go(angle);
}

bool TCharacter::Goto(int32_t x, int32_t y)
{
    int32_t angle = ConvertToFacing(pos, S3DPoint(x, y, pos.z));
    if (Go(angle))
    {
        doing->target.x = x;
        doing->target.y = y;
        doing->target.z = pos.z;
        return true;
    }
    else
        return false;
}

bool TCharacter::Stop(char *name)
{
    if (!IsMoving() && !IsDoing(ACTION_PIVOT) &&
        (!name || !doing->Is(name)))                                   // Is a use specified command
        return false;

//  if (doing)
//      doing->stop = true;

    root->interrupt = true;
    root->angle = doing->angle;
    root->moveangle = doing->moveangle;

    SetDesired(root);

    return true;
}

bool TCharacter::Disable()
{
    SetFlags(OF_DISABLED);
    if (doing != root)
        ForceCommand(root);
    return true;
}

// Causes character to jump (in normal mode, use Leap in Combat mode)
bool TCharacter::Jump()
{
    return false;
}

// Sets walk mode
bool TCharacter::SetWalkMode()
{
    if (IsWalkMode())
        return true;

    char aniname[RESNAMELEN];
    if (root->Is(ACTION_ANIMATE))
        strcpy(aniname, "walk");
    else if (root->Is(ACTION_COMBAT))
        strcpy(aniname, GetCombatRoot());
    else if (root->Is(ACTION_BOW))
        strcpy(aniname, GetBowRoot());

    if (!HasActionAni(aniname))
        return false;

    TActionBlock* ab = new TActionBlock(aniname, root->action);
    SetRoot(ab);

    if (IsMoving())         // If moving, change next step to new root
    {
        char *name;
        if (HasActionAni(StName(aniname, "r")))
            name = StName(aniname, "r");
        else
            name = StName(aniname, "f");
        ab = new TActionBlock(*doing, name, doing->action);
    }

    ab->interrupt = true;
    SetDesired(ab);

    return true;
}

// Sets sneak mode
bool TCharacter::SetSneakMode()
{
    if (IsSneakMode())
        return true;

    TActionBlock* ab = new TActionBlock("sneak");
    SetRoot(ab);

    if (doing && doing->action == ACTION_MOVE)      // If moving, change next step to new root
    {
        char *name;
        if (HasActionAni("sneakr"))
            name = "sneakr";
        else
            name = "sneakf";
        ab = new TActionBlock(*doing, name, ACTION_MOVE);
    }

    ab->interrupt = true;
    SetDesired(ab);

    return true;
}

// Sets run mode
bool TCharacter::SetRunMode()
{
    if (IsRunMode())
        return true;

    char aniname[RESNAMELEN];
    if (root->Is(ACTION_ANIMATE))
        strcpy(aniname, "run");
    else if (root->Is(ACTION_COMBAT))
        strcpy(aniname, StName(GetCombatRoot(), "run"));
    else if (root->Is(ACTION_BOW))
        strcpy(aniname, StName(GetBowRoot(), "run"));

    if (!HasActionAni(aniname))
        return false;

    TActionBlock* ab = new TActionBlock(aniname, root->action);
    SetRoot(ab);

    if (doing && IsMoving())        // If moving, change next step to new root
    {
        char *name;
        if (HasActionAni(StName(aniname, "r")))
            name = StName(aniname, "r");
        else
            name = StName(aniname, "l");
        ab = new TActionBlock(*doing, name, doing->action);
    }

    ab->interrupt = true;
    SetDesired(ab);

    return true;
}

// Attempt to play a pivot animation towards the given delta angle
bool TCharacter::Pivot(int32_t angle)
{
    if (IsDoing(ACTION_PIVOT) && doing->angle == angle)
        return true;

    if (doing != root && !IsMoving())
        return false;

    angle = angle & 255;

    if (angle == GetFace())
        return true;

    char buf[30];

  // Get name of pivot animation for this direction
    int32_t anglediff = AngleDiff(GetFace(), angle);
    int32_t absdiff = abs(anglediff);
    strcpy(buf, root->name);
    strcat(buf, "pivot");
    if (anglediff == 32)
        strcat(buf, "fl");
    else if (anglediff == -32)
        strcat(buf, "fr");
    else if (anglediff == 64)
        strcat(buf, "l");
    else if (anglediff == -64)
        strcat(buf, "r");
    else if (anglediff == 96)
        strcat(buf, "bl");
    else if (anglediff == -96)
        strcat(buf, "br");
    else if (anglediff == 128)
        strcat(buf, "al");
    else if (anglediff == -128)
        strcat(buf, "ar");

  // If no real pivot animation, just turn the root state
    int32_t turnrate = MAXTURNRATE;
    if (!HasActionAni(buf))
    {
        strcpy(buf, root->name);
      // Make turn faster further around he goes
        turnrate = MAKETURNRATE(absdiff);
    }

    TActionBlock* ab = new TActionBlock(buf, ACTION_PIVOT);
    ab->angle = ab->moveangle = angle;
    ab->turnrate = turnrate;
    ab->interrupt = true;
    ab->noroot = true;
    SetDesired(ab);

    return true;
}

// This obviously does nothing right now
bool TCharacter::FollowChar(TObjectInstance* inst)
{
    return false;
}

bool TCharacter::Pickup(TObjectInstance* inst)
{
    if (!inst || !inst->IsInventoryItem())
        return false;

    inst->RemoveFromMap();
    if (!Inventory.GetContainer()->AddToInventory(inst))
        TextBar.Print("Can't carry any more.");

/*  
    if (!inst || !inst->IsInventoryItem())
        return;

    desired = new TActionBlock("pickup");
    desired->obj = inst;*/

    return true;
}

bool TCharacter::Pull(TObjectInstance* inst)
{
    TActionBlock* ab = new TActionBlock("Pull Front");
    int32_t state = inst->GetState();
    int32_t direction = (((TLever*)inst)->targetpos.z / 64);

    ab->obj = inst;
    ab->priority = true;    // Won't do it otherwise

    ab->data = malloc(sizeof(int32_t));

    if ((direction == 3) && (state == EXIT_OPEN))
        state = LEVER_PUSHNORTH;
    else if ((direction == 3) && (state == EXIT_CLOSED))
        state = LEVER_PULLSOUTH;
    else if ((direction == 1) && (state == EXIT_OPEN))
        state = LEVER_PULLNORTH;
    else if ((direction == 1) && (state == EXIT_CLOSED))
        state = LEVER_PUSHSOUTH;

    ((int32_t *)ab->data)[0] = state;

    SetDesired(ab);

    return true;
}

// Attempts to use something in the direction character is facing
bool TCharacter::TryUse()
{
    S3DPoint pos, v;
    GetPos(pos);
    ConvertToVector(GetFace(), 60, v);
    pos += v;

    int32_t list[MAXFOUNDOBJS];
    int32_t n = MapPane.FindObjectsInRange(pos, list, 60);

    TObjectInstance* best = nullptr;
    int32_t bestdist;

    for (int32_t i = 0; i < n; i++)
    {
        TObjectInstance* oi = MapPane.GetInstance(list[i]);
        if (!oi)
            continue;

        if (oi->ObjClass() == OBJCLASS_CONTAINER ||
            oi->ObjClass() == OBJCLASS_EXIT)
        {
            int32_t dist = Distance(oi);

            if (best == nullptr || dist <= bestdist)
            {
                best = oi;
                bestdist = dist;
            }
        }
    }

    if (!best)
    {
        TextBar.Print("Nothing to use");
        return false;
    }
    
    TextBar.Print("Use %s", best->GetName());
    best->Use(this);

    return true;
}

// Attempts to get something in the direction character is facing
bool TCharacter::TryGet()
{
    S3DPoint pos, v;
    GetPos(pos);
    ConvertToVector(GetFace(), 60, v);
    pos += v;

    int32_t list[MAXFOUNDOBJS];
    int32_t n = MapPane.FindObjectsInRange(pos, list, 60);

    TObjectInstance* best = nullptr;
    int32_t bestdist;

    for (int32_t i = 0; i < n; i++)
    {
        TObjectInstance* oi = MapPane.GetInstance(list[i]);
        if (!oi)
            continue;

        if (oi->CursorType() != CURSOR_NONE || oi->IsInventoryItem())
        {
            int32_t dist = Distance(oi);

            if (best == nullptr || dist <= bestdist)
            {
                best = oi;
                bestdist = dist;
            }
        }
    }

    if (!best)
    {
        TextBar.Print("Nothing to get");
        return false;
    }

    TextBar.Print("Get %s", best->GetName());
    Pickup(best);

    return true;
}

bool TCharacter::Say(char *string, int32_t wait, char *anim, char *sound)
{
    if (!string)
        return false;

  // Play sound file
    bool played = false;
    if (sound && PlaySpeech)
    {
      int32_t soundid = SoundPlayer.FindSound(sound);
      if (soundid >= 0)
      {
        if (SoundPlayer.Mount(soundid))
        {
            TSound* sound = SoundPlayer.GetSound(soundid);
            if (sound)
            {
                wait = sound->GetLength() * FRAMERATE / 100;     // Get seconds to wait
            }
            S3DPoint p, mp;
            GetPos(p);
            MapPane.GetMapPos(mp);
            p -= mp;
            played = SoundPlayer.Play(soundid, 0, 0, &p);
            SoundPlayer.Unmount(soundid);
        }
      }
    } 
        
    char buf[128];
    DialogLine(string, buf, 128);  // Translate dialog line (convert [tags])

    TActionBlock* ab;
    if (anim)
        ab = new TActionBlock(anim, ACTION_SAY);
    else
        ab = new TActionBlock("say", ACTION_SAY);

    if (!played || ShowDialog)
        ab->data = (void *)strdup(buf);
    else
        ab->data = nullptr;

    if (wait < 0)
        ab->wait = 20 + max(15, strlen(buf) * 5 / 4);  // Ratio of ticks to chars
    else
        ab->wait = wait;
    ab->loop = true;
    SetDesired(ab);

    return true;
}

bool TCharacter::SayTag(int32_t tagid, int32_t wait, char *anim)
{
    char *line = DialogList.GetLine(tagid);
    char *tag = DialogList.GetTag(tagid);

    if (!line || !tag)
        return false;

    return Say(line, wait, anim, tag);
}

bool TCharacter::SayTag(char *tag, int32_t wait, char *anim)
{
    int32_t tagid = DialogList.FindLine(tag);
    if (tagid < 0)
        return false;

    return SayTag(tagid, wait, anim);
}

// Begins drawing bow or crossbow
bool TCharacter::DrawBow()
{
    if (!IsBowMode() || IsBowDrawn() || !FindObjInventory(OBJCLASS_AMMO, AT_ARROW))
        return false;

    if (IsMoving())
        Stop();

    TActionBlock* ab = new TActionBlock(StName(root->name, "aim"), ACTION_BOWAIM);
    ab->interrupt = true;
    ab->angle = ab->moveangle = GetFace();
    SetDesired(ab);

    return true;
}

// Causes character to aim at given angle before shooting bow
bool TCharacter::AimBow(int32_t angle)
{
    if (!IsBowMode() || !IsBowDrawn())
        return false;

    if (doing->stop)    // No more aiming if arrow was shot
        return true;

    doing->angle = doing->moveangle = angle;
    int32_t absdiff = abs(AngleDiff(GetFace(), angle));
    doing->turnrate = MAKETURNRATE(absdiff);

    return true;
}

bool TCharacter::AimBowLeft()
{
    return AimBow((GetFace() - chardata->bowaimspeed) & 255);
}

bool TCharacter::AimBowRight()
{
    return AimBow((GetFace() + chardata->bowaimspeed) & 255);
}

// Shoots bow or crossbow
bool TCharacter::ShootBow(int32_t angle)
{
    if (!IsBowMode() || !IsBowDrawn())
        return false;

    if (PlayScreen.GameFrame() - lastbowshot <= chardata->bowwait)
        return false;

  // Inform aim animation that it should stop (will wait for pivot though)
    doing->stop = true;
    doing->angle = doing->moveangle = angle;
    int32_t absdiff = abs(AngleDiff(GetFace(), angle));
    doing->turnrate = MAKETURNRATE(absdiff);

  // Queue the shoot action (after pivot)
    TActionBlock* ab = new TActionBlock(StName(root->name, "shoot"), ACTION_BOWSHOOT);
    ab->angle = ab->moveangle = angle;
    SetDesired(ab);

  // Save the bow shot timestamp so we don't shoot too fast
    lastbowshot = PlayScreen.GameFrame();   

    return true;
}

// Is character aiming bow
bool TCharacter::IsBowDrawn()
{
    if (!IsBowMode())
        return false;

    return !stricmp(StName(root->name, "aim"), doing->name);
}

// Per-call diagnostic state captured by IsValidAttack so FindButtonAttack
// can log *which* check rejected the first button-matching attack.
thread_local int32_t      g_dbg_attack_rej_button  = -1;
thread_local const char  *g_dbg_attack_rej_reason  = nullptr;
thread_local int32_t      g_dbg_attack_rej_tdist   = 0;
thread_local int32_t      g_dbg_attack_rej_mindist = 0;
thread_local int32_t      g_dbg_attack_rej_maxdist = 0;

// REVSYNC: retail TCharacter::IsValidAttack @ 0x4d1120
// Source-port of the retail decompile in
// recon/classes/cls_0x5a7b98.cpp lines 9532-10278. Replaces the
// pre-release Cinematix version. Retail differs from the pre-release in
// a few important ways:
//   * impact-loop has fancier fallback when target lacks the named
//     impact ani (tries a "<bodytype>_to_dead" / generic alternate)
//   * separate handling for daytime/bow attacks (a retail-only
//     CA_BOWATTACK-ish flag we don't yet have; TODO below)
//   * uses GetCharData()->attacks_default when chardata->attacks[i] is
//     null (covered by TVirtualArray semantics in our codebase)
bool TCharacter::IsValidAttack(int32_t attacknum, int32_t &impactnum, int32_t &damage,
    int32_t tdist, int32_t id, int32_t pcnt, int32_t dmgpcnt, int32_t flagmask, int32_t flags)
{
    impactnum = -1;
    damage = 0;

    auto note_reject = [&](const char *reason) {
        if (g_dbg_attack_rej_button >= 0 &&
            objclass == OBJCLASS_PLAYER &&
            id == g_dbg_attack_rej_button &&
            !g_dbg_attack_rej_reason)
        {
            g_dbg_attack_rej_reason = reason;
        }
    };

    if ((uint32_t)attacknum >= (uint32_t)chardata->attacks.NumItems())
        { note_reject("attacknum out of range"); return false; }

    SCharAttackData* ad = &(chardata->attacks[attacknum]);
    TCharacter* targ = Fighting();

    // TODO retail: at 0x4d1147 retail rejects when (this->mbr_0x128 & 2)
    // is set and the attack is CA_PLAYANIM. mbr_0x128 is an unidentified
    // status/flags slot (see recon/discovered/field_map.md). Skipping
    // until we identify it — likely an "in cinematic / non-interactive"
    // gate that doesn't apply to the source port yet.

    if (!IsFighting())
        { note_reject("!IsFighting"); return false; }

    // Daytime/bow special-case at 0x4d1190:
    //   if (!daytime || !player) {
    //       if (!(ad->flags & 0x10000)) {
    //           if (Fatigue() < ad->fatigue) return false;
    //       } else {
    //           // bow attack threshold check on a different field
    //           if (piVar18[0x3d] < Fatigue()) return false;
    //       }
    //   } else if (ad->flags & 0x10000) {
    //       return false;  // players in daytime can't use bow attacks
    //   }
    //
    // TODO retail: 0x10000 is a CA_* flag we don't yet have in our
    // CA_* defines, and piVar18[0x3d] is a field of SCharAttackData
    // beyond our struct. For now we apply only the standard fatigue
    // check; the retail-specific bow-fatigue path is left as a TODO.
    if (Fatigue() < ad->fatigue)
        { note_reject("fatigue too low"); return false; }

    // Matches flags
    if ((ad->flags & flagmask) != flags)
        { note_reject("flagmask mismatch"); return false; }

    // Button id matches attack button id (for controller/keyboard buttons)
    if (id >= 0 && ad->button != id)
        return false;  // expected for most rows; not interesting to log

    // Percentage value is less than percent parameter (for random attack finding)
    if (ad->attackpcnt < pcnt)
        { note_reject("attackpcnt < pcnt"); return false; }

    // In range
    if (ad->maxdist > 0 && targ)
    {
        if (tdist < ad->mindist || tdist > ad->maxdist)
        {
            if (g_dbg_attack_rej_button == id && objclass == OBJCLASS_PLAYER && !g_dbg_attack_rej_reason)
            {
                g_dbg_attack_rej_tdist   = tdist;
                g_dbg_attack_rej_mindist = ad->mindist;
                g_dbg_attack_rej_maxdist = ad->maxdist;
            }
            note_reject("range");
            return false;
        }
    }

    // Mode gating (retail 0x4d12bf-0x4d12ee)
    if (ad->flags & CA_SNEAKMODE)
    {
        if (!root) { note_reject("CA_SNEAKMODE no root"); return false; }
        if (!root->Is("sneak")) { note_reject("CA_SNEAKMODE !sneak"); return false; }
    }
    else if (ad->flags & CA_WALKMODE)
    {
        if (!root) { note_reject("CA_WALKMODE no root"); return false; }
        if (!root->Is("walk")) { note_reject("CA_WALKMODE !walk"); return false; }
    }
    else if (ad->flags & CA_BOWMODE)
    {
        if (!root) { note_reject("CA_BOWMODE no root"); return false; }
        if (root->action != ACTION_BOW) { note_reject("CA_BOWMODE !ACTION_BOW"); return false; }
    }
    else
    {
        // Default: retail just checks `root->action == ACTION_COMBAT`
        // and rejects on a single specific transition-state name we
        // haven't recovered (DAT_005e0318). Until that string is
        // identified, only enforce the action gate so we don't
        // accidentally reject legitimate combat states.
        if (!root) { note_reject("default-mode no root"); return false; }
        if (root->action != ACTION_COMBAT)
            { note_reject("default-mode !ACTION_COMBAT"); return false; }
        // TODO retail: DAT_005e0318 transition-state reject (was guessed
        // as "comhand" — that guess was rejecting Locke's "hand" root).
    }

    // CA_PLAYANIM gating (retail 0x4d12ef-0x4d1349)
    if (ad->flags & CA_PLAYANIM)
    {
        if (ad->attackname[0] == 'c' || ad->attackname[0] == 'C')
        {
            if (!root) { note_reject("CA_PLAYANIM 'c'-prefix no root"); return false; }
            if (!root->Is("combat")) { note_reject("CA_PLAYANIM 'c'-prefix !combat"); return false; }
        }
        if (ad->attackname[0] != 'w' && ad->attackname[0] != 'W')
            return true;
        if (!root) { note_reject("CA_PLAYANIM 'w'-prefix no root"); return false; }
        if (!root->Is("walk")) { note_reject("CA_PLAYANIM 'w'-prefix !walk"); return false; }
        return true;
    }

    // Magical attack: switch on a stat-gate field (retail 0x4d1356)
    if (ad->flags & CA_MAGICATTACK)
    {
        if (waitticks != 0)
            { note_reject("CA_MAGICATTACK waitticks!=0"); return false; }
        waitticks = waitticks - 1;
        return true;
    }

    if (waittype != 0 && objclass != OBJCLASS_PLAYER)
        return false;  // monster waittype gate, never fires for player

    // Not still doing another attack (retail 0x4d139c)
    if (doing && doing->action == ACTION_ATTACK && doing->attack &&
        frame < doing->attack->nextwait)
        { note_reject("still in prior attack"); return false; }

    // Has the named attack animation
    if (!HasActionAni(ad->attackname))
        { note_reject("no attack animation"); return false; }

    // CA_MOVING gate (retail 0x4d13c2): doing must be MOVE/COMBATMOVE/BOWMOVE
    if (ad->flags & CA_MOVING)
    {
        if (!doing) { note_reject("CA_MOVING no doing"); return false; }
        if (doing->action != ACTION_MOVE &&
            doing->action != ACTION_COMBATMOVE &&
            doing->action != ACTION_BOWMOVE)
            { note_reject("CA_MOVING !moving"); return false; }
    }

    // CA_RUNNING gate (retail 0x4d13e8)
    if (ad->flags & CA_RUNNING)
    {
        if (!root) { note_reject("CA_RUNNING no root"); return false; }
        bool isrun = false;
        if (root->action == ACTION_COMBAT &&
            (root->Is("combatrun") || root->Is("handrun")))
            isrun = true;
        else if (root->action == ACTION_BOW && root->Is("bowrun"))
            isrun = true;
        else if (IsRunMode())
            isrun = true;
        if (!isrun) { note_reject("CA_RUNNING !running"); return false; }
    }

    // Target-state gates
    if (targ)
    {
        if (ad->flags & CA_ATTACKSTUN)
        {
            if (!targ->doing || targ->doing->action != ACTION_STUN)
                { note_reject("CA_ATTACKSTUN !stunned"); return false; }
        }
        if (ad->flags & CA_ATTACKDOWN)
        {
            if (!targ->doing || targ->doing->action != ACTION_KNOCKDOWN)
                { note_reject("CA_ATTACKDOWN !down"); return false; }
        }
        if (ad->responsename[0] != '\0' &&
            !targ->doing->Is(ad->responsename))
            { note_reject("response wrong state"); return false; }
    }

    if ((ad->flags & (CA_CHAIN | CA_AUTOCOMBO)) && ad->chainname[0] != '\0')
    {
        if (!lastattack) { note_reject("chain no lastattack"); return false; }
        if (stricmp(lastattack->attackname, ad->chainname) != 0)
            { note_reject("chain mismatch"); return false; }
        if (lastattack->chainexptime < (PlayScreen.GameFrame() - lastattackticks))
            { note_reject("chain expired"); return false; }
    }

    if (ad->flags & CA_INTERACTIVE)
    {
        if (!targ) { note_reject("CA_INTERACTIVE no targ"); return false; }
        if (targ->doing && targ->doing->attack &&
            (targ->doing->attack->flags & CA_INTERACTIVE))
            { note_reject("CA_INTERACTIVE targ already interactive"); return false; }
        if (targ->doing && targ->doing->impact &&
            (targ->doing->impact->flags & 0x80))
            { note_reject("CA_INTERACTIVE targ impact 0x80"); return false; }
    }

    if (targ && targ->doing && targ->doing->impact &&
        (targ->doing->impact->flags & 0x80))
        { note_reject("targ impact 0x80"); return false; }

    if (objclass == OBJCLASS_PLAYER)
    {
        TPlayer* player = (TPlayer*)this;

        if (!(ad->weaponmask & (1 << WeaponType())))
            { note_reject("weaponmask mismatch"); return false; }
        if (player->Skill(SK_ATTACK) < ad->attackskill)
            { note_reject("attack skill too low"); return false; }
        if (player->WeaponSkill(WeaponType()) < ad->weaponskill)
            { note_reject("weapon skill too low"); return false; }

        // TODO retail: 0x4d162d adds a "sunsetflipper" attack-name
        // special-case requiring the target's bodytype to be a
        // specific value (DAT_005e0330). Skipping — only affects one
        // hardcoded named attack we don't ship.
    }

    // Damage calculation (retail 0x4d171c). Only run when the caller
    // didn't already set damage (passes -1 sentinel). Retail tracks two
    // sentinels (damage and a randomness field, both -1 by default).
    if (targ && damage == 0 /* sentinel: caller wants us to compute */)
    {
        // Base attack value: damageMod-scaled weapon damage
        // adjusted by attack-modifier and dmgpcnt.
        damage = targ->CalculateDamage(WeaponDamage(),
            GetDamageType(WeaponType(), ad->flags), ad->damagemod) * dmgpcnt / 100;
    }

    if (ad->flags & CA_DEATH)
    {
        if (!targ) { note_reject("CA_DEATH no targ"); return false; }
        if (damage < targ->Health()) { note_reject("CA_DEATH dmg<hp"); return false; }
    }

    // Impact-loop with fallback (retail 0x4d19c0-0x4d1d3a). For each
    // declared impact in the attack we accept it when EITHER:
    //   * damage >= target's current health AND impact has CAI_DEATH
    //   * (no impact selected yet) damage is in the impact's range
    // and the target either:
    //   * has the named impact animation directly, OR
    //   * (for CAI_DEATH impacts) has a fallback "to_dead" transition
    //     animation built from the target's body root (see retail
    //     string-manipulation around 0x4d1bc0).
    //
    // This is the most intricate part of retail combat — getting it
    // right is what unblocks attacking monsters whose imagery doesn't
    // include every named impact. The retail decompile builds an
    // alternate name "<root>_to_dead" / "<root>_to_impact" by string
    // concatenation. We mirror that semantically using StName().
    if (targ && ad->numimpacts > 0)
    {
        SCharAttackImpact* ai = ad->impacts;
        for (int32_t i = 0; i < ad->numimpacts; i++, ai++)
        {
            const bool deathimp = (ai->flags & CAI_DEATH) != 0;
            const bool deathmatch = deathimp && (damage >= targ->Health());
            const bool rangematch = (impactnum < 0) &&
                (damage >= ai->damagemin && damage <= ai->damagemax);
            if (!(deathmatch || rangematch))
                continue;

            // Bit 0x80 / piVar13[0x15]: retail flag we don't model — see
            // TODO above. The retail path runs FindClearPath here for
            // some impacts; we skip for now since we can't identify the
            // bit (CAI_? — possibly "needs line-of-sight").
            // TODO retail: ai->flags bit 0x80 + piVar13[0x15] LoS test.

            bool ok = false;
            if (!deathimp)
            {
                // Plain impact: target must have the named impact ani.
                ok = targ->HasActionAni(ai->impactname);
                if (ok && ai->loopname[0] != '\0')
                    ok = targ->HasActionAni(ai->loopname);
            }
            else
            {
                // Death impact: try the named ani first; if absent
                // build a "<root>_to_dead" or "<root>_to_impact"
                // transition name. Retail's string-builder path uses
                // BuildActionName(target, buf, "") to get the root,
                // then suffixes "_to_d","ead" or "..." (DAT_005e00c4-
                // 005e00d4). We approximate with StName(root,"to_dead")
                // and "to_impact".
                if (targ->HasActionAni(ai->impactname))
                {
                    ok = true;
                }
                else if (stricmp(ai->impactname, "combat_to_dead") == 0)
                {
                    // Build alternate from target root: "<root>_to_dead"
                    const char *targroot = (targ->root && targ->root->name[0])
                        ? targ->root->name : "combat";
                    const char *alt = StName(targroot, "to_dead");
                    ok = targ->HasActionAni(alt);
                }
                else
                {
                    // TODO retail: DAT_005e0350 — second specific
                    // impact name retail special-cases (likely
                    // "bow_to_dead" or "<root>_to_dead"). Falling back
                    // to the named ani here.
                    ok = targ->HasActionAni(ai->impactname);
                }

                if (ok && ai->loopname[0] != '\0')
                {
                    if (!targ->HasActionAni(ai->loopname))
                    {
                        // Retail tries a "_to_<loop>" alternate.
                        const char *targroot = (targ->root && targ->root->name[0])
                            ? targ->root->name : "combat";
                        const char *alt = StName(targroot, ai->loopname);
                        ok = targ->HasActionAni(alt);
                    }
                }
            }

            if (ok)
                impactnum = i;
        }

        // Retail 0x4d1d18: if we ran out of declared impacts and the
        // *last* impact (piVar13 here) had bit 0x80 set with health
        // already exceeded, reject. We don't model bit 0x80 — skip.

        // If no impact was selected, the attack still validates (retail
        // returns 1 — the impact-loop is purely advisory unless CA_DEATH
        // forces it). The caller's damage path will use the default
        // chardata->impacts list in DoAttack via ResolveHit.
    }

    // Final health-vs-damage cinematic gate (retail 0x4d1d3a): when
    // this character has charflags bit 0x80 set ("ChainHits enabled"
    // or similar) and the attack would NOT kill the target, reject
    // unless the attack is a CA_INTERACTIVE death.
    // TODO retail: charflags bit 0x80 unidentified (CF_ values stop at
    // 0x0008). Skipping that gate.

    // Schedule a TPlayScreen "fight begin" hook for non-player target
    // (retail 0x4d1da9): meth_0x4d2e30(target). We don't have that
    // hook plumbed yet — skipping is benign.
    return true;
}

// REVSYNC: retail TPlayer::meth_0x4d1dd0 (player-specific button dispatch,
// 3-mode loop) — recon/classes/cls_0x5b4f30.cpp lines 1161-1191.
// (Retail TCharacter::meth_0x4d1ff0_FindButtonAttack is a different
// chain-retry helper used only from ButtonAttack's combo path; it gates
// on CA_INTERACTIVE so all CA_HAND attacks would be skipped — wrong for
// the main "press SWING" path.) The 3-mode pass walks the attack list
// three times trying CA_RESPONSE then CA_SPECIAL then plain attacks, so
// chain/special attacks beat normals when both are eligible.
bool TCharacter::FindButtonAttack(int32_t id, int32_t dmgpcnt, int32_t &attacknum,
    int32_t &impactnum, int32_t &damage, bool isaction)
{
    if (!IsFighting())
    {
        if (objclass == OBJCLASS_PLAYER)
            log_warn("[combat-dbg] FindButtonAttack(btn=%d): !IsFighting", id);
        return false;
    }

    int32_t tdist = Fighting() ? Distance(Fighting()) : 10000;
    bool dbg = (objclass == OBJCLASS_PLAYER);

    for (int32_t mode = 0; mode < 3; mode++)
    {
        int32_t flags;
        if      (mode == 0) flags = CA_RESPONSE;
        else if (mode == 1) flags = CA_SPECIAL;
        else                flags = 0;
        if (isaction) flags |= CA_ACTION;

        if (dbg && mode == 2)
        {
            extern thread_local int32_t g_dbg_attack_rej_button;
            extern thread_local const char *g_dbg_attack_rej_reason;
            extern thread_local int32_t g_dbg_attack_rej_tdist, g_dbg_attack_rej_mindist, g_dbg_attack_rej_maxdist;
            g_dbg_attack_rej_button = id;
            g_dbg_attack_rej_reason = nullptr;
            g_dbg_attack_rej_tdist = g_dbg_attack_rej_mindist = g_dbg_attack_rej_maxdist = 0;
        }

        const int32_t n = chardata->attacks.NumItems();
        for (int32_t a = 0; a < n; a++)
        {
            if (IsValidAttack(a, impactnum, damage, tdist, id, 0, dmgpcnt,
                              CA_RESPONSE | CA_SPECIAL, flags))
            {
                attacknum = a;
                if (dbg)
                {
                    extern thread_local int32_t g_dbg_attack_rej_button;
                    g_dbg_attack_rej_button = -1;
                }
                return true;
            }
        }
    }

    if (dbg)
    {
        extern thread_local const char *g_dbg_attack_rej_reason;
        extern thread_local int32_t g_dbg_attack_rej_tdist, g_dbg_attack_rej_mindist, g_dbg_attack_rej_maxdist;
        extern thread_local int32_t g_dbg_attack_rej_button;
        TPlayer *p = (TPlayer *)this;
        const char *reason = g_dbg_attack_rej_reason ? g_dbg_attack_rej_reason : "(none — no button match)";
        if (g_dbg_attack_rej_reason && strcmp(g_dbg_attack_rej_reason, "range") == 0)
        {
            log_warn("[combat-dbg] FindButtonAttack(btn=%d) no match. "
                     "first_reject='range' tdist=%d need=[%d..%d] (Locke is %s) "
                     "attacks=%d wpn=%d fatigue=%d/%d",
                     id, g_dbg_attack_rej_tdist,
                     g_dbg_attack_rej_mindist, g_dbg_attack_rej_maxdist,
                     (g_dbg_attack_rej_tdist < g_dbg_attack_rej_mindist ? "too close" : "too far"),
                     chardata ? chardata->attacks.NumItems() : -1,
                     (int)WeaponType(),
                     (int)Fatigue(), (int)p->MaxFatigue());
        }
        else
        {
            log_warn("[combat-dbg] FindButtonAttack(btn=%d) no match. "
                     "first_reject='%s' attacks=%d wpn=%d combat=%d sneak=%d bow=%d "
                     "fighting=%d fatigue=%d/%d skill_atk=%d wpnskill=%d",
                     id, reason,
                     chardata ? chardata->attacks.NumItems() : -1,
                     (int)WeaponType(),
                     (int)IsCombat(), (int)IsSneakMode(), (int)IsBowMode(),
                     (int)IsFighting(),
                     (int)Fatigue(), (int)p->MaxFatigue(),
                     (int)p->Skill(SK_ATTACK),
                     (int)p->WeaponSkill(WeaponType()));
        }
        g_dbg_attack_rej_button = -1;
    }
    return false;
}

// REVSYNC: retail TCharacter::FindPcntAttack @ 0x4d1eb0
// Source-port of the retail decompile in
// recon/classes/cls_0x5a7b98.cpp lines 10284-10320. Loops random
// indices into chardata->attacks up to 2*N times.
bool TCharacter::FindPcntAttack(int32_t pcnt, int32_t dmgpcnt, int32_t &attacknum,
    int32_t &impactnum, int32_t &damage)
{
    // Retail requires root to be COMBAT or BOW
    if (!root) return false;
    if (root->action != ACTION_COMBAT && root->action != ACTION_BOW)
        return false;

    // TODO retail: meth_0x46ea20 / chardata->attacks_default side-call.

    int32_t tdist = Fighting() ? Distance(Fighting()) : 10000;

    const int32_t n = chardata->attacks.NumItems();
    const int32_t limit = n * 2;
    for (int32_t i = 0; i < limit; i++)
    {
        int32_t a = random(0, n - 1);
        if (IsValidAttack(a, impactnum, damage, tdist, -1, pcnt, dmgpcnt, 0, 0))
        {
            attacknum = a;
            return true;
        }
    }
    return false;
}

// Executes a particular attack (using index into SCharData's attack array)
bool TCharacter::DoAttack(int32_t attacknum, int32_t impactnum, int32_t damage)
{
    if ((uint32_t)attacknum >= (uint32_t)chardata->attacks.NumItems())
        return false;

  // Get attack info from char data
    SCharAttackData* ad = &(chardata->attacks[attacknum]);

  // Get target
    TCharacter* targ = (TCharacter*)doing->obj;

  // Do magic attack
    if (ad->flags & CA_MAGICATTACK)
        return CastByName(ad->spellname, (TObjectInstance* *)&targ, (targ)?1:0, &(ad->spellsource));

  // Get action type
    ACTION a;
    if (ad->flags & CA_PLAYANIM)
        a = ACTION_COMBAT;
    else
        a = ACTION_ATTACK;  

  // Setup action block
    TActionBlock* ab = new TActionBlock(ad->attackname, a);
    ab->obj = targ;
    ab->attack = ad;
    if (impactnum >= 0)
        ab->impact = &(ad->impacts[impactnum]);
    else
        ab->impact = nullptr; // Use default impact
    ab->damage = damage;
    ab->interrupt = true;   // Play animation now
    ab->noroot = true;      // don't go back to the root anim before playing the new one

    SetDesired(ab);

  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

    return true;
}

// Find attack based on the button the player pressed
bool TCharacter::ButtonAttack(int32_t buttonid)
{
 // Check if chain attack is valid...
    if (lastattack && 
        (lastattack->flags & CA_CHAIN) &&
        (PlayScreen.GameFrame() - lastattackticks <= lastattack->chainexptime) &&
        (chainhits < MAXCHAINHITS))
    {
        chainhits++;
        return true;
    }

    int32_t dmgpcnt = random(1, 50) + random(1, 50);
    int32_t attacknum, impactnum, damage;
    bool found = FindButtonAttack(buttonid, dmgpcnt, attacknum, impactnum, damage);
    if (found)
        return DoAttack(attacknum, impactnum, damage);
    else
        return false;
}

// Find an action in RULES.DEF based on given button number
bool TCharacter::ButtonAction(int32_t buttonid)
{
    int32_t dmgpcnt = random(1, 50) + random(1, 50);
    int32_t attacknum, impactnum, damage;
    bool found = FindButtonAttack(buttonid, dmgpcnt, attacknum, impactnum, damage, true);
    if (found)
        return DoAttack(attacknum, impactnum, damage);
    else
        return false;
}

// Find a random attack (for a monster)
bool TCharacter::RandomAttack(int32_t pcnt)
{
    int32_t dmgpcnt = random(1, 50) + random(1, 50);
    int32_t attacknum, impactnum, damage;
    bool found = FindPcntAttack(pcnt, dmgpcnt, attacknum, impactnum, damage);
    if (found)
    {
        return DoAttack(attacknum, impactnum, damage);
    }
    else 
        return false;
}

// Do a specific attack
bool TCharacter::SpecificAttack(int32_t attacknum)
{
    TCharacter* targ = Fighting();
    int32_t tdist = 0;
    if (targ)
        tdist = Distance(targ);

    int32_t dmgpcnt = random(1, 50) + random(1, 50);
    int32_t impactnum, damage;
    bool valid = IsValidAttack(attacknum, impactnum, damage, tdist, -1, -1, dmgpcnt, 0, 0);

    if (valid)
        return DoAttack(attacknum, impactnum, damage);
    else
        return false;
}

bool TCharacter::Leap(int32_t angle)
{
    if (!IsFighting())
        return false;

    int32_t roundangle = ((GetFace() + 15) & 0xE0); // round to 8 dirs
    int32_t diff = (angle - roundangle) & 255;
    int32_t anim = diff / 32;

    TActionBlock* ab = nullptr;

    char *sfx;

    switch (anim)
    {
        case 0: sfx = "leapf"; break;
        case 1: sfx = "leapfr"; break;
        case 2: sfx = "leapr"; break;
        case 3: sfx = "leapbr"; break;
        case 4: sfx = "leapb"; break;
        case 5: sfx = "leapbl"; break;
        case 6: sfx = "leapl"; break;
        case 7: sfx = "leapfl"; break;
    }

    char animname[RESNAMELEN];
    strcpy(animname, StName(root->name, sfx));

    if (!HasActionAni(animname))
        return false;

    ab = new TActionBlock(animname, ACTION_COMBATLEAP);
    ab->interrupt = true;

    if (ab && doing)    // Copy current target
        ab->obj = doing->obj;

    SetDesired(ab);

    return true;
}

bool TCharacter::Block(int32_t frames)
{
    if (!IsFighting() || !(IsDoing(ACTION_COMBAT) || IsDoing(ACTION_IMPACT)))
        return false;

    TCharacter* targ = (TCharacter*)doing->obj;

    char *blockanim = "block";
    bool synchronize = false;

    if (targ)
    {
        if (targ->IsDoing(ACTION_ATTACK) &&                         // Char is attacking
            targ->GetFrame() < targ->GetDoing()->attack->blocktime) // And we're in time to block
        {
            blockanim = targ->GetDoing()->attack->blockname;        // Get desired block name
            if (!HasActionAni(blockanim))
                blockanim = "block";                                // Use default block
            else
            {
    //          if (targ->GetDoing()->attack->flags & CA_SNAPBLOCK) // If special block, and needs snap, do snap
    //              SnapDist(targ, targ->doing->impact->snapdist);
            }
        }
        else
        {
            return false;
        }
    }

    if (!HasActionAni(blockanim)) // Do we have this particular block?
        return false;

  // Ok, now start the block (note that AF_SYNCHRONIZE will cause frames to sync with attack
    TActionBlock* ab = new TActionBlock(blockanim, ACTION_BLOCK);
    ab->obj = doing->obj;
    if (frames < 0)
        ab->wait = random(chardata->blockmin, chardata->blockmax);
    else
        ab->wait = frames;
    ab->interrupt = true;
    ab->loop = true;
    SetDesired(ab);

  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

    return true;
}

bool TCharacter::StopBlock()
{
    if (!IsFighting() || !IsDoing(ACTION_BLOCK))
        return false;

    doing->wait = 0;

    return true;
}

bool TCharacter::Dodge()
{
    if (!IsFighting() || !IsDoing(ACTION_COMBAT))
        return false;

    TActionBlock* ab = new TActionBlock("dodge", ACTION_DODGE);
    ab->obj = doing->obj;
    SetDesired(ab);

    return true;
}

// REVSYNC: retail TCharacter::SideStep @ 0x4d6220
//   recon/discovered/cls_0x5a7b98_TCharacter_GoCmd_4d6220.cpp (size 398).
// Cartwheel sidestep: queues a "sidestepl" / "sidestepr" animation that
// steps the character ~90 degrees off facing. Used by the AI body to
// dodge blockers and by the in-range attack tree when a character is in
// the line of attack. When called with dir=0 (or any non-l/r byte),
// retail picks L/R at random — fed twice in a row, that's the
// "cartwheel both ways" pattern.
//
// Retail uses bare anim names ("sidestepl", not "comhand_sidestepl");
// HasActionAni resolves transitions from the current root via
// FindTransitionState.
bool TCharacter::SideStep(char dir)
{
    if (!doing)
        return false;

  // Retail FUN_004d6220:26 — gate: only proceed if doing->name is NOT
  // already prefixed with "sidestep" (i.e. we're not already in a
  // sidestep). Prevents re-queuing a fresh sidestep on top of an
  // in-progress one.
    if (doing->name && strncmp(doing->name, "sidestep", 8) == 0)
        return false;

  // Retail randomises L/R when caller didn't specify (lines 28-31).
    if (dir != 'l' && dir != 'r')
        dir = random(0, 1) ? 'l' : 'r';

    char animname[10];
    animname[0] = 's'; animname[1] = 'i'; animname[2] = 'd'; animname[3] = 'e';
    animname[4] = 's'; animname[5] = 't'; animname[6] = 'e'; animname[7] = 'p';
    animname[8] = dir; animname[9] = '\0';

  // Retail FUN_004d6220:33 calls vftbl[0x1f0/4] which is HasActionAni
  // (or its variant) on the bare anim name — so transitions from the
  // current root are searched automatically.
    if (!HasActionAni(animname))
        return false;

  // Retail uses action=3 (ACTION_COMBAT) — the sidestep stays inside the
  // combat root rather than swapping to ACTION_DODGE; keeps the character
  // ready to attack again on the next tick.
    TActionBlock* ab = new TActionBlock(animname, ACTION_COMBAT);
    ab->obj       = doing->obj;
    ab->moveangle = doing->moveangle;

  // Step direction: 90° left or right of facing. Retail FUN_004d6220:49
  //   ab->angle = (dir == 'l' ? face + 0x40 : face + 0xc0) & 0xff
    int32_t face = GetFace();
    if (dir == 'l') ab->angle = (face + 0x40) & 0xff;
    else            ab->angle = (face + 0xc0) & 0xff;

  // Retail FUN_004d6220:51 — turnrate=8 (limits in-step turn speed).
    ab->turnrate = 8;

  // Retail FUN_004d6220:52 — flags = (flags & ~nowaitdone) | interrupt | noroot
  //   bit 0x10 = interrupt, bit 0x20 = nowaitdone, bit 0x200 = noroot.
    ab->interrupt  = 1;
    ab->noroot     = 1;
    ab->nowaitdone = 0;

    SetDesired(ab);
    return true;
}

bool TCharacter::Pulp(S3DPoint vel, int32_t piece_count, int32_t blood_count)
{
    // I'm not dead yet. I think I'll go for a walk.
    // oh wait, I am pulped!
    if (IsDoing(ACTION_PULP) || Flags() & OF_DISABLED)
        return false;

    // create the action block
    TActionBlock* ab = new TActionBlock("pulped", ACTION_PULP);
    ab->priority = true;
    ab->action = ACTION_PULP;
    ForceCommand(ab);
    SetRoot(ab);
    SetDesired(ab);
    Disable();

    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));
    def.objclass = OBJCLASS_EFFECT;
    def.level = MapPane.GetMapLevel();
    def.pos = Pos();
    def.facing = GetFace();
    def.objtype = EffectClass.FindObjType("PULP");

    TPulpEffect* pulpstuff = (TPulpEffect*)MapPane.GetInstance(MapPane.NewObject(&def));
    if (!pulpstuff)
        return false;

    // init all the params
    pulpstuff->Set(vel, this, piece_count, blood_count);

    return true;
}

bool TCharacter::Burn()
{
    if (!HasActionAni("onfire"))
        return false;

    if (!burning)
    {
        SObjectDef def;
        memset(&def, 0, sizeof(SObjectDef));
        def.objclass = OBJCLASS_EFFECT;
        def.level = MapPane.GetMapLevel();
        def.pos = Pos();
        def.facing = GetFace();
        def.objtype = EffectClass.FindObjType("BURN");

        TBurnEffect* burn = (TBurnEffect*)MapPane.GetInstance(MapPane.NewObject(&def));
        if (!burn)
            return false;
        burning = burn;

        // init all the params
        burn->Set(this);

        // now make him do the uncontrolable chicken on fire dance of death!
        // create action block
        TActionBlock* ab = new TActionBlock("onfire", ACTION_BURN);
        ab->priority = true;
        ForceCommand(ab);
    }
    else
    {
        ((TBurnEffect*)burning)->ResetFrameCount();
    }

    return true;
}


bool TCharacter::KnockBack(S3DPoint frompos)
{
    S3DPoint pos;

    if (IsParalized())
        return true;

    GetPos(pos);
    float dx = (float)(frompos.x - pos.x), dy = (float)(frompos.y - pos.y);
    float ang = (float)atan2(dy, dx);
    TActionBlock* ab = new TActionBlock("cimpk", ACTION_IMPACT);
    ab->priority = true;
    ForceCommand(ab);
    Face((int32_t)((ang * 256) / M_2PI));
    
    return true;
}

// Returns the correct angle movce animation given the current movedir, facedir, and root name
char *TCharacter::GetAngleMoveAnim(int32_t movedir, int32_t facedir, char *root, char *animname, int32_t buflen)
{
    int32_t faceangle = ((facedir + 15) & 0xE0);
    int32_t dirangle = ((movedir + 15) & 0xE0); // round to 8 dirs
    int32_t diff = (dirangle - faceangle) & 255;
    int32_t anim = diff / 32;

    char *sfx;
    switch (anim)
    {
        case 0: sfx = "f"; break;
        case 1: sfx = "fr"; break;
        case 2: sfx = "r"; break;
        case 3: sfx = "br"; break;
        case 4: sfx = "b"; break;
        case 5: sfx = "bl"; break;
        case 6: sfx = "l"; break;
        case 7: sfx = "fl"; break;
    }
    
    strncpyz(animname, StName(root, sfx), buflen);

    if (!HasActionAni(animname))
        strncpyz(animname, StName(root, "f"), buflen);

    return animname;
}

ACTION TCharacter::GetMoveAction(ACTION action)
{
    switch (action)
    {
      case ACTION_ANIMATE:
      case ACTION_MOVE:
        return ACTION_MOVE;
      case ACTION_COMBAT:
      case ACTION_COMBATMOVE:
        return ACTION_COMBATMOVE;
      case ACTION_BOW:
      case ACTION_BOWMOVE:
        return ACTION_BOWMOVE;
    }
    return ACTION_NONE;
}

ACTION TCharacter::GetLeapAction(ACTION action)
{
    switch (action)
    {
      case ACTION_ANIMATE:
      case ACTION_MOVE:
        return ACTION_LEAP;
      case ACTION_COMBAT:
      case ACTION_COMBATMOVE:
        return ACTION_COMBATLEAP;
    }
    return ACTION_NONE;
}

bool TCharacter::BeginFighting(TCharacter* target, ACTION action)
{
  // Prevent auto combat from being called again for 
  // all currently visible characters.
    SetHasSeenAutoCombat(false);

  // Don't target dead guys
    if (target && (target->IsDead() || target->IsInvisibleSpell()))
        target = nullptr;

  // If player doesn't have bow, escape out
    if (action == ACTION_BOW && ObjClass() == OBJCLASS_PLAYER)
    {
            TPlayer* player = (TPlayer*)this;
            if (player->RangedWeapon() == nullptr)
                return false;
    }

    if (!target)
        target = FindClosestEnemy(GetFace(), 32);

    if (root->action == action)
        return SetFighting(target);

    char *rootname;
    if (action == ACTION_BOW)
        rootname = GetBowRoot();
    else if (action == ACTION_COMBAT)
        rootname = GetCombatRoot();
    else
        Error("Invalid fighting action");

  // Doesn't have a combat state.. don't keep calling this function!
    if (FindState(rootname) < 0)
        return false;

    if (IsMoving())
        Stop();

    TActionBlock* ab = new TActionBlock(rootname, action);
    ab->obj = target;
    ab->priority = (FindTransitionState(doing->name, ab->name) >= 0);
        // Priority if has transition only, otherwise it will go immediately
        // to correct state without possibility of being interrupted
    ab->interrupt = true;     // Interrupt whatever I'm doing now
    if (doing->Is("walk"))
        doing->priority = false; // Allow him to override an EndCombat() action

    if (target)
    {
        int32_t newangle = AngleTo(target);
        ab->angle = newangle;
    }
    else
        ab->angle = GetFace();
    ab->moveangle = GetMoveAngle();

    SetDesired(ab);

    if (GetScript())
        GetScript()->Trigger(TRIGGER_COMBAT); // Trigger the combat script

    nextattack = -1;

    return true;
}

bool TCharacter::EndFighting()
{
    if (!IsFighting())
        return true;

    if (FindState("walk") < 0)      // Avoid trying to set walk state in the future
        return false;

    if (IsMoving())
        Stop();

    TActionBlock* ab = new TActionBlock("walk");
    ab->priority = (FindTransitionState(doing->name, ab->name) >= 0);
        // Priority if has transition only, otherwise it will go immediately
        // to correct state without possibility of being interrupted
    if (doing == root)
        doing->priority = false; // Allow him to override a BeginCombat() action
    SetDesired(ab);

  // Make sure moving angle equals face (it doesn't during a combat move)
    SetMoveAngle(GetFace());

  // Clear autocombat fields for all currently visible characters
    SetHasSeenAutoCombat(false);

    return true;
}

bool TCharacter::SetFighting(TCharacter* newtarget)
{
    if (newtarget && newtarget->IsDead())   // Can't target dead guys
        return false;
    if (!IsFighting())
        return BeginCombat(newtarget);
    if (doing->obj == newtarget)
        return true;
    doing->obj = newtarget;
    desired->obj = newtarget;
    root->obj = newtarget;
    if (newtarget)
    {
        int32_t newangle = AngleTo(newtarget);
        doing->angle = newangle;
        desired->angle = newangle;
        root->angle = newangle;
    }

    return true;
}

void TCharacter::Wait(int32_t waitlen)
{
    waittype = WAIT_TICKS;
    waitticks = waitlen;
}

bool TCharacter::PlayAnim(char *string)
{
    if (!HasActionAni(string))
        return false;

    TActionBlock* ab = new TActionBlock(string, ACTION_ANIMATE);
    SetDesired(ab);

    return true;
}

bool TCharacter::ExecutingQueued()
{
    if (desired && (desired->Is("walkl") || desired->Is("walkr")) &&
        (desired->target.x || desired->target.y))
        return true;

    return false;
}

int32_t TCharacter::CursorType(TObjectInstance* inst)
{
    if (inst)
        return CURSOR_NONE;

    if (IsDead())
    {
        if (RealNumInventoryItems() > 0)
            return CURSOR_HAND;         // loot the corpse
        else
            return CURSOR_NONE;         // He's dead, Jim
    }

    if (Aggressive())
        return CURSOR_NONE;
//      return CURSOR_SWORDS;           // kick its ass

    return CURSOR_MOUTH;                // chat for a bit
}

bool TCharacter::Use(TObjectInstance* user, int32_t with)
{
    if (with >= 0)
    {
        TObjectInstance* inst = MapPane.GetInstance(with);
        if (!inst)
            return false;
        if (GetScript())
            GetScript()->Trigger(TRIGGER_GET, inst->GetName());
        if (user && user->GetScript())
            GetScript()->Trigger(TRIGGER_GIVE, inst->GetName());
        return true;
    }

    if (IsDead() && user)
    {
        // loot the corpse
        TInventoryIterator i(this);
        TObjectInstance* oi = i.Item();

        if (oi)
        {
            if ((uint32_t)user->FindFreeInventorySlot() >= MAXINVITEMS)
                TextBar.Print("Can't carry any more.");
            else
            {
                oi->RemoveFromInventory();
                user->AddToInventory(oi);

                char buf[80];
                sprintf(buf, "%s taken from corpse of %s.", oi->GetName(), GetName());
                TextBar.Print(buf);
            }

            return true;
        }

        return false;
    }

    if (!Aggressive() && user == (TObjectInstance*)Player)
    {
      // Face eachother
        S3DPoint upos;
        user->GetPos(upos);
        int32_t angle = ConvertToFacing(pos, upos);
        Face(angle);
        angle = ConvertToFacing(upos, pos);
        user->Face(angle);

      // Start DIALOG section
        if (GetScript())
            GetScript()->Trigger(TRIGGER_DIALOG);

        return true;
    }

    return false;
}

// REVSYNC: retail TCharacter::CharBlocking / FindCharInLine @ 0x4d4db0
//   recon/discovered/cls_0x5a7b98_TCharacter_CharBlocking_4d4db0.cpp (size 253).
//   Iterates characters within a 0xe0 (224) radius of the search position,
//   filters out self / dead / interactive-attack-flagged / paralised /
//   movetopos-active / sleeping-with-flag-2 / fallen, then returns the
//   first whose 2D distance minus radius minus their radius is <= 0.
//   Our 128 range is tighter than retail's 224 but matches what the
//   pre-snapshot source has shipped with. Retail-side filters we have
//   NOT folded in (low-impact for AI parity):
//     - doing->attack & 0x2000000 ("INTERACTIVE attack") suppression on
//       the scanned char.
//     - imagery flag 0x800 mask on the scanned char.
//     - OF_PARALIZE / movetopos / sleeping-with-bit-2 gates on the
//       scanned char.
//   The Demo 1 Araknid case never hits these branches: no spider has
//   INTERACTIVE attacks active in walk-mode, and the player isn't going
//   to be paralised while a spider's running at it. Folding the rest in
//   is a Tier-2 chore.
TCharacter* TCharacter::CharBlocking(TObjectInstance* inst, const S3DPoint& pos, int32_t radius)
{
    int32_t range = 128; // This should be about right

    for (TMapIterator i(pos, range, CHECK_NOINVENT | CHECK_MAPRECT, OBJSET_CHARACTER); i; i++)
    {
        TCharacter* chr = (TCharacter*)i.Item();

        if (chr && chr != inst && !chr->IsDead())
        {
            int32_t dist = ::Distance(pos, chr->Pos());
            dist -= radius;
            dist -= chr->Radius();

            if (dist < 1)
                return chr;
        }
    }

    return nullptr;
}

// ------------- Streaming functions ------------------

// Loads object data from the sector
void TCharacter::Load(RTInputStream is, int32_t version, int32_t objversion)
{
    if (objversion >= 3)
        LOAD_BASE(TComplexObject)
    else
        TComplexObject::Load(is, version, 0);

  // Get saved last pulse values (so we can figure what has happened to char)
    if (objversion < 1)
        return;

  // Last time any health/fatigue/mana was recovered
    is >> lasthealthrecov;
    is >> lastfatiguerecov;
    is >> lastmanarecov;

    if (objversion >= 4)
    {
        is >> lastpoisondamage;
    }
    else
    {
        lastpoisondamage = -1;
    }

    if (objversion >= 2)
    {
        is >> teleport_position.x;
        is >> teleport_position.y;
        is >> teleport_position.z;
        is >> teleport_level;
    }
    else
    {
        teleport_position.x = teleport_position.y = teleport_position.z = -1;
        teleport_level = -1;
    }

    if (ObjClass() == OBJCLASS_CHARACTER && chardata)
    {
        if (Health() > chardata->health)
            SetHealth(chardata->health);
        if (MaxHealth() > chardata->health)
            SetMaxHealth(chardata->health);
        if (Fatigue() > chardata->fatigue)
            SetFatigue(chardata->fatigue);
        if (MaxFatigue() > chardata->fatigue)
            SetMaxFatigue(chardata->fatigue);
        if (Mana() > chardata->mana)
            SetMana(chardata->mana);
        if (MaxMana() > chardata->mana)
            SetMaxMana(chardata->mana);
    }
}

// Saves object data to the sector
void TCharacter::Save(RTOutputStream os)
{
    SAVE_BASE(TComplexObject)

  // Last time any health/fatigue/mana was recovered
    os << lasthealthrecov;
    os << lastfatiguerecov;
    os << lastmanarecov;

  // Last time poison damage was charged
    os << lastpoisondamage;

  // Our teleport positions
    os << teleport_position.x;
    os << teleport_position.y;
    os << teleport_position.z;
    os << teleport_level;
}

// make a character visible
void TCharacter::MakeVisible()
{
    is_invisible = false;

    TCharAnimator* animator = (TCharAnimator*)GetAnimator();

    for(int32_t i = 0; i < animator->Get3DImagery()->NumMaterials(); ++i)
    {
        S3DMat mat;
        animator->Get3DImagery()->GetMaterial(i, &mat);

        S3DMaterial &m = mat.matdesc;

        m.ambient.a = 1.0f;
        m.diffuse.a = 1.0f;
        m.specular.a = 1.0f;

        animator->Get3DImagery()->SetMaterial(i, &mat);
    }
}

// make a character invisible
void TCharacter::MakeInvisible()
{
    is_invisible = true;

    TCharAnimator* animator = (TCharAnimator*)GetAnimator();

    for(int32_t i = 0; i < animator->Get3DImagery()->NumMaterials(); ++i)
    {
        S3DMat mat;
        animator->Get3DImagery()->GetMaterial(i, &mat);
        S3DMaterial &m = mat.matdesc;

        m.ambient.a = .5f;
        m.diffuse.a = .5f;
        m.specular.a = .5f;

        animator->Get3DImagery()->SetMaterial(i, &mat);
    }
}

void TCharacter::SetFade(int32_t amt, int32_t amt2, int32_t amt3)
{
    fade = amt;
    fade_step = amt2;
    fade_limit = amt3;
}

int32_t TCharacter::GetFade(void)
{
    return fade;
}

void TCharacter::UpdateFade(void)
{
    fade -= fade_step;

    if (fade_limit == -1)
    {
        if (fade < 0)
            fade = 0;
        if (fade > 100)
            fade = 100;
    }
    else if (fade_step > 0)
    {
        if (fade < fade_limit)
            fade = fade_limit;
        if (fade > 100)
            fade = 100;
    }
    else if (fade_step < 0)
    {
        if (fade > fade_limit)
            fade = fade_limit;
        if (fade < 0)
            fade = 0;
    }

}

// set to cast mode
bool TCharacter::SetCast(char* ani, TObjectInstance* target, int32_t invoke_delay)
{
//  int32_t incombat = IsFighting();
    char puthere[50];
    strcpy(puthere, ani);
    if (!stricmp(ani, "invoke1"))
    {
        strcpy(puthere,"cinv1");
        puthere[4] = '1';
        puthere[5] = '\0';
        if (IsCombat())
        {
            if (IsRunMode())
                strcpy(puthere,"crinv1");
        }
        else if (IsHandCombat())
        {
            puthere[0] = 'h';
            if (IsRunMode())
                puthere[0] = 'r';
        }
        else if (IsBowMode())
        {
            puthere[0] = 'b';
            if (IsRunMode())
                strcpy(puthere,"brinv1");
        }
        else if (IsWalkMode())
            puthere[0] = 'w';
        else if (IsSneakMode())
            puthere[0] = 's';
    }
    if (!stricmp(ani, "invoke2"))
    {
        strcpy(puthere,"cinv2");
        puthere[4] = '2';
        puthere[5] = '\0';
        if (IsCombat())
        {
            if (IsRunMode())
                strcpy(puthere,"crinv2");
        }
        else if (IsHandCombat())
        {
            puthere[0] = 'h';
            if (IsRunMode())
                puthere[0] = 'r';
        }
        else if (IsBowMode())
        {
            puthere[0] = 'b';
            if (IsRunMode())
                strcpy(puthere,"brinv2");
        }
        else if (IsWalkMode())
            puthere[0] = 'w';
        else if (IsSneakMode())
            puthere[0] = 's';
    }
    if (!stricmp(ani, "invoke3"))
    {
        strcpy(puthere,"cinv3");
        puthere[4] = '3';
        puthere[5] = '\0';
        if (IsCombat())
        {
            if (IsRunMode())
                strcpy(puthere,"crinv3");
        }
        else if (IsHandCombat())
        {
            puthere[0] = 'h';
            if (IsRunMode())
                puthere[0] = 'r';
        }
        else if (IsBowMode())
        {
            puthere[0] = 'b';
            if (IsRunMode())
                strcpy(puthere,"brinv3");
        }
        else if (IsWalkMode())
            puthere[0] = 'w';
        else if (IsSneakMode())
            puthere[0] = 's';
    }
    if (!stricmp(ani, "invoke4"))
    {
        strcpy(puthere,"cinv4");
        puthere[4] = '4';
        puthere[5] = '\0';
        if (IsCombat())
        {
            if (IsRunMode())
                strcpy(puthere,"crinv4");
        }
        else if (IsHandCombat())
        {
            puthere[0] = 'h';
            if (IsRunMode())
                puthere[0] = 'r';
        }
        else if (IsBowMode())
        {
            puthere[0] = 'b';
            if (IsRunMode())
                strcpy(puthere,"brinv4");
        }
        else if (IsWalkMode())
            puthere[0] = 'w';
        else if (IsSneakMode())
            puthere[0] = 's';
    }
    if (!stricmp(ani, "invoke5"))
    {
        strcpy(puthere,"cinv5");
        puthere[4] = '1';
        puthere[5] = '\0';
        if (IsCombat())
        {
            if (IsRunMode())
                strcpy(puthere,"crinv5");
        }
        else if (IsHandCombat())
        {
            puthere[0] = 'h';
            if (IsRunMode())
                puthere[0] = 'r';
        }
        else if (IsBowMode())
        {
            puthere[0] = 'b';
            if (IsRunMode())
                strcpy(puthere,"brinv5");
        }
        else if (IsWalkMode())
            puthere[0] = 'w';
        else if (IsSneakMode())
            puthere[0] = 's';
    }

        
    if (!HasActionAni(puthere))
    {
        strcpy(puthere, "invoke");
        if (!HasActionAni(puthere))
            return false;
    }

    // create the action block
    TActionBlock* ab = new TActionBlock(puthere, ACTION_INVOKE);
    ab->obj = target;
    ab->priority = true;
    ForceCommand(ab);
    invokedelay = invoke_delay;

    return true;
}

// cast a spell using talismans, automating the targeting
bool TCharacter::Cast(char* talismans, S3DPoint* sourcepos)
{
    TObjectInstance* targ = Fighting();

    return CastByTalismans(talismans, &targ, (targ)?1:0, sourcepos);
}

// cast a spell by using its name
bool TCharacter::CastByName(char* name, TObjectInstance* *target, int32_t numtargs, S3DPoint* sourcepos)
{
    return SpellManager.CastByName(name, this, target, numtargs, sourcepos);
}

// cast a spell by using a list of talismans
bool TCharacter::CastByTalismans(char* talismans, TObjectInstance* *target, int32_t numtargs, S3DPoint* sourcepos)
{
    return SpellManager.CastByTalismans(talismans, this, target, numtargs, sourcepos);
}

// Flail - Make the character act a fool
bool TCharacter::Flail()
{
    if (IsFlailing())
        return true;

    TActionBlock* ab = new TActionBlock("impact", ACTION_FLAIL);
    ab->priority = true;
    ForceCommand(ab);

    return true;
/*
    if (!burning)
    {
        SObjectDef def;
        memset(&def, 0, sizeof(SObjectDef));
        def.objclass = OBJCLASS_EFFECT;
        def.level = MapPane.GetMapLevel();
        def.pos = Pos();
        def.facing = GetFace();
        def.objtype = EffectClass.FindObjType("BURN");

        TBurnEffect* burn = (TBurnEffect*)MapPane.GetInstance(MapPane.NewObject(&def));
        if (!burn)
            return false;
        burning = burn;

        // init all the params
        burn->Set(this);

        // now make him do the uncontrolable chicken on fire dance of death!
        // create action block
        TActionBlock* ab = new TActionBlock("onfire", ACTION_BURN);
        ab->priority = true;
        ForceCommand(ab);
    }
    else
    {
        ((TBurnEffect*)burning)->ResetFrameCount();
    }

    return true;
*/
}
