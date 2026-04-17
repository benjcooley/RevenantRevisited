# User Interface System

## Overview

Revenant features a sophisticated, modular UI system built around a hierarchical pane-based architecture. The system provides a consistent interface for both gameplay and the built-in level editor, with support for complex interactions, modal dialogs, and dynamic content management.

## Architecture

### Pane Hierarchy

The UI system is built on a foundation of `TPane` objects that can be nested and managed by `TScreen` objects:

```
TScreen
├── TPane (Base class)
    ├── TButtonPane (Button containers)
    │   ├── TMultiCtrlPane (Multifunction control)
    │   └── TStatPane (Statistics display)
    ├── TDialogPane (Conversation interface)
    ├── TInventory (Item management)
    ├── TAutoMap (Map display)
    ├── TTextBar (Status messages)
    ├── TStatusBar (Editor status)
    ├── TMapPane (Level editor map)
    └── TTextPane (Text editing)
        ├── TConsolePane (Command console)
        ├── TScriptPane (Script editor)
        └── TScrollEditorPane (Scrollable text)
```

### Core Pane System

#### TPane Base Class

All UI elements inherit from `TPane`, which provides:

```cpp
class TPane {
    // Position and size management
    int32_t x, y, width, height;
    int32_t scrollx, scrolly;
    
    // State management
    bool isopen, hidden, ignoreinput, dirty;
    
    // Virtual interface
    virtual bool Initialize();
    virtual void Close();
    virtual void Show();
    virtual void Hide();
    virtual void DrawBackground();
    virtual void Animate(bool draw);
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void MouseMove(int32_t button, int32_t x, int32_t y);
    virtual void KeyPress(int32_t key, bool down);
    virtual void CharPress(int32_t key, bool down);
};
```

**Key Features:**
- **Automatic scrolling** with scroll position management
- **Dirty flag system** for efficient redrawing
- **Input handling** with mouse and keyboard events
- **Background buffer management** for smooth animations
- **Coordinate transformation** between pane and screen coordinates

#### TScreen Management

Screens manage collections of panes and handle input routing:

```cpp
class TScreen {
    TPaneArray panes;                   // Array of managed panes
    int32_t exclusive[NUMEXCLUSIVEPANES]; // Exclusive pane system
    bool complete[NUMEXCLUSIVEPANES];   // Complete exclusion flags
    
    // Pane management
    int32_t AddPane(PTPane pane, int32_t panenum = -1);
    bool RemovePane(PTPane pane);
    bool SetExclusivePane(int32_t panenum, bool completeexclusion = false);
    
    // Input routing
    virtual void MouseClick(int32_t button, int32_t x, int32_t y);
    virtual void KeyPress(int32_t key, bool down);
};
```

## Multifunction Panel System

### TMultiCtrlPane

The multifunction control system provides access to four main game panels:

```cpp
class TMultiCtrlPane : public TButtonPane {
    int32_t curpane;                    // Currently active pane
    TMulti* buttondata;                 // Button graphics
    
    void ActivatePane(int32_t pane);    // Switch between panels
    int32_t GetActivePane();            // Get current panel
};
```

### Panel Layout

#### Button Configuration
```cpp
// Equipment Panel (Button 1)
NewButton("Equip", 30+BUTTONRADIUS, 2+BUTTONRADIUS, BUTTONRADIUS, BUTTONRADIUS, 
          '1', BtnEquip, equipdown, equipup, true, false, true, 1);

// Map Panel (Button 2)  
NewButton("Map", 2+BUTTONRADIUS, 35+BUTTONRADIUS, BUTTONRADIUS, BUTTONRADIUS,
          '2', BtnMap, mapdown, mapup, true, false, false, 1);

// Spell Panel (Button 3)
NewButton("Spell", 2+BUTTONRADIUS, 77+BUTTONRADIUS, BUTTONRADIUS, BUTTONRADIUS,
          '3', BtnSpell, spelldown, spellup, true, false, false, 1);

// Stats Panel (Button 4)
NewButton("Stat", 30+BUTTONRADIUS, 109+BUTTONRADIUS, BUTTONRADIUS, BUTTONRADIUS,
          '4', BtnStats, skilldown, skillup, true, false, true, 1);
```

#### Panel Management
- **Single active panel** at any time
- **Smooth transitions** between panels
- **State preservation** when switching
- **Keyboard shortcuts** for quick access

### Panel Types

#### 1. Equipment Panel (EquipPane)
- **Character appearance** display
- **Equipped items** visualization
- **Equipment slots** management
- **Item statistics** display

#### 2. AutoMap Panel (AutoMap)
- **Explored areas** visualization
- **Current location** indicator
- **Map annotations** and markers
- **Zoom and pan** functionality

#### 3. Spell Panel (SpellPane)
- **Talisman management** interface
- **Spell construction** tools
- **Spell library** access
- **Quick spell** assignment

#### 4. Statistics Panel (StatPane)
- **Character attributes** display
- **Combat statistics** tracking
- **Experience and level** information
- **Skill progression** visualization

## Button System

### TButton Class

Buttons provide interactive UI elements with rich functionality:

```cpp
class TButton {
    char name[NAMELEN];                 // Button identifier
    int32_t x, y, w, h;                 // Position and size
    bool radial;                        // Circular button shape
    bool toggle;                        // Toggle button behavior
    int32_t radiogroup;                 // Radio button grouping
    int32_t repeatrate;                 // Auto-repeat functionality
    PTBitmap upbitmap, downbitmap;      // Visual states
    uint16_t key;                       // Keyboard shortcut
    bool down;                          // Current state
    void (*buttonfunc)();               // Click handler
};
```

### Button Types

#### Standard Buttons
- **Click to activate** functionality
- **Visual feedback** with up/down states
- **Keyboard shortcuts** for accessibility

#### Toggle Buttons
- **State persistence** between clicks
- **Visual indication** of current state
- **Automatic state management**

#### Radio Buttons
- **Group-based selection** (only one active per group)
- **Automatic deselection** of other group members
- **Group management** functions

#### Repeat Buttons
- **Auto-repeat** functionality for continuous actions
- **Configurable repeat rate** and timing
- **Mouse and keyboard** support

### TButtonPane

Button containers provide organized button management:

```cpp
class TButtonPane : public TPane {
    TPointerArray<TButton, MAXBUTTONS> Buttons;
    int32_t clicked;                    // Last clicked button
    
    bool NewButton(PTButton b);         // Add button to container
    void ClearGroup(int32_t group);     // Clear radio group
    void CheckGroup(int32_t group);     // Initialize radio group
};
```

## Inventory System

### TInventory Pane

The inventory system provides drag-and-drop item management:

```cpp
class TInventory : public TPane {
    TObjectInstance* container;         // Current inventory owner
    int32_t grabslot;                   // Currently dragged slot
    int32_t heldslot;                   // Slot for pane transfers
    bool isdragging;                    // Drag state
    int32_t startposx, startposy;       // Drag origin
};
```

### Inventory Features

#### Drag and Drop
- **Visual feedback** during dragging
- **Slot-based** item placement
- **Cross-pane transfers** (inventory to equipment)
- **Invalid drop** handling

#### Container Management
- **Dynamic container** switching
- **Nested inventory** support
- **Owner hierarchy** traversal
- **Container-specific** behaviors

#### Slot System
- **Grid-based** item placement
- **Slot validation** and bounds checking
- **Item stacking** and management
- **Visual slot** highlighting

## Text and Console System

### TTextPane Base Class

Text panes provide rich text editing and display capabilities:

```cpp
class TTextPane : public TPane {
    // Text management
    virtual bool Input(char *string);   // Text input handling
    virtual bool Output(char *string);  // Text output display
    
    // Cursor management
    int32_t cx, cy;                     // Cursor position
    int32_t oldcx, oldcy;               // Previous cursor position
};
```

### TConsolePane

The in-game console provides direct script access:

```cpp
class TConsolePane : public TTextPane {
    // Thread management
    static unsigned _stdcall CommandThread(void *arg);
    static int32_t GetChar();
    static int32_t GetLine(char *buffer, int32_t buffersize);
    
    // Mouse chaining
    void ChainMouse(void (*cfunc)(int32_t, int32_t), ...);
    void SetupMouseBitmap(int32_t xspan, int32_t yspan);
    
    // Bounds editing
    void SetBounds();
    bool AdjustingBounds();
    bool AddAxis(TObjectInstance* inst);
    void ClearAxis();
};
```

### Console Features

#### Command Execution
- **Real-time command** processing
- **Script integration** with game objects
- **Error handling** and feedback
- **Command history** and recall

#### Thread Safety
- **Separate input thread** for responsiveness
- **Thread-safe command** execution
- **Non-blocking** input processing
- **Safe termination** handling

#### Mouse Integration
- **Mouse chaining** for value adjustment
- **Visual feedback** during adjustments
- **Bounds editing** for object placement
- **Axis visualization** for positioning

### TScriptPane

Script editing with syntax highlighting and validation:

```cpp
class TScriptPane : public TTextPane {
    // Script-specific features
    void SetScript(TScript* script);    // Load script for editing
    void ValidateScript();              // Syntax validation
    void FormatScript();                // Code formatting
};
```

## Status and Information Display

### TTextBar

Real-time status and feedback display:

```cpp
class TTextBar : public TPane {
    // Message management
    void SetText(char *text);           // Set display text
    void ClearText();                   // Clear display
    void ScrollText();                  // Text scrolling
    
    // Animation
    void AnimateText();                 // Text animation effects
    void FadeText();                    // Fade in/out effects
};
```

### TStatusBar

Editor status information and controls:

```cpp
class TStatusBar : public TPane {
    // Status display
    void SetStatus(char *status);       // Set status message
    void ShowCoordinates(int32_t x, int32_t y, int32_t z); // Position display
    void ShowSelection(int32_t count);  // Selection count
    
    // Editor controls
    void Delete();                      // Delete selected objects
    void Undo();                        // Undo last action
    void Redo();                        // Redo last action
};
```

## Map and Level Editor UI

### TMapPane

Level editor map display and interaction:

```cpp
class TMapPane : public TPane {
    // Map display
    void SetZoom(float zoom);           // Zoom level control
    void SetPan(int32_t x, int32_t y);  // Pan position
    void CenterOnObject(TObjectInstance* obj); // Center view
    
    // Selection
    void SelectObjects(SRect rect);     // Rectangle selection
    void SelectObject(TObjectInstance* obj); // Single selection
    void ClearSelection();              // Clear all selections
};
```

### TAutoMap

Game automap display and navigation:

```cpp
class TAutoMap : public TPane {
    // Map data
    bool explored[MAXSECTORS];          // Explored area tracking
    S3DPoint playerpos;                 // Player position
    
    // Display options
    void SetShowGrid(bool show);        // Grid display toggle
    void SetShowObjects(bool show);     // Object display toggle
    void SetShowAnnotations(bool show); // Annotation display
};
```

## Input Handling

### Event Routing

The UI system provides sophisticated input routing:

```cpp
// Screen-level input routing
void TScreen::MouseClick(int32_t button, int32_t x, int32_t y) {
    // Find pane under cursor
    PTPane pane = FindPaneAt(x, y);
    
    // Convert to pane coordinates
    int32_t panex = x - pane->GetPosX();
    int32_t paney = y - pane->GetPosY();
    
    // Route to pane handler
    pane->MouseClick(button, panex, paney);
}
```

### Input Priorities

1. **Exclusive panes** get first priority
2. **Modal dialogs** block other input
3. **Active panes** receive input before hidden ones
4. **Pane hierarchy** determines input order

### Mouse Handling

#### Click Detection
- **Button hit testing** with pixel-perfect accuracy
- **Drag and drop** with visual feedback
- **Multi-button** support (left, right, middle)
- **Click state** tracking (down, up, double-click)

#### Movement Tracking
- **Hover effects** for interactive elements
- **Drag operations** with bounds checking
- **Cursor changes** for different contexts
- **Smooth tracking** with interpolation

### Keyboard Handling

#### Key Events
- **Raw key events** (KeyPress) for game controls
- **Processed events** (CharPress) for text input
- **Modifier key** support (Ctrl, Alt, Shift)
- **Key repeat** handling

#### Shortcuts
- **Global shortcuts** for common actions
- **Pane-specific** shortcuts for local functions
- **Context-sensitive** key handling
- **Shortcut conflicts** resolution

## Visual System

### Rendering Pipeline

```cpp
void TScreen::TimerTick(bool draw) {
    if (draw) {
        // Background rendering
        DrawBackground();
        
        // Pane background rendering
        for (TPaneIterator i(panes); i; ++i) {
            if (!i.Item()->IsHidden()) {
                i.Item()->DrawBackground();
            }
        }
        
        // 3D world rendering
        Animate(true);
        
        // Pane overlay rendering
        for (TPaneIterator i(panes); i; ++i) {
            if (!i.Item()->IsHidden()) {
                i.Item()->Animate(true);
            }
        }
        
        // Final overlay
        Overlay(true);
    }
}
```

### Background Management

#### Background Buffers
- **Automatic allocation** for scrolling panes
- **Efficient restoration** for animations
- **Memory management** for large displays
- **Buffer sharing** between similar panes

#### Scrolling Support
- **Smooth scrolling** with interpolation
- **Scroll position** management
- **Background restoration** during scroll
- **Scroll bounds** enforcement

### Animation System

#### Frame-Based Animation
- **60 FPS** target for smooth animation
- **Frame skipping** for performance
- **Animation timing** synchronization
- **Interpolation** for smooth transitions

#### Visual Effects
- **Fade in/out** effects for panes
- **Slide animations** for transitions
- **Pulse effects** for attention
- **Color cycling** for status indicators

## Performance Optimization

### Dirty Flag System

```cpp
class TPane {
    bool dirty;                         // Needs redraw flag
    
    void SetDirty(bool newdirty) { dirty = newdirty; }
    bool IsDirty() { return dirty; }
    
    virtual void Update() { dirty = true; }
};
```

### Efficient Redrawing
- **Dirty flag** tracking for minimal redraws
- **Rectangle clipping** for partial updates
- **Background caching** for static elements
- **Layered rendering** for complex interfaces

### Memory Management
- **Shared resources** between similar panes
- **Automatic cleanup** of unused resources
- **Resource pooling** for frequently used elements
- **Memory-efficient** bitmap storage

## UI Resource System

### TMulti Resource System

The `TMulti` class provides a unified interface for accessing UI resources:

```cpp
class TMulti : public TMultiData {
    // Resource loading
    static TMulti* LoadMulti(char *name);
    static TMulti* LoadMulti(char *name, int32_t id);
    
    // Resource access by name
    void *Object(char *name);
    PTBitmap Bitmap(char *name);
    PTAnimation Animation(char *name);
    TFont* Font(char *name);
    PTWaveData Wave(char *name);
    
    // Resource access by index
    void *Object(int32_t i);
    PTBitmap Bitmap(int32_t i);
    PTAnimation Animation(int32_t i);
    TFont* Font(int32_t i);
};
```

### Resource File Structure
```cpp
class TMultiData {
    int32_t numoffsets;                     // Number of resources
    OFFSET names[MAXMULTIOFFSETS];      // Resource name offsets
    OFFSET offsets[MAXMULTIOFFSETS];    // Resource data offsets
};
```

### Core UI Resources
- **playscrn.dat** - Main play screen interface graphics
- **intrface.dat** - Interface button graphics and controls
- **dialog.dat** - Dialog box backgrounds and elements
- **equipscr.dat** - Equipment screen graphics
- **editor.dat** - Level editor interface graphics
- **health.dat** - Health bar graphics
- **stamina.dat** - Stamina bar graphics
- **death.dat** - Death screen graphics
- **scroll.dat** - Scroll and book interface graphics
- **book.dat** - Book reading interface graphics

## Integration with Game Systems

### Game State Integration
- **Real-time updates** from game objects
- **Event-driven** UI updates
- **State synchronization** between UI and game
- **Performance monitoring** and optimization

### Script Integration
- **UI callbacks** from script system
- **Dynamic content** generation
- **Event handling** for script triggers
- **Debug interface** for script development

### Editor Integration
- **Live editing** capabilities
- **Real-time preview** of changes
- **Undo/redo** system integration
- **Multi-user** editing support 