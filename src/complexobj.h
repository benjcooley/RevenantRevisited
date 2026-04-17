// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                complexobj.h - TComplexObject module                   *
// *************************************************************************

#ifndef _COMPLEXOBJ_H
#define _COMPLEXOBJ_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

// States of completion for actions
#define COM_PENDING         0           // Can't start command yet
#define COM_EXECUTING       1           // Command is being executed
#define COM_COMPLETED       2           // Command is complete
#define COM_IMPOSSIBLE      3           // Can't get to this state from here

// Animate action (the default)
typedef enum {
    ACTION_NONE,
    ACTION_ANIMATE,
    ACTION_MOVE,
    ACTION_COMBAT,
    ACTION_COMBATMOVE,
    ACTION_COMBATLEAP,
    ACTION_COLLAPSE,
    ACTION_ATTACK,
    ACTION_BLOCK,
    ACTION_DODGE,
    ACTION_MISS,
    ACTION_INVOKE,
    ACTION_IMPACT,
    ACTION_STUN,
    ACTION_KNOCKDOWN,
    ACTION_FLYBACK,
    ACTION_SAY,
    ACTION_PIVOT,
    ACTION_PULL,
    ACTION_DEAD,
    ACTION_PULP,
    ACTION_BURN,
    ACTION_FLAIL,
    ACTION_SLEEP,
    ACTION_LEAP,
    ACTION_BOW,
    ACTION_BOWMOVE,
    ACTION_BOWAIM,
    ACTION_BOWSHOOT,
} ACTION;

// Note: Please define derived class actions in increments of 100 for each 
// derived class (i.e. CHARACTER 100-199, PLAYER 200-299

// Action block - data about a desired or executing object action (state)
_CLASSDEF(TActionBlock)
class TActionBlock
{
    // !!!!***** WARNING *****!!!! DO NOT PUT VIRTUAL FUNCTIONS IN THIS OBJECT!!!
    // !!!! --- THIS MEANS YOU!!  !!!!

  public:
    TActionBlock() { ClearBlock(); }
    TActionBlock(const char *n, ACTION a = ACTION_ANIMATE);               // Animation/action
    TActionBlock(const char *n, const char *str, ACTION a = ACTION_ANIMATE);    // Animation+string/action
    TActionBlock(TActionBlock &ab, const char *str = nullptr, ACTION = ACTION_NONE); // Copies another action block
    
    ~TActionBlock() { if (data) free(data); }

    bool Is(const char *s) const;           // Multipurpose match (?=any one char, *=0 or more chars, #=any num, [xx]=any one char in braces)
    bool Is(ACTION a) const { return action == a; } // Is an action
    bool IsRight(const char *state) const;  // Is left step for movement
    bool IsLeft(const char *state) const;   // Is right step for movement
    bool IsStep(const char *state) const;   // Is step for movement
    bool IsOneOf(const char *state) const;  // true if state is one of the master state (i.e. "attack1" is one of "attack")
    bool IsPartOf(const char *prefix, const char *state = nullptr) const; // true if state has the given prefix and state name, (doesn't care about suffix)
    void SwapRoot(const char *root, const char *newroot);  // Swaps the root part of the name with a new root

    int32_t StateNum() const;             // Returns number of state (i.e. 1 for "attack1")
    void ClearBlock();                    // Clears the action block

    ACTION action;                        // Action id (what we're doing)
    char name[RESNAMELEN];                // Name of state
    int32_t frame;                        // Frame number to start on when command begins
    int32_t wait;                         // Delay for current command
    int32_t angle, moveangle, turnrate;   // Angle of movement and turn rate
    S3DPoint target;                      // Target location for movement
    TObjectInstance* obj;                 // Target object for action
    PSCharAttackData attack;              // Attack info (if is attack/impact/death/stun/knockdown action)
    PSCharAttackImpact impact;            // Impact info (if is attack/impact/death/stun/knockdown action)
    int32_t damage;                       // Damage attack will do (if hits)
    void *data;                           // Data field (such as text for say)
    union
    {
      uint32_t flags;
      struct
      {
        uint32_t firsttime : 1;     // If this is the first time through
        uint32_t transition : 1;    // If currently transitioning to this state
        uint32_t terminating : 1;   // Action block is shutting down
        uint32_t priority : 1;      // Action can't be changed by calling SetDesired() until played
        uint32_t interrupt : 1;     // Interrupts current doing animation unless doing has priority
        uint32_t nowaitdone : 1;    // Allows desired to interrupt this action
        uint32_t dontforce : 1;     // Don't force the animation if transition doesn't exist
        uint32_t stop : 1;          // Abort this action
        uint32_t waitpivot : 1;     // For movenent actions, wait until pivot done before moving
        uint32_t noroot : 1;        // Don't use this as a root state (even if playing a root animation)
        uint32_t loop : 1;          // Loop this command
      };
    };
};

// Helper functions to make it easy to make state names
char *StName(char *name, int32_t num);
char *StName(char *name, char *str);

// TComplexObject adds the state transition code to TObjectInstance.
// It is not intended for use as a seperate object class any more than
// TObjectInstance, but rather as a parent class in order to give certain
// object classes advanced state functionality.

_CLASSDEF(TComplexObject)
class TComplexObject : public TObjectInstance
{
  public:
    void ClearComplexObj();

    TComplexObject(TObjectImagery* newim) : TObjectInstance(newim) { ClearComplexObj(); }
    TComplexObject(SObjectDef* def, TObjectImagery* newim) : TObjectInstance(def, newim) { ClearComplexObj(); }

    virtual void Pulse();
      // Main pulse (done before frame is drawn)

    bool IsInRoot() { return (doing == root); }
        // Returns whether the object is in their root state or not

    void Try(const char *state)
        { PTActionBlock ab = new TActionBlock(state); ab->angle = GetFace(); SetDesired(ab); }
        // Trys to change to the given state
    int32_t Force(const char *state)
        { PTActionBlock ab = new TActionBlock(state); ab->angle = GetFace(); return ForceCommand(ab); }
        // Forces the given state
    // this is a hack               ---v
    bool IsDoing(const char *state) const { return doing ? doing->Is(state) : false; }
        // Returns true if currently doing the given state
    bool IsDesired(const char *state) const { return desired ? desired->Is(state) : false; }
        // Returns true if the desired state matches the given state
    bool IsRoot(const char *state) const { return root ? root->Is(state) : false; }
        // Returns true if the desired state matches the given state
    // this is a hack               ---v
    bool IsDoing(ACTION action) const { return doing ? doing->action == action : false; }
        // Returns true if currently doing the given state
    bool IsDesired(ACTION action) const { return desired ? desired->action == action : false; }
        // Returns true if the desired state matches the given state
    bool IsRoot(ACTION action) const { return root ? root->action == action : false; }
        // Returns true if the desired state matches the given state
    virtual bool HasActionAni(const char *name, const char *from = nullptr)
       { if (!*name) return false; 
           else if (FindState(name) >= 0) return true; 
           else return FindTransitionState(from?from:root->name, name) >= 0; }
        // Returns true if the object supports the given action with either a state
        // which matches the action, or a transition to that action from the
        // 'from' state or the root state if 'from' is nullptr.  If this function returns
        // true, the action is garanteed to have an animation to play.
    const char* GetState() { return (const char*) doing->name; }
        // return the state name that the object is doing
    virtual void Notify(int32_t notify, void *ptr);
        // Notify Action (check root,desired, and doing for deleted target obj)

  // Streaming functions
    virtual int32_t ObjVersion() { return 1; }
        // Returns the object version for this object
    virtual void Load(RTInputStream is, int32_t version, int32_t objversion);
        // Loads object data from the sector
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector


  protected:
    virtual void UpdateAction(int32_t bits = 0);
      // Called by pulse() to update the current action block
    virtual int32_t TryCommand(PTActionBlock ab, int32_t bits = 0);
      // Main command function - attempt to go to desired state
    virtual int32_t ForceCommand(PTActionBlock ab, int32_t bits = 0);
      // Called in some special cases to force a new state - be careful with this one
    virtual PTActionBlock GetRoot() { return root; }
      // Gets the current root action block
    virtual void SetRoot(PTActionBlock ab);
      // Set root to ab and and update pointers
    virtual PTActionBlock GetDoing() { return doing; }
      // Gets the current doing action block
    virtual void SetDoing(PTActionBlock ab);
      // Set doing to ab and update pointers
    virtual PTActionBlock GetDesired() { return desired; }
      // Gets the current desired action block
    virtual void SetDesired(PTActionBlock ab);
      // Set desired pointer and update pointers
    virtual bool IsFinalState() { return false; }
      // Returns whether character is in their last days

    // Resolve functions - redefine in derived classes for new or different functionality
    virtual int32_t ResolveAction(int32_t bits = 0);
      // Calls various resolve functions (very object-specific)

    virtual const char *DefaultRootState() { return "still"; }
      // Returns the default root state for this object - redefine as necessary

    // Pointers for the state changes
    PTActionBlock doing;        // Pointer to what they are currently doing
    PTActionBlock desired;      // Pointer to what they *want* to be doing
    PTActionBlock root;         // Root state (return here by default)
};

#endif