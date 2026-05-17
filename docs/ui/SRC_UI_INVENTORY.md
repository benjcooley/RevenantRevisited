# SRC_UI_INVENTORY — Revenant Revisited UI Code Audit

**Status:** Phase B Inventory (pre-release snapshot analysis)  
**Purpose:** Characterize all UI-related source files and flag divergence from retail.  
**Generated:** 2026-05-16

---

## Overview

This inventory catalogs every UI-related class in `src/` for the Revenant Revisited reconstruction port. The pre-release snapshot has drifted significantly from the shipped retail game. **Phase B work incorrectly assumed standalone TStatusBar / THealthBar / TStaminaBar classes existed in retail; they do not.** This audit supports Phase C forensic recovery by identifying which classes are likely retail-compatible vs. pre-release-only.

The retail HUD is described as: two combined health/mana/character panels (upper-left + upper-right), full right-side multifunction panel, bottom quickspell+shelf area, transparent game-log overlay, conditional dialog/book/scroll overlays, and small bottom-right button bar. The current src/ does not clearly map to this structure.

---

## Section 1: Core Framework (Substrate — Carry Forward)

### TPane (Screen Pane Object)
- **Path:** `src/screen.h` (517 lines), `src/screen.cpp` (1014 lines)
- **Class hierarchy:** Root pane class; no parent
- **Stated purpose** (from `screen.h:74-79`): "The pane object creates a virtual view pane on a game screen. The view pane has its own origin, its own clipping rectangle, and handles its own mouse movement, clicks, joystick, and keyboard input."
- **Public surface:**
  - Lifecycle: `Initialize()`, `Close()`
  - Geometry: `GetPosX/Y()`, `GetWidth/Height()`, `Resize()`, `SetPos()`, `SetSize()`, `WasResized()`
  - Visibility/state: `Show()`, `Hide()`, `IsHidden()`, `IsDirty()`, `SetDirty()`
  - Input: `MouseClick()`, `MouseMove()`, `KeyPress()`, `CharPress()`, `Joystick()`
  - Rendering: `PutToScreen()`, `Draw()`, `Animate()`, `DrawBackground()`, `Overlay()`
  - Scrolling: `SetScrollPos()`, `GetScrollX/Y()`, `GetNewScrollX/Y()`
  - Retained-mode hierarchy (A.2a): `AddChild()`, `RemoveChild()`, `GetParent()`, `Children()`
  - Layout (A.2b): `SetLayoutKind()`, `SetSizePolicy()`, `SetPadding()`, `SetMargin()`, `SetSpacing()`, `SetAnchor()`, `MeasureSelf()`, `LayoutChildren()`, `RunLayoutPass()`
  - Clip rect (A.2f): `SetClipRect()`, `ClearClipRect()`, `HasClipRect()`, `GetClipRect()`
  - Alpha fade-clip (A.2g): `SetClipFade()`, `GetClipFade()`
  - Canvas resize (A.2h): `OnCanvasResize()`
- **Render hooks:** None in TPane itself; subclasses override `DrawBackground()` and `Animate()`. Legacy CPU surface via `Display.Box()`, `Display.WriteText()` in subclasses; new path via `Renderer->DrawBitmap()` not yet wired.
- **Global instances:** None directly; instance pointers stored in `TScreen::panes[]` array.
- **State of Initialize:** Returns `true` (stub; subclasses override). Requires no external dependencies for base functionality.
- **Retail correspondence:** **Extremely likely.** TPane is foundational and matches the described retained-mode architecture in ARCHITECTURE.md. Pre-release layout system (A.2b/A.2f/A.2g) adds modern flexbox/layout concepts not in retail, but the vanilla-pane fallback (Fixed sizing, None layout, explicit x/y) preserves the 1998 original.

---

### TScreen (Game Screen Object)
- **Path:** `src/screen.h` (517 lines), `src/screen.cpp` (1014 lines)
- **Class hierarchy:** Root screen manager; no parent
- **Stated purpose** (from `screen.h:381-384`): "The TScreen object represents a game screen. Each game screen can access the display surface and handle input directly, or it can have multiple TPane objects which handle various parts of the screen."
- **Public surface:**
  - Lifecycle: `Initialize()`, `Close()`
  - Pane management: `FindPane()`, `AddPane()`, `RemovePane()`, `SetExclusivePane()`, `ReleaseExclusivePane()`, `InCompleteExclusion()`, `RedrawAllPanes()`
  - Frame control: `Tick()`, `DrawFrame()`, `TimerTick()` (legacy deprecated), `Pulse()`, `Animate()`, `DrawBackground()`
  - Input: `MouseClick()`, `MouseMove()`, `KeyPress()`, `CharPress()`, `Joystick()`
  - Lifecycle hooks: `ShowScreen()` (static), `EndCurrentScreen()` (static), `IsDone()`, `SetDone()`
  - Frame count: `FrameCount()`, `ResetFrameCount()`
  - Canvas resize (A.2h): `OnCanvasResize()`
- **Render hooks:** `DrawBackground()`, `Animate()`, `Pulse()` per-pane dispatch. New frame pipeline: `Tick()` (sim only), `DrawFrame()` (render + overlay composite).
- **Global instances:** `CurrentScreen` (managed via `ShowScreen()` / `EndCurrentScreen()`).
- **State of Initialize:** Abstract base returns `false`. Subclasses override. No dependencies on Player/GameData for the base class.
- **Retail correspondence:** **Very likely.** TScreen mirrors the described architecture. Frame pipeline migration (Tick/DrawFrame vs. legacy TimerTick) is port-specific, not retail-drift.

---

### TButton (Standalone Button Class)
- **Path:** `src/button.h` (128 lines), `src/button.cpp` (337 lines)
- **Class hierarchy:** Standalone; not a TPane
- **Stated purpose** (from `button.h:23-29`): Button management with state, visual feedback (up/down bitmaps), hotkeys, toggle/radio groups, and repeat rates.
- **Public surface:**
  - State: `SetState()`, `GetState()`, `Invert()`, `IsToggle()`, `RadioGroup()`, `Repeats()`
  - Visual: `SetUpBitmap()`, `SetDownBitmap()`, `SetLevel()`, `Draw()`, `Animate()`
  - Input: `OnButton()` (hit test), `IsKey()` (hotkey match)
  - Visibility: `Show()`, `Hide()`, `IsHidden()`
  - Callback: `ButtonFunc()` (execute)
- **Render hooks:** `Draw()` calls legacy `PTBitmap::Draw()` (CPU surface or software renderer). No modern path yet.
- **Global instances:** None; instances collected in `TButtonPane::Buttons[]` array.
- **State of Initialize:** No Initialize; construction-time setup only.
- **Retail correspondence:** **Very likely.** Simple, stateful button primitive matching 1998 UI patterns.

---

### TButtonPane (Pane Container for Buttons)
- **Path:** `src/button.h` (128 lines), `src/button.cpp` (337 lines)
- **Class hierarchy:** `TButtonPane : public TPane`
- **Stated purpose:** Container and input dispatcher for a set of TButton instances; handles mouse/keyboard dispatch to buttons.
- **Public surface:**
  - Button management: `NewButton()`, `Button()`, `ClearGroup()`, `CheckGroup()`, `RedrawButtons()`
  - Input dispatch: `MouseClick()`, `MouseMove()`, `KeyPress()`
  - Rendering: `DrawBackground()`, `Animate()`
- **Render hooks:** Iterates buttons and calls `TButton::Draw()` (CPU surface).
- **Global instances:** None directly; subclassed by specialized panes (TSpellPane, TEquipPane, TStatPane, TDeathPane, TQuickSpellPane, TMultiCtrlPane, TScrollPane, TBookPane).
- **State of Initialize:** Base impl provided; subclasses may override.
- **Retail correspondence:** **Likely.** Pattern matches radio/toggle button container common in '90s UIs.

---

## Section 2: HUD Components (In-Game Overlays)

### THealthBar (Health Status Bar)
- **Path:** `src/statusbar.h` (57 lines), `src/statusbar.cpp` (109 lines)
- **Class hierarchy:** `THealthBar : public TStatusBar : public TPane`
- **Stated purpose:** Animated health indicator with level-based coloration (hue tint).
- **Public surface:**
  - Inherits from TStatusBar: `GetLevel()`, `SetLevel()`, `ChangeLevel()`, `GetHue()`
  - Pane inherited: standard lifecycle/rendering
- **Render hooks:** `DrawBackground()` (via TStatusBar) uses legacy `Display.PutHue()` + `Display.Put()` to composite tube bitmap with overlays.
- **Global instances:** `extern THealthBar HealthBar;` (revenant.h:215). Used by gameplay code to update player health display. Grepped in: playscreen.cpp (no direct refs observed), textbar.cpp (health display), character.cpp (damage events).
- **State of Initialize:** `Initialize()` calls `TStatusBar::Initialize()`, then loads `tubedata = TMulti::LoadMulti("health.dat")`. Succeeds standalone if asset exists.
- **Retail correspondence:** **Flagged as suspect.** Position hardcoded to `HEALTHBARX` (16), `HEALTHBARY` (pre-release define, not retail-verified). Standalone bar is a pre-release architectural choice; retail may have combined health/mana into a single character panel. Phase B Phase B incorrectly built assuming this class exists in retail.
- **Size comparison:** src/statusbar.cpp 109 lines vs. no recon decomp found.

---

### TStaminaBar (Stamina/Fatigue Status Bar)
- **Path:** `src/statusbar.h` (57 lines), `src/statusbar.cpp` (109 lines)
- **Class hierarchy:** `TStaminaBar : public TStatusBar : public TPane`
- **Stated purpose:** Animated stamina (fatigue) indicator with complementary hue tint.
- **Public surface:** Identical to THealthBar; inherits from TStatusBar.
- **Render hooks:** `DrawBackground()` uses legacy `Display.PutHue()` + `Display.Put()` to animate stamina tube.
- **Global instances:** `extern TStaminaBar StaminaBar;` (revenant.h:216). No obvious in-tree refs for direct updates.
- **State of Initialize:** `Initialize()` calls `TStatusBar::Initialize()`, then loads `tubedata = TMulti::LoadMulti("stamina.dat")`.
- **Retail correspondence:** **Flagged as suspect.** Same concerns as THealthBar; standalone stamina bar is a pre-release design choice. Retail may have combined or positioned differently.
- **Size comparison:** src/statusbar.cpp 109 lines vs. no recon decomp found.

---

### TStatusBar (Base Status Bar Class)
- **Path:** `src/statusbar.h` (57 lines), `src/statusbar.cpp` (109 lines)
- **Class hierarchy:** `TStatusBar : public TPane`
- **Stated purpose:** Abstract base for any animated level-bar (health, stamina, mana, etc.); handles smooth interpolation and hue-based coloring.
- **Public surface:**
  - Lifecycle: `Initialize()`, `Close()`, `DrawBackground()`
  - Level control: `GetLevel()`, `SetLevel()`, `ChangeLevel()`
  - Color: `GetHue()` (overridable by subclass)
- **Render hooks:** `DrawBackground()` interpolates level toward targetlevel, then composites tube + overlays via legacy Display calls.
- **Global instances:** None directly; subclassed only by THealthBar and TStaminaBar.
- **State of Initialize:** Base stub; sets defaults, awaits subclass to load `tubedata`.
- **Retail correspondence:** **Flagged as suspect.** No equivalent found in recon. Class design assumes independent bar panes; retail may have unified character panel.

---

### TTextBar (Text Display Bar)
- **Path:** `src/textbar.h` (50 lines), `src/textbar.cpp` (127 lines)
- **Class hierarchy:** `TTextBar : public TPane`
- **Stated purpose** (from `textbar.h:12-18`): "This is the lame little text bar that goes just under the map… tells the player completely useless information like, 'The door is now open' or 'Got the Hammer of Wounding'… It also now does something kind of useful - displaying the name and health of the creature Locke is fighting when there is nothing else to print."
- **Public surface:**
  - Text: `Print(fmt, ...)`, `Clear()`
  - Opponent health: `SetHealthDisplay()`, `ClearHealthDisplay()`, `SetLevels()`
- **Render hooks:** `DrawBackground()` uses legacy Display calls to render text and optional health bar snippet.
- **Global instances:** None declared; likely instantiated in TPlayScreen or managed pane.
- **State of Initialize:** `Initialize()` sets up defaults; `Close()` cleans up.
- **Retail correspondence:** **Likely.** Feature set (floating text status line + target health) matches described "game-log" and in-game feedback.

---

### TCursorHud (Cursor Drawing Overlay)
- **Path:** `src/cursor.h` (42 lines), `src/cursor.cpp` (214 lines)
- **Class hierarchy:** `TCursorHud : public THudDrawable` (not a TPane)
- **Stated purpose:** Modern renderer HUD drawable that composites the current mouse cursor bitmap and shadow at (cursorx, cursory).
- **Public surface:**
  - `Draw()` override; paints cursor + shadow to current swapchain overlay2d pass
- **Render hooks:** New path via `Renderer->DrawBitmap()` calls in `Draw()`.
- **Global instances:** Registered/unregistered by active game mode (TGameMode::OnEnter/OnExit).
- **State of Initialize:** No Initialize; THudDrawable lifecycle managed by renderer.
- **Retail correspondence:** **Uncertain.** Cursor rendering itself is retail, but the THudDrawable abstraction is port-specific. Likely retail has equivalent cursor drawing logic, just not via modern HUD queue.

---

### TAutoMap (Minimap / Auto-Mapper Pane)
- **Path:** `src/automap.h` (128 lines), `src/automap.cpp` (1621 lines)
- **Class hierarchy:** `TAutoMap : public TPane`
- **Stated purpose** (from `automap.h:22-27`): "The automapper pane is a small version of the automatic mapping system which keeps track of where the player has been and displays a portion of this map on the mini-pane."
- **Public surface:**
  - Lifecycle: `Initialize()`, `Close()`, `LoadMapGraphics()`, `FreeMapGraphics()`
  - Map control: `GetMapNumber()`, `GetMaskColorActive()`, `SetMaskColorActive()`, `RecordTravels()`
  - Input: `MouseClick()`, `MouseMove()`
  - Serialization: `WriteAutoMapData()`, `ReadAutoMapData()`
  - Rendering: `DrawBackground()` (draws minimap via pixel merging)
- **Render hooks:** Legacy CPU surface; manually composes pixels into a PTBitmap, no modern path.
- **Global instances:** None declared; likely instantiated in TPlayScreen multifunction panel.
- **State of Initialize:** Loads map graphics from internal cache; depends on GameData for map names/data. Succeeds if map data available.
- **Retail correspondence:** **Likely.** Automap feature is described in original game; implementation details (pixel merging, reveal mask) may vary from retail but feature exists.

---

## Section 3: Game-State Overlays (Conditional, Non-HUD)

### TDialogPane (NPC Dialog / Conversation)
- **Path:** `src/dialog.h` (117 lines), `src/dialog.cpp` (426 lines)
- **Class hierarchy:** `TDialogPane : public TPane`
- **Stated purpose** (from `dialog.h:69-72`): "Dialog pane, for interacting with NPCs in conversation"
- **Public surface:**
  - NPC: `SetCharacter()`, `GetCharacter()`
  - Choices: `AddChoice()`, `SetChoice()`, `HasResponded()`, `GetResponseLabel()`, `GetResponse()`, `ResetResponses()`, `Skip()`
  - Rendering: `DrawBackground()`, `Animate()`
  - Input: `MouseClick()`, `MouseMove()`, `KeyPress()`
- **Render hooks:** Loads `TMulti* dialogdata` (UI frame), renders character + text + choice buttons via legacy Display.
- **Global instances:** None declared; instantiated in TPlayScreen, hidden by default, shown on NPC interaction.
- **State of Initialize:** Loads `dialog.dat` via `TMulti::LoadMulti()`. Requires GameData for dialog strings (LANGUAGE.DEF).
- **Retail correspondence:** **Likely.** Conversation system is a core game mechanic. Pane layout may differ, but feature is retail.

---

### TScrollPane (Scroll / Document Reader)
- **Path:** `src/scroll.h` (87 lines), `src/scroll.cpp` (331 lines)
- **Class hierarchy:** `TScrollPane : public TButtonPane`
- **Stated purpose:** Pane for reading scrolls/documents with text wrapping and scrolling controls.
- **Public surface:**
  - Scroll: `SetScroll()`, `Scroll()`
  - Rendering: `DrawBackground()`, `Initialize()`, `Close()`
  - Input: `KeyPress()`
- **Render hooks:** Loads scroll frame (TMulti), renders text via TFont, legacy Display path.
- **Global instances:** None declared; instantiated conditionally.
- **State of Initialize:** Loads `scroll.dat` via TMulti. Requires TFont for text rendering.
- **Retail correspondence:** **Likely.** Scrolls are interactive game objects; readable overlay is a core feature.

---

### TBookPane (Book Reader — Extended Scroll)
- **Path:** `src/scroll.h` (87 lines), `src/scroll.cpp` (331 lines)
- **Class hierarchy:** `TBookPane : public TScrollPane : public TButtonPane : public TPane`
- **Stated purpose:** Specialized scroll pane for multi-page books; extends TScrollPane.
- **Public surface:**
  - Inherits from TScrollPane; overrides `DrawBackground()` for book-specific rendering.
- **Render hooks:** Legacy Display path via TScrollPane.
- **Global instances:** None declared.
- **State of Initialize:** Calls TScrollPane::Initialize(); expects book.dat instead of scroll.dat.
- **Retail correspondence:** **Likely.** Books are special scroll type; feature exists in retail.

---

### TDeathPane (Death Screen)
- **Path:** `src/death.h` (31 lines), `src/death.cpp` (85 lines)
- **Class hierarchy:** `TDeathPane : public TButtonPane : public TPane`
- **Stated purpose:** Overlay pane shown when player dies; offers respawn/reload options.
- **Public surface:**
  - Lifecycle: `Initialize()`, `Close()`, `DrawBackground()`
  - Inherits button input from TButtonPane.
- **Render hooks:** Loads `death.dat` (TMulti), renders via legacy Display.
- **Global instances:** None declared; instantiated conditionally on player death.
- **State of Initialize:** Loads death.dat. Minimal dependencies.
- **Retail correspondence:** **Likely.** Death is a game event; overlay is standard.

---

## Section 4: Multifunction Panel (Conditional / Multiplex Panes)

### TEquipPane (Equipment / Character Panel)
- **Path:** `src/equip.h` (80 lines), `src/equip.cpp` (210 lines)
- **Class hierarchy:** `TEquipPane : public TButtonPane : public TPane`
- **Stated purpose** (from `equip.h:33-34`): "Displays a portrait of the main character with the various weapons, armor, and adornments he is currently wearing, and allows user to interactively change these around."
- **Public surface:**
  - Slot management: `OnSlot()` (hit-test), `GetHeldSlot()` (for inter-pane transfers)
  - Dragging: `Scroll()` (section scrolling)
  - Animation: `DrawAnim()` (for object preview)
  - Rendering: `DrawBackground()`, `Animate()`, `Initialize()`, `MouseClick()`
- **Render hooks:** Loads `equip.dat` (TMulti); renders portrait + equipment slots via legacy Display + animations.
- **Global instances:** None declared; multiplexed pane managed by TMultiCtrlPane.
- **State of Initialize:** Loads equip.dat. Depends on Player for equipment data.
- **Retail correspondence:** **Likely.** Character/equipment panel is described in retail UI layout.

---

### TInventory (Inventory Container Pane)
- **Path:** `src/inventory.h` (62 lines), `src/inventory.cpp` (255 lines)
- **Class hierarchy:** `TInventory : public TPane`
- **Stated purpose** (from `inventory.h:14-17`): "Inventory for Our Hero which allows him to fool around with his possessions."
- **Public surface:**
  - Container: `GetContainer()`, `GetTopContainer()`, `SetContainer()`
  - Dragging: `GetHeldSlot()`, `SwapSlots()`, `OnSlot()` (slot hit-test)
  - Animation: `DrawAnim()` (object preview during drag)
  - Rendering: `DrawBackground()`, `Animate()`
  - Input: `MouseClick()`, `MouseMove()`
- **Render hooks:** Renders inventory grid + item previews via legacy Display + animations.
- **Global instances:** `extern TInventory Inventory;` (revenant.h:217). Updated on item pickup/drop/use.
- **State of Initialize:** Initializes slot tracking, connects to container object. Depends on Player/GameData.
- **Retail correspondence:** **Likely.** Inventory management is a core mechanic; pane layout may differ.

---

### TSpellPane (Spell / Talisman Assembly Pane)
- **Path:** `src/spellpane.h` (130 lines), `src/spellpane.cpp` (565 lines)
- **Class hierarchy:** `TSpellPane : public TButtonPane : public TPane`
- **Stated purpose** (from `spellpane.h:60-61`): "This pane is where the user assembles spells with various talismans and invokes them."
- **Public surface:**
  - Talisman assembly: `AddTal()`, `RemoveTal()`, `GetSpell()`
  - Quickspells: (see TQuickSpellPane below)
  - Input: `MouseClick()`, `MouseMove()`, `Scroll()`
  - Rendering: `DrawBackground()`
  - Options: `ToggleTalismanNames()`, `ShowTalismanNames()`
- **Render hooks:** Legacy Display path; renders talisman grid + names conditionally.
- **Global instances:** `extern TSpellPane SpellPane;` (revenant.h:222). Multiplexed pane in multifunction panel.
- **State of Initialize:** Loads spell.dat (TMulti); depends on spell system / talisman definitions.
- **Retail correspondence:** **Likely.** Spell/magic system is core; pane layout in multifunction panel.

---

### TQuickSpellPane (Quick Spell Buttons)
- **Path:** `src/spellpane.h` (130 lines), `src/spellpane.cpp` (565 lines)
- **Class hierarchy:** `TQuickSpellPane : public TButtonPane : public TPane`
- **Stated purpose** (from `spellpane.h:110-111`): "A bunch of buttons above the inventory giving the player quicker access to their spells."
- **Public surface:**
  - Spell slots: `Invoke()`, `Clear()`, `Backspace()`, `AddTalisman()`, `Set()`
  - Rendering: `DrawBackground()`
  - Input: `MouseClick()`
- **Render hooks:** Legacy Display path; renders 4 quickspell buttons + current spell assembly.
- **Global instances:** None declared; likely managed as part of TSpellPane.
- **State of Initialize:** Inherits from TButtonPane; loads quickspell graphics (positions QUICKSPELLX, QUICKSPELLY).
- **Retail correspondence:** **Likely.** Quick-access spellbar is standard in action RPGs.

---

### TStatPane (Character Stats / Skills Pane)
- **Path:** `src/statpane.h` (47 lines), `src/statpane.cpp` (307 lines)
- **Class hierarchy:** `TStatPane : public TButtonPane : public TPane`
- **Stated purpose** (from `statpane.h:18-19`): "The stat pane is a display of the main characters abilities/skills/stats."
- **Public surface:**
  - Display: `Scroll()`, `ExpandAll()`, `ContractAll()`
  - Buttons: `ScrollUpButton()`, `ScrollDownButton()`
  - Rendering: `DrawBackground()`
  - Input: `MouseClick()`, `MouseMove()`
- **Render hooks:** Legacy Display path; renders stat table + expandable sections.
- **Global instances:** None declared; multiplexed pane in multifunction panel.
- **State of Initialize:** Loads stat.dat (TMulti); depends on Player for stat data.
- **Retail correspondence:** **Likely.** Character stats display is a common UI element; may be combined or separated in retail.

---

### TMultiCtrlPane (Multipane Control / Tab Buttons)
- **Path:** `src/multictrl.h` (57 lines), `src/multictrl.cpp` (96 lines)
- **Class hierarchy:** `TMultiCtrlPane : public TButtonPane : public TPane`
- **Stated purpose** (from `multictrl.h:22-23`): "The pane which contains the buttons which flip through the various multifunction panels."
- **Public surface:**
  - Pane switching: `ActivatePane()`, `GetActivePane()`, `RedrawCurPane()`, `RedrawOverhangButtons()`
  - Visibility: `Show()`, `Hide()` (toggles all multipanes)
  - Lifecycle: `Initialize()`, `Close()`
- **Render hooks:** Manages button state; delegates rendering to child buttons (legacy Display).
- **Global instances:** None declared; likely instantiated in TPlayScreen.
- **State of Initialize:** Loads button graphics (TMulti). No gameplay dependencies.
- **Retail correspondence:** **Likely.** Tab/multiplex UI is standard; positions and imagery may differ.

---

## Section 5: Map/World Rendering Pane

### TMapPane (Main Playfield / World Rendering)
- **Path:** `src/mappane.h` (598 lines), `src/mappane.cpp` (4452 lines)
- **Class hierarchy:** `TMapPane : public TPane`
- **Stated purpose** (from `mappane.h:104-107`): "The TMapPane object implements the display, scrolling, animation, and game control for the game map. This includes loading and saving sectors, adding, removing, moving, and using objects, animating objects, and scrolling."
- **Public surface:** Exhaustive (~100 public methods). Key categories:
  - Lifecycle: `Initialize()`, `Close()`, `CreateBackgroundBuffers()`, `FreeBackgroundBuffers()`
  - World state: `LoadCurMap()`, `SaveCurMap()`, `ClearCurMap()`, `SaveAllSectors()`, `FreeAllSectors()`, `ReloadSectors()`, `UpdateSectors()`, `UpdateActiveWindow()`
  - Object management: `MakeIndex()`, `NewObject()`, `AddObject()`, `RemoveObject()`, `DeleteObject()`, `FindObject()`, `FindClosestObject()`, `OnObject()`, `GetInstance()`, `RegisterInstance()`, `UnregisterInstance()`
  - Movement/interaction: `MoveObject()`, `Use()`, `Face()`, `TransferObject()`, `CheckPos()`, `LineOfSight()`
  - Rendering: `DrawBackground()`, `DrawUpdateRect()`, `DrawWalkMap()`, `AnimateObjects()`, `AnimateBackground()`, `AnimateSelectedObjects()`
  - Lighting: `SetDLightIntensity()`, `GetDLightIntensity()`, `SetAmbientLight()`, `GetAmbientLight()`, `SetAmbientColor()`, `GetAmbientColor()`, `FadeAmbient()`, `UpdateLights()`, `DrawDLight()`
  - Walkmap: `GetWalkHeight()`, `TransferWalkmap()`, `ClearWalkmap()`, `ExtractWalkmap()`, `CalculateWalkmap()`, `AdjustWalkmap()`, `TransferAllWalkmaps()`, `RefreshWalkmaps()`, `SnapWalkDisplay()`
  - Threading: `BeginUpdateThread()`, `EndUpdateThread()`, `PauseUpdate()`, `ResumeUpdate()`, `FlushUpdate()`, `LockSectors()`, `LockObjects()`, etc.
  - Center-on: `CenterOnObj()`, `CenterOnPos()`, `IsScrollCenterOn()`, `SaveCenterOnState()`, `RestoreCenterOnState()`
  - Positioning: `GetMapPos()`, `SetMapPos()`, `GetMapLevel()`, `SetMapLevel()`, `PosX()`, `PosY()`
  - Input: `MouseClick()`, `MouseMove()`, `KeyPress()`, `UpdateMouseMovement()`, `SetMode()`, `ResetDrag()`, `Clicked()`
  - Iteration: `TMapIterator` companion class for object queries
- **Render hooks:** Legacy CPU path via `Display.*()` calls for unlit/lit/z-buffer compositing. Threading system (Win32 events/CreateThread) stubbed for SDL/cross-platform.
- **Global instances:** `extern TMapPane MapPane;` (revenant.h:214). Central world-state object accessed by gameplay code.
- **State of Initialize:** Calls `TPane::Initialize()`, initializes 3D scene state, dynamic light, money system, exit system, sector arrays. **Heavily coupled to GameData / sectors / Player.** Requires loaded map.
- **Retail correspondence:** **Very likely.** TMapPane is the retail core of the in-game world view. Implementation may have diverged (threading stubs, new layout system), but the feature set is authentic.
- **Size comparison:** src/mappane.cpp 4452 lines vs. recon likely has corresponding size (large retail class).

---

## Section 6: Main Game Screen

### TPlayScreen (Gameplay Screen)
- **Path:** `src/playscreen.h` (236 lines), `src/playscreen.cpp` (1175 lines)
- **Class hierarchy:** `TPlayScreen : public TScreen`
- **Stated purpose** (from `playscreen.h:1-26`): "TPlayScreen drives an in-game session: world rendering through the owned TMapRenderer, command/input dispatch, game-time tracking, and pane composition for HUD / inventory / editor / dialogs."
- **Public surface:**
  - Lifecycle: `Initialize()`, `Close()`, `SpawnDefaultPlayer()`
  - Frame loop: `Update()`, `RenderFrame()`, `Tick()`, `DrawFrame()` (legacy Pulse/Animate/DrawBackground preserved for compat)
  - Input: `MouseClick()`, `MouseMove()`, `KeyPress()`, `Joystick()`, `Command()`
  - Mode flags: `IsFullScreen()`, `SetFullScreen()`, `IsDemoMode()`, `SetDemoMode()`, `IsControlOn()`, `SetControlOn()`
  - Pane management: `SetNextPane()`, `HideLowerPanes()`, `ShowLowerPanes()`, `MultiUpdate()`
  - Save/load: `LoadGame()`, `LoadGameFile()`, `SaveGame()`, `NewGame()`, `SaveMap()`
  - Game time: `GameFrame()`, `GameTime()`, `SetGameTime()`, `TimeOfDay()`, `Daylight()`, `DayTimeFlag()`
  - Overlay: `AddPostCharAnim()`, `AddPostCharText()` (stubbed pending new render pipeline)
  - Map access: `MapRenderer()`
  - Command processing: `UpdateMove()` (public helper for runtime command dispatch)
- **Render hooks:** New frame pipeline via `TMapRenderer` for world rendering; panes composite via `TScreen::DrawFrame()`. Legacy `DrawBackground()` is now a no-op (no BITMAP.100 backdrop).
- **Global instances:** `extern TPlayScreen PlayScreen;` (revenant.h:212). Managed via TScreen::ShowScreen() / EndCurrentScreen().
- **State of Initialize:** Calls `mapRenderer->InitializeFromStartupArgs()`, `AreaManager.Initialize()`, `ControlMap.Initialize()`. Heavily coupled to GameData, sectors, Player, time system. **Requires loaded world.**
- **Retail correspondence:** **Likely to very likely.** The 1998 retail TPlayScreen exists (see attic/src/playscreen.* for legacy impl); current version is a port-refactor. Feature set is authentic; frame pipeline is port-specific.
- **Size comparison:** src/playscreen.cpp 1175 lines vs. recon cls_0x5a7b98_likely_TPlayScreen.cpp 16332 lines. **Large divergence suggests retail had more elaborate initialization / pane setup code; current port is simplified. Forensic opportunity: check recon for missing gameplay hooks (command dispatch, save/load flow, pane lifecycle).**

---

## Section 7: Testing / Editor Infrastructure

### TTestScreen (Bring-Up / Test Harness)
- **Path:** `src/testscreen.h` (33 lines), `src/testscreen.cpp` (80 lines)
- **Class hierarchy:** `TTestScreen : public TScreen`
- **Stated purpose** (from `testscreen.h:7-8`): "Bring-up harness for the retail sync port… routed in via --test=<mode> so we can exercise one subsystem at a time as we synchronize classes from Ghidra."
- **Public surface:**
  - Lifecycle: `Initialize()`, `Close()`
  - Frame loop: `Pulse()`, `DrawBackground()`, `Animate()`, `KeyPress()`, `MouseClick()`, `MouseMove()`
- **Render hooks:** Minimal; used for isolated subsystem testing.
- **Global instances:** `extern TTestScreen TestScreen;` (testscreen.h:33).
- **State of Initialize:** No dependencies; stub for testing.
- **Retail correspondence:** **Port-specific.** Testing infrastructure; not in retail.

---

### TUITextBarTest (TextBar Test Mode)
- **Path:** `src/uitextbartest.h` (21 lines), `src/uitextbartest.cpp` (146 lines)
- **Class hierarchy:** TScreen or standalone test
- **Stated purpose:** Test harness for TTextBar rendering and input.
- **Public surface:** Test-only methods.
- **Render hooks:** Invokes TTextBar methods for validation.
- **Global instances:** None; test-only.
- **State of Initialize:** N/A
- **Retail correspondence:** **Port-specific.** Testing infrastructure.

---

## Section 8: Asset Infrastructure

### TFont (Bitmap Font)
- **Path:** `src/font.h` (90 lines), `src/font.cpp` (319 lines)
- **Class hierarchy:** `TFont : public TFontData`
- **Stated purpose:** Bitmap font wrapper; provides glyph metrics and bitmap access for CPU-based text rendering.
- **Public surface:**
  - Glyphs: `GetChar()`, `FirstChar()`, `Numchars()`, `StartHeight()`, `DrawRight()`, `DrawLeft()`
  - Text metrics: `FindNumLinesInText()`
- **Render hooks:** `GetChar()` returns PTBitmap for each glyph; caller uses legacy Display to draw.
- **Global instances:** None directly; loaded via FontTable lookup.
- **State of Initialize:** Loaded by TFontTable from resource (TFont* is raw malloc'd memory, no ctor).
- **Retail correspondence:** **Very likely.** Bitmap fonts are a 1998 standard; rendering path is legacy CPU.

---

### TFontTable (Font Registry)
- **Path:** `src/fonttable.h` (104 lines), `src/fonttable.cpp` (268 lines)
- **Class hierarchy:** Standalone registry
- **Stated purpose:** Loads FONT.DEF, caches bitmap font atoms, provides lookup by name. Bridges BMFONT (bitmap) and WINFONT (TrueType) entries.
- **Public surface:**
  - Lifecycle: `Initialize()`, `Close()`
  - Lookup: `Font()`, `FindFont()`, `Bitmap()`
  - Metadata: TGenericFont bundles bitmap atoms, color, metrics, TrueType descriptor.
- **Render hooks:** None; data provider only.
- **Global instances:** `extern TFontTable* FontTable;` (fonttable.h:104). Created during game startup.
- **State of Initialize:** Parses FONT.DEF; loads bitmap atoms on demand; WINFONT entries deferred until TTF rasterizer plugged in.
- **Retail correspondence:** **Likely.** Font management is a standard subsystem. WINFONT support is a port-era addition (TTF rendering); BMFONT is retail-compatible.

---

### SFontAtlas / Font Atlasing
- **Path:** `src/font.h` (90 lines)
- **Stated purpose:** Modern texture-atlas cache for fonts (GPU path). One RGBA8 texture per TFont* key, with per-glyph UV rects.
- **Public surface:**
  - Cache: `BuildFontAtlas()`, `FindFontAtlas()`, `DestroyAllFontAtlases()`
  - TTF support: `BuildTTFAtlas()`, `DestroyAllTTFAtlases()`
- **Render hooks:** Populated on demand; consumed by `Renderer->DrawText()` (new path, not yet wired for UI text).
- **Retail correspondence:** **Port-specific.** Modern texture atlasing; retail used CPU-blitted glyphs.

---

### AutoMapData
- **Path:** `src/automapdata.h` (79 lines)
- **Stated purpose:** Data structures for automap pixel reveal masks, map graphics caching.
- **Public surface:** Data-only; no public methods, just arrays and graphics pointers.
- **Retail correspondence:** **Likely.** Supporting data for TAutoMap; retail feature.

---

### CharStats
- **Path:** `src/charstats.h` (65 lines)
- **Stated purpose:** Character stat aggregation (attributes, skills, health, stamina, mana). Data-only struct used by TStatPane and HUD.
- **Public surface:** Data-only.
- **Retail correspondence:** **Very likely.** Core character attribute system.

---

### FontData
- **Path:** `src/fontdata.h` (29 lines)
- **Stated purpose:** Base structure for bitmap font metrics (firstchar, numchars, startheight[], drawright[], drawleft[]). Raw resource layout.
- **Public surface:** Data-only; inherited by TFont.
- **Retail correspondence:** **Very likely.** Bitmap font resource format is 1998 standard.

---

## Section 9: Suspect / Off-Spec Classes

### TStatusBar, THealthBar, TStaminaBar
- **Issue:** Phase B incorrectly assumed these standalone classes exist in retail. Evidence strongly suggests they do not.
  - Comment in testmodes.cpp:38 explicitly flags "encoded were derived from the pre-release src/ defines (HEALTHBARX etc)"
  - Positions hardcoded to pre-release constants (HEALTHBARX=16, STAMINABARX=612)
  - No recon decomp found for either class
  - User description of retail HUD mentions "two combined health/mana/character panels upper-left + upper-right" — not standalone bars
- **Verdict:** **Pre-release architecture. Retail likely unified these into character panel panes (one per corner). These classes must be rewritten or removed in Phase C.**

---

## Section 10: Open Questions for Forensic Recovery

### Large Size Discrepancies
- **TPlayScreen:** src/playscreen.cpp 1175 lines vs. recon 16332 lines. **16x growth suggests retail had extensive pane lifecycle, save/load flow, and command dispatch logic not preserved in port. Check recon for:**
  - Pane construction / destruction sequences
  - Save-game format negotiation
  - Command queuing / deferred execution
  - HUD state caching (health bar animations, text bar history)

### Missing Recon Mappings
- **THealthBar, TStaminaBar, TStatusBar:** No recon files found. **Are these classes retail at all, or pure pre-release?** Check Ghidra for object files:
  - Search retail binary for function names like `THealthBar::Initialize`, `TStaminaBar::GetHue`
  - Check data symbols for `HealthBar`, `StaminaBar` globals
  - If not found, confirm Phase B direction was wrong and these must be removed

### Multifunction Panel Layout
- **TMultiCtrlPane, TEquipPane, TStatPane, TSpellPane, TQuickSpellPane:** Currently all multiplexed in one "multifunction" region (MULTIPANEX, MULTIPANEY). **Retail may have had different layout:**
  - Check recon for separate pane positions (right-side stacks, overlays, etc.)
  - Determine if TMultiCtrlPane (tab switcher) even existed in retail or if the four panes were statically visible regions

### Quickspell / Shelf
- **TQuickSpellPane:** Described as "buttons above the inventory" — fits "bottom quickspell+shelf" in retail description. **Verify positioning and icon format in recon.**

### Game-Log Overlay
- **TTextBar / transparent overlay:** Current impl positions text bar once under map (TEXTBARX, TEXTBARY). **Retail "transparent game-log overlay" may be a scrollback buffer with alpha-blend compositing, not a static pane. Check recon for scrollback data structures.**

### Dialog / Book / Scroll Overlays
- **TDialogPane, TBookPane, TScrollPane:** All currently position at hardcoded locations (DIALOGX etc.) when shown. **Retail may have centered them or positioned relative to screen. Check recon pane positioning.**

### Character Portrait Integration
- **TEquipPane renders character portrait.** Is the portrait a separate pane, or part of the combined "health/mana/character panel"? **Check recon for character-panel layout and how portrait is animated on equipment change.**

### Bottom-Right Button Bar
- **Small bottom-right button bar mentioned in retail description not clearly identified in src/.** Could be:
  - Part of TMultiCtrlPane (buttons to switch panes)
  - A separate TButtonPane instance
  - Not yet ported
  - **Check recon for a small button cluster pane in that region.**

### Cursor Handling
- **TCursorHud is new (THudDrawable-based).** Check recon for original cursor drawing:
  - Was it a pane (TPane with low z-order)?
  - Was it blitted directly in the main loop?
  - Did it have shadow/corner bitmaps? (Yes, based on cursor.h SetMouseShadow, SetMouseCornerBitmap functions — these are retail interface.)

### TMapPane Threading
- **Threading system is stubbed (#if 0).** Retail Win32 version used CreateThread + Win32 events. Port has no thread pool equivalent yet. **Check recon for:**
  - Thread safety assumptions (object lock patterns)
  - Synchronization points (update queue completion)
  - Performance implications (single-threaded UpdateLoop() may be slower on multi-core)

---

## Summary Table: Retail Confidence by Class

| Class | Type | Retail Confidence | Notes |
|-------|------|-------------------|-------|
| TPane | Framework | **Very High** | Foundational; vanilla behavior unchanged |
| TScreen | Framework | **Very High** | Screen manager; core to architecture |
| TButton | Framework | **Very High** | Simple primitive; 1998-era |
| TButtonPane | Framework | **High** | Container pattern; likely retail |
| TMapPane | HUD / World | **Very High** | Retail core; features authentic |
| TPlayScreen | Screen | **High** | Main game screen; port refactor but authentic features |
| THealthBar | HUD | **Low** | Pre-release design; standalone bar not in retail |
| TStaminaBar | HUD | **Low** | Pre-release design; standalone bar not in retail |
| TStatusBar | HUD | **Low** | Abstract base for bars; suspect |
| TTextBar | HUD | **High** | Status/feedback line; retail feature |
| TEquipPane | HUD | **High** | Character/equipment panel; retail feature |
| TInventory | HUD | **High** | Inventory management; core feature |
| TSpellPane | HUD | **High** | Spell assembly; core feature |
| TQuickSpellPane | HUD | **Medium** | Quick access buttons; likely retail but positioning may differ |
| TStatPane | HUD | **Medium** | Character stats; may be integrated into character panel in retail |
| TMultiCtrlPane | HUD | **Medium** | Pane tab switcher; may not be a standalone pane in retail |
| TDialogPane | Overlay | **High** | NPC conversation; core feature |
| TScrollPane | Overlay | **High** | Document reader; core feature |
| TBookPane | Overlay | **High** | Book variant; core feature |
| TDeathPane | Overlay | **High** | Death screen; core feature |
| TAutoMap | HUD | **Medium** | Minimap; retail feature, but implementation may differ |
| TCursorHud | HUD | **Medium** | Cursor drawing; retail has equivalent, but abstraction is port-specific |
| TFont | Asset | **Very High** | Bitmap font; standard subsystem |
| TFontTable | Asset | **High** | Font registry; retail likely had equivalent (numeric IDs → names is port modernization) |
| TTestScreen | Test | **N/A** | Port-specific testing harness |
| TUITextBarTest | Test | **N/A** | Port-specific testing harness |
| TAutoMapData | Asset | **High** | Supporting data for automap |
| CharStats | Asset | **Very High** | Character stat struct; core type |
| FontData | Asset | **Very High** | Bitmap font metrics; standard format |

---

## Recommendations for Phase C

1. **Immediately deprioritize TStatusBar / THealthBar / TStaminaBar.** These are flagged as pre-release designs. Research recon binary for evidence of retail equivalents before implementing.

2. **Audit pane positioning constants** (MULTIPANEX, HEALTHBARX, etc.) against recon to validate layout. Consider centralizing position defs.

3. **Prioritize forensic mapping:**
   - Match recon TPlayScreen methods to current TPlayScreen to identify missing pane-lifecycle hooks
   - Search recon for character-panel structure (unified health/mana/character?)
   - Verify bottom-right button bar identity

4. **Multifunction panel rationalization:** Determine if TMultiCtrlPane is needed, or if panes were statically visible in retail.

5. **Threading deferred:** Keep stubs until port reaches stable gameplay. Win32 threading can be a follow-on.

6. **Cursor integration:** TCursorHud is sound; cursor-drawing interface (SetMouseBitmap, SetMouseShadow) is retail. Keeps THudDrawable pattern.

7. **Build inventory for gameplay subsystems** (command dispatch, save/load, dialog/script state) once UI is stabilized. HUD is foundational but not sufficient; gameplay glue is next audit.

---

