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

  // Set initial health/fatigue/mana values
    SetHealth(MaxHealth());
    SetFatigue(MaxFatigue());
    SetMana(MaxMana());

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
}

void TCharacter::Pulse()
{
//  if (!animator)  // Quick hack to fix super slodown... BEN  (if not on screen, ignore me)
//      return;

  // Pulse the animator
    if (animator)
        animator->Pulse();

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
        PTCharacter target = Fighting();
        if (target)
            target->SignalHostility(this, target);

      // Update the health/etc. for player's target
        if ((PTPlayer)this == Player && target)
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
            PTObjectInstance inst = MapPane.GetInstance(index);
            if (inst)
            {
                ((PTBloodEffect)inst)->SetParams(height, 0, 64, 255, 5, 1);
            }
        }
    }

    if (IsDead())
    {
      // When we're dead, we're dead!
        if (!IsDoing(ACTION_DEAD))
            ForceCommand(new TActionBlock("dead", ACTION_DEAD));

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
                PSCharAttackData ad = &(chardata->attacks[c]);

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
        PTCharacter targ = FindClosestEnemy(GetFace(), 32); // Updates enemy list and HasSeen list
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
    PTCharAnimator anim = (PTCharAnimator)GetAnimator();
    if (anim)
    {
        PTWeaponSwipe weaponswipe = anim->GetWeaponSwipe();
        if (weaponswipe && weaponswipe->GetInitialized())
            weaponswipe->Animate();
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
        comstate = (commanddone || !animator || (flags & OF_INVISIBLE)) ? // If done, or not animating..                                        // Done if finished animating
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
            if (!doing->obj || ((PTCharacter)doing->obj)->IsInRoot())
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

    if (animator && draw && doing && 
        doing->action == ACTION_SAY && 
        doing->data &&
        doing->wait > 12) // Leave a half a second between sentences
    {
        SRect r;
        Display->GetClipRect(r);

        SColor color = { 0, 150, 255 };

        if (this == (PTCharacter)Player)
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
        
        if (IsBadReadPtr(hasseen[c].chr, 4) || 
            hasseen[c].chr && NOTIFY_DELETED(ptr, hasseen[c].chr))
        {
            hasseen[c].chr = nullptr;
            hasseen[c].time = 0;
            hasseen[c].noautocombat = false;
        }
    }
}

#define MAXZMOVE 32

bool TCharacter::Blocked(S3DPoint &pos, S3DPoint &newpos, uint32_t bits, int32_t *height, PTCharacter *bychar)
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

    PTCharacter dummybychar;
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
        pos.z = h;
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
            // check nearby squares in front of char to try to find open path
            if (shovedir < 0)
            {
                for (int32_t i = 0; i < 4; i++)
                {
                    int32_t d;
                    if (i & 1)
                        d = WALKMAPGRANULARITY << (i >> 1);
                    else
                        d = -(WALKMAPGRANULARITY << (i >> 1));

                    S3DPoint v, p;
    //              p = pos;
    //              ConvertToVector(moveangle, 6, v);
    //              p += v;
                    p = newpos;
                    ConvertToVector(moveangle + 64, d, v);
                    p += v;

                  // Check to see if there's open space to right/left
                    if (!Blocked(pos, p, retval)) // This way's ok
                    {
                        if (d < 0)
                            shovedir = (moveangle - 64) & 255; // Go left around obstacle
                        else
                            shovedir = (moveangle + 64) & 255; // Go right around obstacle
                        break;
                    }
                }
            }


            if (shovedir >= 0)
            {
                S3DPoint v;
                ConvertToVector(shovedir, 4, v);
                newpos = pos;
                newpos += v;
                moveaccum = accum;
                if (!Blocked(pos, newpos, retval)) // This way's ok
                    retval &= ~MOVE_BLOCKED; // Go ahead and move there
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
    PTActionBlock action, PTCharacter attacker)
{
  // Calculate total damage
    if (damagetype >= 0)
        damage = CalculateDamage(damage, damagetype, modifier);

  // Apply damage to low level object
    TObjectInstance::Damage(damage);

  // Get impact pointer
    PSCharAttackImpact impactdata = nullptr;

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
        PTActionBlock death = action;

      // Caller didn't give us a special death to use so...
        if (!death)
        {
          // Find a 'death' impact in impact list (if there is one)
            char *deathname = "dead";
            impactdata = chardata->impacts;
            for (int32_t i = 0; i < chardata->numimpacts; i++, impactdata++)
            {
                if (damage >= impactdata->damagemin && 
                    damage <= impactdata->damagemax &&
                    (impactdata->flags & CAI_DEATH) &&
                    (!(impactdata->flags & CAI_WHENSTUNNED) || doing->action == ACTION_STUN) &&
                    (!(impactdata->flags & CAI_WHENDOWN) || doing->action == ACTION_KNOCKDOWN) &&
                    HasActionAni(impactdata->impactname) && 
                    (impactdata->loopname[0] == nullptr || HasActionAni(impactdata->loopname)))
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
        PTActionBlock impact = action;

      // Caller didn't give us a special impact to use so...    
        if (!impact)
        {
          // Find default impact from char's default impact list
            char *impactname = "impact";
            ACTION impactaction = ACTION_IMPACT;
            impactdata = chardata->impacts;
            for (int32_t i = 0; i < chardata->numimpacts; i++, impactdata++)
            {
                if (damage >= impactdata->damagemin && 
                    damage <= impactdata->damagemax &&
                    !(impactdata->flags & CAI_DEATH) &&
                    (!(impactdata->flags & CAI_WHENSTUNNED) || doing->action == ACTION_STUN) &&
                    (!(impactdata->flags & CAI_WHENDOWN) || doing->action == ACTION_KNOCKDOWN) &&
                    HasActionAni(impactdata->impactname) && 
                    (impactdata->loopname[0] == nullptr || HasActionAni(impactdata->loopname)))
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
bool TCharacter::HasSeenMe(PTCharacter me)
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
void TCharacter::SetHasSeen(PTCharacter me)
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
bool TCharacter::HasSeenAutoCombat(PTCharacter me)
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

PTObjectInstance TCharacter::FindObjAhead()
{
    S3DPoint pos, v;
    GetPos(pos);
    ConvertToVector(GetFace(), 60, v);
    pos += v;

    int32_t list[MAXFOUNDOBJS];
    int32_t n = MapPane.FindObjectsInRange(pos, list, 60);

    PTObjectInstance best = nullptr;
    int32_t bestdist;

    for (int32_t i = 0; i < n; i++)
    {
        PTObjectInstance oi = MapPane.GetInstance(list[i]);
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

int32_t TCharacter::ResolveMove(PTActionBlock ab, int32_t bits)
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
            PTActionBlock newab = nullptr;

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
        PTActionBlock newab = new TActionBlock(*doing);
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
bool TCharacter::ResolveHit(PTCharacter targ, 
    PSCharAttackData attack, PSCharAttackImpact attackimpact, int32_t attackdamage)
{
    int32_t damage = 0;
    PSCharAttackImpact impact = nullptr;

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
                PTActionBlock deathab, impactab, hitab;
                deathab = impactab = hitab = nullptr;
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

int32_t TCharacter::ResolveAttack(PTActionBlock ab, int32_t bits)
{
    PTCharacter targ = (PTCharacter)ab->obj;
    PSCharAttackData attack = ab->attack;
    PSCharAttackImpact impact = ab->impact;

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
            PTCharacter chars[MAXHITCHARS];

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
            PTActionBlock missab;

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

int32_t TCharacter::ResolveImpact(PTActionBlock ab, int32_t bits)
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
            ab->impact->loopname[0] != nullptr &&
            FindState(ab->impact->loopname) >= 0)
        {
            PTActionBlock newab = new TActionBlock(*ab, ab->impact->loopname, ab->action);
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
        PTActionBlock newab = new TActionBlock(root->name, ACTION_COMBAT);
        newab->interrupt = true;
        SetDesired(newab);
    }
                        
    return 0;
}

int32_t TCharacter::ResolveBlock(PTActionBlock ab, int32_t bits)
{
    if (ab->wait <= 0 || (doing && doing->stop))
    {
        ab->wait = 0;
        SetDesired(nullptr);
        return COM_COMPLETED;
    }

    return COM_EXECUTING;
}

int32_t TCharacter::ResolveDead(PTActionBlock ab, int32_t bits)
{
  // Do blood for attack
    if (ab->firsttime && (!ab->attack || (ab->attack->flags & CA_BLOOD)))
        EffectBurst("blood", ab->Is("impale") ? 40 : 50);

  // If we're done with dying animation (transition), do the death animation.
    if (commanddone &&
        ab->impact && 
        ab->impact->loopname[0] != nullptr &&
        !ab->Is(ab->impact->loopname) &&
        FindState(ab->impact->loopname) >= 0)
    {
        PTActionBlock newab = new TActionBlock(*ab, ab->impact->loopname, ab->action);
        newab->priority = true;
        newab->interrupt = true;
        desired->priority = false;
        doing->priority = false;
        SetDesired(newab);
        SetRoot(newab);
    }

    return COM_EXECUTING;
}

int32_t TCharacter::ResolveCombat(PTActionBlock ab, int32_t bits)
{
  // Clear goto's when in combat
    ab->target.x = ab->target.y = ab->target.z = 0;

  // If target is dead, change to new target, or end combat
    PTCharacter targ = (PTCharacter)ab->obj;
    PTCharacter newtarg;
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
            PTActionBlock newab = new TActionBlock(animname, doing->action);
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
                PTActionBlock newab = new TActionBlock(animname, doing->action);
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

int32_t TCharacter::ResolveCombatMove(PTActionBlock ab, int32_t bits)
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

int32_t TCharacter::ResolveBowAim(PTActionBlock ab, int32_t bits)
{
    Halt(); // Make sure there's no movement

    if (ab->stop && GetFace() == ab->angle)
        return COM_COMPLETED;

  // Do turning
    AdvanceAngles(ab->angle, ab->angle, ab->turnrate);

    return COM_EXECUTING; // Allow to loop indefinitely until cancelled
}

int32_t TCharacter::ResolveBowShoot(PTActionBlock ab, int32_t bits)
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

        PTObjectInstance arrow = FindObjInventory(OBJCLASS_AMMO, AT_ARROW);
        if (arrow)
            DeleteFromInventory(arrow->GetName(), 1);

        PlayWave("arrow");
    }

    return 0;
}

int32_t TCharacter::ResolveLeap(PTActionBlock ab, int32_t bits)
{
    return ResolveCombat(ab, bits);
}

int32_t TCharacter::ResolvePull(PTActionBlock ab, int32_t bits)
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

int32_t TCharacter::ResolveSay(PTActionBlock ab, int32_t bits)
{
    if (ab->wait <= 0 || (doing && doing->stop))
    {
        ab->wait = 0;
        ForceCommand(root);
        return COM_COMPLETED;
    }

    return COM_EXECUTING;
}

int32_t TCharacter::ResolvePivot(PTActionBlock ab, int32_t bits)
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
        PTObjectInstance inst = MapPane.GetInstance(index);
        if (!inst)
            return;

        ((PTBloodEffect)inst)->SetParams(height, (GetFace() + 128) & 255, 0, 80, 20, random(1, 5));
    }
    else
    {
        def.pos = pos;

        int32_t index = MapPane.NewObject(&def);
        PTObjectInstance inst = MapPane.GetInstance(index);
        if (!inst)
            return;

        inst->CreateAnimator();
        PTParticle3DAnimator anim = (PTParticle3DAnimator)inst->GetAnimator();

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
            pr.pos.x = (float)vect0.x;
            pr.pos.y = (float)vect0.y;
            pr.pos.z = (float)vect0.z;
            pr.pspread.x = (float)3.0;
            pr.pspread.y = (float)3.0;
            pr.pspread.z = (float)3.0;
            pr.dir.x = (float)((float)vect.x / (float)100.0);
            pr.dir.y = (float)((float)vect.y / (float)100.0);
            pr.dir.z = (float)((float)vect.z / (float)100.0);
            pr.spread.x = (float)0.5;
            pr.spread.y = (float)0.5;
            pr.spread.z = (float)0.5;
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

bool TCharacter::IsEnemy(PTCharacter chr)
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

void TCharacter::AI()
{
    if (flags & OF_DISABLED || IsDead() || NoAI)
    {
        if (IsMoving())
            Stop();
        return;
    }

    if (Editor)
        return;

    PTCharacter target = Fighting();
    
  // We don't have a target.. try to find one
    if (!target && !Editor && Aggressive())
    {
        target = FindClosestEnemy(); // Finds the closest visible enemy (if it can see it)
        if (target && Distance((PTCharacter)target) < chardata->combatrangemin)
            BeginCombat(target);
    }
    if (target)
    {
        if (target->IsInvisibleSpell())
        {
            target = FindClosestEnemy(); // Finds the closest visible enemy (if it can see it)
            if (target && Distance((PTCharacter)target) < chardata->combatrangemin)
                BeginCombat(target);
        }
    }
    if (target && doing)
    {
        if (doing->action == ACTION_COMBAT)
        {
            if (nextattack > 0)
                nextattack--;

            if (nextattack == 0)
            {
                if (random(1,100) <= 20/*chardata->blockfreq*/ && !(flags & OF_ICED))
                    Block();
                else
                {
                    bool attacked = RandomAttack(random(1,100));
                    if (!attacked && Distance(target) > 80)
                    {
                        target_out_of_sight = false;
                        Go(AngleTo(target));
                    }
                }

                nextattack--;
            }
            else

            if (nextattack <= 0)
            {
                nextattack = 
                    random(chardata->minattackfreq * FRAMERATE / 100, 
                           chardata->maxattackfreq * FRAMERATE / 100);
            }
        }
        else if (doing->action == GetMoveAction(root->action))
        {
            if (Fighting())
            {
                if (Distance(Fighting()) < (Radius() + Fighting()->Radius() + chardata->maxattackrange))
                    Stop();
                else
                {
                    if (target_out_of_sight)
                    {
                        int32_t needed_angle = AngleTo(Fighting());

//                      if (target_last_angle > needed_angle)
//                          doing->moveangle = real_angle - 25;
//                      else
                            doing->moveangle = target_last_angle;
                        if (::Distance(pos,last_position_start_point) > last_position_distance)
                            target_out_of_sight = false;
                    }
                    else
                    {
                        if (CanSeeCharacter(target))
                        {
                            last_position_count--;
                            if (last_position_count <= 0)
                            {
                                last_position_count = 2;
                                target->GetPos(target_last_position);
                            }
                            doing->moveangle = AngleTo(Fighting()); // Go towards char
                        }
                        else
                        {
                            int32_t needed_angle = AngleTo(Fighting());
                            int32_t target_angle = ConvertToFacing(pos, target_last_position) + 63;
                            last_position_distance = ::Distance(pos,target_last_position);
                            GetPos(last_position_start_point);
                            target_out_of_sight = true;
                            if (target_angle > needed_angle)
                                target_last_angle = target_angle + 30;
                            else
                                target_last_angle = target_angle - 30;
                        }
                    }
                }       
            }
        }
    }
}

bool TCharacter::CanHearCharacter(PTCharacter chr)
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

bool TCharacter::CanSeeCharacter(PTCharacter chr, int32_t angle)
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
int32_t TCharacter::FindCharacters(PTCharacter chars[], int32_t maxchars, 
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
        PTCharacter chr = (PTCharacter)i.Item();

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
            PTCharacter temp = chars[0];
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

PTCharacter TCharacter::FindCharacter(int32_t range, int32_t angle, int32_t anglerange, int32_t flags)
{
    PTCharacter chr;
    int32_t numchars = FindCharacters(&chr, 1, range, angle, anglerange, flags);
    if (numchars > 0)
        return chr;
    else
        return nullptr;
}

PTCharacter TCharacter::FindCharacterAhead(int32_t angle, int32_t anglerange)
{
    return FindCharacter(512, angle, anglerange, 0);
}

PTCharacter TCharacter::FindClosestEnemy(int32_t angle, int32_t anglerange)
{
    return FindCharacter(-1, angle, anglerange, FINDCHAR_ENEMY | FINDCHAR_SEE | FINDCHAR_HEAR);
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

void TCharacter::SignalMovement(PTObjectInstance actor)
{
}

void TCharacter::SignalHostility(PTObjectInstance actor, PTObjectInstance target)
{
    if (target != this)
        return;
}

void TCharacter::SignalAttack(PTObjectInstance actor, PTObjectInstance target)
{
    if (target != this)
        return;

    if (actor == doing->obj && 
        random(1, 100) <= BlockPcnt())
          Block();

    // if we're a monster, then target our attacker...
    if (ObjClass() != OBJCLASS_PLAYER)
        SetFighting((PTCharacter)actor);
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
    PTActionBlock ab = nullptr;

  // What is our moveaction for this mode
    ACTION moveaction = GetMoveAction(root->action);

    if (!IsDoing(root->action) && !IsDoing(moveaction))
        return false; // Can't move right now

  // Do we start fighting a new character?
    PTCharacter newtarg = FindClosestEnemy(angle, 32); // Hack to get has seen setup

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

        PTCharacter targ = (PTCharacter)doing->obj;

      // Always check if we're pointing to a new target when we move in a new direction
        if (newtarg && 
            newtarg != (PTCharacter)doing->obj && 
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

    PTActionBlock ab = new TActionBlock(aniname, root->action);
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

    PTActionBlock ab = new TActionBlock("sneak");
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

    PTActionBlock ab = new TActionBlock(aniname, root->action);
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

    PTActionBlock ab = new TActionBlock(buf, ACTION_PIVOT);
    ab->angle = ab->moveangle = angle;
    ab->turnrate = turnrate;
    ab->interrupt = true;
    ab->noroot = true;
    SetDesired(ab);

    return true;
}

// This obviously does nothing right now
bool TCharacter::FollowChar(PTObjectInstance inst)
{
    return false;
}

bool TCharacter::Pickup(PTObjectInstance inst)
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

bool TCharacter::Pull(PTObjectInstance inst)
{
    PTActionBlock ab = new TActionBlock("Pull Front");
    int32_t state = inst->GetState();
    int32_t direction = (((PTLever)inst)->targetpos.z / 64);

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

    PTObjectInstance best = nullptr;
    int32_t bestdist;

    for (int32_t i = 0; i < n; i++)
    {
        PTObjectInstance oi = MapPane.GetInstance(list[i]);
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

    PTObjectInstance best = nullptr;
    int32_t bestdist;

    for (int32_t i = 0; i < n; i++)
    {
        PTObjectInstance oi = MapPane.GetInstance(list[i]);
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
            PTSound sound = SoundPlayer.GetSound(soundid);
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

    PTActionBlock ab;
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

    PTActionBlock ab = new TActionBlock(StName(root->name, "aim"), ACTION_BOWAIM);
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
    PTActionBlock ab = new TActionBlock(StName(root->name, "shoot"), ACTION_BOWSHOOT);
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

// Checks attack to see if it is valid or not
bool TCharacter::IsValidAttack(int32_t attacknum, int32_t &impactnum, int32_t &damage,
    int32_t tdist, int32_t id, int32_t pcnt, int32_t dmgpcnt, int32_t flagmask, int32_t flags)
{
    impactnum = -1;
    damage = 0;

    if ((uint32_t)attacknum >= (uint32_t)chardata->attacks.NumItems())
        return false;

    PSCharAttackData ad = &(chardata->attacks[attacknum]);
    PTCharacter targ = Fighting();

  // Matches flags
    if ((ad->flags & flagmask) != flags)
        return false;

  // Button id matches attack button id (for controller/keyboard buttons)
    if (id >= 0 && ad->button != id)
        return false;

  // Percentage value is less than percent parameter (for random attack finding)
    if (ad->attackpcnt < pcnt)
        return false;
        
  // In range
    if (ad->maxdist > 0 && targ && (tdist < ad->mindist || tdist > ad->maxdist))
        return false;;

  // Check to see if we are in proper mode
    if ((ad->flags & CA_SNEAKMODE) && !IsSneakMode())
        return false;
    else if ((ad->flags & CA_WALKMODE) && !IsWalkMode())
        return false;
    else if ((ad->flags & CA_BOWMODE) && !IsBowMode())
        return false;
    else if (!IsCombat())
        return false;

  // If magical attack or play animation, we're all done!
    if (ad->flags & CA_MAGICATTACK || ad->flags & CA_PLAYANIM)
        return true;

  // Not still doing another attack
    if (doing->action == ACTION_ATTACK && frame < doing->attack->nextwait)
        return false;

  // Has animation
    if (!HasActionAni(ad->attackname))
        return false;

  // Not too tired
    if (Fatigue() < ad->fatigue)
        return false;

  // Check if is a moving attack
    if ((ad->flags & CA_MOVING) &&                                          // Doing move attack
        (!(IsMoving() || (IsDoing(ACTION_ATTACK) && (doing->attack->flags & CA_MOVING)) ) || // Not doing move attack
         abs(AngleDiff(GetFace(), GetMoveAngle())) > 32) )                  // Or not facing direction of move
        return false;

  // Check if is a moving attack
    if ((ad->flags & CA_RUNNING) &&                                         // Doing move attack
        (!(IsRunMode() || (IsDoing(ACTION_ATTACK) && (doing->attack->flags & CA_RUNNING)) ) || // Not doing move attack
         abs(AngleDiff(GetFace(), GetMoveAngle())) > 32) )                  // Or not facing direction of move
        return false;

   // Check for attack when opponent stunned or down
    if (targ && (ad->flags & CA_ATTACKSTUN) && !targ->IsDoing(ACTION_STUN))
        return false;

  // Check for attack when opponent down
    if (targ && (ad->flags & CA_ATTACKDOWN) && !targ->IsDoing(ACTION_KNOCKDOWN))
        return false;

  // If a response, make sure target character is in correct state to respond to
    if (targ && ad->responsename[0] != 0 && !targ->doing->Is(ad->responsename))
        return false;

  // Check if chain attack is valid...
    if ((ad->flags & (CA_CHAIN | CA_AUTOCOMBO)) && ad->chainname[0] != nullptr && 
      (!lastattack || 
       stricmp(lastattack->attackname, ad->chainname) != 0 ||
       PlayScreen.GameFrame() - lastattackticks > lastattack->chainexptime))
        return false;

  // Check player skills, etc.  
    if (objclass == OBJCLASS_PLAYER)
    {
        PTPlayer player = (PTPlayer)this;
        
      // Is using correct weapon for this attack
        if (!(ad->weaponmask & (1 << (WeaponType() - 1))))
            return false;

      // Player must have high enough attack skill
        if (player->Skill(SK_ATTACK) < ad->attackskill)
            return false;

      // Player must have high enough weapon skill for the given weapon being used
        if (player->WeaponSkill(WeaponType()) < ad->weaponskill)
            return false;
    }

  // Calculate damage attack will do to target now
    if (targ)
    {
        damage = targ->CalculateDamage(WeaponDamage(), 
            GetDamageType(WeaponType(), ad->flags), ad->damagemod) * dmgpcnt / 100;
    }

  // If a death attack, make sure the guy will actually die...
    if (ad->flags & CA_DEATH)
    {
        if (!targ)
            return false;
        if (damage < targ->Health())
            return false;
    }
    
  // If impact, make sure character has appropriate impact/death/stun animation
    if (ad->numimpacts > 0 && targ)
    {
        PSCharAttackImpact ai = ad->impacts;
        for (int32_t i = 0; i < ad->numimpacts; i++, ai++)
        {
            if (((damage >= targ->Health()) && (ai->flags & CAI_DEATH)) ||  // Is death impact (overrides any damage impacts)
                (impactnum < 0 &&                                           // or death impact not already set and...
                 damage >= ai->damagemin && damage <= ai->damagemax))       // is normal/stun/knockdown impact
            {
                if (!targ->HasActionAni(ai->impactname)) // Don't have this impact ani, don't do this attack!
                    return false;
                if (ai->loopname[0] != nullptr && !targ->FindState(ai->loopname)) // Needs the loop too!
                    return false;
                impactnum = i;  // Remember what impact we're using
            }
        }
    }

    return true;
}

// Finds a valid attack given an attack id (i.e. controller button)
bool TCharacter::FindButtonAttack(int32_t id, int32_t dmgpcnt, int32_t &attacknum, int32_t &impactnum, int32_t &damage, bool isaction)
{
    if (!IsFighting())
        return false;

    int32_t flags;

    int32_t tdist;
    if (Fighting())
        tdist = Distance(Fighting());
    else
        tdist = 10000;

  // Loop through array three times (1-combo attack, 2-special attack, 3-normal attack)
  // This allows combos, specials, and normals to use the same attack buttons on the joypad/
  // keyboard.  Specials and combo's will only happen when they are available and all
  // parameters qualify, otherwise this routine will cascade down to the normal attacks.
    for (int32_t mode = 0; mode < 3; mode++)
    {

    // Look for combos first, then specials, then just normals
      if (mode == 0)
        flags = CA_RESPONSE;
      else if (mode == 1)
        flags = CA_SPECIAL;
      else
        flags = 0;

      if (isaction)
        flags |= CA_ACTION;

    // Loop through attack list to find a valid attack
      for (int32_t attack = 0; attack < chardata->attacks.NumItems(); attack++)
      {
        if (IsValidAttack(attack, impactnum, damage, tdist, id, 0, dmgpcnt, CA_RESPONSE | CA_SPECIAL, flags))
        {
            attacknum = attack;
            return true;
        }
      }
    }

    return false;
}

// Finds a valid attack given a randomly generated percentage (0-100) number
bool TCharacter::FindPcntAttack(int32_t pcnt, int32_t dmgpcnt, int32_t &attacknum, int32_t &impactnum, int32_t &damage)
{
    if (!IsFighting() || !Fighting())
        return false;

    int32_t tdist = Distance(Fighting());

  // Loop through attack list to find a valid attack
  // Loops through twice.. should be able to find one that works by then...
    for (int32_t attack = 0; attack < chardata->attacks.NumItems() * 2; attack++)
    {
        int32_t randattack = random(0, chardata->attacks.NumItems() - 1);
        if (IsValidAttack(randattack, impactnum, damage, tdist, -1, pcnt, dmgpcnt, 0, 0))
        {
            attacknum = randattack;
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
    PSCharAttackData ad = &(chardata->attacks[attacknum]);

  // Get target
    PTCharacter targ = (PTCharacter)doing->obj;

  // Do magic attack
    if (ad->flags & CA_MAGICATTACK)
        return CastByName(ad->spellname, (PTObjectInstance *)&targ, (targ)?1:0, &(ad->spellsource));

  // Get action type
    ACTION a;
    if (ad->flags & CA_PLAYANIM)
        a = ACTION_COMBAT;
    else
        a = ACTION_ATTACK;  

  // Setup action block
    PTActionBlock ab = new TActionBlock(ad->attackname, a);
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
    PTCharacter targ = Fighting();
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

    PTActionBlock ab = nullptr;

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

    PTCharacter targ = (PTCharacter)doing->obj;

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
    PTActionBlock ab = new TActionBlock(blockanim, ACTION_BLOCK);
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

    PTActionBlock ab = new TActionBlock("dodge", ACTION_DODGE);
    ab->obj = doing->obj;
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
    PTActionBlock ab = new TActionBlock("pulped", ACTION_PULP);
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

    PTPulpEffect pulpstuff = (PTPulpEffect)MapPane.GetInstance(MapPane.NewObject(&def));
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

        PTBurnEffect burn = (PTBurnEffect)MapPane.GetInstance(MapPane.NewObject(&def));
        if (!burn)
            return false;
        burning = burn;

        // init all the params
        burn->Set(this);

        // now make him do the uncontrolable chicken on fire dance of death!
        // create action block
        PTActionBlock ab = new TActionBlock("onfire", ACTION_BURN);
        ab->priority = true;
        ForceCommand(ab);
    }
    else
    {
        ((PTBurnEffect)burning)->ResetFrameCount();
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
    PTActionBlock ab = new TActionBlock("cimpk", ACTION_IMPACT);
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

bool TCharacter::BeginFighting(PTCharacter target, ACTION action)
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
            PTPlayer player = (PTPlayer)this;
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

    PTActionBlock ab = new TActionBlock(rootname, action);
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

    PTActionBlock ab = new TActionBlock("walk");
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

bool TCharacter::SetFighting(PTCharacter newtarget)
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

    PTActionBlock ab = new TActionBlock(string, ACTION_ANIMATE);
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

int32_t TCharacter::CursorType(PTObjectInstance inst)
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

bool TCharacter::Use(PTObjectInstance user, int32_t with)
{
    if (with >= 0)
    {
        PTObjectInstance inst = MapPane.GetInstance(with);
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
        PTObjectInstance oi = i.Item();

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

    if (!Aggressive() && user == (PTObjectInstance)Player)
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

PTCharacter TCharacter::CharBlocking(PTObjectInstance inst, RS3DPoint pos, int32_t radius)
{
    int32_t range = 128; // This should be about right

    for (TMapIterator i(pos, range, CHECK_NOINVENT | CHECK_MAPRECT, OBJSET_CHARACTER); i; i++)
    {
        PTCharacter chr = (PTCharacter)i.Item();

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

    if (ObjClass() == OBJCLASS_CHARACTER)
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

    PTCharAnimator animator = (PTCharAnimator)GetAnimator();

    for(int32_t i = 0; i < animator->Get3DImagery()->NumMaterials(); ++i)
    {
        S3DMat mat;
        animator->Get3DImagery()->GetMaterial(i, &mat);

        D3DMATERIAL &m = mat.matdesc;

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

    PTCharAnimator animator = (PTCharAnimator)GetAnimator();

    for(int32_t i = 0; i < animator->Get3DImagery()->NumMaterials(); ++i)
    {
        S3DMat mat;
        animator->Get3DImagery()->GetMaterial(i, &mat);
        D3DMATERIAL &m = mat.matdesc;

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
bool TCharacter::SetCast(char* ani, PTObjectInstance target, int32_t invoke_delay)
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
    PTActionBlock ab = new TActionBlock(puthere, ACTION_INVOKE);
    ab->obj = target;
    ab->priority = true;
    ForceCommand(ab);
    invokedelay = invoke_delay;

    return true;
}

// cast a spell using talismans, automating the targeting
bool TCharacter::Cast(char* talismans, PS3DPoint sourcepos)
{
    PTObjectInstance targ = Fighting();

    return CastByTalismans(talismans, &targ, (targ)?1:0, sourcepos);
}

// cast a spell by using its name
bool TCharacter::CastByName(char* name, PTObjectInstance *target, int32_t numtargs, PS3DPoint sourcepos)
{
    return SpellManager.CastByName(name, this, target, numtargs, sourcepos);
}

// cast a spell by using a list of talismans
bool TCharacter::CastByTalismans(char* talismans, PTObjectInstance *target, int32_t numtargs, PS3DPoint sourcepos)
{
    return SpellManager.CastByTalismans(talismans, this, target, numtargs, sourcepos);
}

// Flail - Make the character act a fool
bool TCharacter::Flail()
{
    if (IsFlailing())
        return true;

    PTActionBlock ab = new TActionBlock("impact", ACTION_FLAIL);
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

        PTBurnEffect burn = (PTBurnEffect)MapPane.GetInstance(MapPane.NewObject(&def));
        if (!burn)
            return false;
        burning = burn;

        // init all the params
        burn->Set(this);

        // now make him do the uncontrolable chicken on fire dance of death!
        // create action block
        PTActionBlock ab = new TActionBlock("onfire", ACTION_BURN);
        ab->priority = true;
        ForceCommand(ab);
    }
    else
    {
        ((PTBurnEffect)burning)->ResetFrameCount();
    }

    return true;
*/
}
