# HUD and User Interface: The Main Screen Layout

## Overview

Revenant's main game screen is divided into several distinct UI panels, each serving a specific purpose in the player's interaction with the game world. The interface follows a traditional RPG layout with status bars, inventory, and multifunction panels.

## The Main Screen Layout

### Core Game Area
- **Map Pane**: The central 2.5D isometric view where the game world is displayed
- **Full Screen Mode**: Map pane can expand to fill the entire screen
- **Windowed Mode**: Map pane is contained within a bordered window

### Status Bars (Left and Right)

**Health Bar (Left Side)**
- Position: Left edge of screen, vertical orientation
- Purpose: Shows Locke's current health as a fluid-filled tube
- Visual: Animated liquid that flows up/down based on health percentage
- Color: Red hue that changes intensity based on health level
- Range: 0-1000 internal units (0-100% display)

**Stamina Bar (Right Side)**
- Position: Right edge of screen, vertical orientation  
- Purpose: Shows Locke's current mana/stamina for spell casting
- Visual: Same tube design as health bar
- Color: Blue hue that changes based on stamina level
- Range: 0-1000 internal units (0-100% display)

### Inventory Panel (Bottom)

**Layout and Function**
- Position: Bottom center of screen
- Size: 382x85 pixels, containing 14 inventory slots (7x2 grid)
- Purpose: Displays and manages Locke's carried items
- Interaction: Drag-and-drop interface for item management

**Inventory Features**
- **Container System**: Can display inventory of any object (player, chests, corpses)
- **Slot Management**: 40x40 pixel item slots with 5-pixel spacing
- **Visual Feedback**: Items show animations and can be dragged between slots
- **Equipment Integration**: Items can be dragged to equipment pane

### Quick Spell Buttons (Above Inventory)

**Purpose and Layout**
- Position: Above inventory panel, spanning most of screen width
- Size: 437x35 pixels with 4 spell buttons
- Function: Rapid access to frequently used spells
- Design: Horizontal row of spell construction buttons

**Spell System Integration**
- **Talisman Display**: Shows talisman symbols for each spell
- **Quick Casting**: One-click spell invocation
- **Spell Construction**: Players can build spells by adding talismans
- **Persistent Storage**: Spells are saved with the character

### Text Bar (Center Bottom)

**Information Display**
- Position: Center bottom, above quick spell buttons
- Size: 198x14 pixels
- Purpose: Displays game messages, spell feedback, and status information
- Content: Combat results, spell success/failure, item descriptions

### Multifunction Panel System (Right Side)

**Control Panel (Buttons)**
- Position: Right side, vertical button strip
- Purpose: Switches between different multifunction panels
- Buttons: Equipment (1), Map (2), Spells (3), Stats (4)

**Multifunction Panes**
- **Equipment Pane**: Shows equipped items and character appearance
- **AutoMap Pane**: Displays explored areas and current location
- **Spell Pane**: Advanced spell construction and talisman management
- **Stats Pane**: Character statistics and abilities

### Dialog System

**Dialog Pane**
- Purpose: Handles NPC conversations and scripted events
- Integration: Works with the scripting system for story progression
- Display: Text-based conversations with character portraits

## UI Design Philosophy

### Information Hierarchy
1. **Critical Information**: Health/stamina always visible
2. **Frequent Actions**: Quick spells easily accessible
3. **Detailed Information**: Available through multifunction panels
4. **Contextual Information**: Text bar for immediate feedback

### Visual Consistency
- **Color Coding**: Red for health, blue for stamina, consistent UI colors
- **Animation**: Fluid animations for status bars and spell effects
- **Feedback**: Visual and text feedback for all player actions

### Accessibility
- **Keyboard Shortcuts**: Number keys (1-4) for panel switching
- **Mouse Interface**: Full drag-and-drop support
- **Visual Cues**: Clear button states and hover effects

### Panel Visibility Control

**Show/Hide Functionality**
- **Inventory Panel**: Can be hidden to reveal more game world
- **Quick spell button toggling** for vertical space
- **Multifunction pane minimizing** for gameplay focus
- **Text bar hiding** for immersive experience

**Independent UI Toggle Keys**
- **B key (default)**: Toggles the **Lower Panel** (quick spell buttons and hotkey items)
- **V key (default)**: Toggles the **entire Side Panel** (inventory, stats, spell builder, automap)
- **Space bar**: **Full Screen Mode** - Hides all UI elements simultaneously for maximum game view

**Maximizing Game View:**
- Full screen mode expansion
- Minimal UI mode with just health/stamina bars
- Contextual panel display
- Customizable layout options

**Benefits:**
- Immersive gameplay with more screen real estate
- Combat focus during intense sequences
- Better exploration experience
- Cinematic moments without UI clutter

## Technical Implementation

### Pane System
- **TPane Base Class**: All UI elements inherit from this
- **Background Areas**: Optimized rendering with background caching
- **Dirty Flag System**: Only redraws changed areas for performance

### Resource Management
- **playscrn.dat**: Main UI resource file containing backgrounds and buttons
- **intrface.dat**: Interface button graphics and controls
- **health.dat/stamina.dat**: Status bar graphics and animations

### State Management
- **Player Integration**: UI automatically updates when player changes
- **Container System**: Inventory can display any object's contents
- **Spell Persistence**: Quick spells are saved with character data

## User Experience Flow

### Typical Gameplay
1. **Combat**: Health/stamina bars show real-time status
2. **Exploration**: AutoMap tracks progress and location
3. **Inventory Management**: Drag items between slots and equipment
4. **Spell Casting**: Use quick spells or construct new ones
5. **Character Development**: Monitor stats and equipment in detail panes

### Interface Responsiveness
- **Immediate Feedback**: Text bar shows results of actions
- **Visual Updates**: Status bars animate smoothly
- **Context Switching**: Quick access to different information types

Revenant's HUD design prioritizes information accessibility while maintaining visual clarity. The multifunction panel system allows detailed information to be available without cluttering the main game view, while critical information (health, stamina) remains always visible. 