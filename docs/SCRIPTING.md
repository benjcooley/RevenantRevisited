# Scripting System and Console

## Overview

Revenant features a sophisticated continuation-based scripting system that allows objects to execute complex behaviors while maintaining their execution state. Scripts can pause execution, wait for conditions, and resume from where they left off, making them ideal for creating dynamic, responsive game world interactions.

## Continuation-Based Scripting

### Key Concept: Script Continuation

Unlike traditional scripting systems that execute commands sequentially and terminate, Revenant's scripts are **continuation-based**, meaning:

- **State Preservation**: Scripts maintain their execution state (instruction pointer, variables, call stack)
- **Pause and Resume**: Scripts can pause execution and resume later from the exact same point
- **Non-Blocking**: Scripts don't block the game engine while waiting
- **Concurrent Execution**: Multiple scripts can run simultaneously with different pause states

### Script Execution Model

```cpp
class TScript {
    char *ip;                    // Instruction pointer (next line to execute)
    int32_t priority;            // Current execution priority
    SScriptBlock block[MAXDEPTH]; // Call stack for conditionals/loops
    int32_t depth;               // Current stack depth
    bool IsPaused() { return (priority & SCRIPT_PAUSED); }
};
```

## Script Structure

### Script Prototypes

Scripts are defined as prototypes that can be instantiated for specific objects:

```cpp
class TScriptProto {
    char *name;                  // Script name/criteria
    char *text;                  // Script source code
    TTriggerArray triggers;      // Trigger definitions
    int32_t numtriggers;         // Number of triggers
};
```

### Trigger System

Scripts are activated through various trigger types:

```cpp
#define TRIGGER_NONE      0
#define TRIGGER_ALWAYS    1     // Always running script
#define TRIGGER_TRIGGER   2     // Manual trigger (trigger command)
#define TRIGGER_DIALOG    3     // Dialog trigger (character clicked)
#define TRIGGER_PROXIMITY 4     // Proximity trigger (character in range)
#define TRIGGER_CUBE      5     // Cube trigger (object enters area)
#define TRIGGER_ACTIVATE  6     // Object activation trigger
#define TRIGGER_USE       7     // Use object trigger
#define TRIGGER_GIVE      8     // Give item trigger
#define TRIGGER_GET       9     // Get item trigger
#define TRIGGER_COMBAT    10    // Combat mode trigger
#define TRIGGER_DEAD      11    // Death trigger
```

## Script Language

### Basic Syntax

Scripts use a simple command-based language with object-oriented syntax:

```
// Object context commands
<object>.say "Hello world"
<object>.go 45
<object>.face player

// Global commands
set flag = 1
if condition
    begin
        <object>.say "Condition is true"
    end
```

### Control Flow

#### Conditionals
```
if <condition>
    begin
        <command block>
    end
else
    begin
        <alternate block>
    end
```

#### Loops
```
while <condition>
    begin
        <command block>
    end
```

#### Labels and Jumps
```
jump <label>
<label>:
    <commands>
```

### Wait Commands

The core of the continuation system - commands that pause script execution:

#### Time-Based Waits
```
wait 100        // Wait 100 frames
```

#### Event-Based Waits
```
wait response   // Wait for dialog response
wait char <name> // Wait for specific character
```

#### Return Values
Wait commands return `CMD_WAIT`, signaling the script system to pause execution and resume later.

## Script Execution Engine

### Execution Loop

```cpp
void TScript::Continue(TObjectInstance* context)
{
    if (IsPaused() || pauseall)
        return;
        
    TStringParseStream s(ip, strlen(ip));
    TToken t(s);
    
    int32_t retval = CommandInterpreter(context, t, MINCMDABREV);
    
    if (retval == CMD_WAIT) {
        // Script pauses here, will resume later
        priority |= SCRIPT_PAUSED;
        return;
    }
    
    // Update instruction pointer for next execution
    ip = s.GetPos();
}
```

### Priority System

Scripts use a priority system to manage execution order:

- **Higher priority** scripts execute first
- **Same priority** scripts execute in round-robin fashion
- **Paused scripts** maintain their priority for resumption

### Script Manager

The `TScriptManager` coordinates all script execution:

```cpp
class TScriptManager {
    void ContinueAllScripts();   // Execute all active scripts
    void PauseAllScripts();      // Pause all scripts globally
    void ResumeAllScripts();     // Resume all paused scripts
    PTScript ObjectScript(TObjectInstance* inst); // Get object's script
};
```

## Game State System

### Global State Variables

Scripts can access and modify global game state:

```cpp
// Set global state
set quest_completed = 1
set door_unlocked = true

// Read global state
if quest_completed == 1
    begin
        <object>.say "Quest is complete!"
    end
```

### Local Script Variables

Scripts can use local variables for temporary storage:

```cpp
// Local variables are automatically managed
set local_counter = 0
while local_counter < 5
    begin
        <object>.say "Count: " + local_counter
        set local_counter = local_counter + 1
        wait 30
    end
```

## In-Game Console

### Console Interface

The in-game console provides direct access to the scripting system:

```cpp
class TConsolePane {
    bool Initialize();
    void Show();
    void Hide();
    bool Output(char *string);
    bool Input(char *string);
    void KeyPress(int32_t key, bool down);
};
```

### Console Thread

The console runs in a separate thread for responsive input:

```cpp
unsigned _stdcall TConsolePane::CommandThread(void *arg)
{
    char tmpbuf[256];
    
    while (console->GetLine(tmpbuf, 256)) {
        TStringParseStream s(tmpbuf, strlen(tmpbuf));
        TToken t(s);
        
        // Execute command with current context
        CommandInterpreter(context, t, MINCMDABREV);
        
        console->Output(PROMPT);
    }
}
```

### Command Context

Commands can be executed in different contexts:

#### Global Context
```
say "Hello"           // No specific object context
set flag = 1          // Global state modification
```

#### Object Context
```
<object>.say "Hello"  // Specific object context
<object>.go 45        // Object-specific command
```

#### Selection Context
When objects are selected, commands apply to all selected objects:
```
select guard1
select guard2
say "Attention!"      // Both guards speak
```

## Command Categories

### Object Manipulation
- `add` - Create new objects
- `delete` - Remove objects
- `move` - Move objects
- `pos` - Set object position
- `rotate` - Rotate objects
- `state` - Change object state
- `frame` - Set animation frame

### Character Control
- `say` - Character speaks
- `go` - Move in direction
- `goto` - Move to position
- `face` - Face direction
- `combat` - Enter/exit combat
- `attack` - Attack target
- `block` - Defensive stance
- `cast` - Cast spell

### Script Control
- `begin/end` - Code blocks
- `if/else` - Conditionals
- `while` - Loops
- `jump` - Goto label
- `set` - Set variables
- `wait` - Pause execution
- `trigger` - Manual trigger

### Game State
- `set` - Global state variables
- `get` - Get object properties
- `stat` - Object statistics
- `visible` - Object visibility
- `level` - Change game level

### Editor Commands
- `select` - Select objects
- `save` - Save game/map
- `load` - Load game/map
- `script` - Edit object script
- `help` - Command help

## Illustrative Examples

### The Persistent Guard: A Continuation Story

Imagine a guard named Marcus stationed at the castle gate. His script runs continuously, but he doesn't just patrol mindlessly - he responds to events and maintains his state throughout the day.

**Marcus's Script:**
```
begin
    set patrol_phase = 0
    
    while 1
        begin
            if patrol_phase == 0
                begin
                    say "Time to start my patrol..."
                    go 0
                    wait 180
                    set patrol_phase = 1
                end
            
            if patrol_phase == 1
                begin
                    say "Checking the north tower..."
                    go 90
                    wait 120
                    set patrol_phase = 2
                end
            
            if patrol_phase == 2
                begin
                    say "East gate looks secure..."
                    go 180
                    wait 120
                    set patrol_phase = 3
                end
            
            if patrol_phase == 3
                begin
                    say "Back to my post..."
                    go 270
                    wait 120
                    set patrol_phase = 0
                end
        end
end
```

**What happens when the player approaches Marcus:**

1. **Frame 0**: Marcus is at `patrol_phase = 2`, saying "East gate looks secure..."
2. **Frame 1**: Player clicks on Marcus, triggering a dialog script
3. **Frame 2**: Marcus's patrol script **pauses** at the `wait 120` command
4. **Frame 3**: Dialog script runs: Marcus says "Halt! Who goes there?"
5. **Frame 4**: Player responds, dialog ends
6. **Frame 5**: Marcus's patrol script **resumes** exactly where it left off - still waiting for the remaining 118 frames
7. **Frame 120**: Marcus continues: "Back to my post..." and completes his patrol cycle

The key insight: Marcus never "forgets" where he was in his patrol. His script state is preserved through the interruption.

### The Living Door: Event-Driven Continuation

Consider a magical door that responds to proximity and maintains its state:

**Door Script:**
```
begin
    set door_state = 0    // 0=closed, 1=opening, 2=open, 3=closing
    
    while 1
        begin
            if door_state == 0
                begin
                    // Door is closed, waiting for someone to approach
                    wait 1
                end
            
            if door_state == 1
                begin
                    say "The door begins to open..."
                    state 1
                    wait 60
                    say "The door is now open"
                    set door_state = 2
                end
            
            if door_state == 2
                begin
                    // Door is open, waiting before closing
                    wait 300
                    say "The door begins to close..."
                    set door_state = 3
                end
            
            if door_state == 3
                begin
                    state 0
                    wait 60
                    say "The door is closed"
                    set door_state = 0
                end
        end
end
```

**Proximity Trigger Script:**
```
if player_in_range
    begin
        if door_state == 0
            begin
                set door_state = 1
                trigger "door_opening"
            end
    end
```

**The Narrative:**
- The door starts closed (`door_state = 0`)
- Player approaches, triggering the proximity script
- Door state changes to 1 (opening)
- Main door script detects the state change and begins opening animation
- Door script pauses during the 60-frame opening animation
- Meanwhile, other scripts continue running (player movement, other objects)
- After 60 frames, door script resumes and sets state to 2 (open)
- Door waits 300 frames, then begins closing process
- The cycle continues, with the door maintaining its state through all interruptions

### The Merchant's Tale: Complex State Management

Meet Sarah, a merchant with a complex inventory system and conversation tree:

**Sarah's Main Script:**
```
begin
    set items_in_stock = 10
    set conversation_phase = 0
    set player_greeted = 0
    
    while items_in_stock > 0
        begin
            if player_nearby && player_greeted == 0
                begin
                    say "Welcome to my shop, traveler!"
                    set player_greeted = 1
                    set conversation_phase = 1
                    wait response
                end
            
            if conversation_phase == 1
                begin
                    say "I have " + items_in_stock + " fine swords for sale."
                    say "Would you like to buy one for 50 gold?"
                    set conversation_phase = 2
                    wait response
                end
            
            if conversation_phase == 2
                begin
                    if player_choice == "yes"
                        begin
                            if player_gold >= 50
                                begin
                                    say "Excellent choice! Here's your sword."
                                    give player "fine_sword"
                                    take player 50
                                    set items_in_stock = items_in_stock - 1
                                    set conversation_phase = 3
                                end
                            else
                                begin
                                    say "I'm sorry, but you don't have enough gold."
                                    set conversation_phase = 4
                                end
                        end
                    else
                        begin
                            say "Perhaps another time then."
                            set conversation_phase = 4
                        end
                    
                    wait response
                end
            
            if conversation_phase == 3
                begin
                    say "Thank you for your purchase! Come back soon!"
                    set conversation_phase = 0
                    set player_greeted = 0
                    wait 180
                end
            
            if conversation_phase == 4
                begin
                    say "Good day to you."
                    set conversation_phase = 0
                    set player_greeted = 0
                    wait 180
                end
            
            wait 30
        end
    
    say "I'm all out of stock! Come back tomorrow!"
end
```

**The Story Unfolds:**
1. **Initial State**: Sarah has 10 swords, no conversation active
2. **Player Approaches**: Sarah greets them and pauses for response
3. **Player Responds**: Sarah offers a sword and pauses again
4. **Player Buys**: Sarah processes the transaction, updates inventory, and thanks them
5. **State Preservation**: Throughout this entire conversation, Sarah's script maintains:
   - Her inventory count
   - Which phase of conversation she's in
   - Whether she's already greeted the player
   - Her exact position in the script execution

**What makes this powerful:**
- If the player walks away mid-conversation, Sarah's script pauses but remembers everything
- If the player returns later, Sarah can continue from where they left off
- Multiple players could interact with Sarah simultaneously, each with their own conversation state
- Sarah's inventory is persistent and affects her behavior

### The Quest System: Global State Continuation

A quest system that tracks multiple objectives and maintains state across the entire game:

**Quest Manager Script:**
```
begin
    // Initialize quest states
    set quest_sword_found = 0
    set quest_guard_spoken = 0
    set quest_door_unlocked = 0
    set quest_completed = 0
    
    while quest_completed == 0
        begin
            if quest_sword_found == 1 && quest_guard_spoken == 1 && quest_door_unlocked == 1
                begin
                    say "You have completed the ancient quest!"
                    give player "quest_reward"
                    set quest_completed = 1
                    trigger "quest_finished"
                end
            
            wait 60
        end
end
```

**Sword Finding Script (attached to sword object):**
```
if player_touches
    begin
        if quest_sword_found == 0
            begin
                say "You found the ancient sword!"
                set quest_sword_found = 1
                trigger "sword_found"
            end
    end
```

**Guard Conversation Script:**
```
if player_clicks
    begin
        if quest_sword_found == 1
            begin
                say "Ah, you have the sword! The door code is 'Ancient'."
                set quest_guard_spoken = 1
                trigger "guard_informed"
            end
        else
            begin
                say "Find the ancient sword first, then return to me."
            end
    end
```

**Door Unlocking Script:**
```
if player_says "Ancient"
    begin
        if quest_guard_spoken == 1
            begin
                say "The door unlocks with a magical glow!"
                set quest_door_unlocked = 1
                state 1
                trigger "door_unlocked"
            end
    end
```

**The Quest Narrative:**
1. **Player starts**: All quest flags are 0, quest manager waits
2. **Player finds sword**: Sword script sets `quest_sword_found = 1`
3. **Player talks to guard**: Guard checks if sword is found, then provides door code
4. **Player unlocks door**: Door checks if guard was spoken to, then unlocks
5. **Quest completion**: Quest manager detects all flags are set and awards completion

**Continuation Benefits:**
- Player can save/load the game at any point, and all quest progress is preserved
- Multiple quests can run simultaneously without interfering with each other
- Quest state persists even if the player leaves the area and returns later
- Scripts can be interrupted (combat, other events) and resume with correct state

## Script Examples

### Simple NPC Behavior
```
// Guard patrol script
begin
    while 1
        begin
            say "Patrolling..."
            go 0
            wait 120
            go 90
            wait 120
            go 180
            wait 120
            go 270
            wait 120
        end
end
```

### Interactive Door
```
// Door script with proximity trigger
if player_in_range
    begin
        say "Door opens"
        state 1
        wait 60
        state 0
        say "Door closes"
    end
```

### Quest System
```
// Quest completion script
if player_has_key
    begin
        set quest_completed = 1
        say "You have completed the quest!"
        give player "reward_sword"
        trigger "quest_done"
    end
```

### Complex Behavior
```
// Merchant with inventory management
begin
    set items_in_stock = 10
    
    while items_in_stock > 0
        begin
            if player_nearby
                begin
                    say "Welcome! I have " + items_in_stock + " items."
                    wait response
                    
                    if player_buys
                        begin
                            set items_in_stock = items_in_stock - 1
                            give player "merchant_item"
                            say "Thank you for your purchase!"
                        end
                end
            
            wait 30
        end
    
    say "I'm all out of stock!"
end
```

## Advanced Features

### Script Inheritance

Scripts can inherit from parent prototypes, allowing for shared behavior:

```cpp
TScriptProto* parent = GetParentScript();
if (parent) {
    // Inherit triggers and behavior
    triggers = parent->triggers;
}
```

### Dynamic Script Loading

Scripts can be loaded and modified at runtime:

```cpp
// Load script from file
ScriptManager.Load("scripts/merchant.scr", merchant_object);

// Modify script text
merchant_object->GetScript()->SetText(new_script_text);
```

### Script Debugging

The console provides debugging capabilities:

```cpp
// View script state
<object>.script          // Open script editor
<object>.getstate        // Get current state
<object>.stat health     // Get object statistics
```

## Performance Considerations

### Efficient Scripting

- **Continuation-based execution** prevents blocking
- **Priority system** ensures important scripts run first
- **Pause/resume** allows for complex behaviors without performance impact
- **Event-driven triggers** reduce unnecessary script execution

### Memory Management

- Scripts are lightweight, containing only instruction pointers
- Script prototypes are shared between instances
- Local variables are automatically managed
- Paused scripts don't consume significant resources

## Integration with Game Systems

### Object System Integration
- Scripts are attached to object instances
- Scripts can access all object properties and methods
- Object events can trigger script execution

### Save Game Integration
- Script state is preserved in save games
- Paused scripts resume correctly after loading
- Global state variables are saved and restored

### Map System Integration
- Scripts can modify map objects and sectors
- Proximity and cube triggers work with map geometry
- Scripts can spawn and manage map objects

This continuation-based scripting system provides Revenant with a powerful and flexible way to create dynamic, interactive game worlds while maintaining excellent performance and responsiveness. 